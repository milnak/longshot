
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
 

typedef struct GameState {
  unsigned char switches;
  unsigned char dispense;
  unsigned char score;
  unsigned char ballCount;
  unsigned char _terminator;
} outGameState;

typedef struct GameStatus
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
} inGameState;

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

  // kick off the update loop
  
  while (TRUE)
  {
    // write our requests
    // set this so the Arduino knows we're done sending over the wire
    outGameState._terminator = "\0";
    unsigned char* outStateMem = &outGameState;

    for (int i = 0; i < sizeof(outGameState); i++, outStateMem++)
      serialPutchar(fd, outStateMem);

    serialFlush( fd )

    // read the data from the arduino
    unsigned char* inStateMem = &inGameState;
    while (serialDataAvail (fd))
    {
       *inStateMem = serialGetchar(fd);
       inStateMem += sizeof(unsigned char); // we're only reading a byte at a time
    }

    // now respond accordingly to the states

    delay(300) ;
  }

  return 0 ;
}
