#ifndef DEBUG_PRINTF_H
#define DEBUG_PRINTF_H


#ifdef DEBUG
#include <stdio.h>
/*
 * DEBUG_PRINTF: prints a debug message that includes the file name,
 * line number, and a custom message.
 *
 * Usage:
 *   DEBUG_PRINTF("Value: %d", value);
 */
#define DEBUG_PRINTF(fmt, ...)                                                 \
  do {                                                                         \
    fprintf(stderr, "[DEBUG] %s:%d: " fmt "\n", __FILE__, __LINE__,            \
            ##__VA_ARGS__);                                                    \
  } while (0)
#else
#define DEBUG_PRINTF(fmt, ...) ((void)0)
#endif

#endif /* DEBUG_PRINTF_H */
