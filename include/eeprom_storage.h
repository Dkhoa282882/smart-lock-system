#pragma once
#include <Arduino.h>




void savePasswordToEEPROM(const String password);
String readPasswordFromEEPROM();