#include <EEPROM.h>
#include "EepromUtil.h"
#include "Guest.h"
#include <Arduino.h>
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>



class KeyManager
{
  public:
    KeyManager(void);
    Guest getGuest(byte id);
    boolean guestExists(byte id);
    Guest addGuest(Guest guest);
    boolean removeGuest(byte id);
	  byte guestCount(void);
  private:
  	void initIndexes(void);
  	byte guestIdArray[10];
};
