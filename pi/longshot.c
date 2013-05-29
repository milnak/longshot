
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <wiringPi.h>
#include <wiringSerial.h>

#include "longshot.h"

struct MachineOutState gMachineOut;
struct MachineInState gMachineIn;

int readInt(int fd) {
  int i = 0;
  int value = 0;
  for (i = 0; i < sizeof(int); i++)
  {
     unsigned char lastByte = serialGetchar(fd);
     value |= lastByte << (24 - (8 * i));
  }
  return value;
}

int main ()
{
  int fd;
  
  // open our USB connection
  if ((fd = serialOpen("/dev/ttyUSB0", 57600)) < 0)
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
    

  // kick off the update loop
  while (1)
  {
    // write our requests
    unsigned char* outStatePtr = (unsigned char*)&gMachineOut;

    int i = 0;
    for (i = 0; i < sizeof(gMachineOut); i++, outStatePtr++)
    {
      serialPutchar(fd, *outStatePtr);
    }

    gMachineIn.ticketsDispensed = readInt(fd);
    printf("ticketsDispensed: %d\n", gMachineIn.ticketsDispensed);

    gMachineIn.scoreClicks = readInt(fd);
    printf("scoreClicks: %d\n", gMachineIn.scoreClicks);

    gMachineIn.hundredClicks = readInt(fd);
    printf("hundredClicks: %d\n", gMachineIn.hundredClicks);

    gMachineIn.ballClicks = readInt(fd);
    printf("ballClicks: %d\n", gMachineIn.ballClicks);

    gMachineIn.coinClicks = readInt(fd);
    printf("coinClicks: %d\n", gMachineIn.coinClicks);

    gMachineIn.upClicks = readInt(fd);
    printf("upClicks: %d\n", gMachineIn.upClicks);

    gMachineIn.downClicks = readInt(fd);
    printf("downClicks: %d\n", gMachineIn.downClicks);

    gMachineIn.selectClicks = readInt(fd);
    printf("selectClicks: %d\n", gMachineIn.selectClicks);

    gMachineIn.setupClicks = readInt(fd);
    printf("setupClicks: %d\n", gMachineIn.setupClicks);

    serialFlush( fd );
    delay(300);

    // now respond accordingly to the states
    //printf("Read: %d bytes. Score Clicks: %d\n", bytesRead, gMachineIn.scoreClicks);
  }

  return 0 ;
}