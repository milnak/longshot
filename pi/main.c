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

    DumpMachineState();
  }
  printf("Something got fucked somewhere");
  return 0 ;
}