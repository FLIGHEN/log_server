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
        perror("fork failed");
        return -1;
    }

    if (pid > 0) {
        _exit(0);
    }

    if (setsid() == -1) {
        perror("setsid failed");
        return -1;
    }

    close(STDIN_FILENO);
    is_daemon = 1;

    return 0;
}

int daemon_is_active(void)
{
    return is_daemon;
}
