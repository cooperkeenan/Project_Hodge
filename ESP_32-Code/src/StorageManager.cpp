#include <Preferences.h>
#include "StorageManager.h"

const char* StorageManager::NAMESPACE = "eink";
const char* StorageManager::LAST_MESSAGE_KEY = "lastMsgId";

Preferences preferences;

void StorageManager::init() {
    preferences.begin(NAMESPACE, false);
}

String StorageManager::getLastMessageId() {
    return preferences.getString(LAST_MESSAGE_KEY, "");
}

void StorageManager::saveLastMessageId(const String& messageId) {
    preferences.putString(LAST_MESSAGE_KEY, messageId);
    Serial.println("Saved message ID to storage: " + messageId);
}