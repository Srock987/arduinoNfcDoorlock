#include <Arduino.h>
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

class NfcContainer
{
  public:
    NfcContainer( NdefMessage message );
    String getType(void);
    String getContent(void);
    String getExtraContent(void);
    byte getByteContent(void);
  private:
    String messageType;
    String messageContent;
    String extraContent;
    byte byteContent;
    String getStringFromRecord(NdefRecord record);
};
