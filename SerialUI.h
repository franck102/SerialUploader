//
// Created by Franck on 3/14/2018.
//

#ifndef ARDUINO_SERIALUI_H
#define ARDUINO_SERIALUI_H

#include "UploaderUI.h"

class SerialUI : public UploaderUI
{
public:
    SerialUI(const SerialUI&) = delete;
    void operator=(const SerialUI&) = delete;

    SerialUI(HardwareSerial &line = Serial, uint32_t baudRate = 115200ul) :
            _line(line), _baudRate(baudRate)
    {};

    bool begin() override;
    unsigned long getBaudRate() override;

    size_t write(uint8_t uint8) override;

    void flush() override;

private:
    HardwareSerial &_line;
    uint32_t _baudRate;

};


#endif //ARDUINO_SERIALUI_H
