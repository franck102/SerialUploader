#include <avr/wdt.h>
#include "SerialUI.h"

int8_t SerialUI::choose(UIChoices &choices)
{
    choices.prompt(_line);
    _line.println();
    int choiceCount;
    for (choiceCount = 0; choices.hasNext(); choiceCount++) {
        _line.print(F("\t["));
        _line.print((char) ('a' + choiceCount));
        _line.print(F("] "));
        choices.next(_line);
        _line.println();
    }

    while (_line.available()) _line.read();
    choices.prompt(_line);
    _line.print(F(" ([a]): "));
    _line.flush();
    wdt_disable();
    while (!_line.available());
    wdt_enable(WDTO_8S);
    int entered = _line.read();
    _line.println((char) entered);
    switch (entered) {
        case 0x0A:
        case 0x0D:
            // CR / LF
            return 0;
        default:
            int idx = entered - 'a';
            if (idx >= 0 && idx < choiceCount) {
                return idx;
            }
    }
    return choose(choices);
}

bool SerialUI::begin()
{
    _line.begin(_baudRate);
    while (!_line);

    return true;
}

size_t SerialUI::write(uint8_t c)
{
    return _line.write(c);
}

//void SerialUI::flush()
//{
//    _line.flush();
//}

