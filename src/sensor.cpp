#include "sensor.h"

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VL53L0X.h>

#define SENSOR_I2C_ADDR_BASE 0x30
#define SENSOR_DEFAULT_ADDR 0x29
#define SENSOR_MAX_RANGE_MM 2000

static Adafruit_VL53L0X s_tof[SENSOR_COUNT];
static bool s_sensor_ok[SENSOR_COUNT];
static const uint8_t s_xshut_pin[SENSOR_COUNT] = {
    SENSOR_XSHUT_FRONT, SENSOR_XSHUT_RIGHT, SENSOR_XSHUT_LEFT};
static const char *s_name[SENSOR_COUNT] = {"FRONT", "RIGHT", "LEFT"};

bool sensor_init(void)
{
    pinMode(SENSOR_I2C_SDA, INPUT_PULLUP);
    pinMode(SENSOR_I2C_SCL, INPUT_PULLUP);
    Wire.begin(SENSOR_I2C_SDA, SENSOR_I2C_SCL);

    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        pinMode(s_xshut_pin[i], OUTPUT);
        digitalWrite(s_xshut_pin[i], LOW);
    }
    delay(10);

    bool all_ok = true;
    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        digitalWrite(s_xshut_pin[i], HIGH);
        delay(10);

        // begin() blocks forever on a sensor that never responds, so probe
        // for it on the bus first and skip it if absent.
        Wire.beginTransmission(SENSOR_DEFAULT_ADDR);
        bool present = Wire.endTransmission() == 0;
        s_sensor_ok[i] = present && s_tof[i].begin(SENSOR_I2C_ADDR_BASE + i, false, &Wire,
                                                     Adafruit_VL53L0X::VL53L0X_SENSE_HIGH_ACCURACY);
        if (!s_sensor_ok[i])
        {
            Serial.printf("[SENSOR] %s init failed\n", s_name[i]);
            all_ok = false;
        }
    }
    return all_ok;
}

static uint16_t read_one(sensor_id_t id, bool *in_range)
{
    if (!s_sensor_ok[id])
    {
        *in_range = false;
        return SENSOR_MAX_RANGE_MM;
    }

    VL53L0X_RangingMeasurementData_t m;
    s_tof[id].rangingTest(&m, false);

    *in_range = (m.RangeStatus != 4);
    return *in_range ? (uint16_t)m.RangeMilliMeter : SENSOR_MAX_RANGE_MM;
}

bool sensor_read_all(sensor_reading_t *out)
{
    bool front_ok, right_ok, left_ok;
    out->front_mm = read_one(SENSOR_FRONT, &front_ok);
    out->right_mm = read_one(SENSOR_RIGHT, &right_ok);
    out->left_mm = read_one(SENSOR_LEFT, &left_ok);
    return front_ok || right_ok || left_ok;
}
