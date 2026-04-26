#include <stdio.h>

#include "stats.h"

stats_t stats = {0, 0, 0};

void stats_print(void)
{
    printf("\n=== statistics ===\n");
    printf("messages: %lu\n", stats.messages);
    printf("bytes: %llu\n", stats.bytes);
    printf("alarms: %lu\n", stats.alarms);
    printf("==================\n");
    fflush(stdout);
}

void stats_add_message(void)
{
    stats.messages++;
}

void stats_add_bytes(unsigned long long bytes)
{
    stats.bytes += bytes;
}

void stats_add_alarm(void)
{
    stats.alarms++;
}
