
#include "machine.h"
#include "longshot.h"


int main ()
{
  InitMachine();  

  // kick off the update loop
  while (1)
  {
    UpdateMachine();

    // now respond accordingly to the states
    //printf("Read: %d bytes. Score Clicks: %d\n", bytesRead, gMachineIn.scoreClicks);
  }

  return 0 ;
}