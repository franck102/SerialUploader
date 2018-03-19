#include "SDSketchSource.h"

class SDChoices : public UIChoices
{
public:
    SDChoices(FatFile *dir, SdFile &file, FString prompt, const char *suffix) :
            _dir(dir), _file(file), _prompt(prompt), _suffix(suffix), _multi(false)
    {
        _dir->rewind();
        next();
    }

    void prompt(Print &display) override
    {
        display.print(_prompt);
    }

    bool hasNext() override
    {
        return _file.isOpen();
    }

    void next(Print &display) override
    {
        if (_file.isOpen()) {
            _file.printName(&display);
        }
        next();
    }

    void next() override
    {
        if (_file.isOpen()) {
            _multi = true;
            _file.close();
        }
        while (_file.openNext(_dir, O_READ)) {
            _file.getName(_filename, 16);
            if (strlen(_filename) > strlen(_suffix) && strcmp(_filename + strlen(_filename) - 4, _suffix) == 0) {
                return;
            }
            _file.close();
        }
    }

    void openFile(SdFile &file, int8_t choice)
    {
        _dir->rewind();
        for (int i = 0; i <= choice; ++i) {
            next();
        }
    }

    bool foundSome()
    {
        return _multi;
    }

private:
    FatFile *_dir;
    SdFile &_file;
    char _filename[16];
    FString _prompt;
    const char *_suffix;
    bool _multi;
};

bool SDSketchSource::begin()
{
    SDChoices choices(_sd.vwd(), _file, F("Choose the sketch to upload:"), ".HEX");
    int8_t choice = _ui.choose(choices);

    if (choice < 0) {
        _ui.println(choices.foundSome() ?
                    F("More than one .hex file found on SD card") :
                    F("No hex file found on SD card."));
        return false;
    }

    choices.openFile(_file, choice);
    _ui.print(F("Sketch file: "));
    _file.printName(&_ui);
    _ui.println();
    return true;
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
