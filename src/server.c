#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "daemon.h"
#include "logging.h"
#include "server.h"
#include "signals.h"
#include "stats.h"

static void usage(const char *progname)
{
    fprintf(stderr, "Usage: %s [--foreground | --daemon]\n", progname);
}

run_mode_t parse_args(int argc, char **argv)
{
    if (argc == 1) {
        return FOREGROUND;
    }

    if (argc == 2) {
        if (strcmp(argv[1], "--foreground") == 0) {
            return FOREGROUND;
        }

        if (strcmp(argv[1], "--daemon") == 0) {
            return DAEMON;
        }
    }

    usage(argv[0]);
    exit(EXIT_FAILURE);
}

static int handle_open_signals(void)
{
    if (got_sigterm) {
        got_sigterm = 0;
        printf("SIGTERM received while waiting for writer, exiting immediately\n");
        stats_print();
        return 1;
    }

    if (got_sigint) {
        got_sigint = 0;
        printf("SIGINT received while waiting for writer, exiting\n");
        stats_print();
        return 1;
    }

    if (got_sigalrm) {
        got_sigalrm = 0;
        stats_add_alarm();
        printf("server is alive, waiting for data\n");
        fflush(stdout);
        alarm(ALARM_INTERVAL);
    }

    if (got_sigusr1) {
        got_sigusr1 = 0;
        printf("SIGUSR1 received (stats request)\n");
        stats_print();
    }

    if (got_sighup) {
        got_sighup = 0;

        if (daemon_is_active()) {
            printf("SIGHUP received, already running as daemon\n");
            stats_print();
            return 0;
        }

        printf("SIGHUP received, daemonizing...\n");

        if (daemonize() == -1) {
            fprintf(stderr, "daemonize failed\n");
            return -1;
        }

        if (setup_logging(DAEMON, LOG_PATH) == -1) {
            fprintf(stderr, "setup_logging after daemonize failed\n");
            return -1;
        }

        printf("daemonization complete\n");
        stats_print();
    }

    return 0;
}

static int handle_read_signals(int fd, int *exit_after_current_writer)
{
    if (got_sigterm) {
        got_sigterm = 0;
        printf("SIGTERM received while reading, exiting immediately\n");
        stats_print();
        close(fd);
        return 1;
    }

    if (got_sigint) {
        got_sigint = 0;
        printf("SIGINT received, will finish current writer before exiting\n");
        *exit_after_current_writer = 1;
        return 0;
    }

    if (got_sigalrm) {
        got_sigalrm = 0;
        stats_add_alarm();
        printf("server is alive while reading\n");
        fflush(stdout);
        alarm(ALARM_INTERVAL);
    }

    if (got_sigusr1) {
        got_sigusr1 = 0;
        printf("SIGUSR1 received (stats request)\n");
        stats_print();
    }

    if (got_sighup) {
        got_sighup = 0;

        if (daemon_is_active()) {
            printf("SIGHUP received, already running as daemon\n");
            stats_print();
            return 0;
        }

        printf("SIGHUP received, daemonizing...\n");

        if (daemonize() == -1) {
            fprintf(stderr, "daemonize failed\n");
            close(fd);
            return -1;
        }

        if (setup_logging(DAEMON, LOG_PATH) == -1) {
            fprintf(stderr, "setup_logging after daemonize failed\n");
            close(fd);
            return -1;
        }

        printf("daemonization complete\n");
        stats_print();
    }

    return 0;
}

int run_fifo_loop(const char *fifo_path)
{
    char buf[BUF_SIZE];
    int exit_after_current_writer = 0;

    while (1) {
        int action = handle_open_signals();

        if (action != 0) {
            return action > 0 ? 0 : -1;
        }

        printf("waiting for writer...\n");
        fflush(stdout);

        int fd = open(fifo_path, O_RDONLY);

        if (fd == -1) {
            if (errno == EINTR) {
                action = handle_open_signals();

                if (action != 0) {
                    return action > 0 ? 0 : -1;
                }

                continue;
            }

            fprintf(stderr, "open(%s) failed: %s\n", fifo_path, strerror(errno));
            return -1;
        }

        printf("writer connected\n");

        while (1) {
            action = handle_read_signals(fd, &exit_after_current_writer);

            if (action != 0) {
                return action > 0 ? 0 : -1;
            }

            ssize_t n = read(fd, buf, BUF_SIZE - 1);

            if (n > 0) {
                stats_add_bytes((unsigned long long)n);

                buf[n] = '\0';
                printf("%s", buf);

                if (buf[n - 1] != '\n') {
                    printf("\n");
                }

                fflush(stdout);
                continue;
            }

            if (n == 0) {
                break;
            }

            if (errno == EINTR) {
                action = handle_read_signals(fd, &exit_after_current_writer);

                if (action != 0) {
                    return action > 0 ? 0 : -1;
                }

                continue;
            }

            fprintf(stderr, "read(%s) failed: %s\n", fifo_path, strerror(errno));
            close(fd);
            return -1;
        }

        printf("writer disconnected\n");
        close(fd);
        stats_add_message();

        if (exit_after_current_writer) {
            printf("exiting after SIGINT\n");
            stats_print();
            return 0;
        }
    }

    return 0;
}
