/**
 * example.c
 * Example usage of the FileIterator library
 */

#include "debug.h"
#include "file_iter.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <filename> [chunk_size]\n", argv[0]);
    return 1;
  }

  const char *filename = argv[1];
  size_t chunk_size = 0; // Use default

  // Use custom chunk size if provided
  if (argc >= 3) {
    chunk_size = atoi(argv[2]);
  }

  // Open the file iterator
  FileIterator *iter = file_iterator_open(filename, chunk_size);
  if (!iter) {
    fprintf(stderr, "Failed to open file: %s\n", filename);
    return 1;
  }

  // Print file information
  printf("File: %s\n", filename);
  printf("Size: %zu bytes\n", file_iterator_get_size(iter));
  printf("Chunk size: %zu bytes\n", iter->chunk_size);

  // Allocate buffer for chunks
  void *buffer = malloc(iter->chunk_size);
  if (!buffer) {
    fprintf(stderr, "Failed to allocate memory for buffer\n");
    file_iterator_close(iter);
    return 1;
  }

  // Process file in chunks
  size_t bytes_read;
  size_t chunk_count = 0;
  size_t total_bytes = 0;

  printf("\nProcessing file in chunks...\n");

  while (file_iterator_next_chunk(iter, buffer, &bytes_read)) {
    chunk_count++;
    total_bytes += bytes_read;

    //    printf("Chunk %zu: %zu bytes (%.2f%% of file)\n", chunk_count,
    //    bytes_read, (float)total_bytes / file_iterator_get_size(iter) * 100);

    // Here you would typically process the chunk data
    // For demonstration, we just print the first few bytes as hex
    if (bytes_read > 0) {

#ifdef PRINT_PREFIX
      printf("First 16 bytes (or less): ");
      for (size_t i = 0; i < bytes_read && i < 16; i++) {
        printf("%02X ", ((unsigned char *)buffer)[i]);
      }
      printf("\n");
#endif
      if (write(1, buffer, bytes_read) == -1) {
        DEBUG_LOG("write failed\n");
        break;
      }
    }
  }

  printf("\nProcessed %zu chunks, %zu bytes total\n", chunk_count, total_bytes);

  // Clean up
  free(buffer);
  file_iterator_close(iter);

  return 0;
}
