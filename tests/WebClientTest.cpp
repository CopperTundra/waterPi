#include "../src/WebClient.h"
#include "MockHttpClient.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <httplib.h>

using ::testing::_;
using ::testing::Return;

TEST(WebClientTest, GetPlantInfoSuccess) {
  MockHttpClient mockClient;
  httplib::Result mockResult;
  mockResult->status = 200;
  mockResult->body =
      R"([{"uid":"1","name":"Plant1","room":"Room1","humidity_threshold":50.0,"watering_time_seconds":30,"sensor_pin_number":1,"valve_pin_number":2}])";

  EXPECT_CALL(mockClient, Get(_, _)).WillOnce(Return(std::move(mockResult)));

  WebClient client(std::string("config.json"), "http://localhost", 1323, 1620, (httplib::Client*)&mockClient);
  bool result = client.getPlantInfo();

  EXPECT_TRUE(result);
  // Add more assertions to verify the behavior
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}