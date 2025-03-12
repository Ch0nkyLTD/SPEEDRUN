# libspeedrun

## http_client API Reference

### Making HTTP Requests
- `make_http_request(const HttpRequest *request, size_t max_response_size)`
  - Caller is responsible for freeing the returned `HttpResponse` using `free_http_response`.

### Managing Responses
- `free_http_response(HttpResponse *response)`
  - Frees all memory associated with the given `HttpResponse`.

### Structures

#### HttpRequest
- `const char *verb`
- `const char *host`
- `const char *path`
- `const char **headers`
- `const void *body`
- `size_t body_size`
- `const char *port`
- `int timeout_seconds`

#### HttpHeader
- `char *key`
- `char *value`

#### HttpResponse
- `int status_code`
- `HttpHeader *headers`
- `void *body`
- `size_t body_size`

## file_iter API Reference

### Opening Files
- `file_iterator_open(const char *path, size_t chunk_size)`
  - Initializes a new `FileIterator` instance. Caller must call `file_iterator_close` to release resources.

### Reading File Chunks
- `file_iterator_next_chunk(FileIterator *iter, void *buffer, size_t *bytes_read)`
  - Reads the next chunk from the file into the provided buffer.

### Iterator Management
- `file_iterator_close(FileIterator *iter)`
  - Closes the iterator and releases all associated resources.
- `file_iterator_reset(FileIterator *iter)`
  - Resets the iterator position back to the beginning of the file.

### File Information
- `file_iterator_get_size(const FileIterator *iter)`
  - Retrieves the total size of the file in bytes.
- `file_iterator_get_position(const FileIterator *iter)`
  - Retrieves the current offset position in bytes.

### Structures

#### FileIterator
- `int fd`
- `void *mapped_data`
- `size_t file_size`
- `size_t current_offset`
- `size_t chunk_size`
- `bool is_open`


## exec_stream API Reference

### Spawning Processes
- `process_stream_spawn(const char *command, size_t chunk_size)`
  - Initializes a new `ProcessStream` instance. Caller must call `process_stream_close` to release resources.

### Reading Output Streams
- `process_stream_read_chunk(ProcessStream *ps, StreamType stream_type, void *buffer, size_t *bytes_read)`
  - Reads the next chunk of data from the specified stream (stdout or stderr).

### Process Lifecycle Management
- `process_stream_has_exited(ProcessStream *ps)`
  - Checks if the child process has exited.
- `process_stream_get_exit_status(ProcessStream *ps, int *exit_status)`
  - Retrieves the exit status of the child process if it has exited.
- `process_stream_terminate(ProcessStream *ps, bool force)`
  - Terminates the child process. If `force` is true, sends `SIGKILL`; otherwise, sends `SIGTERM`.

### Cleanup
- `process_stream_close(ProcessStream *ps)`
  - Closes the stream and releases all associated resources.

### Structures

#### ProcessStream
- `pid_t pid`
- `int stdout_fd`
- `int stderr_fd`
- `size_t chunk_size`
- `bool is_running`
- `int exit_status`
- `bool has_exited`

### Enums

#### StreamType
- `STREAM_STDOUT`
- `STREAM_STDERR`


