#include "machine.h"
#include "longshot.h"

int main(int argc, int *argv[])
{
  int dumpState = 0;
  int c;

  while ((c = getopt (argc, argv, "v:")) != -1) {
    switch (c)
    {
      case 'v':
        dumpState = 1;
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
        ResetMachine();
        InitLongshot();
      }

      
  }
 
  ExitMachine();
  return 0 ;
}