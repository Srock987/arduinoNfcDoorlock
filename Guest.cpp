#include "Guest.h"
#include "EepromUtil.h"





Guest::Guest(byte aId,String aName, String aKey){
id=aId;
name=aName;
key=aKey;
}

Guest::Guest(){}

Guest::Guest(NdefMessage message){
  name=EepromUtil::getStringFromRecord(message.getRecord(1));
  key=EepromUtil::getStringFromRecord(message.getRecord(2));
  
  byte payload[message.getRecord(3).getPayloadLength()];
  message.getRecord(3).getPayload(payload);
  id = payload[0];
}

byte Guest::getId(){
	return id;
}

void Guest::setId(byte aId){
	id = aId;
}

String Guest::getName(){
	return name;
}

String Guest::getKey(){
	return key;
}

static boolean Guest::compareGuests(Guest guest1, Guest guest2){
  if( guest1.getId()==guest2.getId() && 
      guest1.getName().equals(guest2.getName()) &&
      guest1.getKey().equals(guest2.getKey())){
        return true;
      }else return false;
}
	
