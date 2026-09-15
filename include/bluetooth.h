#ifndef BLUETOOTH_H
#define BLUETOOTH_H


#include <Arduino.h>

void bluetooth_init();
void bluetooth_send(const char *message);
void bluetooth_handle();



#endif // BLUETOOTH_H
