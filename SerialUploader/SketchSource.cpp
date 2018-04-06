#include "SketchSource.h"

void SketchSource::reset()
{
    _state = ParseState::Start;
    _address = 0u;
    _byteCount = 0u;
}

int SketchSource::readBytes(uint8_t *buf, uint16_t size)
{
    uint16_t read = 0u;

    while (read < size) {
        switch (_state) {

            case ParseState::Start: {
                for (int c = hexRead(); c != ':'; c = hexRead()) {
                    if (c < 0) return read;
                }
                int newByteCount = readHexByte();
                if (newByteCount < 0) return -1;

                int hadress = readHexByte();
                int laddress = readHexByte();
                if (hadress < 0 || laddress < 0) return -1;
                uint16_t newAddress = ((uint8_t) hadress << 8) + laddress;

                int recordType = readHexByte();
                if (recordType < 0) return -1;
                _state = (recordType == 0) ? ParseState::Data : ParseState::Start;

                if (recordType == 0) {
                    if (_address + _byteCount != newAddress) {
                        _ui.print(F("Gap in addresses in hex file at byte address "));
                        _ui.println(newAddress);
                        return -1;
                    }
                    _address = newAddress;
                    _byteCount = newByteCount;
                    _bytesLeft = _byteCount;
                }
            }
                break;

            case ParseState::Data:
                if (_bytesLeft <= 0) {
                    if (read > 0) {
                        // we are done reading a line
                        // if pageAlign is false, return what we have before loading new address
                        // if true, proceed to next line where we'll check for contiguous addresses
                        if (_pageAlign) {
                            _state = ParseState::Start;
                        }
                        else {
                            return read;
                        }
                    }
                    _state = ParseState::Start;
                } else {
                    int b = readHexByte();
                    if (b < 0) return -1;
                    *buf++ = (uint8_t) b;
                    _bytesLeft--;
                    read++;
                }
                break;
        }
    }
    return read;
}

int SketchSource::readHexByte()
{
    int c1 = hexRead();
    int c2 = hexRead();
    if (c1 < 0 || c2 < 0) {
        return -1;
    }
    return (uint8_t) (((c1 <= '9' ? (c1 - '0') : 10 + (c1 <= 'F' ? c1 - 'A' : c1 - 'a')) << 4) +
                      (c2 <= '9' ? (c2 - '0') : 10 + (c2 <= 'F' ? c2 - 'A' : c2 - 'a')));

}

uint16_t SketchSource::getLineAddress()
{
    return _address;
}

