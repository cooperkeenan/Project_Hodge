#ifndef FIREBASE_CLIENT_H
#define FIREBASE_CLIENT_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

class FirebaseClient {
public:
    bool checkForNewImage(String& imageUrl, unsigned long& timestamp);
    bool downloadImage(const String& url, uint8_t** buffer, size_t* size);
    
private:
    String getLatestImageUrl();
    unsigned long lastCheckedTimestamp = 0;
};

#endif