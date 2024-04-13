#include "CustomSerial.h"

CustomSerial::CustomSerial(int uart_nr) : HardwareSerial(uart_nr) {}

size_t CustomSerial::print(const char* file, int line, const char* str) {
    return print(file, line, str, "");
}

CustomSerial Logger(1);