#include "machine.h"
#include "longshot.h"

int main ()
{
  InitMachine();  
  InitLongshot();

  // kick off the update loop
  while (1)
  {
     if (UpdateMachine())
        UpdateLongshot();

    DumpMachineInState();
    DumpMachineOutState();
  }
 
  
  ExitMachine();

  return 0 ;
}