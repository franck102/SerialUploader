#ifndef ARDUINO_SDSKETCHSOURCE_H
#define ARDUINO_SDSKETCHSOURCE_H
#include <SdFat.h>

#include "SerialUploader.h"
#include "SketchSource.h"

class SDSketchSource : public SketchSource
{
public:
    SDSketchSource(UploaderUI &ui, SdFat &sd) :
            SketchSource(ui), _sd(sd) {};

    int hexRead() override;

    bool begin();

    void reset();

private:
    SdFat &_sd;
    SdFile _file;

};


#endif //ARDUINO_SDSKETCHSOURCE_H
