#ifndef FIFO_H
#define FIFO_H

int ensure_fifo(const char *path);
void cleanup_fifo(const char *path);

#endif
