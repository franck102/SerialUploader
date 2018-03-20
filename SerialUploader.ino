// Serial uploader sketch for Arduino
// License: GNU GENERAL PUBLIC LICENSE Version 2
//
// Edit constants in SerialUploader.h
//
// Credits:
//      https://github.com/robokoding/STK500
//      https://github.com/Optiboot/optiboot
//      https://github.com/nickgammon/arduino_sketches

#include <avr/wdt.h>
#include "SerialUploader.h"
#include "Stk500Client.h"
#include "UploaderUI.h"
#include "SerialUI.h"
#include "SDSketchSource.h"
#include "SDUI.h"
#include "Signatures.h"
#include "BaudRateChoices.h"
#include "LcdUI.h"

SdFat sd;

#ifdef LCD_ADDRESS
SDUI __sdui(sd);
UploaderUI &&ui = LcdUI(__sdui);
#elif SERIAL_UI
UploaderUI &&ui = SerialUI(SERIAL_UI);
#else
UploaderUI &&ui = SDUI(sd);
#endif

#ifdef SERIAL_TARGET
Stk500Client client(ui, SERIAL_TARGET);
#else
Stk500Client client(ui, Serial);
#endif

SketchSource &&sketch = SDSketchSource(ui, sd);

enum class UploadState
{
    Start, CheckFlags, StartSketch, Syncing, Identify, Upload, Success, Error, ShowError, ShowSuccess
};
UploadState uploadState;

uint8_t statusFlags;
int autoBaud;
uint32_t baudRate;
SignatureType mcuSignature;

// Prototypes
// ==============

bool sdBegin();
bool uiBegin();
uint32_t getBaudRate();
void cleanup();
void resetTarget();
bool check(StkResponse response);
bool sync(uint32_t baudRate);
UploadState upload();
void blink(uint8_t count);
bool readSignature();
bool identifyMCU(byte *sig, size_t sigSize);

// Setup
// ===========
void setup()
{
    statusFlags = MCUSR;
    MCUSR = 0x00;

#if defined(DEBUG) && defined(SERIAL_UI)
    SERIAL_UI.begin(115200);
#endif
    wdt_enable(WDTO_8S);

#ifdef PIN_DTR
    pinMode(PIN_DTR, OUTPUT);
    digitalWrite(PIN_DTR, HIGH);
#endif
#ifdef PIN_RESET
    pinMode(PIN_RESET, OUTPUT);
#endif

    uploadState = UploadState::Start;
}

void loop()
{
    wdt_reset();
    switch (uploadState) {

        case UploadState::Start:
            uploadState = sdBegin() && uiBegin() ? UploadState::CheckFlags : UploadState::Error;
            break;

        case UploadState::CheckFlags:
            if (statusFlags & WDRF) {
                ui.println(F("Uploader timed out, please reset"));
                uploadState = UploadState::Error;
            }
            else {
                uploadState = UploadState::StartSketch;
            }
            break;

        case UploadState::StartSketch:
            if (!sketch.begin()) {
                uploadState = UploadState::Error;
            } else {
                baudRate = getBaudRate();
                autoBaud = (baudRate == AUTO_BAUD_RATE) ? 0 : -1;
#ifndef SERIAL_TARGET
                ui.println(F("No programming serial, done."));
                uploadState = UploadState::Success;
#else
                uploadState = UploadState::Syncing;
#endif
            }
            break;

        case UploadState::Syncing:
            if (autoBaud < 0) {
                // Fixed
                uploadState = sync(baudRate) ?
                        UploadState::Identify : UploadState::Error;
            } else if (autoBaud < NUMITEMS(BAUD_RATES)) {
                // Auto-detect
                if (sync(BAUD_RATES[autoBaud])) {
                    uploadState = UploadState::Identify;
                } else {
                    autoBaud++;
                }
            } else {
                // auto-detect, out of options
                ui.println(F("Could not synchronize w/ target"));
                uploadState = UploadState::Error;
            }
            break;

        case UploadState::Identify:
            uploadState = readSignature() ? UploadState::Upload : UploadState::Error;
            break;

        case UploadState::Upload:
            uploadState = upload();
            break;

        case UploadState::Error:
            ui.println(F("Serial upload failed."));
            cleanup();
            uploadState = UploadState::ShowError;
            break;

        case UploadState::ShowError:
            blink(5);
            delay(800);
            break;

        case UploadState::Success:
            cleanup();
            uploadState = UploadState::ShowSuccess;
            break;

        case UploadState::ShowSuccess:
            blink(2);
            delay(800);
            break;
    }
}

uint32_t getBaudRate()
{
    BaudRateChoices choices;
    int8_t choice = ui.choose(choices);
    return (choice <= 0) ? AUTO_BAUD_RATE : BAUD_RATES[choice - 1];
}

bool sdBegin()
{
    sd.begin(SS_SD);
    if (sd.cardErrorCode()) {
#if defined(DEBUG) && defined(SERIAL_UI)
        SERIAL_UI.println(F("Initializing SD card"));
#endif
        return false;
    }
    return true;
}

bool uiBegin()
{
    if (!ui.begin()) {
#if defined(DEBUG) && defined(SERIAL_UI)
        SERIAL_UI.println(F("UI failed to start!"));
#endif
        return false;
    }
    ui.println(F("Serial uploader starting..."));
//    ui.flush();
    return true;
}

void cleanup()
{
    ui.end();
    SPI.end();
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
}

bool sync(uint32_t baudRate)
{
    wdt_reset();
    client.begin(baudRate);

    resetTarget();

    ui.print(F("Trying "));
    ui.print(baudRate);
    ui.print(F(" bps..."));
    // Sync with target
    uint8_t syncCount = 0;
    for (int i = 0; i < 8 && syncCount < 1; i++) {
        wdt_reset();
        Stk status = client.sync();
        if (status == Stk::OK) {
            syncCount++;
        }
    }
    if (syncCount < 1) {
        ui.println(F(" failed"));
        client.end();
        return false;
    }
    ui.println(F(" done."));
    ui.println(F("AVR device initialized and ready"));
    return true;
}

bool readSignature()
{
    StkResponse response;

    response = client.readSignature();
    if (!check(response.status, F("Could not read device signature"))) {
        return false;
    }
    ui.print(F("Device signature: "));
    for (int j = 0; j < response.size; ++j) {
        printByte(ui, response.data[j]);
    }
    ui.println();
    if (! identifyMCU(response.data, response.size)) {
        ui.println(F("Unknown MCU, need page size"));
        return false;
    }
    ui.print(F("AVR part: ")); ui.println(mcuSignature.desc);
    ui.print(F("Flash size: ")); ui.println(mcuSignature.flashSize);
    ui.print(F("Page size: ")); ui.println(mcuSignature.pageSize);

    // Get parameters, for fun
    response = client.getParameter(StkParam::SW_MAJOR);
    uint8_t major = response.data[0];
    response = client.getParameter(StkParam::STK_SW_MINOR);
    ui.print(F("Stk500 kit version: "));
    ui.print(major, DEC);
    ui.print('.');
    ui.println(response.data[0], DEC);
    wdt_reset();

    return true;
}

UploadState upload()
{
    StkResponse response;
    Stk status;

    // Program
    status = client.enterProgramming();
    if (!check(status, F("Could not enter programming mode"))) {
        return UploadState::Error;
    }
    ui.println(F("Writing flash..."));

    uint8_t hex[mcuSignature.pageSize];
    // byte count
    int count;
    uint32_t byteAddress = 0;
    do {
        count = sketch.readBytes(hex, mcuSignature.pageSize);

        if (count < 0) {
            ui.println(F("SD read failure!"));
            return UploadState::Error;
        }
        if (count > 0) {
            status = client.loadAddress(byteAddress / 2);
            if (!check(status, F("Could not load address."))) {
                return UploadState::Error;
            }
            status = client.writeFlash((uint16_t) count, hex);
            if (!check(status, F("Could not program page!"))) {
                return UploadState::Error;
            }
            byteAddress += count;
        }
        wdt_reset();

    } while (count > 0);

    ui.print(byteAddress);
    ui.println(F(" bytes of flash written."));

    ui.println(F("Verifying flash..."));

    sketch.reset();
    byteAddress = 0;
    client.resetAddress();
    uint8_t targetBuf[mcuSignature.pageSize];
    do {
        count = sketch.readBytes(hex, mcuSignature.pageSize);

        if (count < 0) {
            ui.println(F("SD read failure!"));
            return UploadState::Error;
        }
        if (count > 0) {
            status = client.loadAddress(byteAddress / 2);
            if (!check(status, F("Could not load address."))) {
                return UploadState::Error;
            }
            byteAddress += count;

            response = client.readFlash((uint16_t) count, targetBuf);
            if (!check(status, F("Could not read page!"))) {
                return UploadState::Error;
            }
            if (response.size != count) {
                ui.println(F("Read flash error: got "));
                ui.print(response.size);
                ui.print(F(" bytes out of "));
                ui.print(count);
                ui.println(F(" requested"));
                return UploadState::Error;
            }
            for (int i = 0; i < count; ++i) {
                if (hex[i] != targetBuf[i]) {
                    ui.print(F("Vrfy error at byte "));
                    ui.println(byteAddress + i, HEX);
                    ui.print(F("Exp: 0x"));
                    ui.print(hex[i], HEX);
                    ui.print(F(", Got: 0x"));
                    ui.println(targetBuf[i], HEX);
                    return UploadState::Error;
                }
            }
        }
        wdt_reset();

    } while (count > 0);

    ui.print(byteAddress);
    ui.println(F("b verified."));

    status = client.leaveProgramming();
    if (!check(status, F("Could not leave programming mode."))) {
        return UploadState::Error;
    }
    ui.println(F("Uploading done."));
//    client.end();
    return UploadState::Success;
}

bool identifyMCU(byte *sig, size_t sigSize)
{
    for (int j = 0; j < NUMITEMS (AVR_SIGNATURES); j++) {
        memcpy_P(&mcuSignature, &AVR_SIGNATURES[j], sizeof mcuSignature);

        if (memcmp(sig, mcuSignature.sig, sigSize) == 0) {
            return true;
        }
    }
    return false;
}

void blink(uint8_t count)
{
    for (int i = 0; i < count; ++i) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(75);
        digitalWrite(LED_BUILTIN, LOW);
        delay(75);
    }
}

bool check(Stk status, FString msg)
{
    if (status != Stk::OK) {
        ui.print(msg);
        ui.print(F(": 0x"));
        ui.println((uint8_t) status, HEX);
        return false;
    }
    return true;
}

void resetTarget()
{
#ifdef PIN_RESET
    ui.print(F("Resetting target board..."));
    // Reset the target board
    digitalWrite(PIN_RESET, LOW);
    delay(DELAY_RESET);
    digitalWrite(PIN_RESET, HIGH);
    delay(DELAY_RESET);
    ui.println(F(" done."));
#endif
}
