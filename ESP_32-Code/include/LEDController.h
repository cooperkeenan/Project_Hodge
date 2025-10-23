#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

class LEDController {
public:
    void init();
    void off();
    void flashNewMessage();
    void blinkConnecting();
};

#endif