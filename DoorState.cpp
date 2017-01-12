#include "DoorState.h"

DoorState::DoorState(boolean aDoorOpen, boolean aLockWorking,long aTimeStamp){
    doorOpen = aDoorOpen;
    lockWorking = aLockWorking;
    timeStamp = aTimeStamp;
}

boolean DoorState::isDoorOpen() {
    return doorOpen;
}

boolean DoorState::isLockWorking() {
    return lockWorking;
}

long DoorState::getTimeStamp(){
  return timeStamp;
}


