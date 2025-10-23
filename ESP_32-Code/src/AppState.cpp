#include "AppState.h"

AppState::AppState(StorageManager& storageManager) : storage(storageManager) {}

void AppState::init() {
    lastMessageId = storage.getLastMessageId();
    
    if (lastMessageId.length() > 0) {
        Serial.println("Loaded last message ID: " + lastMessageId);
    } else {
        Serial.println("No previous message ID found");
    }
}

bool AppState::hasNewMessage(const String& messageId) {
    if (messageId.length() > 0 && messageId != lastMessageId) {
        lastMessageId = messageId;
        storage.saveLastMessageId(messageId);
        return true;
    }
    return false;
}