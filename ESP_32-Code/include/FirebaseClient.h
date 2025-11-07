#ifndef FIREBASE_CLIENT_H
#define FIREBASE_CLIENT_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

class FirebaseClient {
public:
    bool checkForNewImage(String& imageUrl, unsigned long long& timestamp);
    bool downloadImageToSD(const String& url, const String& filename);  // NEW!
    bool downloadImage(const String& url, uint8_t** buffer, size_t* size);  // Old (deprecated)
    
private:
    String getLatestImageUrl();
    unsigned long long lastCheckedTimestamp = 0;
};

#endif