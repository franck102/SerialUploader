#ifndef ARDUINO_SDUI_H
#define ARDUINO_SDUI_H

#include <SdFat.h>
#include "UploaderUI.h"

class SDUI : public UploaderUI
{

public:
    SDUI(const SDUI&) = delete;
    void operator=(const SDUI&) = delete;

    SDUI(SdFat &sd) : _sd(sd) {}

    bool begin() override;
    size_t write(uint8_t uint8) override;
    void end() override;

protected:
    SdFat &_sd;
    SdFile _boardsFile;
    SdFile _log;

    void findBoardsFile();
};


#endif //ARDUINO_SDUI_H
