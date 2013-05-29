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

  gMachineOut.switches  = 1;
  gMachineOut.dispense  = 2;
  gMachineOut.score     = 3;
  gMachineOut.ballCount = 4;
  // set this so the Arduino knows we're done sending over the wire
  gMachineOut._terminator = '\0';
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

void writeBytes(unsigned char* ptr, unsigned int length) {
    int i = 0;
    for (i = 0; i < length; i++, ptr++)
      serialPutchar(gMachineCommPort, *ptr);
}

void UpdateMachine() {

    // write our requests
    gMachineOutPrev = gMachineOut;
    writeBytes((unsigned char*)&gMachineOut,  sizeof(gMachineOut));

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
    printf("ticketsDispensed: %d\n", gMachineIn.ticketsDispensed);
    printf("scoreClicks: %d\n", gMachineIn.scoreClicks);
    printf("hundredClicks: %d\n", gMachineIn.hundredClicks);
    printf("ballClicks: %d\n", gMachineIn.ballClicks);
    printf("coinClicks: %d\n", gMachineIn.coinClicks);
    printf("upClicks: %d\n", gMachineIn.upClicks);
    printf("downClicks: %d\n", gMachineIn.downClicks);
    printf("selectClicks: %d\n", gMachineIn.selectClicks);
    printf("setupClicks: %d\n", gMachineIn.setupClicks);
}