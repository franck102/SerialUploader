#include <avr/wdt.h>
#include "SerialUI.h"

#define NUM_BAUD_RATES 6
unsigned long BAUD_RATES[NUM_BAUD_RATES] =
        {AUTO_BAUD_RATE, 115200ul, 57600ul, 38400ul, 9600ul, 2400ul};

unsigned long SerialUI::getBaudRate()
{
    _line.println(F("Select baud rate:"));
    for (int i = 0; i < NUM_BAUD_RATES; i++) {
        _line.print(F("\t["));
        _line.print((char) ('a' + i));
        _line.print(F("] "));
        if (BAUD_RATES[i] == AUTO_BAUD_RATE) {
            _line.println(F("auto-detect"));
        } else {
            _line.println(BAUD_RATES[i]);
        }
    }
    while (_line.available()) _line.read();
    _line.print(F("Baud rate ([a]): "));
    _line.flush();
    wdt_disable();
    while (!_line.available());
    wdt_enable(WDTO_8S);
    int entered = _line.read();
    _line.println((char) entered);
    switch (entered) {
        case 0x0A:
        case 0x0D:
            // CR / LF
            return BAUD_RATES[0];
        default:
            int idx = entered - 'a';
            if (idx >= 0 && idx < NUM_BAUD_RATES) {
                return BAUD_RATES[idx];
            }
    }
    // Bad entry
    return getBaudRate();
}

bool SerialUI::begin()
{
    if (!_line) {
        _line.begin(_baudRate);
        while (!_line);
    }
    return true;
}

size_t SerialUI::write(uint8_t c)
{
    return _line.write(c);
}

void SerialUI::flush()
{
    _line.flush();
}

