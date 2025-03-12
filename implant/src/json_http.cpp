
#include "debug.h"
#include "http_client.h"
#include "json.hpp"
#include <string>

/**
 * Make an HTTP POST request with JSON data and parse the response as JSON
 *
 * @param host The target host
 * @param path The target path
 * @param json_body Reference to the JSON object to be sent
 * @param json_response Pointer to store the parsed JSON response
 * @param timeout_seconds Timeout in seconds (0 for default)
 * @param error_message Buffer to store error message (can be NULL)
 * @param error_message_size Size of the error message buffer
 *
 * @return HTTP status code, or -1 on error
 *
 * Note: Caller is responsible for initializing the json_response object
 */
int make_json_http_request(const char *host, const char *port, const char *path,
                           const nlohmann::json &json_body,
                           nlohmann::json *json_response, int timeout_seconds,
                           char *error_message, size_t error_message_size) {
  if (!host || !path || !json_response) {
    if (error_message && error_message_size > 0) {
      strncpy(error_message, "Invalid parameters", error_message_size - 1);
      error_message[error_message_size - 1] = '\0';
    }
    DEBUG_LOG("Invalid parameters for make_json_http_request\n");
    return -1;
  }

  int status_code = -1;
  HttpResponse *response = NULL;

  // Convert JSON to string without exceptions using error handler
  std::string body_str =
      json_body.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace);

  // Check if serialization was successful
  if (body_str.empty() && !json_body.empty()) {
    if (error_message && error_message_size > 0) {
      strncpy(error_message, "Failed to serialize JSON",
              error_message_size - 1);
      error_message[error_message_size - 1] = '\0';
    }
    DEBUG_LOG("Failed to serialize JSON object\n");
    return -1;
  }

  // Create headers with Content-Type: application/json
  const char *headers[] = {"Content-Type: application/json",
                           "Accept: application/json", NULL};

  // Create request
  HttpRequest request = {.verb = "POST",
                         .host = host,
                         .path = path,
                         .headers = headers,
                         .body = body_str.c_str(),
                         .body_size = body_str.length(),
                         .port = port, // Default port
                         .timeout_seconds = timeout_seconds};

  // Make request
  response = make_http_request(&request, 0); // Use default max response size

  if (!response) {
    if (error_message && error_message_size > 0) {
      strncpy(error_message, "Failed to make HTTP request",
              error_message_size - 1);
      error_message[error_message_size - 1] = '\0';
    }
    DEBUG_LOG("Failed to make HTTP request to %s%s\n", host, path);
    return -1;
  }

  // Save status code
  status_code = response->status_code;

  // Parse response body as JSON if there is a body
  if (response->body && response->body_size > 0) {
    // Get response as string
    std::string json_str(static_cast<const char *>(response->body),
                         response->body_size);

    // Check for valid JSON format before parsing
    if (json_str.empty() || (json_str[0] != '{' && json_str[0] != '[')) {
      free_http_response(response);

      if (error_message && error_message_size > 0) {
        strncpy(error_message, "Response is not valid JSON",
                error_message_size - 1);
        error_message[error_message_size - 1] = '\0';
      }
      // print part of string
      DEBUG_LOG("Response is not valid JSON: '%.*s'\n",
                (int)std::min(json_str.length(), (size_t)100),
                json_str.c_str());
      return -1;
    }

    // Parse without exceptions
    *json_response = nlohmann::json::parse(json_str, nullptr, false);

    if (json_response->is_discarded()) {
      // JSON parsing failed
      if (error_message && error_message_size > 0) {
        strncpy(error_message, "Failed to parse JSON response",
                error_message_size - 1);
        error_message[error_message_size - 1] = '\0';
      }
      DEBUG_LOG("Failed to parse JSON response\n");

      free_http_response(response);
      return -1;
    }
  } else {
    // No body or empty body, create an empty JSON object
    *json_response = nlohmann::json::object();
  }

  DEBUG_LOG("JSON HTTP request successful, status code: %d\n", status_code);

  // Free the HTTP response
  free_http_response(response);

  return status_code;
}
