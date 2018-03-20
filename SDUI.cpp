#include "SDUI.h"

const char s_upload_speed[] PROGMEM = "upload.speed";

bool SDUI::begin()
{
    return _log.open(_sd.vwd(), "upload.log", O_WRITE | O_CREAT | O_AT_END);
}

size_t SDUI::write(uint8_t b)
{
    return _log.write(b);
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
