#include "machine.h"
#include "longshot.h"
#include <stdio.h>
#include <sys/time.h>

enum {

  GAMESTATE_IDLE = 0 , 
  GAMESTATE_GAME = 1,
  GAMESTATE_ENDGAME = 2, 
  GAMESTATE_HOLDSCORE =3,
  GAMESTATE_OUTOFTICKETS = 4,
  GAMESTATE_SETUPMODE = 5

};

enum {
  SFX_NO_POINTS,
  SFX_10_POINTS,
  SFX_20_POINTS,
  SFX_30_POINTS,
  SFX_40_POINTS,
  SFX_50_POINTS,
  SFX_100_POINTS,
  SFX_WINNER_SONG,
  SFX_CALL_ATTENDANT,
  SFX_FREE_GAME,
  SFX_ATTRACT_SONG,
  SFX_GAME_START,
  SFX_GAME_OVER,
  SFX_MAX

};

int gTickMatrix[10][9] = { 
  //0-50,60-100,110-200,210-300,310-400,410-500,510-600,610-700,700+//
  { 1,2,3,4,5,9,13,18,30        },
  { 0,1,2,4,7,11,16,22,30       },
  { 0,2,4,7,11,16,23,30,50      },
  { 1,3,5,8,13,20,30,45,60      },
  { 1,3,6,9,14,21,32,45,60      },
  { 2,4,6,10,15,22,33,44,55     },
  { 2,4,6,11,17,24,32,48,60     },
  { 3,6,10,15,21,28,42,70,100   },
  { 3,6,10,15,21,28,36,72,100   },
  { 0,0,0,0,0,0,0,0,0           }
};

//gMachineOut.gameState = GAMESTATE_IDLE;
int gScoreAccumulator = 0;
int gCoinAccumulator = 0;
int gSoundsLoaded = 0;
int gHoldScoreTimer = 0;
int gTotalTicketsEarned = 0;
struct timeval gEndGameTime;
struct timeval gIdleAttractTime;

void StartNewGame() {
    
    //memset(&gMachineOut, 0, sizeof(gMachineOut));
    //memset(&gMachineOutPrev, 0, sizeof(gMachineOutPrev));

    //memset(&gMachineIn, 0, sizeof(gMachineIn));
    //memset(&gMachineInPrev, 0, sizeof(gMachineInPrev));
    gMachineOut.gameState = GAMESTATE_GAME;
    gMachineOut.score = 0;
    gMachineOut.ballCount = 0;
    gScoreAccumulator = 0;
    gCoinAccumulator = 0;
    gTotalTicketsEarned = 0;

    PlaySound(SFX_GAME_START);

    SwitchOff(SWITCH_IDLELIGHT);
    SwitchOff(SWITCH_GAMEOVERLIGHT);
    SwitchOn(SWITCH_SOLENOID);

    if (gMachineIn.ticketError <= 0)
      SwitchOff(SWITCH_BEACONLIGHT);
}

void GoIdle() {
  gMachineOut.gameState = GAMESTATE_IDLE;
  //gMachineOut.score = 0;
  //gMachineOut.ballCount = 0;
  gettimeofday(&gIdleAttractTime,NULL);

  SwitchOn(SWITCH_IDLELIGHT);
  SwitchOff(SWITCH_GAMEOVERLIGHT);
  SwitchOff(SWITCH_WINNERLIGHT);
  SwitchOff(SWITCH_BEACONLIGHT);
  SwitchOff(SWITCH_FREEGAMELIGHT);
}

void EndGame() {
    gMachineOut.gameState = GAMESTATE_ENDGAME;
    //gMachineOut.dispense = 0;
    gettimeofday(&gEndGameTime,NULL);    

    SwitchOn(SWITCH_GAMEOVERLIGHT);

    if (gMachineIn.ticketError > 0) {
        PlaySound(SFX_CALL_ATTENDANT);
        SwitchOn( SWITCH_BEACONLIGHT );
    }
}

#define PRELOAD_SOUND(id, f) \
  sprintf(fullPath, "%s/%d/%s.wav", basePath, gOptionValues[SETUP_OPTION_SOUND_SET], f); \
  PreloadSound(fullPath, id);

void LoadSounds() {

  const char* basePath = "assets/audio/";
  char fullPath[512];

  PRELOAD_SOUND(SFX_NO_POINTS,      "nopoints");
  PRELOAD_SOUND(SFX_10_POINTS,      "10points");
  PRELOAD_SOUND(SFX_20_POINTS,      "20points");
  PRELOAD_SOUND(SFX_30_POINTS,      "30points");
  PRELOAD_SOUND(SFX_40_POINTS,      "40points");
  PRELOAD_SOUND(SFX_50_POINTS,      "50points");
  PRELOAD_SOUND(SFX_100_POINTS,     "100points");
  PRELOAD_SOUND(SFX_WINNER_SONG,    "winner");
  PRELOAD_SOUND(SFX_CALL_ATTENDANT, "callattendant");
  PRELOAD_SOUND(SFX_FREE_GAME,      "freegame");
  PRELOAD_SOUND(SFX_ATTRACT_SONG,   "attract");
  PRELOAD_SOUND(SFX_GAME_START, "GameStart");
  PRELOAD_SOUND(SFX_GAME_OVER, "gameover");
  

  gSoundsLoaded = 1;
}

void InitLongshot() {

  if (gSoundsLoaded == 0)
    LoadSounds();
  
  GoIdle();
}

void UpdateLongshot() {

  ////////////
  // idle
  ////////////
  if (gMachineIn.coinClicks > 0) {
       SwitchOn(SWITCH_COINMETER);
       gCoinAccumulator += gMachineIn.coinClicks;
       if (gCoinAccumulator >= gOptionValues[SETUP_OPTION_COINCOUNT]) {
          StartNewGame();
          return;
        }
     }

    
  if (gMachineOut.gameState == GAMESTATE_IDLE) {

    struct timeval cur_time;
    gettimeofday(&cur_time,NULL);

     if(gOptionValues[SETUP_OPTION_ATTRACT_MODE_TIME_MINS] > 0) {
       if ((cur_time.tv_sec - gIdleAttractTime.tv_sec) > (60 * gOptionValues[SETUP_OPTION_ATTRACT_MODE_TIME_MINS])) {
        PlaySound(SFX_ATTRACT_SONG);
        gettimeofday(&gIdleAttractTime,NULL);
      }
    }

  } 
  ////////////
  // end game
  ////////////
  else if (gMachineOut.gameState == GAMESTATE_ENDGAME) {

    if (gOptionValues[SETUP_OPTION_LAST_SCORE_HOLD_SECS] > 0)
    {
      gMachineOut.gameState = GAMESTATE_HOLDSCORE;
      return;
    }
    else { 
      GoIdle();
      return;
      }

  } 
  ////////////
  // hold score
  ////////////
  else if (gMachineOut.gameState == GAMESTATE_HOLDSCORE){
    struct timeval cur_time;
    gettimeofday(&cur_time,NULL);
      if ((cur_time.tv_sec - gEndGameTime.tv_sec) > gOptionValues[SETUP_OPTION_LAST_SCORE_HOLD_SECS]) {
        if (gMachineOut.score >= gOptionValues[SETUP_OPTION_FREEGAME_SCORE]) { 
          SwitchOn(SWITCH_FREEGAMELIGHT);
          StartNewGame();
        } 
         else {
	        GoIdle();
        }
        return;
      }
  }
  ////////////
  // gameplay
  ////////////
  else if (gMachineOut.gameState == GAMESTATE_GAME) {

    SwitchOff(SWITCH_SOLENOID);

    // score up
    if (gMachineInPrev.hundredClicks < gMachineIn.hundredClicks){
      gScoreAccumulator += (50 * (gMachineIn.hundredClicks - gMachineInPrev.hundredClicks));
      gMachineOut.score += (50 * (gMachineIn.hundredClicks - gMachineInPrev.hundredClicks));
    }
    // score up
    if (gMachineInPrev.scoreClicks < gMachineIn.scoreClicks){
      gScoreAccumulator += (10 * (gMachineIn.scoreClicks - gMachineInPrev.scoreClicks));
      gMachineOut.score += (10 * (gMachineIn.scoreClicks - gMachineInPrev.scoreClicks));
      //gMachineOut.score += gScoreAccumulator; //would need separate var for sound b.c
      //we need to clear this here for it to work correctly
    }

    // balls played
    if (gMachineInPrev.ballClicks < gMachineIn.ballClicks)
    {
       
         //gMachineOut.score += gScoreAccumulator;
        // play the appropriate SFX
        if (gScoreAccumulator == 0) PlaySound(SFX_NO_POINTS);
        else if (gScoreAccumulator <= 10) PlaySound(SFX_10_POINTS);
        else if (gScoreAccumulator <= 20 ) PlaySound(SFX_20_POINTS);
        else if (gScoreAccumulator <= 30 ) PlaySound(SFX_30_POINTS);
        else if (gScoreAccumulator <= 40 ) PlaySound(SFX_40_POINTS);
        else if (gScoreAccumulator <= 50 ) PlaySound(SFX_50_POINTS);
        else if (gScoreAccumulator <= 100) PlaySound(SFX_100_POINTS);

        gScoreAccumulator = 0;

        gMachineOut.ballCount += (gMachineIn.ballClicks - gMachineInPrev.ballClicks);
        if (gMachineOut.ballCount >= gOptionValues[SETUP_OPTION_BALLCOUNT]) {
            EndGame();
        }
    }

    // we haz points! we can haz tix?
    gMachineOut.dispense = 0;
    if (gMachineOut.score > gMachineOutPrev.score) {
        int tableIndex = 0;
        int ticketsEarned = 0;

        if (gMachineOut.score <= 50)  tableIndex = 0;
        if (gMachineOut.score >= 60)  tableIndex++;
        if (gMachineOut.score >= 110) tableIndex++;
        if (gMachineOut.score >= 210) tableIndex++;
        if (gMachineOut.score >= 310) tableIndex++;
        if (gMachineOut.score >= 410) tableIndex++;
        if (gMachineOut.score >= 510) tableIndex++;
        if (gMachineOut.score >= 610) tableIndex++;
        if (gMachineOut.score >= 700) tableIndex++;

        if (tableIndex < 9) {
            ticketsEarned = gTickMatrix[gOptionValues[SETUP_OPTION_TICKETTABLE]][tableIndex];

            if (ticketsEarned > gMachineIn.ticketsDispensed) {
               // int diff = ticketsEarned - gMachineIn.ticketsDispensed;
                int diff = ticketsEarned - gTotalTicketsEarned; //getting wonk trying to compare ticketsDispensed from duino side
                
                if (diff > 0 && gTotalTicketsEarned == 0) {

                  SwitchOn( diff == 1 ? SWITCH_WINNERLIGHT : SWITCH_BEACONLIGHT);
                  //PlaySound(SFX_WINNER_SONG);
                }
                 gTotalTicketsEarned = ticketsEarned;
                 gMachineOut.dispense = diff;

            } else {
            }
        }
    }
  }
  
}
