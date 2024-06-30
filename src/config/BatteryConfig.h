#ifndef BATTERYCONFIG_H
#define BATTERYCONFIG_H

class BatteryConfig {

public:
    static void initialize();
    static unsigned char checkBatteryLevel();
};

#endif //BATTERYCONFIG_H
