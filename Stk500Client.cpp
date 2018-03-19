#include "Stk500Client.h"

void Stk500Client::begin(unsigned long baudRate)
{
    if (_line) {
        _line.end();
        delay(5);
    }
    _line.begin(baudRate);
//    _line.setTimeout(STK_TIMEOUT);

#ifdef SERIAL_RX
    pinMode(SERIAL_RX, INPUT_PULLUP);
#endif
#ifdef SERIAL_TX
    pinMode(SERIAL_TX, INPUT_PULLUP);
#endif
#ifdef PIN_DTR
    digitalWrite(PIN_DTR, LOW);
    delay(DELAY_RESET);
#endif
    while (!_line) {}
    // Drain noise
    while (_line.available() > 0) {
        _line.read();
    }
#ifdef DEBUG
    _ui.print("Stk500 started at ");
    _ui.print(baudRate);
    _ui.println();
#endif
}

void Stk500Client::end()
{
#ifdef PIN_DTR
    digitalWrite(PIN_DTR, HIGH);
#endif
    _line.end();
}

Stk Stk500Client::sync()
{
    const StkResponse &response = execute(StkCommand::GET_SYNC);
    return response.status;
}

const StkResponse &Stk500Client::getSignon()
{
    return execute(StkCommand::GET_SIGN_ON, 7);
}

const StkResponse &Stk500Client::readSignature()
{
    return execute(StkCommand::READ_SIGN, 3, 0, 0);
}

const StkResponse &Stk500Client::getParameter(StkParam param)
{
    uint8_t data[1] = {(uint8_t) param};
    return execute(StkCommand::GET_PARAMETER, 1, 1, data);
}

Stk Stk500Client::enterProgramming()
{
    const StkResponse response = execute(StkCommand::ENTER_PROGMODE);
    _extendedAddress = 0xFF;
    return response.status;
}

Stk Stk500Client::leaveProgramming()
{
    const StkResponse response = execute(StkCommand::LEAVE_PROGMODE);
    return response.status;
}

Stk Stk500Client::loadAddress(uint32_t wordAddress)
{
    uint8_t extendedAddress = (wordAddress >> 16) & 0xFF;
    if (extendedAddress != _extendedAddress) {
        uint8_t data[4];
        data[0] = (uint8_t) AvrCommand::AVR_OP_LOAD_EXT_ADDR;
        data[1] = 0;
        data[2] = extendedAddress;
        data[3] = 0;
        const StkResponse response = execute(StkCommand::UNIVERSAL, 1, 4, data);
        if (response.status != Stk::OK) {
            return response.status;
        }
        _extendedAddress = extendedAddress;
    }
    uint8_t data[2] = {(uint8_t) (wordAddress & 0x00FF), (uint8_t) ((wordAddress & 0xFF00) >> 8)};
    const StkResponse response = execute(StkCommand::LOAD_ADDRESS, 0, 2, data);
    return response.status;
}

Stk Stk500Client::writeFlash(uint16_t count, uint8_t *data)
{
    // Cmnd_STK_PROG_PAGE, bytes_high, bytes_low, memtype, data, Sync_CRC_EOP
    _line.write((uint8_t) StkCommand::PROG_PAGE);
    _line.write((count >> 8) & 0xFF);
    _line.write(count & 0xFF);
    _line.write('F');
    for (int i = 0; i < count; ++i) {
        _line.write(data[i]);
    }
    _line.write((uint8_t) Stk::CRC_EOP);
    _line.flush();
#ifdef DEBUG
    _ui.print(F("SND"));
    printByte(_ui, StkCommand::PROG_PAGE);
    printByte(_ui, (count >> 8) & 0xFF);
    printByte(_ui, count & 0xFF);
    _ui.print(F(" F"));
    printByte(_ui, data[0]);
    _ui.print(F(" ..."));
    printByte(_ui, Stk::CRC_EOP);
    _ui.println();
#endif
#ifdef DELAY_FLASH_WRITE
    delay(DELAY_FLASH_WRITE);
#endif
    switch (getByte()) {
        case (uint8_t) Stk::INSYNC:
            return readData(0);

        default:
            return Stk::FAILED;
    }
}

StkResponse Stk500Client::readFlash(uint16_t count, uint8_t *buf)
{
    _line.write((uint8_t) StkCommand::READ_PAGE);
    _line.write((count >> 8) & 0xFF);
    _line.write(count & 0xFF);
    _line.write('F');
    _line.write((uint8_t) Stk::CRC_EOP);
    _line.flush();
#ifdef DEBUG
    _ui.print(F("SND"));
    printByte(_ui, StkCommand::READ_PAGE);
    printByte(_ui, (count >> 8) & 0xFF);
    printByte(_ui, count & 0xFF);
    _ui.print(F(" F"));
    printByte(_ui, Stk::CRC_EOP);
    _ui.println();
#endif
    switch (getByte()) {
        case (uint8_t) Stk::INSYNC:
            _response.status = readData(count, buf);
            _response.data = _readBuf;
            _response.size = _readLen;
            break;

        default:
            _response.status = Stk::FAILED;
            break;
    }
    return _response;
}

// =================================================

const StkResponse &Stk500Client::execute(StkCommand command, uint8_t responseSize)
{
    return execute(command, responseSize, 0, (uint8_t *) 0);
}

const StkResponse &Stk500Client::execute(StkCommand cmd, uint8_t responseSize, uint8_t argCount, uint8_t *args)
{
    send(cmd, argCount, args);
    uint8_t status = getByte();
    switch (status) {
        case (uint8_t) Stk::INSYNC:
            _response.status = readData(responseSize);
            _response.size = responseSize;
            break;

        default:
            _response.status = (Stk) status;
            break;
    }
    return _response;
}

void Stk500Client::send(StkCommand cmd, uint8_t argCount, uint8_t *args)
{
    _line.write((uint8_t) cmd);
    for (int i = 0; i < argCount; ++i) {
        _line.write(args[i]);
    }
    _line.write((uint8_t) Stk::CRC_EOP);
//    _line.flush();
#ifdef DEBUG
    _ui.print(F("SND"));
    printByte(_ui, cmd);
    for (int i = 0; i < argCount; ++i) {
        printByte(_ui, args[i]);
    }
    printByte(_ui, Stk::CRC_EOP);
    _ui.println();
#endif
}

uint8_t Stk500Client::getByte()
{
    uint32_t start = millis();
    while (_line.available() == 0) {
        if (millis() - start > STK_TIMEOUT) {
            return (uint8_t) Stk::NOSYNC;
        }
    };
    int c = _line.read();
    if (c == -1) {
        return getByte();
    }
    uint8_t b = (uint8_t) c;
#ifdef DEBUG
    _ui.print(F("RCV"));
    printByte(_ui, b);
    _ui.println();
#endif
    // 0 is not a valid stk500 status, it's probably noise on the line
    return b == 0 ? (uint8_t) Stk::NOSYNC : b;
}

Stk Stk500Client::readData(uint16_t size, uint8_t *buf)
{
//    size_t read = _line.readBytes(buf, size);
//#ifdef DEBUG
//    if (size <= 16 && read > 0) {
//        _ui.print(F("RCV"));
//        for (int i = 0; i < read; ++i) {
//            printByte(_ui, buf[i]);
//        }
//    }
//#endif
//    return (read != size) ? Stk::NOSYNC : toStk(getByte());

    uint32_t start = millis();

    _readLen = 0u;
    if (buf == NULL) {
        buf = _readBuf;
    }
    while (_readLen < size) {
        if (millis() - start > STK_TIMEOUT) {
            return Stk::NOSYNC;
        }
        if (_line.available() > 0) {

            uint8_t b = (uint8_t) _line.read();
#ifdef DEBUG
            if (size <= 16) {
                if (_readLen == 0u) {
                    _ui.print(F("RCV"));
                }
                printByte(_ui, b);
            }
#endif
            buf[_readLen++] = b;
        }
    }
#ifdef DEBUG
    if (_readLen > 0u && size <= 16u) {
        _ui.println();
    }
#endif
    return toStk(getByte());
}

Stk Stk500Client::toStk(uint8_t byte)
{
    return byte >= (uint8_t) Stk::OK && byte <= (uint8_t) Stk::CRC_EOP ?
           (Stk) byte :
           Stk::FAILED;
}

void Stk500Client::resetAddress()
{
    _extendedAddress = 0xFF;
}
