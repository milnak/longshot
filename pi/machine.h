
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
 
struct MachineOutState {
  unsigned char switches;
  unsigned char dispense;
  unsigned int  score;
  unsigned char ballCount;
  unsigned char _terminator;
};

struct MachineInState
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

extern struct MachineOutState gMachineOut;
extern struct MachineInState gMachineIn;

extern struct MachineOutState gMachineOutPrev;
extern struct MachineInState gMachineInPrev;


int InitMachine();
void UpdateMachine();