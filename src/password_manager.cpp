#include "password_manager.h"
#include "config.h"
#include "display.h"
#include "buzzer.h"
#include "eeprom_storage.h"
#include "keypad_task.h"
#include "door_control.h"
#include <EEPROM.h>

static String correctPassword = "6789";

static void HandleNormalPasswordInput(char key, PasswordState &st);
static void HandleChangePasswordInput(char key, PasswordState &st);

void LoadInitPassword() {
    if (EEPROM.read(INIT_FLAG_ADDR) != 0xA5) {
        savePasswordToEEPROM(correctPassword);
        EEPROM.write(INIT_FLAG_ADDR, 0xA5);
        EEPROM.commit();
        Serial.println("EEPROM initialized with default password");
    }
    correctPassword = readPasswordFromEEPROM();
    Serial.print("Password loaded from EEPROM: '");
    Serial.print(correctPassword);
    Serial.println("'");
}

void HandlePasswordInput(char key, PasswordState &st){
    if (st.changePasswordMode) {
        HandleChangePasswordInput(key, st);
    } else {
        HandleNormalPasswordInput(key, st);
    }
}

static void HandleChangePasswordInput(char key, PasswordState &st){
    if (key == '*') {
        lcdPrint("Huy doi MK", "");
        vTaskDelay(pdMS_TO_TICKS(1000));
        MainMenu(st);
        return;
    }
    if (st.awaitingCurrentPassword) {
        if (key == '#') {
            if (st.inputPassword == correctPassword) {
                beepStepOk();
                st.awaitingCurrentPassword = false; st.awaitingNewPassword = true;
                lcdPrint("Nhap MK moi:", "");
                st.inputPassword = "";
            } else {
                beepError();
                lcdPrint("Sai MK hien tai", "");
                vTaskDelay(pdMS_TO_TICKS(1000));
                xQueueReset(keyQueue);
                st.inputPassword = "";
                lcdPrint("Nhap MK hien tai:", "");
            }
        } else {
            if (st.inputPassword.length() < MAX_PASS_LEN) {
                st.inputPassword += key;
                if (xSemaphoreTake(lcdMutex, pdMS_TO_TICKS(200)) == pdTRUE) {
                    lcd.setCursor(0, 1); lcd.print("                ");
                    lcd.setCursor(0, 1); lcd.print(st.inputPassword);
                    xSemaphoreGive(lcdMutex);
                }
            }
        }
    }
    else if (st.awaitingNewPassword) {
        if (key == '#') {
            if (st.inputPassword.length() >= MIN_PASS_LEN) {
                correctPassword = st.inputPassword;
                savePasswordToEEPROM(correctPassword);
                beepSuccess();
                lcdPrint("Da doi MK", "");
                vTaskDelay(pdMS_TO_TICKS(1000));
                MainMenu(st);
            } else {
                beepError();
                lcdPrint("MK moi >3 so", "");
                vTaskDelay(pdMS_TO_TICKS(1000));
                xQueueReset(keyQueue);
                st.inputPassword = "";
                lcdPrint("Nhap MK moi:", "");
            }
        } else {
            if (st.inputPassword.length() < MAX_PASS_LEN) {
                st.inputPassword += key;
                if (xSemaphoreTake(lcdMutex, pdMS_TO_TICKS(200)) == pdTRUE) {
                    lcd.setCursor(0, 1); lcd.print("                ");
                    lcd.setCursor(0, 1); lcd.print(st.inputPassword);
                    xSemaphoreGive(lcdMutex);
                }
            }
        }
    }
}

static void HandleNormalPasswordInput(char key, PasswordState &st){
    if (key == '*') {
        st.inputPassword = "";
        st.changePasswordMode = true;
        st.awaitingCurrentPassword = true;
        lcdPrint("Doi mat khau", "");
        vTaskDelay(pdMS_TO_TICKS(1000));
        lcdPrint("Nhap MK hien tai:", "");
    }
    else {
        if (key == '#') {
            if (st.inputPassword == correctPassword) {
                lcdPrint("Xac thuc MK", "thanh cong !");
                vTaskDelay(pdMS_TO_TICKS(500));
                if (doorControlHandle != NULL && !doorBusy) {
                    doorBusy = true;
                    xTaskNotifyGive(doorControlHandle);
                }
                st.failedAttempts = 0;
                st.mode = 0;
            } else {
                st.failedAttempts++;
                beepError();
                lcdPrint("Sai mat khau!", "");
                vTaskDelay(pdMS_TO_TICKS(500));
                int lockTime = 0;
                if (st.failedAttempts == 2) lockTime = LOCK_TIME_2ND_FAIL_MS;
                else if (st.failedAttempts >= 3) lockTime = LOCK_TIME_3RD_FAIL_MS;
                if (lockTime > 0) {
                    char buf[17];
                    snprintf(buf, sizeof(buf), "%d giay", lockTime / 1000);
                    lcdPrint("Thu lai sau", buf);
                    vTaskDelay(pdMS_TO_TICKS(lockTime));
                    xQueueReset(keyQueue);
                }
                lcdPrint("Nhap mat khau:", "");
            }
            st.inputPassword = "";
        } else {
            if (st.inputPassword.length() < MAX_PASS_LEN) {
                st.inputPassword += key;
                if (xSemaphoreTake(lcdMutex, pdMS_TO_TICKS(200)) == pdTRUE) {
                    lcd.setCursor(0, 1); lcd.print("                ");
                    lcd.setCursor(0, 1); lcd.print(st.inputPassword);
                    xSemaphoreGive(lcdMutex);
                }
            }
        }
    }
}