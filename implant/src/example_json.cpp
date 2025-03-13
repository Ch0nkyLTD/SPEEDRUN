/**
 * Example program demonstrating the use of the HTTP client with JSON
 * functionality
 */

#include "debug.h"
#include "http_client.h"
#include "json.hpp"
#include "json_http.hpp"
#include <iostream>
#include <string>

using json = nlohmann::json;

int main() {
  std::cout << "Fetching IP information from ipinfo.io using JSON API...\n"
            << std::endl;

  // Initialize response JSON object
  json response_json;

  // Buffer for error messages

  // Make the JSON HTTP request
  int status_code =
      make_json_http_request("127.0.0.1", // host
                             "1234",      // port
                             "/", // path - using the explicit JSON endpoint
                             json::object(), // empty JSON object  sends {}
                             &response_json, // response JSON object
                             10              // timeout in seconds
      );

  if (status_code < 0) {
    DEBUG_LOG("Failed\n");
    return 1;
  }

  std::cout << "HTTP Status Code: " << status_code << std::endl;

  if (status_code >= 200 && status_code < 300) {
    // Print the full JSON response with pretty formatting
    std::cout << "\nFull JSON Response:" << std::endl;
    std::cout << "-------------------" << std::endl;
    std::cout << response_json.dump(2) << std::endl; // Indent with 2 spaces
  }

  std::cout << "\nJSON HTTP request completed" << std::endl;

  return 0;
}
