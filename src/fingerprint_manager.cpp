#include "fingerprint_manager.h"
#include "config.h"
#include "display.h"
#include "door_control.h"
#include "buzzer.h"
#include <HardwareSerial.h>
#include <Adafruit_Fingerprint.h>

static HardwareSerial FingerSerial(1);
static Adafruit_Fingerprint finger = Adafruit_Fingerprint(&FingerSerial);

QueueHandle_t fpResultQueue;
TaskHandle_t fingerprintHandle = NULL;

static void HandleVerifyFp();
static void HandleEnrollFp();
static void HandleDeleteFp();

void fingerprintTaskInit() {
    FingerSerial.begin(57600, SERIAL_8N1, 14, 13);
    finger.begin(57600);
    if (!finger.verifyPassword()) {
        Serial.println("Khong tim thay cam bien van tay!");
        lcdPrint("Khong tim thay", "cam bien !");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    fpResultQueue = xQueueCreate(1, sizeof(FpResultMsg));
    if (fpResultQueue == NULL) {
    Serial.println("ERROR: Khong tao duoc fpResultQueue!");
}
}

void HandleFingerMenu(char key, PasswordState &st){
    FpResultMsg resMsg;

    if (key == '1') {
        xTaskNotify(fingerprintHandle, FP_CMD_VERIFY, eSetValueWithOverwrite);         // Gửi mã lệnh FP_CMD_VERIFY = 1 cho task fingerprint thông qua con trỏ fingerprinthandle
        xQueueReceive(fpResultQueue, &resMsg, portMAX_DELAY);                          // nhận về giá trị từ task fingerprint gửi về bỏ vào biến resMSg

        if (resMsg.result == FP_RESULT_SUCCESS) {
            char buf[17];
            snprintf(buf, sizeof(buf), "ID: %d", resMsg.id);
            lcdPrint("Xac thuc thanh cong", buf);
            vTaskDelay(pdMS_TO_TICKS(1000));
            if (doorControlHandle != NULL && !doorBusy){
                doorBusy = true;
                xTaskNotifyGive(doorControlHandle);
            }
            st.mode = 0;
            st.inputPassword = "";
            return;    
        } else if (resMsg.result == FP_RESULT_FAIL) {
            beepError();
            lcdPrint("Van tay khong", "hop le !");
            vTaskDelay(pdMS_TO_TICKS(1000));
        } else {
            beepError();
            lcdPrint("Het thoi gian", "cho !");
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        MainMenu(st);
    } else if (key == '*') {
        xTaskNotify(fingerprintHandle, FP_CMD_ENROLL, eSetValueWithOverwrite);
        xQueueReceive(fpResultQueue, &resMsg, portMAX_DELAY);

        if (resMsg.result == FP_RESULT_SUCCESS) {
            char buf[17];
            beepSuccess();
            snprintf(buf, sizeof(buf), "ID: %d", resMsg.id);
            lcdPrint("Dang ky thanh cong", buf);
        } else if(resMsg.result == FP_RESULT_TIMEOUT){
            beepError();
            lcdPrint("Loi timeout!", "Dang ky that bai");
        }else{
            beepError();
            lcdPrint("Dang ky that bai!","");
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
        MainMenu(st);
    } else if (key == '#') {
        xTaskNotify(fingerprintHandle, FP_CMD_DELETE, eSetValueWithOverwrite);
        xQueueReceive(fpResultQueue, &resMsg, portMAX_DELAY);

        if (resMsg.result == FP_RESULT_SUCCESS) {
            char buf[17];
            beepSuccess();
            snprintf(buf, sizeof(buf), "ID: %d", resMsg.id);
            lcdPrint("Xoa thanh cong", buf);
        } else if (resMsg.result == FP_RESULT_FAIL && resMsg.id == -1){
            beepError();
            lcdPrint("Van tay chua", "duoc dang ky !");  
        } else if(resMsg.result == FP_RESULT_TIMEOUT){
            beepError();
            lcdPrint("Het thoi gian ", "cho !");
        }else{
            beepError();
            lcdPrint("Xoa that bai!","");
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
       MainMenu(st);
    }else if (key == '0'){
        MainMenu(st);
    }
}

void Fingerprint(void *pv){
    while(1){
        uint32_t cmd;
        if (xTaskNotifyWait(0, 0xFFFFFFFF, &cmd, portMAX_DELAY) == pdTRUE) {
            switch(cmd){
                case FP_CMD_VERIFY: HandleVerifyFp(); break;
                case FP_CMD_ENROLL: HandleEnrollFp(); break;
                case FP_CMD_DELETE: HandleDeleteFp(); break;
                default: Serial.println("Unknown FP command !"); break;
            }
        }
    }
}


static void HandleVerifyFp(){
    lcdPrint("Dat ngon tay vao", "cam bien...");
    FpResultMsg resMsg = { FP_RESULT_TIMEOUT, -1 };
    unsigned long startTime = millis();
    while (millis() - startTime < FP_VERIFY_TIMEOUT_MS) {
        int p = finger.getImage();
        if (p == FINGERPRINT_OK) {
            if (finger.image2Tz() == FINGERPRINT_OK) {
                if (finger.fingerSearch() == FINGERPRINT_OK) {
                    resMsg.result = FP_RESULT_SUCCESS;
                    resMsg.id = finger.fingerID;
                } else {
                    resMsg.result = FP_RESULT_FAIL;
                    resMsg.id = -1;
                }
            }
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    xQueueSend(fpResultQueue, &resMsg, portMAX_DELAY);
}

static void HandleDeleteFp(){
    lcdPrint("Dat ngon tay vao", "cam bien...");
    FpResultMsg resMsg = { FP_RESULT_TIMEOUT, -1 };
    unsigned long startTime = millis();

    while (millis() - startTime < FP_DELETE_TIMEOUT_MS) {
        int p = finger.getImage();
        if (p == FINGERPRINT_OK) {
            if (finger.image2Tz() == FINGERPRINT_OK && finger.fingerFastSearch() == FINGERPRINT_OK) {
                int id = finger.fingerID;
                if (finger.deleteModel(id) == FINGERPRINT_OK) {
                    resMsg.result = FP_RESULT_SUCCESS;
                    resMsg.id = id;
                } else {
                    resMsg.result = FP_RESULT_FAIL;
                    resMsg.id = id;
                }
            } else {
                resMsg.result = FP_RESULT_FAIL;
                resMsg.id = -1;
            }
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    xQueueSend(fpResultQueue, &resMsg, portMAX_DELAY);
}

static void HandleEnrollFp() {
   FpResultMsg resMsg = { FP_RESULT_FAIL, -1 };
    int id = -1;
    for (int i = 1; i <= 127; i++) {
    uint8_t p = finger.loadModel(i);
    if (p == FINGERPRINT_OK) continue;                                       // slot đã có template
    if (p == FINGERPRINT_PACKETRECIEVEERR) {
        vTaskDelay(pdMS_TO_TICKS(50));
        p = finger.loadModel(i);                                              // kiểm tra thêm 1 lần coi đúng là lỗi không
        if(p == FINGERPRINT_PACKETRECIEVEERR){
        Serial.println("[Enroll] Loi giao tiep cam bien khi quet ID, huy enroll");
        lcdPrint("Loi cam bien!", "");
        vTaskDelay(pdMS_TO_TICKS(1500));
        xQueueSend(fpResultQueue, &resMsg, portMAX_DELAY);
        return;
        }
        if (p == FINGERPRINT_OK) continue;
    }
    id = i;                                                         
    break;
}
    if (id == -1) {      
        Serial.println("[Enroll] FAIL: khong tim duoc ID trong (bo nho day)"); // sau khi chạy for vẫn = -1 thì bộ nhớ cảm biến đã đầy
        lcdPrint("Bo nho day!", "");
        vTaskDelay(pdMS_TO_TICKS(1500));
        xQueueSend(fpResultQueue, &resMsg, portMAX_DELAY); 
        return;
    }
    Serial.print("[Enroll] Tim duoc ID trong: "); Serial.println(id);
    lcdPrint("Dat ngon tay vao", "cam bien...");
    unsigned long startTime = millis();
    int p = -1;
    while (millis() - startTime < FP_ENROLL_TIMEOUT_MS) {                 // 10000
        p = finger.getImage();
        if (p == FINGERPRINT_OK) break;                           // chụp được ảnh vân tay thì thoát while
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    if (p != FINGERPRINT_OK) {
        Serial.println("[Enroll] FAIL: timeout cho anh lan 1");
        resMsg.result = FP_RESULT_TIMEOUT;                         // nếu không được báo lỗi timeout về handlefingermenu
        xQueueSend(fpResultQueue, &resMsg, portMAX_DELAY);
        return;
    }
    if (finger.image2Tz(1) != FINGERPRINT_OK) {                      // chuyển ảnh vân tay trên thành file mẫu các đặc trưng của vân tay đó
        Serial.println("[Enroll] FAIL: image2Tz(1) loi");
        xQueueSend(fpResultQueue, &resMsg, portMAX_DELAY);           // nếu không ok thì trả về FAIL
        return;
    }

    if (finger.fingerSearch() == FINGERPRINT_OK) {                 // kiểm tra mẫu vân tay trên có trùng với mẫu nào trong bộ nhớ không                      
        Serial.print("[Enroll] FAIL: van tay da ton tai, ID cu = ");
        Serial.println(finger.fingerID);
        lcdPrint("Van tay da ton", "tai !");
        vTaskDelay(pdMS_TO_TICKS(1500));
        xQueueSend(fpResultQueue, &resMsg, portMAX_DELAY); 
        return;
    }
    beepStepOk();
    lcdPrint("Bo ngon tay ra...", "");
    startTime = millis();
    while (finger.getImage() != FINGERPRINT_NOFINGER) {
        if (millis() - startTime > FP_REMOVE_FINGER_TIMEOUT_MS){          // 5000
        Serial.println("[Enroll] canh bao: qua 5s van chua bo tay ra");
        break;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    lcdPrint("Dat lai ngon tay...", "");                              // yêu cầu đặt lại ngón tay để lấy ảnh lần 2
    startTime = millis();
    p = -1;
    while (millis() - startTime < FP_ENROLL_TIMEOUT_MS) {                // 10000
        p = finger.getImage();
        if (p == FINGERPRINT_OK) break;
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    if (p != FINGERPRINT_OK) {
        Serial.println("[Enroll] FAIL: timeout cho anh lan 2");
        resMsg.result = FP_RESULT_TIMEOUT;
        xQueueSend(fpResultQueue, &resMsg, portMAX_DELAY);
        return;
    }
     if (finger.image2Tz(2) != FINGERPRINT_OK) {
        Serial.println("[Enroll] FAIL: image2Tz(2) loi");
        xQueueSend(fpResultQueue, &resMsg, portMAX_DELAY); // FAIL
        return;
    }
    beepStepOk();
    lcdPrint("Dang tao mau...", "");
    vTaskDelay(pdMS_TO_TICKS(500));
    Serial.println("[Enroll] image2Tz(2) OK");
    if (finger.createModel() != FINGERPRINT_OK) {                      // so khớp 2 mẫu ảnh nếu trùng thì tạo mẫu
        Serial.println("[Enroll] FAIL: createModel loi (2 anh khong khop hoac loi cam bien)");
        xQueueSend(fpResultQueue, &resMsg, portMAX_DELAY); // FAIL
        return;
    }
    Serial.println("[Enroll] createModel OK");
    if (finger.storeModel(id) == FINGERPRINT_OK) {                      // lưu mẫu trên vào id tìm được lúc đầu
        resMsg.result = FP_RESULT_SUCCESS;
        resMsg.id = id;
    }else{
        Serial.println("[Enroll] FAIL: storeModel loi (ghi flash that bai)");
    }
    xQueueSend(fpResultQueue, &resMsg, portMAX_DELAY);
}


