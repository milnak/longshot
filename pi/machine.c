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
  LOGICSTATE_SETUP,
  LOGICSTATE_GAME
};

enum {
  SETUP_MODE_MENUSELECT,
  SETUP_MODE_VALUESELECT,
  SETUP_MODE_MAX
};

int gOptionValues[SETUP_OPTION_MAX] = {
  1,    // SETUP_OPTION_COINCOUNT,
  0,    // SETUP_OPTION_TICKETTABLE,
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
struct sample {
  Uint8 *data;
  Uint32 dpos;
  Uint32 dlen;
} activeSounds[NUM_ACTIVE_SOUNDS];

#define NUM_PRELOADED_SOUNDS 128
SDL_AudioCVT preloadedSounds[NUM_PRELOADED_SOUNDS];
int gNumLoadedSounds = 0;


///////////////////////////////////////////////
int IncConfigVal(int val) {
  
  switch (gSetupMenu) 
  {
    case SETUP_OPTION_COINCOUNT:
      return ++val > 10 ? 0 : val;
      break;

    case SETUP_OPTION_TICKETTABLE:
      return ++val > 10 ? 10 : val;
      break;

    case SETUP_OPTION_FREEGAME:
      return ++val > 1 ? 0 : val;
      break;

    case SETUP_OPTION_FREEGAME_SCORE:
      val += 10;
      return val > 900 ? 1 : val;
      break;

    case SETUP_OPTION_BALLCOUNT:
      return ++val > 10 ? 1 : val;
      break;

    case SETUP_OPTION_VOLUME:
      return ++val >= 10 ? 0 : val;
      break;

    case SETUP_OPTION_SOUND_SET:
      return ++val > 4 ? 1 : val;
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
      return --val < 0 ? 10 : val;
      break;

    case SETUP_OPTION_TICKETTABLE:
      return --val < 0 ? 10 : val;
      break;

    case SETUP_OPTION_FREEGAME:
      return --val < 0 ? 1 : val;
      break;

    case SETUP_OPTION_FREEGAME_SCORE:
      val -= 10;
      return val < 1 ? 900 : val;
      break;

    case SETUP_OPTION_BALLCOUNT:
      return --val < 1 ? 10 : val;
      break;

    case SETUP_OPTION_VOLUME:
      return --val < -1 ? 10 : val;
      break;

    case SETUP_OPTION_SOUND_SET:
      return --val < 1 ? 4 : val;
      break;

    default:
      return --val;
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

  printf("Saved config: %s\n", filePath);
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
    printf("Read config: %s\n", filePath);
  } else {
    // defaults 
    SaveConfig();
  }
}

///////////////////////////////////////////////
int InitMachine() {

  LoadConfig();

  int index = 0;

    // open our USB connection
  if ((gMachineCommPort = serialOpen("/dev/ttyUSB0", 57600)) < 0)
  {
    printf("Unable to open serial device: %s\n", strerror (errno));
    return 1 ;
  }

  // see if wiringPi is DTF
  if (wiringPiSetup() == -1)
  {
    printf( "Unable to start wiringPi: %s\n", strerror (errno));
    return 1;
  }

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
  gLogicState == LOGICSTATE_GAME;
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

///////////////////////////////////////////////
int ExitMachine() {
  FreeSoundSlots();

  SDL_CloseAudio();
  SDL_Quit();
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
    //writeBytes((unsigned char*)&gMachineOut,  sizeof(gMachineOut));
    writeInt(gMachineOut.score);
    writeInt(gMachineOut.switches);
    writeInt(gMachineOut.dispense);
    writeInt(gMachineOut.ballCount);
    //writeByte(gMachineOut._terminator);
    
    int command = readInt(gMachineCommPort);

    // save off the last state
    gMachineOutPrev = gMachineOut;
    gMachineInPrev = gMachineIn; 
    
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
    if (command > 0 && gLogicState == LOGICSTATE_SETUP) {
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

        //gOptionValues[gSetupMenu] = ValidateConfigVal(gOptionValues[gSetupMenu]);

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
        SaveConfig();
      }

      return 0;

    } else {

       // enter Setup mode
       if ((gMachineIn.setupClicks - gMachineInPrev.setupClicks) > 0) {
          gLogicState = LOGICSTATE_SETUP;
          LoadConfig();
       }
    
    }
    
    return command;
    delay(300);
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
  printf( "Preloading sound: %s in slot %d... ", &filePath, slot );

  /* Load the sound file and convert it to 16-bit stereo at 22kHz */
  if ( SDL_LoadWAV(filePath, &wave, &data, &dlen) == NULL ) {
    printf(" failed. %s\n", SDL_GetError());
    return;
  }

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
