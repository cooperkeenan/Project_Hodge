#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <Arduino.h>

class LEDController {
public:
    void init();
    void blueLEDOn();
    void blueLEDOff();
    void amberLEDOn();
    void amberLEDOff();
    void pulseBlue();
    void pulseAmber();
    void flashError();
    
private:
    void pulse(int pin);
};

#endif