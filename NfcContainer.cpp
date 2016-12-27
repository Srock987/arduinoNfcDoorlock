#include "NfcContainer.h"


NfcContainer::NfcContainer(NdefMessage message)
  {
  int recordCount = message.getRecordCount();
  for(int i = 0 ; i < recordCount ; i++)
    {
      if(i==0)
      {
        messageType = getStringFromRecord(message.getRecord(i));
      }
      if(i==1)
      {
        messageContent = getStringFromRecord(message.getRecord(i));
      }
      if(i==2)
      {
        extraContent = getStringFromRecord(message.getRecord(i));
      }
      if (i==3)
      {
        byte payload[message.getRecord(i).getPayloadLength()];
        message.getRecord(i).getPayload(payload);
        byteContent = payload[0];
      }
    }
  }

String NfcContainer::getStringFromRecord(NdefRecord record){
  int payloadLength = record.getPayloadLength();
  byte payload[payloadLength];
  record.getPayload(payload);

  String payloadAsString = "";
  for (int c = 0; c < payloadLength; c++) {
    payloadAsString += (char)payload[c];
  }
  return payloadAsString;
}

String NfcContainer::getType(){
  return messageType;
}

String NfcContainer::getContent(){
  return messageContent;
}

String NfcContainer::getExtraContent(){
  return extraContent;
}

byte NfcContainer::getByteContent(){
  return byteContent;
}

