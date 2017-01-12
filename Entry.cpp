#include "Entry.h"

Entry::Entry(String aUserName, long aTimeStamp){
    userName = aUserName;
    timeStamp = aTimeStamp;
}

Entry::Entry(){}


String Entry::getUserName() {
    return userName;
}

void Entry::setUserName(String aUserName) {
    userName = aUserName;
}

long Entry::getTimeStamp() {
    return timeStamp;
}

void Entry::setTimeStamp(long aTimeStamp) {
    timeStamp = aTimeStamp;
}
