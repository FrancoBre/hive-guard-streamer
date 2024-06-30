#include "BatteryConfig.h"
#include "utils/BatteryPins.h"
#include "utils/CustomSerial.h"
#include "Arduino.h"

void BatteryConfig::initialize() {
    // Set the battery pin as an input
    pinMode(VBAT_PIN, INPUT);
}

unsigned char BatteryConfig::checkBatteryLevel() {
    Logger.print(__FILE__, __LINE__, "about to read mf battery level");

    // Read the raw value from the battery pin
    int rawValue = analogRead(VBAT_PIN);

    // Print the raw value
    Logger.print(__FILE__, __LINE__, "Raw value of what's in pin GPIO 13: ", rawValue);

    // Check if the raw value is 0
    if (rawValue == 0) {
        Logger.print(__FILE__, __LINE__, "Error: No reading from battery level pin");
        return 0;
    }

    // Convert the raw value to voltage
    float batteryVoltage = (float) rawValue / 4095.0 * 3.3 * 2 * 1.05;

    Logger.print(__FILE__, __LINE__, "Voltage on GPIO 13: ", batteryVoltage, " V");

    // Calculate the battery percentage
    auto batteryPercentage = (uint8_t) (((batteryVoltage - BATTV_MIN) / (BATTV_MAX - BATTV_MIN)) * 100);

    char batteryLevelStr[50];
    sprintf(batteryLevelStr, "Battery level: %d%%", batteryPercentage);
    Logger.print(__FILE__, __LINE__, batteryLevelStr);
    return batteryPercentage;

//    // Check if the battery voltage is low
//    if (batteryVoltage < BATTV_LOW) {
//        Logger.print(__FILE__, __LINE__, "Low battery!");
//    }
}
