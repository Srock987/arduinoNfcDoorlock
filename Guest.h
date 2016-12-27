#include <Arduino.h>
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>


class Guest
{
  public:
    Guest(byte aId,String aName, String aKey);
    Guest(NdefMessage message);
    Guest();
    byte getId();
    void setId(byte aId);
    String getName();
    String getKey();
    static boolean compareGuests(Guest guest1, Guest guest2);
  private:
  	byte id;
  	String name;
  	String key;
};
