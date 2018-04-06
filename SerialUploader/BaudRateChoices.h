#include "UploaderUI.h"

class BaudRateChoices : public UIChoices
{
public:
    void prompt(Print &display)
    {
        display.print(F("Select baud rate:"));
    }

    bool hasNext()
    {
        return _index < NUMITEMS(BAUD_RATES) + 1;
    }

    void next(Print &display)
    {
        if (_index == 0) {
            display.print(F("auto-detect"));
        }
        else {
            display.print(BAUD_RATES[_index - 1]);
        }
        _index++;
    }

    void next() override
    {
        _index++;
    }

private:
    uint8_t _index = 0;
};
