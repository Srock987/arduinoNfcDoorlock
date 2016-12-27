#include "KeyManager.h"



#define STRING_SPACE 10
#define ARRAY_SIZE 10
#define LIST_BYTE_INDEX 1014

KeyManager::KeyManager(void){
  Serial.begin(9600); 
	initIndexes();
}

void KeyManager::initIndexes(void){
		EepromUtil::eeprom_read_bytes(LIST_BYTE_INDEX,entries,ARRAY_SIZE);
    int size = sizeof(entries)/sizeof(entries[0]);
    Serial.println(size); 
}

Guest KeyManager::addGuest(Guest guest){ 
  char saveKey[STRING_SPACE]={0};
  char saveName[STRING_SPACE]={0};
  strcpy(saveKey, guest.getKey().c_str());
  strcpy(saveName, guest.getName().c_str());
	for (int i = 0; i < sizeof(entries)/sizeof(entries[0]); ++i)
		{
      Serial.println(i); 
			if (entries[i]==0)
			{
				if (EepromUtil::eeprom_write_string(i * 2 *STRING_SPACE, &saveKey[0]) &&
				EepromUtil::eeprom_write_string((i * 2 + 1)* STRING_SPACE, &saveName[0]))
				{
          unsigned char a[1] = {1};
					if(EepromUtil::eeprom_write_bytes(LIST_BYTE_INDEX+i,a,1))
					{
						entries[i]=1;
						guest.setId(i);
            Serial.println("saved");  
            delay(250);
					}
				}

				break;
			}
		}
   Serial.println("savedGuest:");  
   Serial.println(guest.getName());  
   Serial.println(guest.getKey());  
   Serial.println(guest.getId());  
	return guest;
}

boolean KeyManager::removeGuest(byte id){
	if(EepromUtil::eeprom_write_string(LIST_BYTE_INDEX+id,0)){
			entries[id]=0;
			return true;
	}
	return false;
}

Guest KeyManager::getGuest(byte id){
    char loadKey[STRING_SPACE]={0};
    char loadName[STRING_SPACE]={0};
    if(EepromUtil::eeprom_read_string( id * 2 * STRING_SPACE, &loadKey[0],STRING_SPACE ) &&
    EepromUtil::eeprom_read_string( (id * 2 + 1) * STRING_SPACE, &loadName[0], STRING_SPACE ))
    {
        Serial.println("loadSucces");  
    }else{
        Serial.println("loadFail");          
    }
		return Guest(id,loadName,loadKey);
}

boolean KeyManager::guestExists(byte id){
		if (entries[id]==1){
			return true;
		}
		return false;
}

byte KeyManager::guestCount(){
	byte count = 0;
	for (int i = 0; i < sizeof(entries)/sizeof(entries[0]); ++i)
	{
		if (entries[i]==1)
		{
			count++;
		}
	}
	return count;
}






	
