
enum SwitchBits {
 SWITCH_FREEGAMELIGHT,
 SWITCH_GAMEOVERLIGHT,
 SWITCH_WINNERLIGHT,
 SWITCH_BEACONLIGHT,
 SWITCH_COINMETER,
 SWITCH_TICKETMETER,
 SWITCH_SOLENOID,
 SWITCH_IDLELIGHT
};
 
struct MachineOutState {
  unsigned int score;
  unsigned int switches;
  unsigned int dispense;
  unsigned int ballCount;
  unsigned char _terminator;
} __attribute__((__packed__));

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
} __attribute__((__packed__));

extern struct MachineOutState gMachineOut;
extern struct MachineInState gMachineIn;

extern struct MachineOutState gMachineOutPrev;
extern struct MachineInState gMachineInPrev;


int InitMachine();
void UpdateMachine();