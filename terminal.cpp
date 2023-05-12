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

void sigchldHandler(int signum) {
    if (signum == SIGCHLD) {
        int result = 0;
        waitpid(childpid, &result, WNOHANG);

        if (WIFEXITED(result)) {
            std::cout << "Error: SIGCHLD shell process exited" << std::endl;

            // Reset canonical mode for parent process stdin
            /*termios termios_p {};
            termios_p.c_lflag = termios_p.c_lflag & ICANON;
            tcsetattr(0, TCSANOW, &termios_p);*/
            system("reset");
            exit(1);
        }
    }
}

void saveTerminal() {
    // Fetch original terminal settings
    tcgetattr(STDIN_FILENO, &ogterm);
}

void restoreTerminal() { 
    // Restore original terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &ogterm);
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
        int slavefd = 0;
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

int cursorX = 0;
int cursorY = 0;

static bool esc = false;
static bool csi = false;
static bool osc = false;

static std::vector<std::string> argstr;
static int argidx = 0;

void clearargs() {
    argstr.clear();
    argidx = 0;
}

static std::string history = "";

void writeFrameBuff(const char *buff, size_t n) {
    for (int i = 0; i < n; i++) {
        char c = buff[i];
        history += c;

        // CSI Escape sequences
        if (csi) {
            if (!(c >= 48 && c <= 57)) {
                if (c == ';') {
                    argidx++;
                    continue;
                }

                if (c == 'A') {  // move cursor up
                    if (argstr.size() == 1) { 
                        int n1 = std::stoi(argstr[0]);
                        if (cursorY - n1 > -1)
                            cursorY -= n1;
                        else
                            cursorY = 0;
                    }
                    else
                        if (cursorY > 0)
                            cursorY--;
                        
                    clearargs();
                    continue;
                }
                if (c == 'B') {  // move cursor down
                    if (argstr.size() == 1) {
                        int n1 = std::stoi(argstr[0]);
                        if (cursorY + n1 < HEIGHT)
                            cursorY += n1;
                        else
                            cursorY = HEIGHT - 1;
                    }
                    else
                        if (cursorY < HEIGHT - 1)
                            cursorY++;
                        
                    clearargs();
                    continue;
                }
                if (c == 'C') {  // move cursor forward
                    if (argstr.size() == 1) {
                        int n1 = std::stoi(argstr[0]); 
                        if (cursorX + n1 < WIDTH)
                            cursorX += n1;
                        else
                            cursorX = WIDTH - 1;
                    }
                    else
                        if (cursorX < WIDTH - 1)
                            cursorX++;
                        
                    clearargs();
                    continue;
                }
                if (c == 'D') {  // move cursor back
                    if (argstr.size() == 1) {
                        int n1 = std::stoi(argstr[0]);
                        if (cursorX - n1 > -1)
                            cursorX -= n1;
                        else
                            cursorX = 0;
                    }
                    else
                        if (cursorX > 0)
                            cursorX--;
                        
                    clearargs();
                    continue;
                }

                if (c == 'E') {  // move cursor to beggining of next line
                    if (argstr.size() == 1) {
                        int n1 = std::stoi(argstr[0]);
                        if (cursorY + n1 < HEIGHT)
                            cursorY += n1;
                    }
                    else
                        if (cursorY < HEIGHT - 1)
                            cursorY++;
                        
                    cursorX = 0;
                    clearargs();
                    continue;
                }
                if (c == 'F') {  // move cursor to beggining of previous line
                    if (argstr.size() == 1) {
                        int n1 = std::stoi(argstr[0]);
                        if (cursorY - n1 > -1)
                            cursorY -= n1;
                    }
                    else
                        if (cursorY > 0)
                            cursorY--;
                        
                    cursorX = 0;
                    clearargs();
                    continue;
                }

                if (c == 'G') {  // move cursor to absolute column
                    if (argstr.size() == 1) {
                        int n1 = std::stoi(argstr[0]);
                        cursorX = n1 - 1;
                    }
                    if (argstr.size() == 0)
                        cursorX = 0;
                    
                    clearargs();
                    continue;
                }

                if (c == 'H') {  // move cursor to absolute row and column
                    if (argstr.size() == 2) {
                        int n1 = 0;
                        int n2 = 0;

                        if (argstr[0].length() == 0)    // row
                            n1 = 1;
                        else
                            int n1 = std::stoi(argstr[0]);

                        if (argstr[1].length() == 0)    // column
                            n2 = 1;
                        else
                            int n2 = std::stoi(argstr[1]);

                        cursorY = n1 - 1;
                        cursorX = n2 - 1;
                    }
                    if (argstr.size() == 1) {
                        int n1 = std::stoi(argstr[0]);    // row
                        cursorY = n1 - 1;
                        cursorX = 0;
                    }
                    if (argstr.size() == 0) {
                        cursorY = 0;
                        cursorX = 0;
                    }
                    
                    clearargs();
                    continue;
                }

                if (c == 'J') {     // clear part of the screen
                    int n1 = 0;
                    if ((argstr.size() == 1 && (n1 = std::stoi(argstr[0])) == 0) || argstr.size() == 0)   // cursor to end of screen
                        for (int j = (cursorY * WIDTH) + cursorX; j < WIDTH * HEIGHT; j++)
                            frameBuffer[j] = 0;
                    if (argstr.size() == 1 && (n1 = std::stoi(argstr[0])) == 1)                         // cursor to beggining
                        for (int j = (cursorY * WIDTH) + cursorX; j >= 0; j--)
                            frameBuffer[j] = 0;
                    if (argstr.size() == 1 && (n1 = std::stoi(argstr[0])) == 2)                         // entire screen
                        for (int j = 0; j < WIDTH * HEIGHT; j++)
                            frameBuffer[j] = 0;
                    if (argstr.size() == 1 && (n1 = std::stoi(argstr[0])) == 3)                         // entire screen and scrollback buffer
                        for (int j = 0; j < WIDTH * HEIGHT; j++)
                            frameBuffer[j] = 0;
                }

                if (c == 'K') {  // clear part of the line
                    int n1 = 0;
                    if ((argstr.size() == 1 && (n1 = std::stoi(argstr[0])) == 0) || argstr.size() == 0)   // cursor to end
                        for (int x = cursorX; x < WIDTH - 1; x++)
                            frameBuffer[(cursorY * WIDTH) + x] = 0;
                    if (argstr.size() == 1 && (n1 = std::stoi(argstr[0])) == 1)                         // cursor to beggining
                        for (int x = cursorX; x >= 0; x--)
                            frameBuffer[(cursorY * WIDTH) + x] = 0;
                    if (argstr.size() == 1 && (n1 = std::stoi(argstr[0])) == 2)                         // entire line
                        for (int x = 0; x < WIDTH - 1; x++)
                            frameBuffer[(cursorY * WIDTH) + x] = 0;
                    
                    clearargs();
                    continue;
                }

                if (c == 'S') {     // scroll up whole page
                    clearargs();
                    continue;
                }

                if (c == 'T') {     // scroll down whole page
                    clearargs();
                    continue;
                }

                if (c == 'f') {     // i have no clue what it does, ANSI wiki is not very clear
                    clearargs();
                    continue;
                }

                if (c == 'm') {     // Select Graphic Rendition: completely unsupported
                    clearargs();
                    continue;
                }
            } else {
                if (argidx > (int(argstr.size()) - 1))
                    argstr.push_back(std::string(""));

                argstr[argidx] += c;

                continue;
            }
        }

        if (osc) {                  // Operating System Command: completely unsupported
            if (c == 7 || c == 27) {
                osc = false;
                clearargs();
                continue;
            }
            else {
                if (c == ';') {
                    argidx++;
                    continue;
                }

                if (argidx > (int(argstr.size()) - 1))
                    argstr.push_back(std::string(""));

                argstr[argidx] += c;

                continue;
            }
        }

        csi = (esc && (c == '['));
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

    while (true) {
        if (poll(pfds, nfds, 2000) < 0) {
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