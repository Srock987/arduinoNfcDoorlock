#include <Arduino.h>



class DoorState
{
    public:
    DoorState(boolean aDoorOpen, boolean aLockWorking,long aTimeStamp);
    long getTimeStamp();
    boolean isDoorOpen();
    boolean isLockWorking();

    private:
     boolean doorOpen;
     boolean lockWorking;
     long timeStamp;
};
