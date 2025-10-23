#ifndef FIREBASE_CLIENT_H
#define FIREBASE_CLIENT_H

#include <Arduino.h>

String getLatestMessageId();
bool downloadImage(String messageId);

#endif