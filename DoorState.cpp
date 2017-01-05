    #include "Guest.h"

    DoorState::DoorState(boolean doorOpen, boolean lockWorking){
        this.doorOpen = doorOpen;
        this.lockWorking = lockWorking;
    }

    boolean DoorState::isDoorOpen() {
        return doorOpen;
    }

    boolean DoorState::isLockWorking() {
        return lockWorking;
    }