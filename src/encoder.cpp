#include "encoder.h"
typedef struct
{
    int8_t pin_a;
    int8_t pin_b;
} encoder_pins_t;

static const encoder_pins_t pins[2] =
{
    {LEFT_ENC_A, LEFT_ENC_B},
    {RIGHT_ENC_A, RIGHT_ENC_B}
};

volatile long left_ticks = 0;
volatile long right_ticks = 0;

void IRAM_ATTR encoder_isr(void *arg)
{
    int id = (int)(intptr_t)arg;

    bool a = digitalRead(pins[id].pin_a);
    bool b = digitalRead(pins[id].pin_b);

    if (id == 0)
        left_ticks += (a == b) ? 1 : -1;
    else
        right_ticks += (a == b) ? 1 : -1;
}

void encoder_setup()
{
    for (int i = 0; i < 2; i++)
    {
        pinMode(pins[i].pin_a, INPUT_PULLUP);
        pinMode(pins[i].pin_b, INPUT_PULLUP);

        attachInterruptArg(
            digitalPinToInterrupt(pins[i].pin_a),
            encoder_isr,
            (void *)(intptr_t)i,
            CHANGE
        );
    }
}


float LeftWheelTravel() {
    return (static_cast<float>(left_ticks) / TICKS_PER_REV) * WHEEL_CIRCUMFERENCE;
    }

float RightWheelTravel() {
    return (static_cast<float>(right_ticks) / TICKS_PER_REV) * WHEEL_CIRCUMFERENCE;
    }

float GetDistanceTraveled() {
        return (LeftWheelTravel() + RightWheelTravel()) / 2.0;
    }

float GetLeftWheelSpeed() {
        return (LeftWheelTravel() / (millis() / 1000.0));
    }

float GetRightWheelSpeed() {
        return (RightWheelTravel() / (millis() / 1000.0));
    }

float GetAverageSpeed() {
        return (GetLeftWheelSpeed() + GetRightWheelSpeed()) / 2.0;
    }