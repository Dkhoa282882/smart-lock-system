#pragma once
#include <Arduino.h>
#include <menu_state.h>

void LoadInitPassword();
void HandlePasswordInput(char key, PasswordState &st);