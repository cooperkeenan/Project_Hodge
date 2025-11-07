#ifndef DISPLAY_CONTROLLER_H
#define DISPLAY_CONTROLLER_H

#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <SD.h>

class DisplayController {
public:
    void init();
    void powerOn();
    void powerOff();
    void showMessage(const char* message);
    void clear();
    void displayImageFromSD(const String& filename);  // NEW!
    
private:
    GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT>* display = nullptr;
    bool isPowered = false;
};

#endif