#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "fifo.h"

static int fifo_ready = 0;

void cleanup_fifo(const char *path)
{
    if (!fifo_ready) {
        return;
    }

    if (unlink(path) == -1) {
        fprintf(stderr, "unlink(%s) failed: %s\n", path, strerror(errno));
        return;
    }

    fifo_ready = 0;
    printf("FIFO removed: %s\n", path);
}

int ensure_fifo(const char *path)
{
    if (mkfifo(path, 0600) == 0) {
        fifo_ready = 1;
        printf("FIFO created: %s\n", path);
        return 0;
    }

    if (errno != EEXIST) {
        fprintf(stderr, "mkfifo(%s) failed: %s\n", path, strerror(errno));
        return -1;
    }

    struct stat st;

    if (stat(path, &st) == -1) {
        fprintf(stderr, "stat(%s) failed: %s\n", path, strerror(errno));
        return -1;
    }

    if (!S_ISFIFO(st.st_mode)) {
        fprintf(stderr, "%s exists, but it is not FIFO\n", path);
        return -1;
    }

    fifo_ready = 1;
    printf("FIFO already exists, using it: %s\n", path);
    return 0;
}
