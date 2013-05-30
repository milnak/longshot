#include "machine.h"
#include "longshot.h"

void InitLongshot() {

}

void UpdateLongshot() {

    int oldScore = gMachineOut.score;

    if (gMachineInPrev.hundredClicks < gMachineIn.hundredClicks)
        gMachineOut.score += (50 * (gMachineIn.hundredClicks - gMachineInPrev.hundredClicks));
    
    if (gMachineInPrev.scoreClicks < gMachineIn.scoreClicks)
        gMachineOut.score += (10 * (gMachineIn.scoreClicks - gMachineInPrev.scoreClicks));

    if (gMachineInPrev.ballClicks < gMachineIn.ballClicks)
        gMachineOut.ballCount += (gMachineIn.ballClicks - gMachineInPrev.ballClicks);
    
    gMachineOut.dispense = 0;
    int scoreDiff = gMachineOut.score - oldScore;
    if (scoreDiff > 100) {
        //gMachineOut.dispense = (int)scoreDiff/100;
    }
}
