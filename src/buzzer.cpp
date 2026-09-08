#include "buzzer.h"
#include "config.h"


void buzzerInit() {
    pinMode(BUZZER_PIN, OUTPUT);
}

void beepStepOk() {                            // gọi khi thực hiện bước nào đó thành công
    tone(BUZZER_PIN, 1500, 100);
    vTaskDelay(pdMS_TO_TICKS(120));
    noTone(BUZZER_PIN);
}

void beepSuccess() {                           // báo thành công khi hoàn thành
    tone(BUZZER_PIN, 1200, 60);
    vTaskDelay(pdMS_TO_TICKS(80));
    tone(BUZZER_PIN, 1600, 60);
    vTaskDelay(pdMS_TO_TICKS(80));
    tone(BUZZER_PIN, 2000, 100);
    vTaskDelay(pdMS_TO_TICKS(120));
    noTone(BUZZER_PIN);
}

void beepError() {                            // báo lỗi khi thất bại
    tone(BUZZER_PIN, 250, 120);
    vTaskDelay(pdMS_TO_TICKS(120));
    tone(BUZZER_PIN, 200, 180);
    vTaskDelay(pdMS_TO_TICKS(180));
    noTone(BUZZER_PIN);
}
