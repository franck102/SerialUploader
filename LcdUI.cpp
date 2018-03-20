#include <Wire.h>
#include "LcdUI.h"

bool LcdUI::begin()
{
    Serial.begin(115200);
    if (!_sdUI.begin()) {
        return false;
    }
    _lcd.begin(16, 2);
    delay(50);
    _lcd.clear();
    return true;
}

size_t LcdUI::write(uint8_t c)
{
    _sdUI.write(c);
#ifdef DEBUG
    if (c == '\n' || c == '\r') {
        Serial.println("\\n");
    } else if (c >= ' ' && c <= '}') {
        Serial.print((char) c);
    } else {
        Serial.print(" ");
        Serial.print("0x");
        Serial.print(c, HEX);
    }
#endif
    switch (c) {
        case '\n':
        case '\r':
            _cr = true;
            return 1;

        default:
            if (_cr || _col >= 16) {
//                uint32_t now = millis();
//                if (now - _lastClear < 500ul) {
//                    delay(500ul - (now - _lastClear));
//                }
                _lcd.clear();
                for (int i = 0; i < _col; ++i) {
                    _lcd.write(buf[i]);
                }
                _cr = false;
                _col = 0;
//                _lastClear = millis();
                _lcd.setCursor(_col, 1);
            }
            buf[_col++] = c;
            return _lcd.write(c);
    }
}

void LcdUI::end()
{
    _sdUI.end();
}
