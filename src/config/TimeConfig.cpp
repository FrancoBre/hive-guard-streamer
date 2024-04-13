#include "TimeConfig.h"
#include "esp_sntp.h"
#include "Arduino.h"
#include <ctime>

// TODO si metemos lo de que el esp se active o desactive cuando es de noche o de día, se puede hacer acá
//  le va a tener que llegar la latitud y longitud del master o zone id, y la hora de la puesta y salida del sol
//  a la conf global que le va a llegar al POST /status o POST /config
//  o tambien le puede llegar desde los parametros del wifi manager
//  también podemos hacer que le llegue el server ntp, pero me parece al pedo
void TimeConfig::initialize() {
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();

    time_t now = 0;
    struct tm timeinfo = {0};
    int retry = 0;
    const int retry_count = 10;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        delay(2000);
    }
    time(&now);
    localtime_r(&now, &timeinfo);
}
