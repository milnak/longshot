#include <Shifter.h>

Shifter shifter(3, 2, 3, 4);  //Setup Shifter instance with number of digits and pin numbers

void setup()
{
  
}

void loop()
{
  int x;
  
  //shifter.clear(); 
  for (x = 0; x < 100; x++)
   { 
  shifter.display(x);
  delay(250);
   }
}
