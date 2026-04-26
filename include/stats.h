#ifndef STATS_H
#define STATS_H

typedef struct {
    unsigned long messages;
    unsigned long long bytes;
    unsigned long alarms;
} stats_t;

extern stats_t stats;

void stats_print(void);
void stats_add_message(void);
void stats_add_bytes(unsigned long long bytes);
void stats_add_alarm(void);

#endif
