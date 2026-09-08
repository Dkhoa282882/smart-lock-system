#include "menu_state.h"
#include "keypad_task.h"
#include "display.h"

void MainMenu(PasswordState &st){
    xQueueReset(keyQueue);
    st.mode = 0;
    st.changePasswordMode = false;
    st.awaitingCurrentPassword = false;
    st.awaitingNewPassword = false;
    st.inputPassword = "";
    lcdPrint("1: Nhap Mat khau", "2: Van tay");
}