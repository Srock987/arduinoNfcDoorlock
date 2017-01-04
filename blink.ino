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


  SoftwareSerial mySerial(8, 9);
  PN532_SPI pn532spi(SPI, 10);
  SNEP nfc(pn532spi);
  uint8_t ndefBuf[128];
  KeyManager keyManager;
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

  const String WIFI_TAG_GUESTLIST_REQUEST = "WIFI_TAG_GUESTLIST_REQUEST";
  const String WIFI_TAG_ADD_GUEST_REQUEST = "WIFI_TAG_ADD_GUEST_REQUEST";
  const String WIFI_TAG_DELETE_GUEST_REQUEST = "WIFI_TAG_DELETE_GUEST_REQUEST";
  const String WIFI_CONFIGURATION_TAG = "WIFI_CONFIGURATION_TAG";
  

void setup() {
  Serial.begin(9600); 
  mySerial.begin(9600);
  //configureWiFi();
}

void loop() {
    readNfc();   
    delay(1000);
    handleSerialRequest();
}

void configureWiFi(){
    WiFiCredentials loadedCredentials("","");
    loadedCredentials.loadCredentials();
    if(loadedCredentials.getSsid().length()!=0&&loadedCredentials.getPassword().length()!=0){
      char ssidChar[loadedCredentials.getSsid().length()+1];
      char passChar[loadedCredentials.getPassword().length()+1];
      loadedCredentials.getSsid().toCharArray(ssidChar,loadedCredentials.getSsid().length()+1);
      loadedCredentials.getPassword().toCharArray(passChar,loadedCredentials.getPassword().length()+1);    
      mySerial.write("WIFI_CONFIGURATION_TAG");
      mySerial.write("\n");
      mySerial.write(ssidChar);
      mySerial.write("\n");
      mySerial.write(passChar);
      mySerial.write("\n");
      mySerial.write("\r");
      Serial.println("Credentials sent:");
      Serial.println(ssidChar);
      Serial.println(passChar);
    }else {
      Serial.println("Credentials empty");
    }
}

void readNfc(){
    int msgSize = nfc.read(ndefBuf, sizeof(ndefBuf),500);
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
    mySerial.write("\r");
  }else if(request.substring(0,request.indexOf("\n")).equals(WIFI_TAG_ADD_GUEST_REQUEST)){
    String test1 = request.substring(request.indexOf("\n"));
    Guest decodedGuest = decodeGuest(test1);
    decodedGuest = keyManager.addGuest(decodedGuest);
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& response1 = jsonBuffer.createObject();
    encodeGuest(decodedGuest, response1);
    response1.printTo(mySerial);
    mySerial.write("\r");
  }else if(request.substring(0,request.indexOf("\n")).equals(WIFI_TAG_DELETE_GUEST_REQUEST)){
    String test1 = request.substring(request.indexOf("\n"));
    Guest decodedGuest = decodeGuest(test1);
    if(keyManager.guestExists(decodedGuest.getId())){
      if(keyManager.removeGuest(decodedGuest.getId())){
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& response2 = jsonBuffer.createObject();
        encodeGuest(decodedGuest, response2);
        response2.printTo(mySerial);
        mySerial.write("\r");
      }
    }
  }
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


void initMaster(NdefMessage& message){
  if(message.getRecordCount()==4){
  if (EepromUtil::getStringFromRecord(message.getRecord(1)).equals(MASTER_KEY)){
    WiFiCredentials credentials(EepromUtil::getStringFromRecord(message.getRecord(2)),EepromUtil::getStringFromRecord(message.getRecord(3)));
    credentials.saveCredentials();
    configureWiFi();
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


