#ifndef MOCKWEBCLIENT_H
#define MOCKWEBCLIENT_H

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "../src/GlobalVars.h"
#include "../src/Plant.h"
#include "../src/WebClient.h"

class MockWebClient {
public:
  struct PlantPayload {
    std::string uid;
    std::string name;
    std::string room;
    float humidity_threshold = 0.0f;
    uint16_t watering_time_seconds = 0;
    float current_humidity = 0.0f;
    uint8_t sensor_pin_number = 0;
    uint8_t valve_pin_number = 0;
  };

  struct PostedPlantValue {
    std::string uid;
    float current_humidity = 0.0f;
  };

  MockWebClient(std::string configPath);
  MockWebClient(std::string configPath, const char *url);
  MockWebClient(std::string configPath, const char *url, uint16_t portDest,
                uint16_t portSrc);
  ~MockWebClient();

  void spawnThread();
  void stop();
  bool isAlive() const;

  bool getPlantInfo();
  bool postValues();
  bool postWebhookEvents();

  bool handlePostPlantInfo(const PlantPayload &payload,
                           std::string *error_message = nullptr);
  bool handlePatchPlantInfo(const std::string &uid, const PlantPayload &payload,
                            std::string *error_message = nullptr);
  bool handleDeletePlantInfo(const std::string &uid,
                             std::string *error_message = nullptr);
  bool handleTriggerWater(const std::string &uid,
                          std::optional<uint16_t> seconds,
                          std::string *error_message = nullptr);
  bool handleStopWater(const std::string &uid,
                       std::string *error_message = nullptr);

  void setServerPlants(const std::vector<PlantPayload> &plants);
  void clearServerPlants();
  void setPlantHumidity(const std::string &uid, float humidity);
  void setWebhookEventsResult(bool success);
  void setWebhookEventsToPost(const std::vector<std::string> &events);
  void resetCapturedRequests();

  const std::vector<PostedPlantValue> &postedPlantValues() const;
  const std::vector<std::string> &postedWebhookEvents() const;

private:
  std::string _url;
  uint16_t _portDest = 0;
  uint16_t _portSrc = 0;
  std::string _configPath;
  bool _alive = false;

  std::vector<PlantPayload> _serverPlants;
  std::unordered_map<std::string, float> _plantHumidity;
  std::vector<PostedPlantValue> _postedPlantValues;
  std::vector<std::string> _postedWebhookEvents;
  std::vector<std::string> _eventsToPost;
  bool _webhookEventsResult = true;

  Plant *findPlantByUid(const std::string &uid);
  const PlantPayload *findServerPlantByUid(const std::string &uid) const;
};

#endif
