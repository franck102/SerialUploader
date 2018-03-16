#ifndef ARDUINO_SDUI_H
#define ARDUINO_SDUI_H

#include <SdFat.h>
#include "UploaderUI.h"

class SDUI : public UploaderUI
{

public:
    SDUI(SdFat &sd) : _sd(sd) {}

    bool begin() override;

    size_t write(uint8_t uint8) override;

    // Return baudRate, or 0 for auto-detect
    // Scans boards.txt for the first occurence of upload.speed=, returns strol of the next 16 chars.
    uint32_t getBaudRate() override;

    void end() override;

protected:
    SdFat &_sd;
    SdFile _boardsFile;
    SdFile _log;

    void findBoardsFile();
};


#endif //ARDUINO_SDUI_H
