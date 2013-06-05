#include "machine.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include <wiringPi.h>
#include <wiringSerial.h>

enum {
  LOGICSTATE_SETUP,
  LOGICSTATE_GAME
};

enum {
  SETUP_MODE_MENUSELECT,
  SETUP_MODE_VALUESELECT,
  SETUP_MODE_MAX
};

int gOptionValues[SETUP_OPTION_MAX] = {
  4,    // SETUP_OPTION_COINCOUNT,
  0,    // SETUP_OPTION_TICKETTABLE,
  0,    // SETUP_OPTION_FREEGAME,
  1000, // SETUP_OPTION_FREEGAME_SCORE,
  9,    // SETUP_OPTION_BALLCOUNT,
  0,    // SETUP_OPTION_SAVED1,
  0,    // SETUP_OPTION_SAVED2,
  0,    // SETUP_OPTION_SAVED3,
  0,    // SETUP_OPTION_SAVED4,
};

int gLogicState = LOGICSTATE_GAME;
int gSetupMode = SETUP_MODE_MENUSELECT;
int gSetupMenu = 0;

int gMachineCommPort = -1;
struct MachineOutState gMachineOut, gMachineOutPrev;
struct MachineInState gMachineIn, gMachineInPrev;

///////////////////////////////////////////////
void SaveConfig() {
  FILE *f;
  f = fopen("options.dbm", "wb");
  fwrite(&gOptionValues, sizeof(int), SETUP_OPTION_MAX, f);
  fclose(f);
}

///////////////////////////////////////////////
void LoadConfig() {
  FILE *f;
  f = fopen("options.dbm", "rb");
  if (f != NULL) {
    fread(&gOptionValues, sizeof(int), SETUP_OPTION_MAX, f);
    fclose(f);
  } else {
    // defaults 
    SaveConfig();
  }
}

///////////////////////////////////////////////
int InitMachine() {
    // open our USB connection
  if ((gMachineCommPort = serialOpen("/dev/ttyUSB0", 57600)) < 0)
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

  gMachineOut.switches  = 0;
  gMachineOut.dispense  = 0;
  gMachineOut.score     = 0;
  gMachineOut.ballCount = 0;
  gMachineInPrev.scoreClicks = 0;
  return 0;
}

///////////////////////////////////////////////
int readInt() {
  int i = 0;
  int value = 0;
  for (i = 0; i < sizeof(int); i++)
  {
     unsigned int lastByte = serialGetchar(gMachineCommPort);
     value |= lastByte << (24 - (8 * i));
  }
  return value;
}

///////////////////////////////////////////////
void writeByte(unsigned char b) {
  serialPutchar(gMachineCommPort, b);
}

///////////////////////////////////////////////
void writeInt(unsigned int value) {
  writeByte( (value >> 24) & 0xff );
  writeByte( (value >> 16) & 0xff );
  writeByte( (value >> 8) & 0xff );
  writeByte( (value & 0xff) );
}

///////////////////////////////////////////////
void writeBytes(unsigned char* ptr, unsigned int length) {
    int i = 0;
    for (i = 0; i < length; i++, ptr++)
      serialPutchar(gMachineCommPort, *ptr);
}

///////////////////////////////////////////////
int UpdateMachine() {

    // write our requests
    gMachineOutPrev = gMachineOut;
    //writeBytes((unsigned char*)&gMachineOut,  sizeof(gMachineOut));
    writeInt(gMachineOut.score);
    writeInt(gMachineOut.switches);
    writeInt(gMachineOut.dispense);
    writeInt(gMachineOut.ballCount);
    //writeByte(gMachineOut._terminator);
    
    // read in the current state
    gMachineInPrev = gMachineIn; // save off the last state
    gMachineIn.ticketsDispensed = readInt(gMachineCommPort);
    gMachineIn.scoreClicks = readInt(gMachineCommPort);
    gMachineIn.hundredClicks = readInt(gMachineCommPort);
    gMachineIn.ballClicks = readInt(gMachineCommPort);
    gMachineIn.coinClicks = readInt(gMachineCommPort);
    gMachineIn.upClicks = readInt(gMachineCommPort);
    gMachineIn.downClicks = readInt(gMachineCommPort);
    gMachineIn.selectClicks = readInt(gMachineCommPort);
    gMachineIn.setupClicks = readInt(gMachineCommPort);

    // Setup Mode
    if (gLogicState == LOGICSTATE_SETUP) {
      // Select a menu/config option
      if (gSetupMode == SETUP_MODE_MENUSELECT) {
        if ((gMachineIn.upClicks - gMachineInPrev.upClicks) > 0) {
          if (++gSetupMenu >= SETUP_OPTION_MAX) gSetupMenu = 0;
        }

        if ((gMachineIn.downClicks - gMachineInPrev.downClicks) > 0){
          if (--gSetupMenu < 0) gSetupMenu = SETUP_OPTION_MAX - 1;
        }

        if ((gMachineIn.selectClicks - gMachineInPrev.selectClicks) > 0)
          gSetupMode = SETUP_MODE_VALUESELECT;
      }

      // Select a value for an option
      if (gSetupMode == SETUP_MODE_VALUESELECT) {
        if ((gMachineIn.upClicks - gMachineInPrev.upClicks) > 0) {
          gOptionValues[gSetupMenu]++;
        }

        if ((gMachineIn.downClicks - gMachineInPrev.downClicks) > 0) {
          gOptionValues[gSetupMenu]++;
        }

        if ((gMachineIn.selectClicks - gMachineInPrev.selectClicks) > 0) {
          gSetupMode = SETUP_MODE_MENUSELECT;
          SaveConfig();
        }
      }

      // exit setup mode if necessary
      if ((gMachineIn.setupClicks - gMachineInPrev.setupClicks) > 0) {
        gLogicState = LOGICSTATE_GAME;
        gMachineOut.switches |= (1 << SWITCH_IDLELIGHT);
        SaveConfig();
      }

      gMachineOut.switches &= ~(1 << SWITCH_IDLELIGHT);
      gMachineOut.ballCount = gSetupMenu;
      gMachineOut.score = gOptionValues[gSetupMenu];
      return 0;

    } else {

       // enter Setup mode
       if ((gMachineIn.setupClicks - gMachineInPrev.setupClicks) > 0) {
          gLogicState = LOGICSTATE_SETUP;
          LoadConfig();
       }
    
    }
    
    return 1;
    //delay(300);
}

///////////////////////////////////////////////
void DumpMachineOutState() {
    printf("Score: %d\n", gMachineOut.score);
    printf("Switches: %d\n", gMachineOut.switches);
    printf("Dispense: %d\n", gMachineOut.dispense);
    printf("Ball Count: %d\n", gMachineOut.ballCount);
    printf("--------------------------\n");
}

void DumpMachineInState() {
    printf("Tickets Dispensed: %d\n", gMachineIn.ticketsDispensed);
    printf("Score Clicks: %d\n", gMachineIn.scoreClicks);
    printf("Hundred Clicks: %d\n", gMachineIn.hundredClicks);
    printf("Ball Clicks: %d\n", gMachineIn.ballClicks);
    printf("Coin Clicks: %d\n", gMachineIn.coinClicks);
    printf("Up Clicks: %d\n", gMachineIn.upClicks);
    printf("Down Clicks: %d\n", gMachineIn.downClicks);
    printf("Select Clicks: %d\n", gMachineIn.selectClicks);
    printf("Setup Clicks: %d\n", gMachineIn.setupClicks);
    printf("--------------------------\n");
}