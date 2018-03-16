# SerialUploader
Serial upload sketch for Arduino.

This sketch turns your Arduino into a standalone serial programmer.
How to use:

1. Prepare a programmer board with an SD card
2. Set up preferences in SerialUploader.h
3. Upload the SerialUploader.ino sketch to your programmer board
4. Compile the sketch you want to program (e.g. using the Arduino IDE)
   (_Remember to adjust the IDE board settings for the target board!_)
5. Locate the generated <sketch>.ino.hex file on your file system, and copy it to the SD card
6. Move the SD card over to the programmer board
7. Connect the programmer board to the target board:
    - programmer            -> target
    - Vin                   -> Vin
    - Gnd                   -> Gnd
    - PIN_RESET             -> Rst
    - SERIAL_TARGET Rx pin  -> target's Tx pin
    - SERIAL_TARGET Tx pin  -> target's Rx pin
8. Reset the programmer to start the upload sketch

The built-in led on the programmer will show:
- steady light while the upload is in progress
- sequences of 2 quick blinks if the upload succeeded
- sequences of 5 quick blinks if the upload failed

The sketch will try standard upload speeds down from 115200 bps. This works well, but as an alternative you can add a
boards.txt file on the SD card: the sketch will find the first occurence of "upload.speed=" in the file, and will use
the upload speed specified there.
Sample file:
    # Upload speed for serial uploader
    optibootm32.upload.speed=38400

The sketch appends to an upload.log file on the SD card: you can check that on the computer to see what happened.

Tested configurations:
- mega2560 clone programmer uploading through Serial1
- (coming next) Uno programmer uploading through Serial
- Uno target board, optiboot, 115200
- Pro mini target board @ 8MHz, minicore, 38400

## Credits:
- https://github.com/robokoding/STK500
- https://github.com/Optiboot/optiboot
