#include "machine.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <wiringPi.h>
#include <wiringSerial.h>

int gMachineCommPort = -1;
struct MachineOutState gMachineOut, gMachineOutPrev;
struct MachineInState gMachineIn, gMachineInPrev;

int InitMachine() {
    // open our USB connection
  if ((gMachineCommPort = serialOpen("/dev/ttyUSB0", 57600)) < 0)
  {
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
    return 1 ;
  }

  // see if wiringPi is DTF
  if (wiringPiSetup() == -1)
  {
    fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
    return 1 ;
  }

  gMachineOut.switches  = 0;
  gMachineOut.dispense  = 0;
  gMachineOut.score     = 0;
  gMachineOut.ballCount = 0;
  gMachineInPrev.scoreClicks = 0;
  // set this so the Arduino knows we're done sending over the wire
  gMachineOut._terminator = '\n';
  return 0;
}

int readInt() {
  int i = 0;
  int value = 0;
  for (i = 0; i < sizeof(int); i++)
  {
     unsigned char lastByte = serialGetchar(gMachineCommPort);
     value |= lastByte << (24 - (8 * i));
  }
  return value;
}

void writeInt(int value) {
  serialPutchar( (value >> 24) & 0xff );
  serialPutchar( (value >> 16) & 0xff );
  serialPutchar( (value >> 8) & 0xff );
  serialPutchar( value & 0xff );
}

void writeByte(unsigned char b) {
  serialPutchar(gMachineCommPort, b);
}

void writeBytes(unsigned char* ptr, unsigned int length) {
    int i = 0;
    for (i = 0; i < length; i++, ptr++)
      serialPutchar(gMachineCommPort, *ptr);
}

void UpdateMachine() {

    // write our requests
    gMachineOutPrev = gMachineOut;
    //writeBytes((unsigned char*)&gMachineOut,  sizeof(gMachineOut));

    writeInt(gMachineOut.score);
    writeByte(gMachineOut.switches);
    writeByte(gMachineOut.dispense);
    writeByte(gMachineOut.ballCount);
    writeByte(gMachineOut._terminator);

    // read in the current state
    gMachineInPrev = gMachineIn; // save off the last state
    gMachineIn.ticketsDispensed = readInt(gMachineCommPort);
    gMachineIn.scoreClicks = readInt(gMachineCommPort);
    gMachineIn.hundredClicks = readInt(gMachineCommPort);
    gMachineIn.ballClicks = readInt(gMachineCommPort);
    gMachineIn.coinClicks = readInt(gMachineCommPort);
    gMachineIn.upClicks = readInt(gMachineCommPort);
    gMachineIn.downClicks = readInt(gMachineCommPort);
    gMachineIn.selectClicks = readInt(gMachineCommPort);
    gMachineIn.setupClicks = readInt(gMachineCommPort);
    
    serialFlush( gMachineCommPort );
    delay(300);
}

void DumpMachineState() {
    printf("Tickets Dispensed: %d\n", gMachineIn.ticketsDispensed);
    printf("Score Clicks: %d\n", gMachineIn.scoreClicks);
    printf("Hundred Clicks: %d\n", gMachineIn.hundredClicks);
    printf("Ball Clicks: %d\n", gMachineIn.ballClicks);
    printf("Coin Clicks: %d\n", gMachineIn.coinClicks);
    printf("Up Clicks: %d\n", gMachineIn.upClicks);
    printf("Down Clicks: %d\n", gMachineIn.downClicks);
    printf("Select Clicks: %d\n", gMachineIn.selectClicks);
    printf("Setup Clicks: %d\n", gMachineIn.setupClicks);
}