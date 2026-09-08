#include "display.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
SemaphoreHandle_t lcdMutex;

void displayInit() {
    lcd.init();
    lcd.backlight();
    lcdMutex = xSemaphoreCreateMutex();
}

void lcdPrint(const char* line0, const char* line1) {
    if (xSemaphoreTake(lcdMutex, pdMS_TO_TICKS(200)) == pdTRUE) {
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print(line0);
        if (line1) { lcd.setCursor(0, 1); lcd.print(line1); }
        xSemaphoreGive(lcdMutex);
    }
}