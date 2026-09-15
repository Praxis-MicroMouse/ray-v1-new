#include "sensor.h"

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include <math.h>

#define SENSOR_I2C_ADDR_BASE 0x30
#define SENSOR_DEFAULT_ADDR 0x29
#define SENSOR_MAX_RANGE_MM 2000

static Adafruit_VL53L0X s_tof[SENSOR_COUNT];
static bool s_sensor_ok[SENSOR_COUNT];
static uint16_t s_last_valid_mm[SENSOR_COUNT] = {
    SENSOR_MAX_RANGE_MM, SENSOR_MAX_RANGE_MM, SENSOR_MAX_RANGE_MM};
static const uint8_t s_xshut_pin[SENSOR_COUNT] = {
    SENSOR_XSHUT_FRONT, SENSOR_XSHUT_RIGHT, SENSOR_XSHUT_LEFT};
static const char *s_name[SENSOR_COUNT] = {"FRONT", "RIGHT", "LEFT"};

bool sensor_init(void)
{
    pinMode(SENSOR_I2C_SDA, INPUT_PULLUP);
    pinMode(SENSOR_I2C_SCL, INPUT_PULLUP);
    Wire.begin(SENSOR_I2C_SDA, SENSOR_I2C_SCL, 400000);

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

// Linear calibration fit from actual vs measured distance
static uint16_t calibrate(sensor_id_t id, uint16_t raw_mm)
{ // front, right, left
    static const float a[SENSOR_COUNT] = {0.9984f, 0.9690f, 0.9642f};
    static const float b[SENSOR_COUNT] = {-12.81f, -13.60f, -10.56f};
    float corrected = a[id] * raw_mm + b[id];
    return (uint16_t)(corrected < 0 ? 0 : corrected);
}

static uint16_t read_one(sensor_id_t id, bool *in_range)
{
    if (!s_sensor_ok[id])
    {
        *in_range = false;
        return SENSOR_MAX_RANGE_MM;
    }

    VL53L0X_RangingMeasurementData_t m = {};
    s_tof[id].rangingTest(&m, false);

    // Reject anything the sensor can't physically report, not just
    // RangeStatus==4 - catches corrupted I2C reads that report a "valid"
    // status but leave garbage (or an unfilled/zeroed struct) behind.
    *in_range = (m.RangeStatus != 4) &&
                (m.RangeMilliMeter > 0) &&
                (m.RangeMilliMeter <= SENSOR_MAX_RANGE_MM);

    if (!*in_range)
    {
        // Corrupted/invalid reading - discard it and hold the last good
        // value instead of reporting a bogus distance.
        return s_last_valid_mm[id];
    }

    uint16_t raw_mm = (uint16_t)m.RangeMilliMeter;
    raw_mm = calibrate(id, raw_mm);
    s_last_valid_mm[id] = raw_mm;
    return raw_mm;
}

bool sensor_read_all(sensor_reading_t *out)
{
    bool front_ok, right_ok, left_ok;
    out->front_mm = read_one(SENSOR_FRONT, &front_ok);
    out->right_mm = read_one(SENSOR_RIGHT, &right_ok);
    out->left_mm = read_one(SENSOR_LEFT, &left_ok);
    return front_ok || right_ok || left_ok;
}
