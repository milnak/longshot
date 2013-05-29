#include "machine.h"
#include "longshot.h"

void InitLongshot() {
    gMachineOut.dispense  = 2;
}

void UpdateLongshot() {
    gMachineOut.dispense  = 0;

    if (gMachineInPrev.scoreClicks < gMachineIn.scoreClicks)
    {
        gMachineOut.score += (10 * (gMachineIn.scoreClicks - gMachineInPrev.scoreClicks));
    }
     if (gMachineInPrev.ballClicks < gMachineIn.ballClicks)
    {
        gMachineOut.ballCount += ((gMachineIn.ballClicks - gMachineInPrev.ballClicks));
    }
}
