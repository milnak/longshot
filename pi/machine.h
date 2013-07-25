
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
  unsigned int gameState; //adding this because if there is no idle state (hold last score) then things get wonky
} __attribute__((__packed__));

struct MachineInState
{
  unsigned int command;
  unsigned int ticketsDispensed;
  unsigned int scoreClicks;
  unsigned int hundredClicks;
  unsigned int ballClicks;
  unsigned int coinClicks;
  unsigned int upClicks;
  unsigned int downClicks;
  unsigned int selectClicks;
  unsigned int setupClicks;
  unsigned int ticketError;
} __attribute__((__packed__));

enum {
  SETUP_OPTION_COINCOUNT,
  SETUP_OPTION_TICKETTABLE,
  SETUP_OPTION_FREEGAME,
  SETUP_OPTION_FREEGAME_SCORE,
  SETUP_OPTION_BALLCOUNT,
  SETUP_OPTION_VOLUME,
  SETUP_OPTION_SOUND_SET,
  SETUP_OPTION_LAST_SCORE_HOLD_SECS,
  SETUP_OPTION_ATTRACT_MODE_TIME_MINS,
  SETUP_OPTION_MAX
};

enum {
  COMMAND_NONE = 0,
  COMMAND_RESET
};

#define RESET_VAL 255

extern int gDebug;
extern int gOptionValues[SETUP_OPTION_MAX];

extern struct MachineOutState gMachineOut;
extern struct MachineInState gMachineIn;

extern struct MachineOutState gMachineOutPrev;
extern struct MachineInState gMachineInPrev;

// Call this to bootstrap the machine and comms
int InitMachine();

// Call this to update the states and comms
int UpdateMachine();

// End comms
int ExitMachine();

// Call this to clear all states and start fresh
void ResetMachine();

// Audio Helpers
void PreloadSound(const char* wavFilePath, int slot);
void PlaySound(int sound);
void FreeSoundSlots();

// Lights
void SwitchOn(int light);
void SwitchOff(int light);