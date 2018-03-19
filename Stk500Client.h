#ifndef ARDUINO_STK500CLIENT_H
#define ARDUINO_STK500CLIENT_H

#include <Arduino.h>
#include "UploaderUI.h"

typedef struct _SktResponse {
    Stk status;
    uint8_t *data;
    uint16_t size;
} StkResponse;

class Stk500Client
{
public:
    Stk500Client(const Stk500Client&) = delete;
    void operator=(const Stk500Client&) = delete;

    Stk500Client(UploaderUI &ui, HardwareSerial &line) :
            _ui(ui), _line(line),
            _response{Stk::FAILED, _readBuf, 0}
    {};

    void begin(unsigned long baudRate);
    const StkResponse & getSignon();
    Stk sync();
    const StkResponse & getParameter(StkParam param);

    Stk enterProgramming();
    void resetAddress();
    Stk loadAddress(uint32_t wordAddress);

    StkResponse readFlash(uint16_t count, uint8_t *buf);
    Stk writeFlash(uint16_t count, uint8_t *data);

    Stk leaveProgramming();

    void end();

    const StkResponse & readSignature();

private:
    UploaderUI &_ui;
    HardwareSerial &_line;
    static const uint8_t READ_BUF_SIZE = 16;

    uint8_t _readBuf[READ_BUF_SIZE];
    uint16_t _readLen = 0u;
    uint8_t _extendedAddress;
    StkResponse _response;

    uint8_t getByte();
    Stk readData(uint16_t size, uint8_t *buf = 0);
    Stk toStk(uint8_t byte);

    void send(StkCommand cmd, uint8_t argCount = 0, uint8_t *args = 0);
    const StkResponse &execute(StkCommand command, uint8_t responseSize = 0);
    const StkResponse & execute(StkCommand cmd, uint8_t responseSize, uint8_t argCount, uint8_t *args);
};


#endif //ARDUINO_STK500CLIENT_H
