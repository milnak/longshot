#include "machine.h"
#include "longshot.h"

int main ()
{
  InitMachine();  
  InitLongshot();

  // kick off the update loop
  while (1)
  {
    UpdateMachine();
    
    UpdateLongshot();

    DumpMachineInState();
    DumpMachineOutState();
  }
 
  
  return 0 ;
}