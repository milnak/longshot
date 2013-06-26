#include "machine.h"
#include "longshot.h"

#import <stdio.h>


int main(int argc, int *argv[])
{
  // parse args
  int c;

  while ((c = getopt (argc, argv, "d")) != -1) {
    switch (c)
    {
      case 'd':
        gDebug = 1;
        break;
    }
  }

  // setup the hardware and game
  InitMachine();  
  InitLongshot();

  // kick off the update loop
  while (1) {
      int result = UpdateMachine();

      if (result == 1) {
        UpdateLongshot();

        // do these after the game logic update so we can show changes
        DumpMachineInState();
        DumpMachineOutState();
      }
      else if (result == 255) {
        // if we got a reset request, then let's reset the hardware
        // and reset the game state
        if (gDebug) printf("******** RESET ********\n");
        ResetMachine();
        InitLongshot();
      }
  }
 
  ExitMachine();
  return 0 ;
}