#ifndef CONFIG_H
#define CONFIG_H

#define FIFO_PATH "/tmp/log_server_fifo"
#define LOG_PATH "/tmp/log_server.log"

#define BUF_SIZE 1024
#define ALARM_INTERVAL 15

typedef enum {
    FOREGROUND,
    DAEMON
} run_mode_t;

#endif
