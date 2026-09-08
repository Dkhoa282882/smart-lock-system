#pragma once
#include<Arduino.h>

#define EEPROM_SIZE 64                            // EEPROM
#define PASSWORD_ADDR 0
#define INIT_FLAG_ADDR 63

constexpr uint16_t RELAY_PIN  = 26;               // chân phần cứng
constexpr uint16_t BUZZER_PIN = 32;

constexpr byte KEYPAD_ROWS = 4;                   // keypad 3x4
constexpr byte KEYPAD_COLS = 3;

constexpr uint8_t MIN_PASS_LEN = 4;               // độ dài mật khẩu
constexpr uint8_t MAX_PASS_LEN = 6;

constexpr uint32_t LOCK_TIME_2ND_FAIL_MS = 5000;     // thời gian khoá khi nhập sai
constexpr uint32_t LOCK_TIME_3RD_FAIL_MS = 10000;

constexpr uint32_t FP_VERIFY_TIMEOUT_MS = 5000;      // timeout khi dùng vân tay
constexpr uint32_t FP_ENROLL_TIMEOUT_MS = 10000;
constexpr uint32_t FP_DELETE_TIMEOUT_MS = 5000;
constexpr uint32_t FP_REMOVE_FINGER_TIMEOUT_MS = 5000;

constexpr uint32_t DOOR_OPEN_TIME_MS = 5000;         // thời gian chốt mở 
