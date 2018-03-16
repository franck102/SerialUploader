#ifndef ARDUINO_STK500CLIENT_H
#define ARDUINO_STK500CLIENT_H

#include <Arduino.h>
#include "UploaderUI.h"

enum class StkState
{
    Created, Started, SyncWait, InSync, SignonWait
};

typedef struct _SktResponse {
    Stk status;
    uint8_t *data;
    uint8_t size;
} StkResponse;

class Stk500Client
{
public:
    Stk500Client(const Stk500Client&) = delete;
    void operator=(const Stk500Client&) = delete;

    Stk500Client(UploaderUI &ui, HardwareSerial &line) :
            _ui(ui), _line(line), _state(StkState::Created),
            _response{Stk::FAILED, _readBuf, 0}
    {};

    StkState state();

    void begin(unsigned long baudRate);
    const StkResponse & getSignon();
    Stk sync();
    const StkResponse & getParameter(StkParam param);

    Stk enterProgramming();

    Stk loadAddress(uint16_t address);

    StkResponse readFlash(uint8_t count, uint8_t *buf);
    Stk writeFlash(uint8_t count, uint8_t *data);

    Stk leaveProgramming();

    void end();


    const StkResponse & readSignature();

private:
    UploaderUI &_ui;
    HardwareSerial &_line;
    StkState _state;
    static const uint8_t READ_BUF_SIZE = 16;

    uint8_t _readBuf[READ_BUF_SIZE];
    uint8_t _readLen = 0u;
    StkResponse _response;

    void setState(StkState state);
    void printState(Print &out, StkState state);
    uint8_t getByte();
    Stk readData(uint8_t size, uint8_t *buf = 0);
    Stk toStk(uint8_t byte);

    void send(StkCommand cmd, uint8_t argCount = 0, uint8_t *args = 0);
    const StkResponse &execute(StkCommand command, uint8_t responseSize = 0);
    const StkResponse & execute(StkCommand cmd, uint8_t responseSize, uint8_t argCount, uint8_t *args);
};


#endif //ARDUINO_STK500CLIENT_H
