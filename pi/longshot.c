
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

// globals
struct GameState outGameState;
struct MachineStatus inGameState, lastState;

////////////////////////////////////////
// read 4 bytes and assemble into an int
int serialReadInt(int fd) {
  int i = 0;
  int value = 0;
  for (i = 0; i < sizeof(int); i++)
  {
     unsigned char lastByte = serialGetchar(fd);
     value |= lastByte << (24 - (8 * i));
  }
  return value;
}

////////////////////////////////////////
// setup the serial connection and wiringPi
int serialSetup() {
  // open our USB connection
  int fd
  if ((fd = serialOpen("/dev/ttyUSB0", 57600)) < 0)
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;

  // see if wiringPi is DTF
  if (wiringPiSetup() == -1)
    fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;

   // set this so the Arduino knows we're done sending over the wire
  outGameState._terminator = '\0';
  return fd;
}

////////////////////////////////////////
// Write out our machine requests and read in the state
void serialExchange(int fd) {
  
    // write our requests
    unsigned char* outStateMem = (unsigned char*)&outGameState;

    int i = 0;
    for (i = 0; i < sizeof(outGameState); i++, outStateMem++)
      serialPutchar(fd, *outStateMem);

    // copy the last update
    lastState = inGameState;

    // read the machine state
    inGameState.ticketsDispensed = serialReadInt(fd);
    inGameState.scoreClicks = serialReadInt(fd);
    inGameState.hundredClicks = serialReadInt(fd);
    inGameState.ballClicks = serialReadInt(fd);
    inGameState.coinClicks = serialReadInt(fd);
    inGameState.upClicks = serialReadInt(fd);
    inGameState.downClicks = serialReadInt(fd);
    inGameState.selectClicks = serialReadInt(fd);
    inGameState.setupClicks = serialReadInt(fd);

    printf("ticketsDispensed: %d\n", inGameState.ticketsDispensed);
    printf("scoreClicks: %d\n", inGameState.scoreClicks);
    printf("hundredClicks: %d\n", inGameState.hundredClicks);
    printf("ballClicks: %d\n", inGameState.ballClicks);
    printf("coinClicks: %d\n", inGameState.coinClicks);
    printf("upClicks: %d\n", inGameState.upClicks);
    printf("downClicks: %d\n", inGameState.downClicks);
    printf("selectClicks: %d\n", inGameState.selectClicks);
    printf("setupClicks: %d\n", inGameState.setupClicks);

    serialFlush( fd );
    delay(300);
}

void updateGame() {

}

////////////////////////////////////////
// main
////////////////////////////////////////
int main ()
{
  int serialConn = serialSetup();

  while (1)
  {
    serialExchange(serialConn);
    updateGame();
  }

  serialClose(serialConn);
  return 0 ;
}
