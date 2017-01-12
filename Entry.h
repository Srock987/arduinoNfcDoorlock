#include <Arduino.h>

class Entry{
	public:
	Entry(String aUserName, long aTimeStamp);
  Entry();
	String getUserName();
	void setUserName(String aUserName);
	long getTimeStamp();
	void setTimeStamp(long aTimeStamp);
	private:
	String userName;
	long timeStamp;
};
