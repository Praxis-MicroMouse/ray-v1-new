#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>
#include <stdbool.h>

// Three VL53L0X ToF sensors share one I2C bus, distinguished at boot by XSHUT.
#define SENSOR_XSHUT_FRONT 18
#define SENSOR_XSHUT_RIGHT 19
#define SENSOR_XSHUT_LEFT 5

#define SENSOR_I2C_SDA 21
#define SENSOR_I2C_SCL 22

typedef enum
{
    SENSOR_FRONT = 0,
    SENSOR_RIGHT,
    SENSOR_LEFT,
    SENSOR_COUNT
} sensor_id_t;

typedef struct
{
    uint16_t front_mm;
    uint16_t right_mm;
    uint16_t left_mm;
} sensor_reading_t;

// Brings up I2C and all three sensors. Returns true only if all were found.
bool sensor_init(void);

// Reads all three sensors into *out. Returns true if at least one is in range.
bool sensor_read_all(sensor_reading_t *out);

#endif // SENSOR_H
