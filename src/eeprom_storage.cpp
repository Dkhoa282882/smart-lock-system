#include "eeprom_storage.h"
#include "config.h"
#include <EEPROM.h>





void savePasswordToEEPROM(const String password) {
  for (int i = 0; i < password.length(); i++) {
    EEPROM.write(PASSWORD_ADDR + i, password[i]);
  }
  EEPROM.write(PASSWORD_ADDR + password.length(), '\0');
  EEPROM.commit();
}

String readPasswordFromEEPROM() {
  String password = "";
  char ch;
  for (int i = 0; i < 32; i++) {
    ch = EEPROM.read(PASSWORD_ADDR + i);
    if (ch == '\0') break;
    password += ch;
  }
  return password;
}