#include <Arduino.h>

#include "sensor.h"
#include "encoder.h"

void setup()
{
    Serial.begin(115200);

    sensor_init();
    encoder_setup();
}

void loop()
{
    sensor_reading_t reading;

    sensor_read_all(&reading);

    Serial.printf("front=%u left=%u right=%u | "
                  "L_ticks=%ld R_ticks=%ld | "
                  "distance=%.2f cm speed=%.2f cm/s\n",
                  reading.front_mm,
                  reading.left_mm,
                  reading.right_mm,
                  left_ticks,
                  right_ticks,
                  GetDistanceTraveled(),
                  GetAverageSpeed());

    delay(50);
}