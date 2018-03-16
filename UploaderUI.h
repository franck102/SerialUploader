#ifndef ARDUINO_UPLOADERUI_H
#define ARDUINO_UPLOADERUI_H

#include "SerialUploader.h"

class UploaderUI : public Print
{
public:

    virtual uint32_t getBaudRate() = 0;
    virtual bool begin() = 0;
    virtual void end() {};

};


#endif //ARDUINO_UPLOADERUI_H
