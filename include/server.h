#ifndef SERVER_H
#define SERVER_H

#include "config.h"

run_mode_t parse_args(int argc, char **argv);
int run_fifo_loop(const char *fifo_path);

#endif
