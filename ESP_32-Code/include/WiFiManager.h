#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>

class WiFiManager {
public:
    bool connect();
    bool isConnected();
    void printStatus();
    
private:
    const int MAX_RETRIES = 20;
};

#endif