#include "NfcContainer.h"
#include <Arduino.h>
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <snep.h>
#include <NfcAdapter.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include "KeyManager.h"
#include "WiFiCredentials.h"


  SoftwareSerial mySerial(11, 12);
  PN532_SPI pn532spi(SPI, 10);
  SNEP nfc(pn532spi);
  uint8_t ndefBuf[255];
  KeyManager keyManager;
  boolean guestSaved = false;
  boolean credentialsSaved = false;
  
  const String MASTER_KEY = "MASTER_KEY";
  const String NFC_TAG_RECIEVED_MASTER_INITALIZATION = "MASTER_INITALIZATION";
  const String NFC_TAG_RECIEVED_WIFI_INITAIZATION = "WIFI_INITALIZATION";
  const String NFC_TAG_RECIEVED_DOOR_OPENING = "DOOR_OPENING";
  
  const String NFC_TAG_SENT_MASTER_CHECK = "MASTER_CHECK";
  const String NFC_TAG_SENT_WIFI_CHECK = "WIFI_CHECK";
  const String NFC_TAG_SENT_DOOR_CHECK = "DOOR_CHECK";
  
  const String NFC_TAG_SUCCES = "SUCCES";
  const String NFC_TAG_FALIURE = "FALIURE";

  const String WIFI_TAG_GUESTLIST_REQUEST = "WIFI_TAG_GUESTLIST_REQUEST";
  const String WIFI_TAG_ADD_GUEST_REQUEST = "WIFI_TAG_ADD_GUEST_REQUEST";

void setup() {
  Serial.begin(9600); 
  mySerial.begin(9600);
}

void loop() {
    //readNfc();
    handleSerialRequest();
    //delay(500);
}

void readNfc(){
      int msgSize = nfc.read(ndefBuf, sizeof(ndefBuf));
    if (msgSize > 0) {
        NdefMessage msg  = NdefMessage(ndefBuf, msgSize);
        msg.print();
        detectFunction(msg);
    }
}

void handleSerialRequest(){
    if (mySerial.available()) {
    String request = mySerial.readString();
    Serial.println(request);
    writeJsonResponse(request,mySerial);
  }
}

void writeJsonResponse(String request,SoftwareSerial mySerial){

  if(request.equals(WIFI_TAG_GUESTLIST_REQUEST)){
    int guestCount = keyManager.guestCount();
    Guest guestArray[guestCount];
    int guestIndex = 0;
    for(int i = 0 ; i < 10 ; i++){
      if(keyManager.guestExists(i)){
        guestArray[guestIndex] = keyManager.getGuest(i);
        guestIndex++;
      }
    }
    StaticJsonBuffer<200> jsonBuffer;
    JsonArray&  rootArray  = jsonBuffer.createArray();
    for(int i = 0 ; i < guestCount ; i++){
      Serial.println(guestArray[i].getName());
      JsonObject& object = rootArray.createNestedObject();
      encodeGuest(guestArray[i], object);
    }
    rootArray.printTo(mySerial);
    rootArray.printTo(Serial);
    mySerial.write("\r");
  }else{
    String test = request.substring(0,request.indexOf("\n"));
    Serial.println(test);
  if(request.substring(0,request.indexOf("\n")).equals(WIFI_TAG_ADD_GUEST_REQUEST)){
    String test1 = request.substring(request.indexOf("\n"));
    Serial.println(test1);
    Guest decodedGuest = decodeGuest(request.substring(request.indexOf("\n")));
    decodedGuest = keyManager.addGuest(decodedGuest);
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& response = jsonBuffer.createObject();
    encodeGuest(decodedGuest, response);
    response.printTo(mySerial);
    response.printTo(Serial);
    mySerial.write("\r");
  }
  }
}

JsonObject& encodeGuest(Guest guest, JsonObject& object){
   Serial.println("SERIALIZATION\n{ Encoded guest:");
   Serial.println(guest.getId());
   object["id"] = guest.getId();
   Serial.println(guest.getName());
   object["name"] = guest.getName();
   Serial.println(guest.getKey());
   object["key"] = guest.getKey();
   object.printTo(Serial);
     Serial.println("}\nSERIALIZATION");
   return object;
}

Guest decodeGuest(String body){
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& object = jsonBuffer.parseObject(body);
  Guest decodedGuest(object["id"],object["name"],object["key"]);
  return decodedGuest;
}

void detectFunction(NdefMessage message){
    String nfcMessageType =  EepromUtil::getStringFromRecord(message.getRecord(0));
    Serial.println(nfcMessageType);
    if (nfcMessageType.equals(NFC_TAG_RECIEVED_MASTER_INITALIZATION)){
        initMaster(message);
    }else if(nfcMessageType.equals(NFC_TAG_RECIEVED_WIFI_INITAIZATION)){
        checkWiFi(message);
    }else if(nfcMessageType.equals(NFC_TAG_RECIEVED_DOOR_OPENING)){
        Guest guest(message);
        if(!guestSaved){
        addGuest(guest);
        guestSaved = true;
        }else{
        checkOpenKey(guest);
        }
    }
  }


void initMaster(NdefMessage message){
  if(message.getRecordCount()==4){
  if (EepromUtil::getStringFromRecord(message.getRecord(1)).equals(MASTER_KEY)){
    Serial.println("Recieved Credentials:");
    WiFiCredentials credentials(EepromUtil::getStringFromRecord(message.getRecord(2)),EepromUtil::getStringFromRecord(message.getRecord(3)));
    Serial.println("Recieved Credentials:");
    Serial.println(credentials.getSsid());
    Serial.println(credentials.getPassword());
    if(!credentialsSaved){
    credentials.saveCredentials();
    credentialsSaved=true;
    }else{
    WiFiCredentials loadedCredentials("","");
    loadedCredentials.loadCredentials();
    Serial.println("Loaded Credentials:");
    Serial.println(loadedCredentials.getSsid());
    Serial.println(loadedCredentials.getPassword());
    }
    Serial.println("Credentials");
  }else{
    Serial.println("Wrong master password");
  }
  }else{
    Serial.println("Wrong record count");
  }
}

void checkWiFi(NdefMessage message){
  //todo Wifi
}


boolean checkOpenKey(Guest checkGuest){
    boolean check = false;
    Serial.println("checkGuest:");  
    Serial.println(checkGuest.getId());
    Serial.println(checkGuest.getName());
    Serial.println(checkGuest.getKey());
    
    Guest savedGuest = keyManager.getGuest(0);
    check = Guest::compareGuests(checkGuest,savedGuest);
    Serial.println("savedGuest");  
    Serial.println(savedGuest.getName());  
    Serial.println(savedGuest.getKey());  
    Serial.println(savedGuest.getId());  
  if(check){
      Serial.println("comapre succes");
  }else{
      Serial.println("comapre faliure");
  }
  return check;
}

Guest addGuest(Guest guest){
  guest = keyManager.addGuest(guest);
  return guest;
}


