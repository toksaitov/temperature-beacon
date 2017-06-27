temperature-beacon
==================

_temperature-beacon_ is an Arduino sketch for the ESP8266 microcontroller to
send temperature and humidity data received from a Serial channel to the
[temperature-back](https://github.com/toksaitov/temperature-back) server.

## Requirements

* Arduino IDE `>= 1.8.2`
* Any ESP8266 microcontroller

## Usage

Open the `temperature-beacon.ino` sketch in the Arduino IDE and add Wi-Fi
access point information for one or several networks (to fallback).

```C
#define ACCESS_POINT_1_SSID     "SSID 1"
#define ACCESS_POINT_1_PASSWORD "SSID 1 Password"
#define ACCESS_POINT_2_SSID     "Optional SSID 2"
#define ACCESS_POINT_2_PASSWORD "SSID 2 Password"
#define ACCESS_POINT_3_SSID     "Optional SSID 3"
#define ACCESS_POINT_3_PASSWORD "SSID 3 Password"
```

Point the device to your [deployed](https://github.com/toksaitov/temperature-back) back end.

```C
#define BEACON_ENDPOINT "http://example.com/record"
```

Connect the device or a programmer, compile, and upload the program.

Ensure to [setup](https://github.com/toksaitov/temperature-sensor) an Arduino
board with a DHT11 sensor to send temperature data to the ESP8266 being
configured.

## Credits

*temperature-beacon* was created by [Dmitrii Toksaitov](https://github.com/toksaitov).

