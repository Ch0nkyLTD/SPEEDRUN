

// C++ specific declarations (for JSON functionality)
#ifndef NLOHMANN_JSON_HPP
#define NLOHMANN_JSON_HPP
#include "json.hpp"
/**
 * @brief Make an HTTP POST request with JSON data and parse the response as
 * JSON
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
 * @note Caller is responsible for initializing the json_response object
 */
int make_json_http_request(const char *host, const char *port, const char *path,
                           const nlohmann::json &json_body,
                           nlohmann::json *json_response, int timeout_seconds,
                           char *error_message, size_t error_message_size);
#endif // NLOHMANN_JSON_HPP
