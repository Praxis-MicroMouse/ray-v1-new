#include <Arduino.h>

#include "sensor.h"
#include "encoder.h"
#include "bluetooth.h"

void setup()
{
    Serial.begin(115200);

    sensor_init();
    encoder_setup();
    bluetooth_init();
}

void loop()
{
    sensor_reading_t reading;
    sensor_read_all(&reading);

    bluetooth_handle();

        Serial.printf(
        "front=%u left=%u right=%u | L=%ld R=%ld | D=%.2f cm\n",
        reading.front_mm,
        reading.left_mm,
        reading.right_mm,
        left_ticks,
        right_ticks,
        GetDistanceTraveled()
    );

    char data[100];

    snprintf(
        data,
        sizeof(data),
        "S %u %u %u E %ld %ld D %.2f",
        reading.front_mm,
        reading.left_mm,
        reading.right_mm,
        left_ticks,
        right_ticks,
        GetDistanceTraveled()
    );

    bluetooth_send(data);

    delay(50);
}