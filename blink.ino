#include "NfcContainer.h"
#include <Arduino.h>
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <snep.h>
#include <NfcAdapter.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include "KeyManager.h"
#include "WiFiCredentials.h"
#include "DoorState.h"
#include "EntryManager.h"

#define DOORLOCK_PIN 7
#define DOORLOCK_VOLTAGE_PIN 2
#define CURRENT_RESISTOR_PIN 3
#define KONTRAKTON_PIN_IN 6


  PN532_SPI pn532spi(SPI, 10);
  SNEP nfc(pn532spi);
  uint8_t ndefBuf[128];
  KeyManager keyManager;
  EntryManager entryManager;
  boolean guestSaved = false;

  
  const String MASTER_KEY = "MASTER_KEY";
  const String NFC_TAG_RECIEVED_MASTER_INITALIZATION = "MST_INIT";
  const String NFC_TAG_RECIEVED_WIFI_INITAIZATION = "WIFI_INITALIZATION";
  const String NFC_TAG_RECIEVED_DOOR_OPENING = "DOOR_OPENING";
  
  const String NFC_TAG_SENT_MASTER_CHECK = "MASTER_CHECK";
  const String NFC_TAG_SENT_WIFI_CHECK = "WIFI_CHECK";
  const String NFC_TAG_SENT_DOOR_CHECK = "DOOR_CHECK";
  
  const String NFC_TAG_SUCCES = "SUCCES";
  const String NFC_TAG_FALIURE = "FALIURE";

  const String WIFI_TAG_GUESTLIST_REQUEST = "GST_LST";
  const String WIFI_TAG_ADD_GUEST_REQUEST = "A_GST";
  const String WIFI_TAG_DELETE_GUEST_REQUEST = "D_GST";
  const String WIFI_DOOR_STATE_REQUEST = "DOOR_STATE";
  const String WIFI_CONFIGURATION_TAG = "WIFI_CONFIGURATION_TAG";
  

void setup() {
  pinMode(KONTRAKTON_PIN_IN, INPUT);
  pinMode(DOORLOCK_PIN, OUTPUT);
  Serial.begin(9600); 
//  EepromUtil::eeprom_erase_all();
//  Serial.println("erased");
}

void loop() {
  handleSerialRequest();
  delay(500);
  readNfc();  
}

void configureWiFi(){
    WiFiCredentials loadedCredentials("","");
    loadedCredentials.loadCredentials();
    if(loadedCredentials.getSsid().length()!=0&&loadedCredentials.getPassword().length()!=0){
      char ssidChar[loadedCredentials.getSsid().length()+1];
      char passChar[loadedCredentials.getPassword().length()+1];
      loadedCredentials.getSsid().toCharArray(ssidChar,loadedCredentials.getSsid().length()+1);
      loadedCredentials.getPassword().toCharArray(passChar,loadedCredentials.getPassword().length()+1);    
      Serial.write("WIFI_CONFIGURATION_TAG");
      Serial.write("\n");
      Serial.write(ssidChar);
      Serial.write("\n");
      Serial.write(passChar);
      Serial.write("\n");
      Serial.write("\r");
    }
}

void readNfc(){
    int msgSize = nfc.read(ndefBuf, sizeof(ndefBuf),500);
    if (msgSize > 0) {
        NdefMessage msg  = NdefMessage(ndefBuf, msgSize);
        detectFunction(msg);
    }
}

void handleSerialRequest(){
    if (Serial.available()) {
    String request = Serial.readString();
    writeJsonResponse(request);
  }
}

void writeJsonResponse(String request){
    
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
    DynamicJsonBuffer jsonBuffer(46);
    JsonArray&  rootArray  = jsonBuffer.createArray();
    for(int i = 0 ; i < guestCount ; i++){
      JsonObject& object = rootArray.createNestedObject();
      encodeGuest(guestArray[i], object);
    }
    rootArray.printTo(Serial);
    Serial.write("\r");
  }else if(request.equals(WIFI_DOOR_STATE_REQUEST)){
    DynamicJsonBuffer jsonBuffer(114);
    JsonObject& response3 = jsonBuffer.createObject();
    int entriesCount = entryManager.getEntryCount();
    Entry entries[entriesCount];
    for(int i = 0 ; i < entriesCount ; i++){
      entries[i]=entryManager.loadEntry(i);
    }
    encodeDoorState(checkDoorState(),entries,entriesCount, response3);
    response3.printTo(Serial);
    Serial.write("\r");
  }else if(request.substring(0,request.indexOf("\n")).equals(WIFI_TAG_ADD_GUEST_REQUEST)){
    String test1 = request.substring(request.indexOf("\n"));
    Guest decodedGuest = decodeGuest(test1);
    decodedGuest = keyManager.addGuest(decodedGuest);
    DynamicJsonBuffer jsonBuffer(34);
    JsonObject& response1 = jsonBuffer.createObject();
    encodeGuest(decodedGuest, response1);
    response1.printTo(Serial);
    Serial.write("\r");
  }else if(request.substring(0,request.indexOf("\n")).equals(WIFI_TAG_DELETE_GUEST_REQUEST)){
    String test1 = request.substring(request.indexOf("\n"));
    Guest decodedGuest = decodeGuest(test1);
    if(keyManager.guestExists(decodedGuest.getId())){
      if(keyManager.removeGuest(decodedGuest.getId())){
        DynamicJsonBuffer jsonBuffer(34);
        JsonObject& response2 = jsonBuffer.createObject();
        encodeGuest(decodedGuest, response2);
        response2.printTo(Serial);
        Serial.write("\r");
      }
    }
  }
}

JsonObject& encodeDoorState(DoorState doorState,Entry entries[],int entiresSize, JsonObject& object){
  object["doorOpen"] = doorState.isDoorOpen();
  object["lockWorking"] = doorState.isLockWorking();
  object["timeStamp"] = doorState.getTimeStamp();
  JsonArray& array = object.createNestedArray("entries");
  for(int i = 0 ; i < entiresSize; i++){
    JsonObject& entry = array.createNestedObject();
    entry["userName"]=entries[i].getUserName();
    entry["timeStamp"]=entries[i].getTimeStamp();
  }
  return object;
}

JsonObject& encodeGuest(Guest guest, JsonObject& object){
   object["id"] = guest.getId();
   object["name"] = guest.getName();
   object["key"] = guest.getKey();
   return object;
}

Guest decodeGuest(String &body){
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& object = jsonBuffer.parseObject(body);
  Guest decodedGuest(object["id"],object["name"],object["key"]);
  return decodedGuest;
}

void detectFunction(NdefMessage& message){
    String nfcMessageType =  EepromUtil::getStringFromRecord(message.getRecord(0));
    if (nfcMessageType.equals(NFC_TAG_RECIEVED_MASTER_INITALIZATION)){
        initMaster(message);
    }else if(nfcMessageType.equals(NFC_TAG_RECIEVED_DOOR_OPENING)){
        Guest guest(message);
        openDoorLock(checkOpenKey(guest));
    }
}

void openDoorLock(boolean check){
  if(check){
  digitalWrite(DOORLOCK_PIN, HIGH);
  delay(4000);
  digitalWrite(DOORLOCK_PIN, LOW);
  }
}

DoorState checkDoorState(){
  float minLockVoltage = 5.0;
  float minCurrent = 0.2;
  boolean lockWorking = false;
  boolean doorOpen = true;

  digitalWrite(DOORLOCK_PIN,HIGH);
  delay(300);

  int voltsResistorRead = analogRead(CURRENT_RESISTOR_PIN);
  int voltsLockRead = analogRead(DOORLOCK_VOLTAGE_PIN);
  float voltsResistorFloat = (2.0 * voltsResistorRead )/ 204.8 ;
  float voltsLockFloat = (2.0 * voltsLockRead )/ 204.8 ;
  float current = voltsResistorFloat - voltsLockFloat;
  if(voltsLockFloat >= minLockVoltage && current > minCurrent){
    lockWorking = true;
  }
  digitalWrite(DOORLOCK_PIN, LOW);
  
  if(digitalRead(KONTRAKTON_PIN_IN)==HIGH){
    doorOpen = false;
  }
  DoorState doorState(doorOpen,lockWorking,millis());
  return doorState;
}





void initMaster(NdefMessage& message){
  if(message.getRecordCount()==4){
  if (EepromUtil::getStringFromRecord(message.getRecord(1)).equals(MASTER_KEY)){
    WiFiCredentials credentials(EepromUtil::getStringFromRecord(message.getRecord(2)),EepromUtil::getStringFromRecord(message.getRecord(3)));
    credentials.saveCredentials();
    configureWiFi();
  }
}
}


boolean checkOpenKey(Guest checkGuest){
  boolean check = false;
  if(checkGuest.getKey().equals(MASTER_KEY)){
    check = true;
  }else{
      if(keyManager.guestExists(checkGuest.getId())){
         Guest savedGuest = keyManager.getGuest(checkGuest.getId());
         check = Guest::compareGuests(checkGuest,savedGuest);
      }
  }
  if(check){
    Entry entry(checkGuest.getName(),millis());
    entryManager.saveEntry(entry);
  }
  return check;
}


Guest addGuest(Guest guest){
  guest = keyManager.addGuest(guest);
  return guest;
}


