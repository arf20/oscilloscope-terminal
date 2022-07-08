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

int masterfd = 0;
int childpid = 0;

void sigchldHandler(int signum) {
    if (signum == SIGCHLD) {
        int result = 0;
        waitpid(childpid, &result, WNOHANG);

        if (WIFEXITED(result)) {
            std::cout << "Error: SIGCHLD shell process exited" << std::endl;
            exit(1);
        }
    }
}

void createTerminal() {
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

    childpid = 0;
    if ((childpid = fork()) == 0) {
        if (setsid() < 0) {
            std::cout << "setsid failed with " << strerror(errno) << std::endl;
            exit(1);
        }

        int slavefd = 0;
        if ((slavefd = open(slavedev, O_RDWR)) < 0) {
            std::cout << "open failed with " << strerror(errno) << std::endl;
            exit(1);
        }

        if (close(masterfd) < 0) {
            std::cout << "close failed with " << strerror(errno) << std::endl;
            exit(1);
        }

        if (ioctl(slavefd, TIOCSCTTY, 0) < 0) {
            std::cout << "ioctl failed with " << strerror(errno) << std::endl;
            exit(1);
        }

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

        char *const argv[] = {(char*)"bash", (char*)"-l", NULL};

        if (execv("/bin/bash", argv) < 0) {
            std::cout << "execv failed with " << strerror(errno) << std::endl;
            exit(1);
        }

        exit(0);
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
        if (poll(pfds, nfds, 0) < 0) {
            std::cout << "poll failed with " << strerror(errno) << std::endl;
            exit(1);
        }

        if (pfds[0].revents & POLLIN) {
            if (read(masterfd, buff, 1) < 0) {
                std::cout << "read failed with " << strerror(errno) << std::endl;
                exit(1);
            }

            if (write(0, buff, 1) < 0) {
                std::cout << "write failed with " << strerror(errno) << std::endl;
                exit(1);
            }

            fflush(stdout);
        } else if (pfds[1].revents & POLLIN) {
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