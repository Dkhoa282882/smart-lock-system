#pragma once
#include<Arduino.h>
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>

extern TaskHandle_t doorControlHandle;
extern volatile bool doorBusy;

void DoorControlInit();
void DoorControl(void *pv);