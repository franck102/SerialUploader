#ifndef ARDUINO_LCDUI_H
#define ARDUINO_LCDUI_H

#include <LiquidCrystal_I2C.h>
#include "UploaderUI.h"
#include "SDUI.h"

class LcdUI : public UploaderUI
{
public:
    LcdUI(SDUI &sdUI) :
            _sdUI(sdUI),
#ifdef LCD_ADDRESS
            _lcd(LiquidCrystal_I2C(LCD_ADDRESS, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE)),
#else
            _lcd(LiquidCrystal_I2C(0x0)),
#endif
            _row(0), _col(0), _cr(false), _lastClear(0ul)
    {};

    bool begin() override;
    size_t write(uint8_t uint8) override;
    void end() override;


private:
    SDUI &_sdUI;
    LiquidCrystal_I2C _lcd;
    uint8_t _row;
    uint8_t _col;
    uint8_t buf[16];
    bool _cr;
    uint32_t _lastClear;
};


#endif //ARDUINO_LCDUI_H
