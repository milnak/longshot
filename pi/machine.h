/**
 Machine I/O

  This module handles the communications between the game logic 
  that's running on the Rasperry Pi and various outputs. It provides
  a higher level abstraction for reading switches, setting lights, 
  dispensing tickets, playing sounds, and rendering graphics.

  This interface also handles loading, changing, and saving of the configuration options

  For communicating with the cabinet hardware, a serial connection is made 
  with an Arduino and state objects are exchanged. (uses wiringPi)

  For playing audio over the Pi audio board, SDL is used.

  For displaying graphics over the Pi gfx board, SDL is used.

 **/


// These identify the various components in the 
// game cabinet that can be read from (or possibly written to)
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
 
// This chunk gets passed into the cabinet interface from Arduino/Cabinet
struct MachineOutState {
  unsigned int score;
  unsigned int switches;
  unsigned int dispense;
  unsigned int ballCount;
  unsigned int gameState; //adding this because if there is no idle state (hold last score) then things get wonky
} __attribute__((__packed__));

// This chunk gets read in from the Arduino/Cabinet
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

// These are the options for the setup/config menu
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

// If the UpdateMachine() returns this value, then the main loop should 
// reset the machine with ResetMachine() and also reset the game logic
#define RESET_VAL 255

extern int gDebug; // setting this to 1 will print debug output
extern int gOptionValues[SETUP_OPTION_MAX]; // Config option values

// The main chunks that the game should use to read/write hardware states
extern struct MachineOutState gMachineOut;
extern struct MachineInState gMachineIn;

// Previous update chunks (useful for detecting and measuring state changes)
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

// TODO: Graphics Helpers

// Lights
void SwitchOn(int light);
void SwitchOff(int light);