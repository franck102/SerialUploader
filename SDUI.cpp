#include "SDUI.h"

bool SDUI::begin()
{
    return _log.open(_sd.vwd(), "upload.log", O_WRITE | O_CREAT | O_AT_END);
}

size_t SDUI::write(uint8_t b)
{
    return _log.write(b);
}

unsigned long SDUI::getBaudRate()
{
    findBoardsFile();
    if (!_boardsFile.isOpen()) {
        return AUTO_BAUD_RATE;
    }
    int len = strlen_P(s_upload_speed);
    int pos = 0;
    int c;
    _boardsFile.seekSet(0);
    for (c = _boardsFile.read(); c >= 0 && pos < len; c = _boardsFile.read()) {
        if (c == pgm_read_byte_near(s_upload_speed + pos)) {
            pos++;
        }
        else {
            pos = 0;
        }
    }
    if (pos != len) {
        return AUTO_BAUD_RATE;
    }

    while (c >= 0 && c != '=') {
        c = _boardsFile.read();
    }
    char buf[16];
    _boardsFile.read(buf, 16);
#ifdef DEBUG
    Serial.print(F("speed buf \"")); Serial.println(buf); Serial.print(" yields ");
    Serial.println(strtoul(buf, 0, 10));
#endif

    return strtoul(buf, 0, 10);
}

void SDUI::findBoardsFile()
{
    char filename[16];
    _sd.vwd()->rewind();
    while (_boardsFile.openNext(_sd.vwd(), O_READ)) {
        _boardsFile.getName(filename, 16);
        if (strcmp(filename, "boards.txt") == 0) {
#ifdef DEBUG
            Serial.println(F("Found boards.txt file!"));
#endif
            return;
        }
        _boardsFile.close();
    }
#ifdef DEBUG
    Serial.println(F("boards.txt file not found!"));
#endif
}

void SDUI::end()
{
    _log.close();
    _boardsFile.close();
    UploaderUI::end();
}

