#include "SDSketchSource.h"

bool SDSketchSource::begin()
{
    char filename[16];
    _sd.vwd()->rewind();
    while (_file.openNext(_sd.vwd(), O_READ)) {
        _file.getName(filename, 16);
        if (strlen(filename) > 4 && strcmp(filename + strlen(filename) - 4, ".HEX") == 0) {
            break;
        }
        _file.close();
    }
    bool found = _file.isOpen();
    if (found) {
        _ui.print("Found sketch binary: ");
        _file.printName(&_ui);
        _ui.println();
    }
    return found;
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
