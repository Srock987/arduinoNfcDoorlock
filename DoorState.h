#include <Arduino.h>


class DoorState
{
    public:
    DoorState(boolean doorOpen, boolean lockWorking);
    boolean isDoorOpen();
    boolean isLockWorking();

    private:
     boolean doorOpen;
     boolean lockWorking;
};
