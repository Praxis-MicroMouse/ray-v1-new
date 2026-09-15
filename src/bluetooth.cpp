#include "bluetooth.h" 
#include <BluetoothSerial.h>

static BluetoothSerial SerialBT;

void bluetooth_init() {
    SerialBT.begin("ESP32_BT"); // Bluetooth device name
    Serial.println("Bluetooth started. Device is discoverable as 'ESP32_BT'");
}

void bluetooth_send(const char *message) {
    SerialBT.println(message);
}

void bluetooth_handle() {
    if (SerialBT.available())
    {
        String command = SerialBT.readStringUntil('\n');
        command.trim(); // Remove any leading/trailing whitespace

        if (command == "Stop"){
            Serial.println("Stop motors");
        }

        else if (command == "FWD"){
            Serial.println("Move forward");
        }

        else if (command == "BWD"){
            Serial.println("Move backward");
        }

        else if (command == "LEFT"){
            Serial.println("Turn left");
        }

        else if (command == "RIGHT"){
            Serial.println("Turn right");
        }

        else {
            Serial.printf("Unknown command: %s\n", command.c_str());
        }
    }
}
