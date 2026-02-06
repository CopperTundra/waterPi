#include "MockWebClient.h"

#include <algorithm>
#include <utility>

#include "../src/Valve.h"
#include "../src/sensor/DHT22.h"

MockWebClient::MockWebClient(std::string configPath)
    : _portDest(1323), _portSrc(1620), _configPath(std::move(configPath)) {
  _url = std::string("http://localhost") + ":" + std::to_string(_portDest);
  _eventsToPost.assign(WebhookEvent::all_events.begin(),
                       WebhookEvent::all_events.end());
}

MockWebClient::MockWebClient(std::string configPath, const char *url)
    : _portDest(1323), _portSrc(1620), _configPath(std::move(configPath)) {
  _url = std::string(url) + ":" + std::to_string(_portDest);
  _eventsToPost.assign(WebhookEvent::all_events.begin(),
                       WebhookEvent::all_events.end());
}

MockWebClient::MockWebClient(std::string configPath, const char *url,
                             uint16_t portDest, uint16_t portSrc)
    : _portDest(portDest), _portSrc(portSrc),
      _configPath(std::move(configPath)) {
  _url = std::string(url) + ":" + std::to_string(_portDest);
  _eventsToPost.assign(WebhookEvent::all_events.begin(),
                       WebhookEvent::all_events.end());
}

MockWebClient::~MockWebClient() {
  stop();
}

void MockWebClient::spawnThread() {
  _alive = true;
}

void MockWebClient::stop() {
  _alive = false;
}

bool MockWebClient::isAlive() const {
  return _alive;
}

bool MockWebClient::getPlantInfo() {
  std::unique_lock<std::mutex> lock(mutex_plants);
  for (const auto &plant : _serverPlants) {
    Plant *existing = findPlantByUid(plant.uid);
    if (existing) {
      existing->setName(plant.name);
      existing->setRoom(plant.room);
      existing->setHumidityThreshold(plant.humidity_threshold);
      existing->setWateringTime(plant.watering_time_seconds);
      existing->setSensorPin(plant.sensor_pin_number);
      existing->setValvePin(plant.valve_pin_number);
      continue;
    }

    auto *sensor = new DHT22(plant.sensor_pin_number);
    auto *valve = new Valve(ValveType::solenoid, plant.valve_pin_number);
    auto *p = new Plant(plant.name, plant.uid, plant.room, sensor, valve,
                        plant.watering_time_seconds, plant.humidity_threshold);
    plants.push_back(p);
  }

  return true;
}

bool MockWebClient::postValues() {
  std::unique_lock<std::mutex> lock(mutex_plants);
  if (plants.empty()) {
    return false;
  }

  for (auto *plant : plants) {
    float humidity = 0.0f;
    auto it = _plantHumidity.find(plant->getUid());
    if (it != _plantHumidity.end()) {
      humidity = it->second;
    } else {
      const PlantPayload *fallback = findServerPlantByUid(plant->getUid());
      if (fallback) {
        humidity = fallback->current_humidity;
      }
    }

    _postedPlantValues.push_back({plant->getUid(), humidity});
  }

  return true;
}

bool MockWebClient::postWebhookEvents() {
  _postedWebhookEvents.insert(_postedWebhookEvents.end(), _eventsToPost.begin(),
                              _eventsToPost.end());
  return _webhookEventsResult;
}

bool MockWebClient::handlePostPlantInfo(const PlantPayload &payload,
                                        std::string *error_message) {
  std::unique_lock<std::mutex> lock(mutex_plants);
  if (findPlantByUid(payload.uid)) {
    if (error_message) {
      *error_message = "Plant already exists";
    }
    return false;
  }

  auto *sensor = new DHT22(payload.sensor_pin_number);
  auto *valve = new Valve(ValveType::solenoid, payload.valve_pin_number);
  auto *p = new Plant(payload.name, payload.uid, payload.room, sensor, valve,
                      payload.watering_time_seconds,
                      payload.humidity_threshold);
  plants.push_back(p);
  return true;
}

bool MockWebClient::handlePatchPlantInfo(const std::string &uid,
                                         const PlantPayload &payload,
                                         std::string *error_message) {
  std::unique_lock<std::mutex> lock(mutex_plants);
  Plant *plant = findPlantByUid(uid);
  if (!plant) {
    if (error_message) {
      *error_message = "Plant not found";
    }
    return false;
  }

  plant->setName(payload.name);
  plant->setRoom(payload.room);
  plant->setHumidityThreshold(payload.humidity_threshold);
  plant->setWateringTime(payload.watering_time_seconds);
  plant->setSensorPin(payload.sensor_pin_number);
  plant->setValvePin(payload.valve_pin_number);
  return true;
}

bool MockWebClient::handleDeletePlantInfo(const std::string &uid,
                                          std::string *error_message) {
  std::unique_lock<std::mutex> lock(mutex_plants);
  auto it = std::find_if(plants.begin(), plants.end(),
                         [&uid](Plant *p) { return p->getUid() == uid; });
  if (it == plants.end()) {
    if (error_message) {
      *error_message = "Plant not found";
    }
    return false;
  }

  plants.erase(it);
  return true;
}

bool MockWebClient::handleTriggerWater(const std::string &uid,
                                       std::optional<uint16_t> seconds,
                                       std::string *error_message) {
  std::unique_lock<std::mutex> lock(mutex_plants);
  Plant *plant = findPlantByUid(uid);
  if (!plant) {
    if (error_message) {
      *error_message = "Plant not found";
    }
    return false;
  }

  if (seconds.has_value()) {
    plant->waterPlant(*seconds);
  } else {
    plant->waterPlant();
  }
  return true;
}

bool MockWebClient::handleStopWater(const std::string &uid,
                                    std::string *error_message) {
  std::unique_lock<std::mutex> lock(mutex_plants);
  Plant *plant = findPlantByUid(uid);
  if (!plant) {
    if (error_message) {
      *error_message = "Plant not found";
    }
    return false;
  }

  return true;
}

void MockWebClient::setServerPlants(const std::vector<PlantPayload> &plants) {
  _serverPlants = plants;
}

void MockWebClient::clearServerPlants() {
  _serverPlants.clear();
}

void MockWebClient::setPlantHumidity(const std::string &uid, float humidity) {
  _plantHumidity[uid] = humidity;
}

void MockWebClient::setWebhookEventsResult(bool success) {
  _webhookEventsResult = success;
}

void MockWebClient::setWebhookEventsToPost(
    const std::vector<std::string> &events) {
  _eventsToPost = events;
}

void MockWebClient::resetCapturedRequests() {
  _postedPlantValues.clear();
  _postedWebhookEvents.clear();
}

const std::vector<MockWebClient::PostedPlantValue> &
MockWebClient::postedPlantValues() const {
  return _postedPlantValues;
}

const std::vector<std::string> &MockWebClient::postedWebhookEvents() const {
  return _postedWebhookEvents;
}

Plant *MockWebClient::findPlantByUid(const std::string &uid) {
  for (auto *plant : plants) {
    if (plant->getUid() == uid) {
      return plant;
    }
  }
  return nullptr;
}

const MockWebClient::PlantPayload *MockWebClient::findServerPlantByUid(
    const std::string &uid) const {
  auto it = std::find_if(_serverPlants.begin(), _serverPlants.end(),
                         [&uid](const PlantPayload &payload) {
                           return payload.uid == uid;
                         });
  if (it == _serverPlants.end()) {
    return nullptr;
  }
  return &(*it);
}
