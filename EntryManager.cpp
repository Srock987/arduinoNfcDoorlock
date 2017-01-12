#include "EntryManager.h"
#include "EepromUtil.h"


#define STRING_SPACE 10
#define LONG_SPACE 4
#define STARTING_ADDRES 700
#define ENTRIES_SIZE 4

EntryManager::EntryManager(void){
	for(int i = 0 ; i < ENTRIES_SIZE ; i++){
//		byte readByte = EEPROM.read(STARTING_ADDRES+i);
//		if(readByte==1){
//		entries[i]=readByte;
//		}else{
		entries[i]=0;	
//		}
	}
}

void EntryManager::saveEntry(Entry entry){
    char userName[STRING_SPACE]={0};
    strcpy(userName, entry.getUserName().c_str());
  	for(int i = 0 ; i < ENTRIES_SIZE ; i++){
  		if(entries[i]==0){
      			if(EepromUtil::eeprom_write_string(STARTING_ADDRES+i*(STRING_SPACE+LONG_SPACE), &userName[0])){
      				EepromUtil::EEPROMWritelong(STARTING_ADDRES+STRING_SPACE+i*(STRING_SPACE+LONG_SPACE),entry.getTimeStamp());
      				entries[i]=1;
      				break;
      			}else if(i==3){
      				moveListUp();
      				if(EepromUtil::eeprom_write_string(STARTING_ADDRES+i*(STRING_SPACE+LONG_SPACE), &userName[0])){
      				EepromUtil::EEPROMWritelong(STARTING_ADDRES+STRING_SPACE+i*(STRING_SPACE+LONG_SPACE),entry.getTimeStamp());
      				entries[i]=1;
      				break;
      			    }
      		   }
      	}
    }
}

void EntryManager::moveListUp(){
	for(int i = 1 ; i < ENTRIES_SIZE ; i++){
		char userCharName[STRING_SPACE]={0};
		EepromUtil::eeprom_read_string( STARTING_ADDRES+i*(STRING_SPACE+LONG_SPACE), &userCharName[0],STRING_SPACE );
		long timeStamp = EepromUtil::EEPROMReadlong(STARTING_ADDRES+STRING_SPACE+i*(STRING_SPACE+LONG_SPACE));
		
		EepromUtil::eeprom_write_string(STARTING_ADDRES+(i-1)*(STRING_SPACE+LONG_SPACE), userCharName);
		EepromUtil::EEPROMWritelong(STARTING_ADDRES+STRING_SPACE+(i-1)*(STRING_SPACE+LONG_SPACE),timeStamp);
	}
}

Entry EntryManager::loadEntry(int index){
	char userCharName[STRING_SPACE]={0};
	EepromUtil::eeprom_read_string( STARTING_ADDRES+index*(STRING_SPACE+LONG_SPACE), &userCharName[0],STRING_SPACE );
	long timeStamp = EepromUtil::EEPROMReadlong(STARTING_ADDRES+STRING_SPACE+index*(STRING_SPACE+LONG_SPACE));
	Entry entry(userCharName,timeStamp);
	return entry;
}

int EntryManager::getEntryCount(){
	int count = 0;
	for(int i = 0 ; i < ENTRIES_SIZE ; i++){
		if (entries[i]==1)
		{
			count++;
		}
	}
	return count;
}


