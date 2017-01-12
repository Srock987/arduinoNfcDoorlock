#include "KeyManager.h"



#define STRING_SPACE 10
#define ARRAY_SIZE 10
#define LIST_BYTE_INDEX 1014

KeyManager::KeyManager(void){
	initIndexes();
}

void KeyManager::initIndexes(void){
  for(int i = 0 ; i < ARRAY_SIZE ; i++){
    byte readByte = EEPROM.read(LIST_BYTE_INDEX+i);
    if(readByte == 1){ 
      guestIdArray[i]=readByte;
    }else{
      guestIdArray[i]=0;
    }
  }
}

Guest KeyManager::addGuest(Guest guest){ 
  char saveKey[STRING_SPACE]={0};
  char saveName[STRING_SPACE]={0};
  strcpy(saveKey, guest.getKey().c_str());
  strcpy(saveName, guest.getName().c_str());
	for (int i = 0; i < ARRAY_SIZE; ++i)
		{
			if (guestIdArray[i]==0)
			{
				if (EepromUtil::eeprom_write_string(i * 2 *STRING_SPACE, &saveKey[0]) &&
				EepromUtil::eeprom_write_string((i * 2 + 1)* STRING_SPACE, &saveName[0]))
				{
            EEPROM.write(LIST_BYTE_INDEX+i, 1);
						guestIdArray[i]=1;
						guest.setId(i);

  				}

				break;
			}
		}
	return guest;
}

boolean KeyManager::removeGuest(byte id){
      EEPROM.write(LIST_BYTE_INDEX+id, 0);
			guestIdArray[id]=0;
			return true;
}

Guest KeyManager::getGuest(byte id){
    char loadKey[STRING_SPACE]={0};
    char loadName[STRING_SPACE]={0};
    if(EepromUtil::eeprom_read_string( id * 2 * STRING_SPACE, &loadKey[0],STRING_SPACE ) &&
    EepromUtil::eeprom_read_string( (id * 2 + 1) * STRING_SPACE, &loadName[0], STRING_SPACE ))
		return Guest(id,loadName,loadKey);
}

boolean KeyManager::guestExists(byte id){
		if (guestIdArray[id]==1){
			return true;
		}
		return false;
}

byte KeyManager::guestCount(){
	byte count = 0;
	for (int i = 0; i < 10; ++i)
	{
		if (guestIdArray[i]==1)
		{
			count++;
		}
	}
	return count;
}






	
