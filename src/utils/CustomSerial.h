#ifndef CUSTOMSERIAL_H
#define CUSTOMSERIAL_H

#include <HardwareSerial.h>
#include "../handler/LogsHandler.h"
#include "TimeUtils.h"
#include <ctime>
#include <iomanip>
#include <sstream>

class CustomSerial : public HardwareSerial {
public:
    explicit CustomSerial(int uart_nr);

    size_t print(const char *file, int line, const char *str);

    template<typename... Args>
    size_t print(const char *file, int line, Args... args) {
        std::string timestamp = TimeUtils::getTimestamp();
        std::ostringstream logStream;
        logStream << "[" << std::left << std::setw(10) << timestamp << "]"  // Reduced from 20 to 10
                  << "[" << std::left << std::setw(15) << file << ":" << std::setw(5) << line
                  << "] "; // Reduced from 30 to 15

        using List = int[];
        (void) List{0, ((void) (logStream << args), 0)...};

        logStream << "\r\n";
        std::string log = logStream.str();
        LogsHandler::addLog(log);

        Serial.print(log.c_str());

        return HardwareSerial::print(log.c_str());
    }
};

extern CustomSerial Logger;

#endif // CUSTOMSERIAL_H