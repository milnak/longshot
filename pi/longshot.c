
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
int serialConn;


////////////////////////////////////////
// read 4 bytes and assemble into an int
int serialReadInt() {
  int i = 0;
  int value = 0;
  for (i = 0; i < sizeof(int); i++)
  {
     unsigned char lastByte = serialGetchar(serialConn);
     value |= lastByte << (24 - (8 * i));
  }
  return value;
}

////////////////////////////////////////
// setup the serial connection and wiringPi
void serialSetup() {
  // open our USB connection
  if ((serialConn = serialOpen("/dev/ttyUSB0", 57600)) < 0)
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

   // set this so the Arduino knows we're done sending over the wire
  outGameState._terminator = '\0';
}

////////////////////////////////////////
// Write out our machine requests and read in the state
void serialExchange() {
  // write our requests
    unsigned char* outStateMem = (unsigned char*)&outGameState;

    int i = 0;
    for (i = 0; i < sizeof(outGameState); i++, outStateMem++)
      serialPutchar(serialConn, *outStateMem);

    // copy the last update
    lastState = inGameState;

    // read the machine state
    inGameState.ticketsDispensed = serialReadInt();
    inGameState.scoreClicks = serialReadInt();
    inGameState.hundredClicks = serialReadInt();
    inGameState.ballClicks = serialReadInt();
    inGameState.coinClicks = serialReadInt();
    inGameState.upClicks = serialReadInt();
    inGameState.downClicks = serialReadInt();
    inGameState.selectClicks = serialReadInt();
    inGameState.setupClicks = serialReadInt();

    printf("ticketsDispensed: %d\n", inGameState.ticketsDispensed);
    printf("scoreClicks: %d\n", inGameState.scoreClicks);
    printf("hundredClicks: %d\n", inGameState.hundredClicks);
    printf("ballClicks: %d\n", inGameState.ballClicks);
    printf("coinClicks: %d\n", inGameState.coinClicks);
    printf("upClicks: %d\n", inGameState.upClicks);
    printf("downClicks: %d\n", inGameState.downClicks);
    printf("selectClicks: %d\n", inGameState.selectClicks);
    printf("setupClicks: %d\n", inGameState.setupClicks);

    serialFlush( serialConn );
    delay(300);
}

////////////////////////////////////////
// main
////////////////////////////////////////
int main ()
{
 
  serialSetup();

  while (1)
  {
    serialExchange();
    updateGame();
  }

  return 0 ;
}
