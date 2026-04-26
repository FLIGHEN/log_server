#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "daemon.h"
#include "fifo.h"
#include "logging.h"
#include "server.h"
#include "signals.h"

int main(int argc, char **argv)
{
    run_mode_t mode = parse_args(argc, argv);

    if (ensure_fifo(FIFO_PATH) == -1) {
        return EXIT_FAILURE;
    }

    if (mode == FOREGROUND) {
        printf("starting log_server in foreground mode...\n");
    } else {
        printf("starting log_server in daemon mode...\n");
    }

    if (mode == DAEMON) {
        if (daemonize() == -1) {
            cleanup_fifo(FIFO_PATH);
            return EXIT_FAILURE;
        }
    }

    if (setup_logging(mode, LOG_PATH) == -1) {
        cleanup_fifo(FIFO_PATH);
        return EXIT_FAILURE;
    }

    if (setup_signals() == -1) {
        cleanup_fifo(FIFO_PATH);
        return EXIT_FAILURE;
    }

    alarm(ALARM_INTERVAL);

    printf("Log server started successfully.\n");
    printf("FIFO path: %s\n", FIFO_PATH);
    printf("Log path:  %s\n", LOG_PATH);
    printf("Buffer size: %d\n", BUF_SIZE);
    printf("Alarm interval: %d seconds\n", ALARM_INTERVAL);

    int rc = run_fifo_loop(FIFO_PATH);

    cleanup_fifo(FIFO_PATH);

    if (rc == -1) {
        return EXIT_FAILURE;
    }

    return 0;
}
