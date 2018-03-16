#ifndef ARDUINO_SKETCHSOURCE_H
#define ARDUINO_SKETCHSOURCE_H

#include "UploaderUI.h"

enum class ParseState {
    Start, Data
};

class SketchSource
{
public:
    SketchSource(UploaderUI &ui, bool pageAlign = true) :
            _ui(ui), _pageAlign(pageAlign), _state(ParseState::Start)
    {};

    virtual bool begin() = 0;

    virtual int readBytes(uint8_t *buf, uint16_t size);
    // The memory address for the last bytes obtained from readBytes
    uint16_t getLineAddress();

    virtual int hexRead() = 0;

    virtual void reset();

protected:
    UploaderUI &_ui;
    boolean _pageAlign;
    ParseState _state;
    int _byteCount;
    int _bytesLeft;
    uint16_t _address;

    int readHexByte();
};

#endif //ARDUINO_SKETCHSOURCE_H
