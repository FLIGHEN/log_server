#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "daemon.h"

static int is_daemon = 0;

int daemonize(void)
{
    if (is_daemon) {
        return 0;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("first fork failed");
        return -1;
    }

    if (pid > 0) {
        _exit(0);
    }

    if (setsid() == -1) {
        perror("setsid failed");
        return -1;
    }

    pid = fork();

    if (pid < 0) {
        perror("second fork failed");
        return -1;
    }

    if (pid > 0) {
        _exit(0);
    }

    if (chdir("/") == -1) {
        perror("chdir failed");
        return -1;
    }

    int devnull = open("/dev/null", O_RDONLY);

    if (devnull == -1) {
        perror("open(/dev/null) failed");
        return -1;
    }

    if (dup2(devnull, STDIN_FILENO) == -1) {
        perror("dup2(stdin) failed");
        close(devnull);
        return -1;
    }

    close(devnull);

    is_daemon = 1;

    return 0;
}

int daemon_is_active(void)
{
    return is_daemon;
}
