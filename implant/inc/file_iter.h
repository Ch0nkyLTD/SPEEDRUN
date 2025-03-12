
/**
 * file_iterator.h
 * A simple library for reading files in chunks using memory mapping
 */

#ifndef FILE_ITERATOR_H
#define FILE_ITERATOR_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

/**
 * Default chunk size (1MB)
 */
#define DEFAULT_CHUNK_SIZE (1024 * 1024)

/**
 * FileIterator structure to hold file information and state
 */
typedef struct {
  int fd;                /* File descriptor */
  void *mapped_data;     /* Pointer to the memory mapped region */
  size_t file_size;      /* Total size of the file */
  size_t current_offset; /* Current position in the file */
  size_t chunk_size;     /* Size of chunks to yield */
  bool is_open;          /* Flag to track if the iterator is open */
} FileIterator;

/**
 * Initialize a FileIterator for the given file path
 *
 * @param path Path to the file to open
 * @param chunk_size Size of chunks to yield (0 for default)
 * @return Initialized FileIterator or NULL on failure
 */
FileIterator *file_iterator_open(const char *path, size_t chunk_size);

/**
 * Get the next chunk of data from the file
 *
 * @param iter FileIterator instance
 * @param buffer Buffer to store the chunk (must be at least chunk_size bytes)
 * @param bytes_read Pointer to store the number of bytes read
 * @return true if chunk was read, false if end of file or error
 */
bool file_iterator_next_chunk(FileIterator *iter, void *buffer,
                              size_t *bytes_read);

/**
 * Close the FileIterator and release resources
 *
 * @param iter FileIterator instance
 */
void file_iterator_close(FileIterator *iter);

/**
 * Get the total size of the file
 *
 * @param iter FileIterator instance
 * @return Size of the file in bytes
 */
size_t file_iterator_get_size(const FileIterator *iter);

/**
 * Get the current position in the file
 *
 * @param iter FileIterator instance
 * @return Current offset in bytes
 */
size_t file_iterator_get_position(const FileIterator *iter);

/**
 * Reset the iterator to the beginning of the file
 *
 * @param iter FileIterator instance
 * @return true if reset was successful, false otherwise
 */
bool file_iterator_reset(FileIterator *iter);

#endif /* FILE_ITERATOR_H */

#ifdef __cplusplus
}
#endif
