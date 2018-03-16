#include "SDSketchSource.h"

bool SDSketchSource::begin()
{
    SdBaseFile file;
    char filename[16];
    uint16_t found = 0u;
    _sd.vwd()->rewind();
    while (file.openNext(_sd.vwd(), O_READ)) {
        file.getName(filename, 16);
        file.close();
        if (strlen(filename) > 4 && strcmp(filename + strlen(filename) - 4, ".HEX") == 0) {
            if (found == 0) {
                _file.open(filename, O_READ);
            }
            found++;
        }
    }
    switch (found) {
        case 0:
            _ui.println(F("No hex file found on SD card."));
            return false;

        case 1:
            _ui.print("Found sketch binary: ");
            _file.printName(&_ui);
            _ui.println();
            return true;

        default:
            _ui.println(F("More than one hex file found on SD card."));
            return false;
    }
}

void SDSketchSource::reset()
{
    SketchSource::reset();
    _file.seekSet(0);
}

int SDSketchSource::hexRead()
{
    return _file.read();
}
