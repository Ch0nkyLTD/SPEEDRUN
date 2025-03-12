/* debug_logger.h
 * A simple debug logger that only gets compiled in if DEBUG is defined.
 * Logs file, line number, and user message.
 */

#ifndef DEBUG_LOGGER_H
#define DEBUG_LOGGER_H

/* Debug log macro - only enabled when DEBUG is defined */
#ifdef DEBUG
#include <stdio.h>
#include <time.h>
#define DEBUG_LOG(fmt, ...)                                                    \
  do {                                                                         \
    time_t now = time(NULL);                                                   \
    char time_buf[20];                                                         \
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S",                  \
             localtime(&now));                                                 \
    fprintf(stderr, "[DEBUG][%s][%s:%d] " fmt "\n", time_buf, __FILE__,        \
            __LINE__, ##__VA_ARGS__);                                          \
  } while (0)
#else
#define DEBUG_LOG(fmt, ...)                                                    \
  do {                                                                         \
  } while (0) /* No-op when DEBUG is not defined */
#endif

#endif /* DEBUG_LOGGER_H */
