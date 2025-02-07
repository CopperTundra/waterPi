#include "../src/WebClient.h"
#include "MockHttpClient.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <httplib.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;


TEST(ExampleTest, DemonstrateGtestMacros) {
  EXPECT_TRUE(false);
}

TEST(WebClientTest, GetPlantInfoSuccess) {
  WebClient *webClient = new WebClient("config.json", "http://localhost", 1323, 1620);

  // Execute test
  bool result = webClient->getPlantInfo();

  // Verify
  EXPECT_TRUE(result);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}