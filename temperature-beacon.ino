#include <stdlib.h>
#include <inttypes.h>
#include <math.h>

#include "ESP8266WiFi.h"
#include "ESP8266WiFiMulti.h"
#include "ESP8266HTTPClient.h"

#define SERIAL_SPEED 9600
#define SERIAL_DELAY 100

#define ACCESS_POINT_1_SSID     "SSID 1"
#define ACCESS_POINT_1_PASSWORD "SSID 1 Password"
#define ACCESS_POINT_2_SSID     "Optional SSID 2"
#define ACCESS_POINT_2_PASSWORD "SSID 2 Password"
#define ACCESS_POINT_3_SSID     "Optional SSID 3"
#define ACCESS_POINT_3_PASSWORD "SSID 3 Password"

#define BEACON_ENDPOINT    "http://temperature.auca.space/record"
#define BEACON_SETUP_DELAY 4000
#define BEACON_INTERVAL    5000

#define MINIMUM_TEMPERATURE -20.0f
#define MAXIMUM_TEMPERATURE  70.0f
#define MINIMUM_HUMIDITY    0.0f
#define MAXIMUM_HUMIDITY    100.0f
#define FILTERING_CUTOFF    0.6f

ESP8266WiFiMulti wireless_client;
HTTPClient http_client;

typedef union parameter {
  float value;
  int8_t data[sizeof(float)];
} parameter_t;

void filter_measurement(
         parameter_t *previous_parameter,
         parameter_t *parameter,
         float minimum,
         float maximum,
         float cutoff
     ) {
  parameter->value = fmin(fmax(parameter->value, minimum), maximum);

  if (isnan(previous_parameter->value)) {
      previous_parameter->value = parameter->value;
  }

  parameter->value = cutoff * previous_parameter->value + (1.0f - cutoff) * parameter->value;
  previous_parameter->value = parameter->value;
}

void consume_serial_data_to_parameter(char key, parameter_t *parameter)
{
    while (Serial.read() != key) { }

    for (int i = 0; i < sizeof(parameter->value); ++i) {
        while (Serial.available() == 0) { }

        parameter->data[i] = (int8_t) Serial.read();
    }
}

void setup()
{
    Serial.begin(SERIAL_SPEED);

    wireless_client.addAP(ACCESS_POINT_1_SSID, ACCESS_POINT_1_PASSWORD);
    wireless_client.addAP(ACCESS_POINT_2_SSID, ACCESS_POINT_2_PASSWORD);
    wireless_client.addAP(ACCESS_POINT_3_SSID, ACCESS_POINT_3_PASSWORD);
    http_client.setReuse(true);
}

void loop()
{
    delay(BEACON_INTERVAL);

    if (wireless_client.run() == WL_CONNECTED) {
        static parameter_t previous_temperature = { NAN },
                           previous_humidity    = { NAN };
        parameter_t temperature,
                    humidity;

        consume_serial_data_to_parameter('t', &temperature);
        consume_serial_data_to_parameter('h', &humidity);

        filter_measurement(
            &previous_temperature,
            &temperature,
            MINIMUM_TEMPERATURE,
            MAXIMUM_TEMPERATURE,
            FILTERING_CUTOFF
        );
        filter_measurement(
            &previous_humidity,
            &humidity,
            MINIMUM_HUMIDITY,
            MAXIMUM_HUMIDITY,
            FILTERING_CUTOFF
        );

        http_client.begin(BEACON_ENDPOINT);
        http_client.addHeader("Content-Type", "application/x-www-form-urlencoded");
        http_client.POST(String("t=") + String(temperature.value) + String("&h=") + String(humidity.value));
        http_client.end();
    }
}

