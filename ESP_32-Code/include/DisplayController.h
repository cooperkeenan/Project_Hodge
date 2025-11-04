#ifndef DISPLAY_CONTROLLER_H
#define DISPLAY_CONTROLLER_H

#include <GxEPD2_BW.h>
#include <Arduino.h>

class DisplayController {
public:
    void init();
    void powerOn();
    void powerOff();
    void showMessage(const char* message);
    void displayImage(uint8_t* imageData, size_t size);
    void clear();
    
private:
    GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT>* display;
    bool isPowered;
};

#endif