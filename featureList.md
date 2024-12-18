## Web server

- Display each plant humidity
- Display Water Pi app connection status
- Allow the user to set specific limits
- Allow the user to water the plant manually (override)
- Watering history
- Display weather forecast (V2)
- Display the water level of each plant (V2)
- Allow the user to enrole a new plant (with name and limits) (V1.5)

## Water Pi app

- Read plant configuration at start, from a config.json file
- Fetch weather forecast and actual weather from internet (V2)
- Fetch and store sensor data - sensor class
- push to the webserver the data and get the new limits (if available) - webserver com class
- trigger watering if needed (every 30min?) - water class
- read the water level of each tank (V2)
- Communicate with the webserver in a seperate thread, awaken by curl requests (interrupts) and regularly to send informations
- Handle plant objects vector in a list (global variable) - protect access across threads by using a mutex

## Communication protocol

- Curl requests
- JSON format
- API endpoints

JSON communication format:

#### GET_PLANT_INFO (/plants):
(At start + every 30 minutes)

- "uid": "UID",
- "name": "Name",
- "room" : "Room",
- "humidity_threshold": "Threshold humidity"
- "watering_time_seconds": "Watering time (in seconds)"
- "current_humidity" : "Current Humidity"
- "sensor_pin_number" : "GPIO pin number of the sensor"
- "valve_pin_number" : "GPIO pin number of the water valve"

#### POST_PLANT_VALUES (/plants/\[uid\]):
(Every 30 minutes + 2 minutes after watering)

- "current_humidity": "Current Humidity"

#### POST_WEBHOOK_EVENT (/webhooks):
(at start)

- "event": "Event to be subscribed"
    - new_plant
    - watering_request
    - delete_plant
    - patch_plant
    - stop_watering_request
- "url": "URL (including port)"

#### POST_PLANT_INFO (/plants):

Same as GET

New plant from the server

#### PATCH_PLANT_INFO (/plants/\[uid\]):

Modify the plant info from the server

- "name": "Name",
- "room" : "Room",
- "humidity_threshold": "Threshold humidity"
- "watering_time_seconds": "Watering time (in seconds)"
- "current_humidity" : "Current Humidity"
- "sensor_pin_number" : "GPIO pin number of the sensor"
- "valve_pin_number" : "GPIO pin number of the water valve"

#### DELETE_PLANT_INFO (/plants/\[uid\]):

No body

Delete the plant from the server

#### POST_PLANT_TRIGGER_WATER (/plants/\[uid\]/water):

- "watering_time_seconds": "Watering time (in seconds)" (optional)

Water the plant manually from the server

#### POST_STOP_WATER (/plants/\[uid\]/stop-water):

No body

Stop the manual watering (emergency) from the server
