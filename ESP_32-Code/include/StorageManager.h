#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <Arduino.h>

class StorageManager {
private:
    static const char* NAMESPACE;
    static const char* LAST_MESSAGE_KEY;
    
public:
    void init();
    String getLastMessageId();
    void saveLastMessageId(const String& messageId);
};

#endif