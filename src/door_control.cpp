#include"buzzer.h"
#include"config.h"
#include"display.h"
#include"door_control.h"

TaskHandle_t doorControlHandle = NULL;
volatile bool doorBusy = false; 

void DoorControlInit(){
    pinMode(RELAY_PIN, OUTPUT);digitalWrite(RELAY_PIN, LOW);
}

void DoorControl(void *pv){
        while(1){
        if(ulTaskNotifyTake(pdTRUE, portMAX_DELAY)){
         beepSuccess();
         lcdPrint(" Dang mo cua...","");Serial.print("Dang mo cua...");
         digitalWrite(RELAY_PIN, HIGH);
         vTaskDelay(pdMS_TO_TICKS(DOOR_OPEN_TIME_MS));
         digitalWrite(RELAY_PIN, LOW);
         lcdPrint("1: Nhap Mat khau", "2: Van tay");
         doorBusy = false;
        }
    }
}