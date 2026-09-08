#include<Arduino.h>
#include"config.h"
#include"eeprom_storage.h"
#include"buzzer.h"
#include"display.h"
#include"keypad_task.h"
#include"fingerprint_manager.h"
#include"door_control.h"
#include"password_manager.h"
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include<freertos/queue.h>
#include<freertos/semphr.h>
#include<EEPROM.h>


 
void Systemlogic(void *pv){
  static PasswordState st;
  char key;
  while(1){
        BaseType_t got = xQueueReceive(keyQueue, &key, portMAX_DELAY);

        if (got == pdTRUE) {
          if(doorBusy == true){
            continue;                           // bỏ  qua các phím nhấn trong lúc chốt cửa đang mở 
          }
            if (st.mode == 0) {
                if (key == '1') {
                    st.mode = 1;
                    st.inputPassword = "";
                    lcdPrint("Nhap mat khau:", "");
                } else if (key == '2') {
                   st.mode = 2;
                   lcdPrint("1: Xac thuc", "*:Dang ky|#:Xoa");
                }
            } 
            else if (st.mode == 1) {
                HandlePasswordInput(key, st);
            } 
            else if (st.mode == 2){
                HandleFingerMenu( key, st);
            }
        } 
    
    }
  }
 

void setup() {
  Serial.begin(9600);
  EEPROM.begin(EEPROM_SIZE);
  displayInit();                                                                // khởi tạo LCD , tạo mutex cho lcd
  buzzerInit();
  DoorControlInit();
  fingerprintTaskInit();                                                        // khởi tạo cảm biến vân tay , tạo queue chứa giá trị trả về
  LoadInitPassword();                                                           // load mật khẩu đọc từ EEPROM
  keypadTaskInit();                                                             // tạo queue hứng các phím từ keypad
  xTaskCreate(Enterkeypad, "task nhap phim", 2048, NULL, 1, NULL);
  xTaskCreate(Systemlogic, "task logic", 4096, NULL, 2, NULL);
  xTaskCreate(Fingerprint, "task van tay", 4096, NULL, 1, &fingerprintHandle);
  xTaskCreate(DoorControl, "task kiem soat cua", 2048, NULL, 3, &doorControlHandle);
  vTaskDelay(pdMS_TO_TICKS(100));
  lcdPrint("1: Nhap Mat khau", "2: Van tay");                                   // Hiện thị menu tuỳ chọn ngay lúc đầu
}

void loop() {
}


