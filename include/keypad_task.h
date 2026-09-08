#pragma once
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

extern QueueHandle_t keyQueue;

void keypadTaskInit();          
void Enterkeypad(void *pv);     