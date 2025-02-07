
#include <httplib.h>
#include <string>

void configureMockServer(httplib::Server &svr) 
{
  svr.Get("/hi", [](const httplib::Request &req, httplib::Response &res) {
    res.set_content("Bonjour", "text/plain");
  });
  svr.Get("/plants", [](const httplib::Request &req, httplib::Response &res) {
    res.set_content(
        "[{\"uid\":\"123456\",\"name\":\"Test Plant "
        "1\",\"room\":\"Living "
        "Room\",\"humidity_threshold\":50.0,\"watering_time_seconds\":30,"
        "current_humidity\":45.0,"
        "\"sensor_pin_number\":1,\"valve_pin_number\":2}]",
        "application/json");
  });
  svr.Post("/plants/123456", [](const httplib::Request &req, httplib::Response &res) {
    // Get the humidity value from the request body
    std::string body = req.body;
    std::string humidity = body.substr(body.find("current_humidity") + 10, 4);
    res.set_content("Received humidity value: " + humidity, "text/plain");
  });
  svr.Post("/webhooks", [](const httplib::Request &req, httplib::Response &res) {
    std::string body = req.body;
    std::string event = body.substr(body.find("event") + 8, 4);
    std::string url = body.substr(body.find("url") + 6, 4);
    std::string response = "Webhook request received: \r\n";
    if (event == "new_plant") {
      response = "New plant webhook received";
    } else if (event == "watering_request") {
      response = "Watering request webhook received";
    } else if (event == "delete_plant") {
      response = "Delete plant webhook received";
    } else if (event == "patch_plant") {
      response = "Patch plant webhook received";
    } else if (event == "stop_watering_request") {
      response = "Stop watering request webhook received";
    }
    else {
      response = "Unknown event received";
    }
    response += "\r\nURL: " + url;
    res.set_content(response, "text/plain");
  });
}

int main(int argc, char **argv)
{
    httplib::Server svr;
    configureMockServer(svr);

    std::cout << "Starting server on localhost:1323...\r\n";
    svr.listen("localhost", 1323);
    return 0;
}

