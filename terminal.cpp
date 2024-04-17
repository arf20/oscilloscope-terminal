#include "main.hpp"

#include <iostream>
#include <vector>

#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <poll.h>
#include <termios.h>

static int masterfd = 0;
static int childpid = 0;
static termios ogterm {};
bool terminalrunning = true;

void saveTerminal() {
    // Fetch original terminal settings
    tcgetattr(STDIN_FILENO, &ogterm);
}

void restoreTerminal() { 
    // Restore original terminal settings. Also manually set ONLCR.
    ogterm.c_oflag |= ONLCR;
    tcsetattr(STDIN_FILENO, TCSANOW, &ogterm);
}

void sigchldHandler(int signum) {
    if (signum == SIGCHLD) {
        int result = 0;
        waitpid(childpid, &result, WNOHANG);

        if (WIFEXITED(result)) {
            restoreTerminal();
            std::cout << "\r" << std::flush << "Error: SIGCHLD shell process exited" << std::endl;

            // Reset canonical mode for parent process stdin
            /*termios termios_p {};
            termios_p.c_lflag = termios_p.c_lflag & ICANON;
            tcsetattr(0, TCSANOW, &termios_p);*/
            // system("reset");
            exit(1);
        }
    }
}

void createTerminal() {
    // Set non-canonical mode for parent process stdin
    termios termios_p {};
    termios_p.c_lflag = termios_p.c_lflag & ~(ICANON);
    termios_p.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSANOW, &termios_p);

    // Restore terminal upon program exit. 
    atexit(restoreTerminal);

    // Create PTM PTS pair
    // Create PTM from PTMX
    if ((masterfd = posix_openpt(O_RDWR)) < 0) {
        std::cout << "posix_openpt failed with " << strerror(errno) << std::endl;
        exit(1);
    }

    if (grantpt(masterfd) < 0) {
        std::cout << "grantpt failed with " << strerror(errno) << std::endl;
        exit(1);
    }

    if (unlockpt(masterfd) < 0) {
        std::cout << "unlockpt failed with " << strerror(errno) << std::endl;
        exit(1);
    }

    // Get PTS devname of PTM
    char *slavedev = NULL;
    if ((slavedev = ptsname(masterfd)) == NULL) {
        std::cout << "ptsname failed with " << strerror(errno) << std::endl;
        exit(1);
    }

    std::cout << "Terminal at " << slavedev << std::endl;

    if (signal(SIGCHLD, sigchldHandler) == SIG_ERR) {
        std::cout << "signal failed with " << strerror(errno) << std::endl;
        exit(1);
    }

    // Fork a process for the shell
    childpid = 0;
    if ((childpid = fork()) == 0) {
        if (setsid() < 0) {
            std::cout << "setsid failed with " << strerror(errno) << std::endl;
            exit(1);
        }

        // Open PTS for shell process
        static int slavefd = 0;
        if ((slavefd = open(slavedev, O_RDWR)) < 0) {
            std::cout << "open failed with " << strerror(errno) << std::endl;
            exit(1);
        }

        if (close(masterfd) < 0) {  // Not needed in shell process
            std::cout << "close failed with " << strerror(errno) << std::endl;
            exit(1);
        }

        // Hack
        if (ioctl(slavefd, TIOCSCTTY, 0) < 0) {
            std::cout << "ioctl failed with " << strerror(errno) << std::endl;
            exit(1);
        }

        // Close actual PTS
        if (close(0) < 0) {
            std::cout << "close failed with " << strerror(errno) << std::endl;
            exit(1);
        }
        if (close(1) < 0) {
            std::cout << "close failed with " << strerror(errno) << std::endl;
            exit(1);
        }
        if (close(2) < 0) {
            std::cout << "close failed with " << strerror(errno) << std::endl;
            exit(1);
        }

        // Link new PTS with std streams
        if (dup2(slavefd, 0) < 0) {
            std::cout << "dup2 failed with " << strerror(errno) << std::endl;
            exit(1);
        }
        if (dup2(slavefd, 1) < 0) {
            std::cout << "dup2 failed with " << strerror(errno) << std::endl;
            exit(1);
        }
        if (dup2(slavefd, 2) < 0) {
            std::cout << "dup2 failed with " << strerror(errno) << std::endl;
            exit(1);
        }

        // Launch shell
        char *const argv[] = {(char*)"bash", (char*)"-l", NULL};

        if (execv("/bin/bash", argv) < 0) {
            std::cout << "execv failed with " << strerror(errno) << std::endl;
            exit(1);
        }

        exit(0);
    }
}

void pushKey(char v) {
    write(masterfd, &v, 1);
}

void closeTerminal() {
    kill(childpid, SIGHUP);
    terminalrunning = false;
}


int cursorX = 0;
int cursorY = 0;

static bool esc = false;
static bool csi = false;
static bool osc = false;

static std::string csiparams, csiinter;

void clearargs() {
    csiparams.clear();
    csiinter.clear();
}

void writeFrameBuff(const char *buff, size_t n) {
    for (int i = 0; i < n; i++) {
        char c = buff[i];

        // CSI (ESC [) Escape sequences
        if (csi) {
            if (c >= 0x30 && c <= 0x3f) {
                csiparams += c;
                continue;
            }
            if (c >= 0x20 && c <= 0x2f) {
                csiinter += c;
                continue;
            }
            else if (c >= 0x40 && c <= 0x7e) {
                // if final byte 

                if (c == 'A') {  // move cursor up
                    if (csiparams.length() > 0) { 
                        int n = std::stoi(csiparams);
                        if (cursorY - n > -1)
                            cursorY -= n;
                        else
                            cursorY = 0;
                    }
                    else
                        if (cursorY > 0)
                            cursorY--;
                }
                if (c == 'B') {  // move cursor down
                    if (csiparams.length() > 0) {
                        int n = std::stoi(csiparams);
                        if (cursorY + n < HEIGHT)
                            cursorY += n;
                        else
                            cursorY = HEIGHT - 1;
                    }
                    else
                        if (cursorY < HEIGHT - 1)
                            cursorY++;
                }
                if (c == 'C') {  // move cursor forward
                    if (csiparams.length() > 0) {
                        int n = std::stoi(csiparams); 
                        if (cursorX + n < WIDTH)
                            cursorX += n;
                        else
                            cursorX = WIDTH - 1;
                    }
                    else
                        if (cursorX < WIDTH - 1)
                            cursorX++;
                }
                if (c == 'D') {  // move cursor back
                    if (csiparams.length() > 0) {
                        int n = std::stoi(csiparams);
                        if (cursorX - n > -1)
                            cursorX -= n;
                        else
                            cursorX = 0;
                    }
                    else
                        if (cursorX > 0)
                            cursorX--;
                }

                if (c == 'E') {  // move cursor to beggining of next line
                    if (csiparams.length() > 0) {
                        int n = std::stoi(csiparams);
                        if (cursorY + n < HEIGHT)
                            cursorY += n;
                    }
                    else
                        if (cursorY < HEIGHT - 1)
                            cursorY++;
                        
                    cursorX = 0;
                }
                if (c == 'F') {  // move cursor to beggining of previous line
                    if (csiparams.length() > 0) {
                        int n = std::stoi(csiparams);
                        if (cursorY - n > -1)
                            cursorY -= n;
                    }
                    else
                        if (cursorY > 0)
                            cursorY--;
                        
                    cursorX = 0;
                }

                if (c == 'G') {  // move cursor to absolute column
                    if (csiparams.length() > 0) {
                        int n = std::stoi(csiparams);
                        cursorX = n - 1;
                    }
                    else
                        cursorX = 0;
                }

                if (c == 'H') {  // move cursor to absolute row and column
                    if (csiparams.length() > 0) {
                        size_t sci = csiparams.find(";");
                        cursorY = std::stoi(csiparams.substr(0, sci)) - 1;
                        cursorX = std::stoi(csiparams.substr(sci + 1)) - 1;
                    } else {
                        cursorY = cursorX = 0;
                    }
                }

                if (c == 'J') {     // clear part of the screen
                    if (csiparams.length() > 0) {
                        switch (std::stoi(csiparams)) {
                            case 0: {
                                // cursor to end
                                for (int j = (cursorY * WIDTH) + cursorX; j < WIDTH * HEIGHT; j++)
                                    frameBuffer[j] = 0;
                            } break;
                            case 1: {
                                // cursor to beggining
                                for (int j = (cursorY * WIDTH) + cursorX; j >= 0; j--)
                                    frameBuffer[j] = 0;
                            } break;
                            case 2: {
                                // entire
                                for (int j = 0; j < WIDTH * HEIGHT; j++)
                                    frameBuffer[j] = 0;
                            }
                            case 3: {
                                // entire and scrollback (no scrollback)
                                for (int j = 0; j < WIDTH * HEIGHT; j++)
                                    frameBuffer[j] = 0;
                            }
                        }
                    } else {
                        // cursor to end
                        for (int j = (cursorY * WIDTH) + cursorX; j < WIDTH * HEIGHT; j++)
                            frameBuffer[j] = 0;
                    }
                }

                if (c == 'K') {  // clear part of the line
                    if (csiparams.length() > 0) {
                        switch (std::stoi(csiparams)) {
                            case 0: {
                                // cursor to end
                                for (int x = cursorX; x < WIDTH - 1; x++)
                                    frameBuffer[(cursorY * WIDTH) + x] = 0;
                            } break;
                            case 1: {
                                // cursor to beggining
                                for (int x = cursorX; x >= 0; x--)
                                    frameBuffer[(cursorY * WIDTH) + x] = 0;
                            } break;
                            case 2: {
                                // entire
                                for (int x = 0; x < WIDTH - 1; x++)
                                    frameBuffer[(cursorY * WIDTH) + x] = 0;
                            }
                        }
                    } else {
                        // cursor to end
                        for (int x = cursorX; x < WIDTH - 1; x++)
                            frameBuffer[(cursorY * WIDTH) + x] = 0;
                    }
                }

                csi = false;
                clearargs();
                continue; // don't print final char
            }
        }

        if (!csi)
            csi = (esc && (c == '['));
        if (!osc)
            osc = (esc && (c == ']'));
        esc = (c == 27);

        if (csi || osc)
            continue;   // prevent printing [ in ESC [


        // things that affect cursor
        if (c == 8) {   // backspace
            cursorX--;
        }

        if (c == 9) {   // tab
            int col = cursorX / 8;
            cursorX = (col + 1) * 8;
        }

        if (c == '\n') {    // LF
            //cursorX = 0;
            cursorY++;
        }

        if (c == '\r') {    // CR
            cursorX = 0;
        }

        // wrap lines
        if (cursorX >= WIDTH) {
            cursorX = 0;
            cursorY++;
        }
        if (cursorX < 0) {
            cursorX = WIDTH - 1;
            cursorY--;
        }

        // scroll
        if (cursorY >= HEIGHT) {
            for (int y = 0; y < HEIGHT - 1; y++) {
                for (int x = 0; x < WIDTH; x++)
                    frameBuffer[(y * WIDTH) + x] = frameBuffer[((y + 1) * WIDTH) + x];
            }

            for (int x = 0; x < WIDTH; x++)
                    frameBuffer[((HEIGHT - 1) * WIDTH) + x] = 0;

            cursorY = HEIGHT - 1;
        }

        // otherwise, print character
        if (c >= 32 && c <= 126) {
            frameBuffer[(cursorY * WIDTH) + cursorX] = c;
            cursorX++;
        }

        //render();
    }
}

void runTerminal() {
    char *buff = new char[1];

    int nfds = 2;
    pollfd *pfds = new pollfd[nfds];
    memset(pfds, sizeof(pollfd), 0);

    // read masterfd
    pfds[0].fd = masterfd;
    pfds[0].events = POLLIN;

    // read stdin
    pfds[1].fd = 0;
    pfds[1].events = POLLIN;

    while (terminalrunning) {
        if (poll(pfds, nfds, 100) < 0) {
            std::cout << "poll failed with " << strerror(errno) << std::endl;
            exit(1);
        }

        if (pfds[0].revents & POLLIN) {
            if (read(masterfd, buff, 1) < 0) {
                std::cout << "read failed with " << strerror(errno) << std::endl;
                exit(1);
            }

            writeFrameBuff(buff, 1);

        }
        if (pfds[1].revents & POLLIN) {
            if (read(0, buff, 1) < 0) {
                std::cout << "read failed with " << strerror(errno) << std::endl;
                exit(1);
            }

            if (write(masterfd, buff, 1) < 0) {
                std::cout << "write failed with " << strerror(errno) << std::endl;
                exit(1);
            }
        }
    }

    std::cout << "EOF" << std::endl;
}
