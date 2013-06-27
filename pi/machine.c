#include "machine.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include <wiringPi.h>
#include <wiringSerial.h>
#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>

enum {
  LOGICSTATE_SETUP = 1,
  LOGICSTATE_GAME
};

enum {
  SETUP_MODE_MENUSELECT,
  SETUP_MODE_VALUESELECT,
  SETUP_MODE_MAX
};

int gDebug = 0;

int gOptionValues[SETUP_OPTION_MAX] = {
  1,    // SETUP_OPTION_COINCOUNT,
  9,    // SETUP_OPTION_TICKETTABLE,
  0,    // SETUP_OPTION_FREEGAME,
  900, // SETUP_OPTION_FREEGAME_SCORE,
  9,    // SETUP_OPTION_BALLCOUNT,
  5,    // SETUP_OPTION_VOLUME,
  1,    // SETUP_OPTION_SOUND_SET,
  0,    // SETUP_OPTION_SAVED3,
  0,    // SETUP_OPTION_SAVED4,
};

int gLogicState = LOGICSTATE_GAME;
int gSetupMode = SETUP_MODE_MENUSELECT;
int gSetupMenu = 0;

int gMachineCommPort = -1;
struct MachineOutState gMachineOut, gMachineOutPrev;
struct MachineInState gMachineIn, gMachineInPrev;

// Audio stuff
#define kAUDIO_FREQ 22050
#define kAUDIO_FMT AUDIO_S16
#define kAUDIO_CHANNELS 1
#define NUM_ACTIVE_SOUNDS 8
#define NUM_PRELOADED_SOUNDS 128

struct sample {
  Uint8 *data;
  Uint32 dpos;
  Uint32 dlen;
} activeSounds[NUM_ACTIVE_SOUNDS];

SDL_AudioCVT preloadedSounds[NUM_PRELOADED_SOUNDS];
int gNumLoadedSounds = 0;

// increment the value and roll over to the low value
#define INC_AND_WRAP(val, inc, top, bottom) \
  val += inc; \
  return val > top ? bottom : val;

// decrement the value and wrap to the top value
#define DEC_AND_WRAP(val, inc, top, bottom) \
  val -= inc; \
  return val < bottom ? top : val;

///////////////////////////////////////////////
int IncConfigVal(int val) {
  
  switch (gSetupMenu) 
  {
    case SETUP_OPTION_COINCOUNT:
      INC_AND_WRAP(val, 1, 10, 0)
      break;

    case SETUP_OPTION_TICKETTABLE:
      INC_AND_WRAP(val, 1, 10, 0)
      break;

    case SETUP_OPTION_FREEGAME:
      INC_AND_WRAP(val, 1, 1, 0)
      break;

    case SETUP_OPTION_FREEGAME_SCORE:
      INC_AND_WRAP(val, 10, 900, 1)
      break;

    case SETUP_OPTION_BALLCOUNT:
      INC_AND_WRAP(val, 1, 10, 1)
      break;

    case SETUP_OPTION_VOLUME:
      INC_AND_WRAP(val, 1, 10, 0)
      break;

    case SETUP_OPTION_SOUND_SET:
      INC_AND_WRAP(val, 1, 4, 1)
      break;

    default:
      return ++val;
  }
}

///////////////////////////////////////////////
int DecConfigVal(int val) {
  
  switch (gSetupMenu) 
  {
    case SETUP_OPTION_COINCOUNT:
      DEC_AND_WRAP(val, 1, 10, 0)
      break;

    case SETUP_OPTION_TICKETTABLE:
      DEC_AND_WRAP(val, 1, 10, 0)
      break;

    case SETUP_OPTION_FREEGAME:
      DEC_AND_WRAP(val, 1, 1, 0)
      break;

    case SETUP_OPTION_FREEGAME_SCORE:
      DEC_AND_WRAP(val, 10, 900, 1)
      break;

    case SETUP_OPTION_BALLCOUNT:
      DEC_AND_WRAP(val, 1, 10, 1)
      break;

    case SETUP_OPTION_VOLUME:
      DEC_AND_WRAP(val, 1, 10, 0)
      break;

    case SETUP_OPTION_SOUND_SET:
      DEC_AND_WRAP(val, 1, 4, 1)
      break;

    default:
      return ++val;
  }
}

///////////////////////////////////////////////
void SaveConfig() {
  char filePath[1024];
  sprintf(filePath, "%s/options.dbm", get_current_dir_name());

  FILE *f;
  f = fopen(filePath, "wb");
  fwrite(&gOptionValues, sizeof(int), SETUP_OPTION_MAX, f);
  fclose(f);

  if (gDebug) printf("Saved Config: %s\n", filePath);
}

///////////////////////////////////////////////
void LoadConfig() {
  char filePath[1024];
  sprintf(filePath, "%s/options.dbm", get_current_dir_name());

  FILE *f;
  f = fopen(filePath, "rb");
  if (f != NULL) {
    fread(&gOptionValues, sizeof(int), SETUP_OPTION_MAX, f);
    fclose(f);
    if (gDebug) printf("Read Config: %s\n", filePath);
  } else {
    // defaults 
    SaveConfig();
  }
}

///////////////////////////////////////////////
int InitSerial() {

  if (gMachineCommPort >= 0) {
    serialFlush(gMachineCommPort);
    serialClose(gMachineCommPort);
  }

  // open our USB connection
  if ((gMachineCommPort = serialOpen("/dev/ttyUSB0", 57600)) < 0)
  {
    printf("Unable to open serial device: %s\n", strerror (errno));
    return 1 ;
  }

  if (gDebug) printf("---[ SERIAL SUCCESS ]---\n");

  // see if wiringPi is DTF
  if (wiringPiSetup() == -1)
  {
    printf( "Unable to start wiringPi: %s\n", strerror (errno));
    return 1;
  }

  return 0;
}

///////////////////////////////////////////////
int InitMachine() {

  LoadConfig();
  int index = 0;

  // see if SDL is DTF
  if (SDL_Init( SDL_INIT_AUDIO | SDL_INIT_TIMER ) != 0) 
  {
    printf( "Unable to start SDL: %s\n", SDL_GetError() );
    return 1;
  }

  extern void _MixAudio(void *unused, Uint8 *stream, int len);
  SDL_AudioSpec fmt;

  /* Set 16-bit mono audio at 22Khz */
  fmt.freq = kAUDIO_FREQ;
  fmt.format = kAUDIO_FMT;
  fmt.channels = kAUDIO_CHANNELS;
  fmt.samples = 512; /* A good value for games */
  fmt.callback = _MixAudio;
  fmt.userdata = NULL;

  /* Open the audio device and start playing sound! */
  if ( SDL_OpenAudio(&fmt, NULL) < 0 ) {
    fprintf(stderr, "Unable to open SDL audio: %sn", SDL_GetError());
    return 1;
  }

  SDL_PauseAudio(0);

  ResetMachine();
  return 0;
}



///////////////////////////////////////////////
void ResetMachine() {
  if (InitSerial() == 0) {
    gLogicState = LOGICSTATE_GAME;
    gSetupMode = SETUP_MODE_MENUSELECT;
    gSetupMenu = 0;

    gMachineOut.switches  = 0;
    gMachineOut.dispense  = 0;
    gMachineOut.score     = 0;
    gMachineOut.ballCount = 0;
    gMachineInPrev.scoreClicks = 0;

    memset(&gMachineOut, 0, sizeof(gMachineOut));
    memset(&gMachineOutPrev, 0, sizeof(gMachineOutPrev));

    memset(&gMachineIn, 0, sizeof(gMachineIn));
    memset(&gMachineInPrev, 0, sizeof(gMachineInPrev));
  }
}

///////////////////////////////////////////////
int ExitMachine() {
  FreeSoundSlots();

  SDL_CloseAudio();
  SDL_Quit();
}

///////////////////////////////////////////////
int readInt(int* value) {
  int i = 0;
  for (i = 0; i < sizeof(int); i++)
  {
     int c = serialGetchar(gMachineCommPort);
     
     if (c < 0) {
        if (gDebug) printf("### SERIAL ERROR ###\n");
        return -1;
     }

     unsigned int lastByte = (unsigned int)c;
     *value |= lastByte << (24 - (8 * i));
  }
  return 0;
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
  
    // save off the last state
    gMachineOutPrev = gMachineOut;
    gMachineInPrev = gMachineIn; 

    writeInt(gMachineOut.score);
    writeInt(gMachineOut.switches);
    writeInt(gMachineOut.dispense);
    writeInt(gMachineOut.ballCount);
    
    int command = 0;

    if (((readInt(&command) < 0) 
      && (readInt(&gMachineIn.ticketsDispensed) < 0)
      && (readInt(&gMachineIn.scoreClicks) < 0)
      && (readInt(&gMachineIn.hundredClicks) < 0) 
      && (readInt(&gMachineIn.ballClicks) < 0)
      && (readInt(&gMachineIn.coinClicks) < 0)
      && (readInt(&gMachineIn.upClicks) < 0)
      && (readInt(&gMachineIn.downClicks) < 0)
      && (readInt(&gMachineIn.selectClicks) < 0)
      && (readInt(&gMachineIn.setupClicks) < 0))) 
    {
      command = RESET_VAL;
    }

    //if (gDebug) printf("Command: %d\n", command);

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
      else if (gSetupMode == SETUP_MODE_VALUESELECT) {
        if ((gMachineIn.upClicks - gMachineInPrev.upClicks) > 0) {
          gOptionValues[gSetupMenu] = IncConfigVal(gOptionValues[gSetupMenu]);
        }

        if ((gMachineIn.downClicks - gMachineInPrev.downClicks) > 0) {
          gOptionValues[gSetupMenu] = DecConfigVal(gOptionValues[gSetupMenu]);
        }

        if ((gMachineIn.selectClicks - gMachineInPrev.selectClicks) > 0) {
          gSetupMode = SETUP_MODE_MENUSELECT;
          SaveConfig();
        }
      }

      gMachineOut.switches &= ~(1 << SWITCH_IDLELIGHT);
      gMachineOut.ballCount = gSetupMenu;
      gMachineOut.score = gOptionValues[gSetupMenu];

      // exit setup mode if necessary
      if ((gMachineIn.setupClicks - gMachineInPrev.setupClicks) > 0) {
        gLogicState = LOGICSTATE_GAME;
        gMachineOut.switches |= (1 << SWITCH_IDLELIGHT);
        if (gDebug) printf("*** GAME MODE ***\n");
        SaveConfig();
      }
    } else {

       // enter Setup mode
       if ((gMachineIn.setupClicks - gMachineInPrev.setupClicks) > 0) {
          gLogicState = LOGICSTATE_SETUP;
          if (gDebug) printf("*** SETUP MODE ***\n");
          LoadConfig();
       }
    
    }
    
    //serialFlush(gMachineCommPort);
    return command;
}


///////////////////////////////////////////////
void FreeSoundSlots() {
  int index = 0;

  // clear out the audio slots
  for ( index=0; index<NUM_ACTIVE_SOUNDS; ++index ) {
    activeSounds[index].dlen = 0;
  }

  for ( index=0; index<NUM_PRELOADED_SOUNDS; ++index ) {
    if ( preloadedSounds[index].buf )
      free(preloadedSounds[index].buf);

    preloadedSounds[index].len = 0;
  }
}

///////////////////////////////////////////////
void PreloadSound(const char* file, int slot) {
  int index;
  SDL_AudioSpec wave;
  Uint8 *data;
  Uint32 dlen;
  SDL_AudioCVT* cvt = &preloadedSounds[slot];
  char filePath[1024];

  if ( slot < 0 || slot >= NUM_PRELOADED_SOUNDS )
    return;

  if ( cvt->len ) {
    free(cvt->buf);
    cvt->len = 0;
  }

  sprintf(filePath, "%s/%s", get_current_dir_name(), file);

  if (gDebug)
    printf( "Preloading sound: %s in slot %d... ", &filePath, slot );

  /* Load the sound file and convert it to 16-bit stereo at 22kHz */
  if ( SDL_LoadWAV(filePath, &wave, &data, &dlen) == NULL ) {
    if (gDebug)
      printf(" failed. %s\n", SDL_GetError());
    return;
  }

  if (gDebug)
    printf(" length: %d bytes\n", dlen );

  SDL_BuildAudioCVT(cvt, wave.format, wave.channels, wave.freq, kAUDIO_FMT, kAUDIO_CHANNELS, kAUDIO_FREQ);
  cvt->buf = malloc(dlen * cvt->len_mult);
  memcpy(cvt->buf, data, dlen);
  cvt->len = dlen;
  SDL_ConvertAudio(cvt);
  SDL_FreeWAV(data);
}

///////////////////////////////////////////////
void PlaySound(int sound)
{
  if (gDebug)
    printf("Playing sound: %d\n", sound );
  
  SDL_AudioCVT* cvt = &preloadedSounds[sound];
  int index;

  if (sound < 0 || sound >= NUM_PRELOADED_SOUNDS || cvt->len == 0)
    return;
 
  /* Look for an empty (or finished) sound slot */
  for ( index=0; index<NUM_ACTIVE_SOUNDS; ++index ) {
    if ( activeSounds[index].dpos == activeSounds[index].dlen ) {
      break;
    }
  }
  if ( index == NUM_ACTIVE_SOUNDS )
    return; 

  SDL_LockAudio();
  activeSounds[index].data = cvt->buf;
  activeSounds[index].dlen = cvt->len_cvt;
  activeSounds[index].dpos = 0;
  SDL_UnlockAudio();
}

///////////////////////////////////////////////
void _MixAudio(void *unused, Uint8 *stream, int len)
{
  int i;
  Uint32 amount;
  for ( i=0; i<NUM_ACTIVE_SOUNDS; ++i ) {
    amount = (activeSounds[i].dlen-activeSounds[i].dpos);
    if ( amount > len ) {
      amount = len;
    }

    float volScale = ((float)gOptionValues[SETUP_OPTION_VOLUME])/10.0f;
    SDL_MixAudio(stream, &activeSounds[i].data[activeSounds[i].dpos], amount, (int)(volScale * 128.0f) );
    activeSounds[i].dpos += amount;
  }
}



///////////////////////////////////////////////
void DumpMachineOutState() {
    if (gDebug) {
      if (gMachineOutPrev.score != gMachineOut.score) 
        printf("CHANGED: Score: %d\n", gMachineOut.score);
      if (gMachineOutPrev.switches != gMachineOut.switches) 
        printf("CHANGED: Switches: %d\n", gMachineOut.switches);
      if (gMachineOutPrev.dispense != gMachineOut.dispense) 
        printf("CHANGED: Dispense: %d\n", gMachineOut.dispense);
      if (gMachineOutPrev.ballCount != gMachineOut.ballCount) 
        printf("CHANGED: Ball Count: %d\n", gMachineOut.ballCount);
    }
}

void DumpMachineInState() {
    if (gDebug) {
      if (gMachineInPrev.ticketsDispensed != gMachineIn.ticketsDispensed) 
        printf("CHANGED: Tickets Dispensed: %d\n", gMachineIn.ticketsDispensed);

      if (gMachineInPrev.scoreClicks != gMachineIn.scoreClicks) 
        printf("CHANGED: Score Clicks: %d\n", gMachineIn.scoreClicks);
      
      if (gMachineInPrev.hundredClicks != gMachineIn.hundredClicks) 
        printf("CHANGED: Hundred Clicks: %d\n", gMachineIn.hundredClicks);
      
      if (gMachineInPrev.ballClicks != gMachineIn.ballClicks) 
        printf("CHANGED: Ball Clicks: %d\n", gMachineIn.ballClicks);
      
      if (gMachineInPrev.coinClicks != gMachineIn.coinClicks) 
        printf("CHANGED: Coin Clicks: %d\n", gMachineIn.coinClicks);
      
      if (gMachineInPrev.upClicks != gMachineIn.upClicks) 
        printf("CHANGED: Up Clicks: %d\n", gMachineIn.upClicks);
      
      if (gMachineInPrev.downClicks != gMachineIn.downClicks) 
        printf("CHANGED: Down Clicks: %d\n", gMachineIn.downClicks);
      
      if (gMachineInPrev.selectClicks != gMachineIn.selectClicks) 
        printf("CHANGED: Select Clicks: %d\n", gMachineIn.selectClicks);
      
      if (gMachineInPrev.setupClicks != gMachineIn.setupClicks) 
        printf("CHANGED: Setup Clicks: %d\n", gMachineIn.setupClicks);
    }
}
