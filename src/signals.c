#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "signals.h"

volatile sig_atomic_t got_sigint = 0;
volatile sig_atomic_t got_sigterm = 0;
volatile sig_atomic_t got_sigalrm = 0;
volatile sig_atomic_t got_sigusr1 = 0;
volatile sig_atomic_t got_sighup = 0;

static void signal_handler(int signo)
{
    if (signo == SIGINT) {
        got_sigint = 1;
    } else if (signo == SIGTERM) {
        got_sigterm = 1;
    } else if (signo == SIGALRM) {
        got_sigalrm = 1;
    } else if (signo == SIGUSR1) {
        got_sigusr1 = 1;
    } else if (signo == SIGHUP) {
        got_sighup = 1;
    }
}

int setup_signals(void)
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;

    if (sigemptyset(&sa.sa_mask) == -1) {
        fprintf(stderr, "sigemptyset failed: %s\n", strerror(errno));
        return -1;
    }

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        fprintf(stderr, "sigaction(SIGINT) failed: %s\n", strerror(errno));
        return -1;
    }

    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        fprintf(stderr, "sigaction(SIGTERM) failed: %s\n", strerror(errno));
        return -1;
    }

    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        fprintf(stderr, "sigaction(SIGALRM) failed: %s\n", strerror(errno));
        return -1;
    }

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        fprintf(stderr, "sigaction(SIGUSR1) failed: %s\n", strerror(errno));
        return -1;
    }

    if (sigaction(SIGHUP, &sa, NULL) == -1) {
        fprintf(stderr, "sigaction(SIGHUP) failed: %s\n", strerror(errno));
        return -1;
    }

    struct sigaction ignore_sa;

    memset(&ignore_sa, 0, sizeof(ignore_sa));
    ignore_sa.sa_handler = SIG_IGN;

    if (sigemptyset(&ignore_sa.sa_mask) == -1) {
        fprintf(stderr, "sigemptyset failed: %s\n", strerror(errno));
        return -1;
    }

    if (sigaction(SIGQUIT, &ignore_sa, NULL) == -1) {
        fprintf(stderr, "sigaction(SIGQUIT) failed: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}
