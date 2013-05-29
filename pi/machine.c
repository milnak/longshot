#include "machine.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <wiringPi.h>
#include <wiringSerial.h>

int gMachineCommPort = -1;
struct MachineOutState gMachineOut;
struct MachineInState gMachineIn;

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

void UpdateMachine() {
    // write our requests
    unsigned char* outStatePtr = (unsigned char*)&gMachineOut;

    int i = 0;
    for (i = 0; i < sizeof(gMachineOut); i++, outStatePtr++)
    {
      serialPutchar(gMachineCommPort, *outStatePtr);
    }

    gMachineIn.ticketsDispensed = readInt(gMachineCommPort);
    printf("ticketsDispensed: %d\n", gMachineIn.ticketsDispensed);

    gMachineIn.scoreClicks = readInt(gMachineCommPort);
    printf("scoreClicks: %d\n", gMachineIn.scoreClicks);

    gMachineIn.hundredClicks = readInt(gMachineCommPort);
    printf("hundredClicks: %d\n", gMachineIn.hundredClicks);

    gMachineIn.ballClicks = readInt(gMachineCommPort);
    printf("ballClicks: %d\n", gMachineIn.ballClicks);

    gMachineIn.coinClicks = readInt(gMachineCommPort);
    printf("coinClicks: %d\n", gMachineIn.coinClicks);

    gMachineIn.upClicks = readInt(gMachineCommPort);
    printf("upClicks: %d\n", gMachineIn.upClicks);

    gMachineIn.downClicks = readInt(gMachineCommPort);
    printf("downClicks: %d\n", gMachineIn.downClicks);

    gMachineIn.selectClicks = readInt(gMachineCommPort);
    printf("selectClicks: %d\n", gMachineIn.selectClicks);

    gMachineIn.setupClicks = readInt(gMachineCommPort);
    printf("setupClicks: %d\n", gMachineIn.setupClicks);

    serialFlush( gMachineCommPort );
    delay(300);
}