#include "machine.h"
#include "longshot.h"

#import <stdio.h>


int main(int argc, int *argv[])
{
  int c;

  while ((c = getopt (argc, argv, "d")) != -1) {
    switch (c)
    {
      case 'd':
        gDebug = 1;
        break;
    }
  }


  InitMachine();  
  InitLongshot();

  // kick off the update loop
  while (1)
  {
      int result = UpdateMachine();

      if (result == 1) {
        UpdateLongshot();

        if (dumpState) {
          DumpMachineInState();
          DumpMachineOutState();
        }
      }
      else if (result == -1) {
        printf("******** RESET ********\n");
        ResetMachine();
        InitLongshot();
      }

      
  }
 
  ExitMachine();
  return 0 ;
}