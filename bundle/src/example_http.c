#include "debug.h"
#include "http_client.h"
#include <unistd.h>

/**
 * Example usage
 */
int example_usage() {
  // Example request
  const char *headers[] = {"Accept: */*", "User-Agent: SimpleHttpClient/1.0",
                           "Connection: close", NULL};

  HttpRequest request = {
      .verb = "GET",
      .host = "example.com",
      .path = "/",
      .headers = headers,
      .body = NULL,
      .body_size = 0,
      .port = "80",
      .timeout_seconds = 5 // 5 second timeout
  };

  // Make the request
  HttpResponse *response =
      make_http_request(&request, DEFAULT_MAX_RESPONSE_SIZE);
  if (!response) {
    DEBUG_LOG("Failed to get response\n");
    return 1;
  }

  // Print the response
  DEBUG_LOG("Status code: %d\n", response->status_code);

  DEBUG_LOG("Headers:\n");
  for (int i = 0; response->headers[i].key != NULL; i++) {
    DEBUG_LOG("%s: %s\n", response->headers[i].key, response->headers[i].value);
  }

  DEBUG_LOG("Body size: %zu bytes\n", response->body_size);
  size_t out = write(0, response->body, response->body_size);
  // Free the response
  free_http_response(response);
  if (out != -1) {
    return 0;
  }
  return -1;
}

int main() {
  example_usage();
  return 0;
}
