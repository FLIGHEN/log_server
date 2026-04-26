#ifndef SIGNALS_H
#define SIGNALS_H

#include <signal.h>

extern volatile sig_atomic_t got_sigint;
extern volatile sig_atomic_t got_sigterm;
extern volatile sig_atomic_t got_sigalrm;
extern volatile sig_atomic_t got_sigusr1;
extern volatile sig_atomic_t got_sighup;

int setup_signals(void);

#endif
