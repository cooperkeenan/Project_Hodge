#ifndef FIREBASE_CLIENT_H
#define FIREBASE_CLIENT_H

#include <Arduino.h>

class FirebaseClient {
private:
    String buildURL(const String& path);
    
public:
    String getLatestMessageId();
};

#endif