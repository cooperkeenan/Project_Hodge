#ifndef MESSAGE_CHECKER_H
#define MESSAGE_CHECKER_H

#include "FirebaseClient.h"
#include "LEDController.h"
#include "AppState.h"

class MessageChecker {
private:
    FirebaseClient& firebase;
    LEDController& led;
    AppState& state;
    
public:
    MessageChecker(FirebaseClient& fb, LEDController& ledCtrl, AppState& appState);
    void check();
};

#endif