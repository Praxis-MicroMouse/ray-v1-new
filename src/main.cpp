#include <Arduino.h>
#include "sensor.h"

void setup()
{
    Serial.begin(115200);
    sensor_init();
}

void loop()
{
    sensor_reading_t reading;
    sensor_read_all(&reading);
    Serial.printf("front=%u "
                  "left=%u "
                  "right=%u\n",
                  reading.front_mm, reading.left_mm, reading.right_mm);
    delay(50);
}
