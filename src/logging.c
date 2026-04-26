#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "logging.h"

int setup_logging(run_mode_t mode, const char *log_path)
{
    if (mode == FOREGROUND) {
        printf("logging to stdout\n");
        return 0;
    }

    int fd = open(log_path, O_WRONLY | O_CREAT | O_APPEND, 0600);
    if (fd == -1) {
        fprintf(stderr, "open(%s) failed: %s\n", log_path, strerror(errno));
        return -1;
    }

    if (dup2(fd, STDOUT_FILENO) == -1) {
        fprintf(stderr, "dup2(stdout) failed: %s\n", strerror(errno));
        close(fd);
        return -1;
    }

    if (dup2(fd, STDERR_FILENO) == -1) {
        fprintf(stderr, "dup2(stderr) failed: %s\n", strerror(errno));
        close(fd);
        return -1;
    }

    close(fd);

    printf("logging to file: %s\n", log_path);
    return 0;
}
