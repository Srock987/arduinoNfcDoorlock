#include <Arduino.h>

class WiFiCredentials{
	public:
	 WiFiCredentials(String aSsid, String aPassword);
   boolean saveCredentials();
   boolean loadCredentials();
	 String getSsid();
	 String getPassword();
   
	 private:
	 String ssid;
	 String password;
};
