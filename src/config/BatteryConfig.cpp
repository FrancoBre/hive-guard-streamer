#include "BatteryConfig.h"
#include "utils/BatteryPins.h"
#include "utils/CustomSerial.h"
#include "Arduino.h"

void BatteryConfig::initialize() {
    // Set the battery pin as an input
    pinMode(VBAT_PIN, INPUT);
}

void BatteryConfig::checkBatteryLevel() {
    // Read the raw value from the battery pin
    int rawValue = analogRead(VBAT_PIN);

    // Print the raw value
    Logger.print(__FILE__, __LINE__, "Raw value of what's in pin GPIO 13: ", rawValue);

    // Check if the raw value is 0
    if (rawValue == 0) {
        Logger.print(__FILE__, __LINE__, "Error: No reading from battery level pin");
        return;
    }

    // Convert the raw value to voltage
    float batteryVoltage = (float) rawValue / 4095.0 * 3.3 * 2 * 1.05;
//    float battv = (float) rawValue / 4095.0 * 3.3;

    Logger.print(__FILE__, __LINE__, "Voltage on GPIO 13: ", batteryVoltage, " V");

    // Calculate the battery percentage
//    uint8_t batteryPercentage = (uint8_t) (((batteryVoltage - BATTV_MIN) / (BATTV_MAX - BATTV_MIN)) * 100);

//    float tempBattpc = ((battv - BATTV_MIN) / (BATTV_MAX - BATTV_MIN)) * 100;
//    uint8_t battpc = tempBattpc > 100 ? 100 : (uint8_t) tempBattpc;

    // Calculate the battery percentage
    uint8_t batteryPercentage = (uint8_t) (((batteryVoltage - BATTV_MIN) / (BATTV_MAX - BATTV_MIN)) * 100);

    // Create a string with the battery level information
    char batteryLevelStr[50];
    sprintf(batteryLevelStr, "Battery level: %d%%", batteryPercentage);

    // Pass the string to the Logger.print function
    Logger.print(__FILE__, __LINE__, batteryLevelStr);

//    Logger.print(__FILE__, __LINE__, "Battery level: ", batteryPercentage, "%");

    // Check if the battery voltage is low
    if (batteryVoltage < BATTV_LOW) {
        Logger.print(__FILE__, __LINE__, "Low battery!");
    }
}
