#include "keypad_task.h"
#include "config.h"
#include "buzzer.h"
#include <Keypad.h>

QueueHandle_t keyQueue;

static char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
static byte rowPins[KEYPAD_ROWS] = {19, 18, 5, 17};
static byte colPins[KEYPAD_COLS] = {16, 4, 15};

static Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

void keypadTaskInit() {
    keyQueue = xQueueCreate(10, sizeof(char));
    if (keyQueue == NULL) {
    Serial.println("ERROR: Khong tao duoc keyQueue!");
}
}

void Enterkeypad(void *pv) {
    while (1) {
        char key = keypad.getKey();
        if (key) {
            tone(BUZZER_PIN, 1200, 40);
            vTaskDelay(pdMS_TO_TICKS(60));
            noTone(BUZZER_PIN);
            if (xQueueSend(keyQueue, &key, pdMS_TO_TICKS(100)) != pdTRUE) {
                Serial.println("Queue full!");
            }
            Serial.println(key);
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}