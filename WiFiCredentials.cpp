#include "WiFiCredentials.h"
#include "EepromUtil.h"

#define SAVE_INDEX 900
#define STRING_SPACE 20 

WiFiCredentials::WiFiCredentials(String aSsid, String aPassword){
	ssid=aSsid;
	password=aPassword;
}


boolean WiFiCredentials::saveCredentials(){
  char saveSsid[STRING_SPACE]={0};
  char savePassword[STRING_SPACE]={0};
  strcpy(saveSsid, ssid.c_str());
  strcpy(savePassword, password.c_str());
  EepromUtil::eeprom_write_string(SAVE_INDEX,&saveSsid[0]);
  EepromUtil::eeprom_write_string(SAVE_INDEX + STRING_SPACE,&savePassword[0]);
}

boolean WiFiCredentials::loadCredentials(){
  char loadSsid[STRING_SPACE]={0};
  char loadPassword[STRING_SPACE]={0};
  EepromUtil::eeprom_read_string(SAVE_INDEX,&loadSsid[0],STRING_SPACE);
  EepromUtil::eeprom_read_string(SAVE_INDEX + STRING_SPACE,&loadPassword[0],STRING_SPACE);
  ssid=loadSsid;
  password=loadPassword;
}



String WiFiCredentials::getSsid(){
	return ssid;
}
String WiFiCredentials::getPassword(){
	return password;
}
