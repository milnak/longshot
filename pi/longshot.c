#include "machine.h"
#include "longshot.h"

void InitLongshot() {
    //gMachineOut.dispense  = 0;
}

void UpdateLongshot() {
    //gMachineOut.dispense  = 0;
     if (gMachineInPrev.hundredClicks < gMachineIn.hundredClicks)
    {
        gMachineOut.score += (50 * (gMachineIn.hundredClicks - gMachineInPrev.hundredClicks));
    }
}
