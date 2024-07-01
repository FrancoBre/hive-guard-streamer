![hive-guard-logo](https://github.com/FrancoBre/hive-guard-streamer/assets/66085255/411eb002-76a3-4547-96da-c54983a37996)


Hive Guard is a real-time beekeeping monitor. It collects data, analyzes it, stores it, and presents it. It offers live video, environmental data, object detection, and more, making beekeeping modern and efficient.

![90669a15-0587-4185-8131-bea4d6f8a725](https://github.com/FrancoBre/hive-guard-streamer/assets/66085255/c4d189d2-7c13-4c97-a106-c29638d217ec)


This is part of the Hive Guard system, you can find a general overview [here](https://github.com/FrancoBre/HIVE-GUARD)

## Contents
1. [Prerequisites](#prerequisites)
2. [Setup](#setup)
3. [Usage](#usage)
4. [Implementation Details](#implementation-details)
5. [Troubleshooting](#troubleshooting)
6. [What is left to do](#what-is-left-to-do)

## Prerequisites

- ESP32 development board with a camera module.

![ESP32-Cam-Ov2640](https://github.com/FrancoBre/hive-guard-streamer/assets/66085255/0e48cf2f-a1ef-4aaf-9b1f-582e4bea9d64)

- Arduino IDE with ESP32 board support installed, or PlatformIO in VSCode or CLion with AI Thinker ESP32-CAM board with Arduino framework.
- A Wi-Fi network.

## Setup

This C++ project is intended to be deployed using PlatformIO on an ESP32CAM module, equipped with a DHT11 sensor.

To connect this module, you can use the following diagram:

![circuito-esp-dht](https://github.com/FrancoBre/hive-guard-streamer/assets/66085255/49857b9e-5737-4f30-a521-79b4ca66d390)

Then, compile and upload the sketch to your ESP32 board. To do it, you'll need a dev board, and connect it like this:

![esp-dev-board-circuito](https://github.com/FrancoBre/hive-guard-streamer/assets/66085255/486dc199-cd72-4fac-abd6-38d29d169129)

Each time the ESP32 is powered on, it starts an access point named "HG_StreamerAP" and goes into a blocking loop awaiting configuration. The access point is password protected with the password "12345678". You can connect to this access point and configure the Wi-Fi credentials.

## Usage

Once the sketch is uploaded and the ESP32 board is powered on, it will connect to the specified Wi-Fi network and start the camera server.

Then, it'll create a UDP server, which sends a broadcast UDP packet with its data to the broadcast path, an HTTP server, with the following endpoints:

 - `/iAmMaster`: Endpoint for the master server to register its IP.
 - `/status`: Endpoint to check if the server is running.
 - `/logs`: Endpoint to retrieve logs.

And a WebSocket connection, when hit by the master server (refer to [this documentation](https://github.com/FrancoBre/HIVE-GUARD/tree/main?tab=readme-ov-file#detailed-flow) for more details).

The video stream can be accessed by navigating to http://<ESP32_IP_ADDRESS>/stream in a web browser, where <ESP32_IP_ADDRESS> is the IP address of the ESP32 board on your Wi-Fi network.

## Implementation Details

The application is implemented in C++ and uses the ESP32 Camera library to capture video frames from the ESP32 camera module. The video frames are then streamed over HTTP using the ESP HTTP Server library. The application is designed using a class-based approach, with the main components of the application encapsulated in classes:

- *Camera Initialization:* The camera is initialized with the esp_camera_init() function, which is called in the setup() function in main.cpp. The camera configuration is set in the camera_config_t struct.
- *Wi-Fi Connection:* Each time the ESP32 is powered on, it creates an access point. You can connect to this access point and configure the Wi-Fi credentials. This is handled by the WiFiManager library.
- *HTTP Server:* The HTTP server is started in the startCameraServer() function in app_httpd.cpp. The server listens on two ports, one for the web server and one for the stream server.
- *Stream Handler:* The VideoStreamHandler class in VideoStreamHandler.cpp handles HTTP GET requests to the /stream path. It captures a frame from the camera, converts it to JPEG format if necessary, and sends it as a chunk of the HTTP response. The handler also calculates the average frame time for the video stream.
- *Running Average Filter:* A running average filter is implemented in VideoStreamHandler.cpp to calculate the average frame time for the video stream. The filter is used in the handle() function.

The application is built with PlatformIO and the Arduino framework for the ESP32. The platformio.ini file contains the configuration for the PlatformIO environment, including the board type (esp32cam) and the framework (arduino). The application is designed to be run on an ESP32 development board with a camera module. It has been tested with the AI Thinker Model, M5STACK PSRAM Model, and M5STACK WITHOUT PSRAM. The pin configuration for the camera module is set in main.cpp based on the selected camera model.

## Troubleshooting

### Serial Monitor

You may encounter issues when trying to view the application logs in the Serial Monitor. As a workaround, you can use the screen command in the terminal to connect to the ESP32's serial port. For example:

```bash
screen /dev/tty.usbserial-1210 115200
```

Where `/dev/tty.usbserial-1210` is the device name and `115200` is the baud rate.  

### Identifying the Connected Device
To identify which device the ESP32 is connected to, you can use the `ls /dev/tty.*` command in the terminal. This command will list all tty devices, including your ESP32 if it's connected. The ESP32 device name usually starts with `/dev/tty.usbserial-` followed by a series of numbers and letters.

```bash
ls /dev/tty.*
```

## What is left to do

### Battery
There is [a branch](https://github.com/FrancoBre/hive-guard-streamer/tree/feature/send-battery-again) with a logic for streaming battery over. You can find more insight [here](https://github.com/FrancoBre/HIVE-GUARD/tree/main#Battery).

The idea is to power the device using lithium 3.7V batteries, like this ones:

![f0053fb5-f5dc-472d-a787-e0a9171f0853](https://github.com/FrancoBre/hive-guard-streamer/assets/66085255/3b6d4a1b-c254-4085-870e-f55906fb058c)

![3136847c-81c6-4879-8725-120cda46ae51](https://github.com/FrancoBre/hive-guard-streamer/assets/66085255/1dd097f4-4f08-4952-bd34-3a4d4a7b72a1)

With this in mind, we crafted this circuit, based on [this youtube tutorial](https://www.youtube.com/watch?v=zvejuO7SAV4&t=305s):

![e3fa7598-a10c-4194-8c51-0ff814a0bb56](https://github.com/FrancoBre/hive-guard-streamer/assets/66085255/6ebb47cb-702f-4353-a97f-a1341637250b)

The batteries produces 11V, which are then passed to a BMS module to protect the batteries from damaging, which are then passed to a step down module which passes the 4.2V that the ESP32CAM module along with the DHT11 needs.

Then, for the ESP32 to monitor its own battery level, we found this amazing piece of engineering called a circuit divider in [this webpage](https://mansfield-devine.com/speculatrix/2021/08/esp32-room-thermometer-with-18650-battery-level-indicator/)

The whole circuit looks like this:

![bef464ca-6f88-4533-8d88-14d1ba645444](https://github.com/FrancoBre/hive-guard-streamer/assets/66085255/fe3bcfbd-24b0-4d18-8abd-48a93c46a878)

We read the raw voltage from the general purpose pin 13, convert it into a percentage, using the low and high battery voltages for an ESP32CAM, and voilá! we have the percentage to send over to the master server.
But hold on... this reading only works before the WiFi has been activated. Then, this pin is unable to read any further voltage

![bc6c243e-f390-44c5-961b-7f187d45c153](https://github.com/FrancoBre/hive-guard-streamer/assets/66085255/ca35e8e7-a70e-4ff3-8574-b0279d827151)

You can read our proposal for a solution [here](https://github.com/FrancoBre/HIVE-GUARD/tree/main#Battery).
