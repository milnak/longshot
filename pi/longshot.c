#include "machine.h"
#include "longshot.h"

void InitLongshot() {
    gMachineOut.dispense  = 2;
}

void UpdateLongshot() {
    gMachineOut.dispense  = 0;
}
