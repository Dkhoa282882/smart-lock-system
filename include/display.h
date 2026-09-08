#pragma once
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

extern LiquidCrystal_I2C lcd;
extern SemaphoreHandle_t lcdMutex;

void displayInit();                                    
void lcdPrint(const char* line0, const char* line1);