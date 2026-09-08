#pragma once
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include "menu_state.h"

typedef enum { FP_CMD_VERIFY = 1, FP_CMD_ENROLL = 2, FP_CMD_DELETE = 3 } FpCommand;
typedef enum { FP_RESULT_SUCCESS, FP_RESULT_FAIL, FP_RESULT_TIMEOUT } FpResult;

struct FpResultMsg {
    FpResult result;
    int id;
};

extern QueueHandle_t fpResultQueue;
extern TaskHandle_t fingerprintHandle;

void fingerprintTaskInit();      // khởi tạo cảm biến + queue, gọi 1 lần trong setup()
void Fingerprint(void *pv); 
void HandleFingerMenu(char key, PasswordState &st);