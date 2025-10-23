#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "LEDController.h"

class WiFiManager {
private:
    LEDController& led;
    
public:
    WiFiManager(LEDController& ledController);
    bool connect();
    bool isConnected();
};

#endif