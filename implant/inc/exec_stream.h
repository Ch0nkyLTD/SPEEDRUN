/**
 * process_stream.h
 * A library for spawning child processes and streaming their output
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef PROCESS_STREAM_H
#define PROCESS_STREAM_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>
/**
 * Debug logging macro
 */

/**
 * Default chunk size (1MB)
 */
#define DEFAULT_CHUNK_SIZE (1024 * 1024)

/**
 * Output stream types
 */
typedef enum { STREAM_STDOUT = 0, STREAM_STDERR = 1, STREAM_MAX } StreamType;

/**
 * ProcessStream structure to hold process information and state
 */
typedef struct {
  pid_t pid;         /* Process ID of the child */
  int stdout_fd;     /* File descriptor for stdout */
  int stderr_fd;     /* File descriptor for stderr */
  size_t chunk_size; /* Size of chunks to yield */
  bool is_running;   /* Flag to track if the process is running */
  int exit_status;   /* Exit status of the child process */
  bool has_exited;   /* Flag to track if the process has exited */
} ProcessStream;

/**
 * Initialize a ProcessStream and spawn the specified command
 * Child process will inherit the parent's environment variables
 *
 * @param command Command to execute (will be passed to /bin/sh -c)
 * @param chunk_size Size of chunks to yield (0 for default)
 * @return Initialized ProcessStream or NULL on failure
 */
ProcessStream *process_stream_spawn(const char *command, size_t chunk_size);

/**
 * Get the next chunk of data from the specified stream
 *
 * @param ps ProcessStream instance
 * @param stream_type Type of stream to read from (STREAM_STDOUT or
 * STREAM_STDERR)
 * @param buffer Buffer to store the chunk (must be at least chunk_size bytes)
 * @param bytes_read Pointer to store the number of bytes read
 * @return true if chunk was read, false if end of stream or error
 */
bool process_stream_read_chunk(ProcessStream *ps, StreamType stream_type,
                               void *buffer, size_t *bytes_read);

/**
 * Check if the process has exited
 *
 * @param ps ProcessStream instance
 * @return true if process has exited, false otherwise
 */
bool process_stream_has_exited(ProcessStream *ps);

/**
 * Get the exit status of the process
 *
 * @param ps ProcessStream instance
 * @param exit_status Pointer to store the exit status
 * @return true if exit status was retrieved, false otherwise
 */
bool process_stream_get_exit_status(ProcessStream *ps, int *exit_status);

/**
 * Terminate the process if it's still running and clean up resources
 *
 * @param ps ProcessStream instance
 * @param force If true, send SIGKILL; otherwise, send SIGTERM
 */
void process_stream_terminate(ProcessStream *ps, bool force);

/**
 * Close the ProcessStream and release resources
 *
 * @param ps ProcessStream instance
 */
void process_stream_close(ProcessStream *ps);

#endif /* PROCESS_STREAM_H */

#ifdef __cplusplus
}
#endif
