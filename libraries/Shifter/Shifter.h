/*
  Shifter.h - Library
  Code for driving multiple common Anode 7 Segment Displays using the TI TPIC6B595 8-Bit Shift Register
  
This has been written for use with the Arduino UNO
  25.02.2011
  
Released into the public domain.

 
 Connect between shift registers:-
  SRCLRPin, SRCKPin, RCKPin and connect the SEROUT to SERIN of each cascaded shift register
 
Make sure the G Pin is grounded
  Make sure SRCLR is tied to 5V
*/


#ifndef Shifter_h

#define Shifter_h


#include "Arduino.h"


class Shifter
{
  public:
   
 Shifter(int NumOfDigits, int SRCKPin, int SERINPin, int RCKPin);
    
 void clear();
 int shift;
 int last_shift;
 int display(int intNumber, int bC);
    
 void idle();
 
private:
    
 int _intNumber;
    
 int _NumOfDigits;
    
 int _SRCKPin;
    
 int _SERINPin;
    
 int _RCKPin;
  

};


#endif

