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
    Serial.printf("front=%u right=%u left=%u\n",
                  reading.front_mm, reading.right_mm, reading.left_mm);
    delay(100);
}
