#include "main.hpp"

#include <iostream>

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

int masterfd = 0;
int childpid = 0;

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

void createTerminal() {
    // Set non-canonical mode for parent process stdin
    termios termios_p {};
    termios_p.c_lflag = termios_p.c_lflag & ~(ICANON);
    termios_p.c_cc[VTIME] = 1;
    tcsetattr(0, TCSANOW, &termios_p);

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

bool esc = false;
bool csi = false;

void writeFrameBuff(const char *buff, size_t n) {
    for (int i = 0; i < n; i++) {
        char c = buff[i];

        csi = (esc && (c == '['));
        esc = (c == 127);

        if (csi) {
            if (c == 'K' && cursorX > 0) {
                frameBuffer[(cursorY * WIDTH) + (cursorX - 1)] = 0;
                continue;
            }
        }

        if (c == 8) {
            cursorX--;
        }

        if (c == 9) {
            int col = cursorX / 8;
            cursorX = (col + 1) * 8;
        }

        if (c == '\n') {
            //cursorX = 0;
            cursorY++;
        }

        if (c == '\r') {
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

        // otherwise, print
        if (c >= 32 && c <= 126) {
            frameBuffer[(cursorY * WIDTH) + cursorX] = c;
            cursorX++;
        }

        render();
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