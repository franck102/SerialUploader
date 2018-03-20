#ifndef ARDUINO_UPLOADERUI_H
#define ARDUINO_UPLOADERUI_H

#include "SerialUploader.h"

class UIChoices
{
public:
    virtual void prompt(Print &display) = 0;
    virtual bool hasNext() = 0;
    virtual void next(Print &display) = 0;
    virtual void next() = 0;
};

class UploaderUI : public Print
{
public:
    virtual int8_t choose(UIChoices &choices);
    virtual bool begin() = 0;
    virtual void end()
    {};

};

#endif //ARDUINO_UPLOADERUI_H

//
//class ArrayChoice : public UIChoices
//{
//public:
//    ArrayChoice(FString prompt, uint8_t count, FString *entries) :
//            _prompt(prompt), _count(count), _entries(entries), _next(0) {}
//
//public:
//
//    void prompt(Print &display) override
//    {
//        display.print(_prompt);
//    }
//
//    boolean hasNext() override
//    {
//        return _next < _count;
//    }
//
//    void next(Print &display) override
//    {
//        display.println(_entries[_next++]);
//    }
//
//    void next() override
//    {
//        _next++;
//    }
//
//private:
//    uint8_t _count;
//    uint8_t _next;
//    FString _prompt;
//    FString *_entries;
//
//};
