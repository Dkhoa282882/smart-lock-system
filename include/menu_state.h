#pragma once
#include <Arduino.h>

struct PasswordState {
    uint8_t mode = 0;
    String inputPassword = "";
    bool changePasswordMode = false;
    bool awaitingCurrentPassword = false;
    bool awaitingNewPassword = false;
    uint8_t failedAttempts = 0;
};

void MainMenu(PasswordState &st);