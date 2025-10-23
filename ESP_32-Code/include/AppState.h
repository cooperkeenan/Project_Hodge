#ifndef APP_STATE_H
#define APP_STATE_H

#include <Arduino.h>
#include "StorageManager.h"

class AppState {
private:
    String lastMessageId;
    StorageManager& storage;
    
public:
    AppState(StorageManager& storageManager);
    void init();
    bool hasNewMessage(const String& messageId);
};

#endif