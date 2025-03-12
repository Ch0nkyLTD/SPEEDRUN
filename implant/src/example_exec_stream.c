/**
 * process_example.c
 * Example usage of the ProcessStream library
 */

#include "exec_stream.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_CHUNKS 50000
// Helper function to print chunk data with stream info
void print_chunk(const char *stream_name, const void *data, size_t size) {
  printf("=== %s (%zu bytes) ===\n", stream_name, size);

  // Print the chunk content (limited to printable ASCII)
  const unsigned char *bytes = (const unsigned char *)data;
  for (size_t i = 0; i < size && i < 100; i++) {
    if (bytes[i] >= 32 && bytes[i] <= 126) {
      putchar(bytes[i]);
    } else if (bytes[i] == '\n') {
      putchar('\n');
    } else {
      putchar('.');
    }
  }

  if (size > 100) {
    printf("\n... [%zu more bytes] ...", size - 100);
  }

  printf("\n\n");
}

int main(int argc, char *argv[]) {
  // Example of setting an environment variable in the parent process
  // that will be inherited by the child process
  setenv("EXAMPLE_VAR", "This is inherited by the child process", 1);
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <command> [chunk_size]\n", argv[0]);
    return 1;
  }

  const char *command = argv[1];
  size_t chunk_size = 0; // Use default

  // Use custom chunk size if provided
  if (argc >= 3) {
    chunk_size = atoi(argv[2]);
  }

  printf("Spawning command: %s\n", command);
  printf("Using chunk size: %zu bytes\n",
         chunk_size > 0 ? chunk_size : DEFAULT_CHUNK_SIZE);
  printf("Environment variables will be inherited from parent process\n");

  // Spawn the process
  ProcessStream *ps = process_stream_spawn(command, chunk_size);
  if (!ps) {
    fprintf(stderr, "Failed to spawn process\n");
    return 1;
  }

  // Allocate buffers for stdout and stderr
  void *stdout_buffer = malloc(ps->chunk_size);
  void *stderr_buffer = malloc(ps->chunk_size);

  if (!stdout_buffer || !stderr_buffer) {
    fprintf(stderr, "Failed to allocate memory for buffers\n");
    process_stream_close(ps);
    free(stdout_buffer);
    free(stderr_buffer);
    return 1;
  }

  // Read chunks until the process exits and all output is consumed
  size_t stdout_bytes, stderr_bytes;
  size_t stdout_total = 0, stderr_total = 0;
  bool stdout_open = true, stderr_open = true;
  int iterations = 0;

  printf("\nReading process output...\n\n");

  while (stdout_open || stderr_open) {
    iterations++;

    // Try to read from stdout
    if (stdout_open) {
      if (process_stream_read_chunk(ps, STREAM_STDOUT, stdout_buffer,
                                    &stdout_bytes)) {
        if (stdout_bytes > 0) {
          stdout_total += stdout_bytes;
          print_chunk("STDOUT", stdout_buffer, stdout_bytes);
        }
      } else {
        stdout_open = false;
        printf("STDOUT stream closed\n");
      }
    }

    // Try to read from stderr
    if (stderr_open) {
      if (process_stream_read_chunk(ps, STREAM_STDERR, stderr_buffer,
                                    &stderr_bytes)) {
        if (stderr_bytes > 0) {
          stderr_total += stderr_bytes;
          print_chunk("STDERR", stderr_buffer, stderr_bytes);
        }
      } else {
        stderr_open = false;
        printf("STDERR stream closed\n");
      }
    }

    // If no data was received but streams are still open, sleep a bit
    if ((stdout_open && stdout_bytes == 0) ||
        (stderr_open && stderr_bytes == 0)) {
      usleep(10000); // 10ms
    }

    // Check if process has exited
    if (process_stream_has_exited(ps)) {
      int exit_status;
      if (process_stream_get_exit_status(ps, &exit_status)) {
        printf("Process has exited with status: %d\n", exit_status);
      }
    }

    // Cap number of iterations
    if (iterations > MAX_CHUNKS) {
      printf("Too many iterations, breaking loop\n");
      break;
    }
  }

  printf("\nProcess output complete\n");
  printf("Total bytes read: STDOUT=%zu, STDERR=%zu\n", stdout_total,
         stderr_total);

  // Check final exit status
  int exit_status;
  if (process_stream_get_exit_status(ps, &exit_status)) {
    printf("Final exit status: %d\n", exit_status);
  }

  // Clean up
  free(stdout_buffer);
  free(stderr_buffer);
  process_stream_close(ps);

  return 0;
}
