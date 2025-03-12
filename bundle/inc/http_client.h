/**
 * @file http_client.h
 * @brief Simple HTTP client implementation using sockets
 */

#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Default timeout in seconds for HTTP operations
 */
#define DEFAULT_TIMEOUT_SECONDS 5

/**
 * @brief Default maximum response size (5 MB)
 */
#define DEFAULT_MAX_RESPONSE_SIZE (5 * 1024 * 1024)

/**
 * @brief HTTP request structure
 */
typedef struct {
  const char *verb;     /**< HTTP method (GET, POST, etc.) */
  const char *host;     /**< Host to connect to */
  const char *path;     /**< Path of the resource */
  const char **headers; /**< Null-terminated array of headers */
  const void *body;     /**< Request body */
  size_t body_size;     /**< Size of the request body */
  const char *port;     /**< Port (optional, defaults to 80) */
  int timeout_seconds;  /**< Timeout in seconds (0 for default) */
} HttpRequest;

/**
 * @brief HTTP header key-value pair
 */
typedef struct {
  char *key;   /**< Header name */
  char *value; /**< Header value */
} HttpHeader;

/**
 * @brief HTTP response structure
 */
typedef struct {
  int status_code;     /**< HTTP status code */
  HttpHeader *headers; /**< Null-terminated array of headers */
  void *body;          /**< Response body */
  size_t body_size;    /**< Size of the response body */
} HttpResponse;

/**
 * @brief Make an HTTP request
 *
 * @param request The HTTP request configuration
 * @param max_response_size Maximum size of response to accept (0 for default)
 * @return HttpResponse* Response object (must be freed with free_http_response)
 * or NULL on error
 */
HttpResponse *make_http_request(const HttpRequest *request,
                                size_t max_response_size);

/**
 * @brief Free an HTTP response and all associated memory
 *
 * @param response The response to free
 */
void free_http_response(HttpResponse *response);

#ifdef __cplusplus
} // extern "C"

#endif // __cplusplus

#endif // HTTP_CLIENT_H
