// @TODO: implant
#include "debug.h"
#include "json.hpp"

using json = nlohmann::json;

void show_json_demo() {

  json out;
  out["foobar"] = 13;
  DEBUG_PRINTF("json: %s", out.dump().c_str());
  char val[] = "{\"foo\":123}";
  json data = json::parse(val);

  DEBUG_PRINTF("json: %s", data.dump().c_str());
}

int main() {
  DEBUG_PRINTF("Starting implant...\n");
  show_json_demo();
  DEBUG_PRINTF("Implant shutting down...\n");
  return 0;
}
