#include "Entry.h"
#include <Arduino.h>

class EntryManager{
	public:
	EntryManager(void);
	void saveEntry(Entry entry);
	Entry loadEntry(int index);
	int getEntryCount();
	private:
	byte entries[4];
	void moveListUp();
};
