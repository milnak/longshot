
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
      serialPutchar(fd, *outStateMem);


    // read the data from the arduino
    int bytesRead = 0;
    unsigned char* inStateMem = (unsigned char*)&inGameState;
    
    while (serialDataAvail(fd))
    //for (;bytesRead < sizeof(struct MachineStatus);)
    {
       //*inStateMem = serialGetchar(fd);
       //inStateMem++; // we're only reading a byte at a time
       //bytesRead++;

       printf("Got: %d\n", serialGetchar(fd));
    }

    printf("**Done for now.**\n");

    serialFlush( fd );
    // now respond accordingly to the states
    //printf("Read: %d bytes. Score Clicks: %d\n", bytesRead, inGameState.scoreClicks);
  }

  return 0 ;
}
