
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <wiringPi.h>
#include <wiringSerial.h>

enum SwitchBits {
 SWITCH_FREEGAMELIGHT,
 SWITCH_GAMEOVERLIGHT,
 SWITCH_WINNERLIGHT,
 SWITCH_BEACONLIGHT,
 SWITCH_COINMETER,
 SWITCH_TICKETMETER,
 SWITCH_SOLENOID,
 SWITCH_TICKETDISPENSER
};
 

struct GameState {
  unsigned char switches;
  unsigned char dispense;
  unsigned char score;
  unsigned char ballCount;
  unsigned char _terminator;
};

struct MachineStatus
{
  unsigned int ticketsDispensed;
  unsigned int scoreClicks;
  unsigned int hundredClicks;
  unsigned int ballClicks;
  unsigned int coinClicks;
  unsigned int upClicks;
  unsigned int downClicks;
  unsigned int selectClicks;
  unsigned int setupClicks;
};

struct GameState outGameState;
struct MachineStatus inGameState;

int readInt(int fd) {
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

  outGameState.switches  = 1;
  outGameState.dispense  = 2;
  outGameState.score     = 3;
  outGameState.ballCount = 4;
  // set this so the Arduino knows we're done sending over the wire
  outGameState._terminator = '\0';
    

  // kick off the update loop
  while (1)
  {
    // write our requests
    unsigned char* outStateMem = (unsigned char*)&outGameState;

    int i = 0;
    for (i = 0; i < sizeof(outGameState); i++, outStateMem++)
    {
      serialPutchar(fd, *outStateMem);
    }

    inGameState.ticketsDispensed = readInt(fd);
    printf("ticketsDispensed: %d\n", inGameState.ticketsDispensed);

    inGameState.scoreClicks = readInt(fd);
    printf("scoreClicks: %d\n", inGameState.scoreClicks);

    inGameState.hundredClicks = readInt(fd);
    printf("hundredClicks: %d\n", inGameState.hundredClicks);

    inGameState.ballClicks = readInt(fd);
    printf("ballClicks: %d\n", inGameState.ballClicks);

    inGameState.coinClicks = readInt(fd);
    printf("coinClicks: %d\n", inGameState.coinClicks);

    inGameState.upClicks = readInt(fd);
    printf("upClicks: %d\n", inGameState.upClicks);

    inGameState.downClicks = readInt(fd);
    printf("downClicks: %d\n", inGameState.downClicks);

    inGameState.selectClicks = readInt(fd);
    printf("selectClicks: %d\n", inGameState.selectClicks);

    inGameState.setupClicks = readInt(fd);
    printf("setupClicks: %d\n", inGameState.setupClicks);

    serialFlush( fd );
    delay(300);

    // now respond accordingly to the states
    //printf("Read: %d bytes. Score Clicks: %d\n", bytesRead, inGameState.scoreClicks);
  }

  return 0 ;
}
