#include "machine.h"
#include "longshot.h"
#include <stdio.h>

enum {
  GAMESTATE_IDLE,
  GAMESTATE_GAME
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
  SFX_MAX
};

int gTickMatrix[9][9] = { 
  //0-50,60-100,110-200,210-300,310-400,410-500,510-600,610-700,700+//
  { 1,2,3,4,5,9,13,18,30        },
  { 0,1,2,4,7,11,16,22,30       },
  { 0,2,4,7,11,16,23,30,50      },
  { 1,3,5,8,13,20,30,45,60      },
  { 1,3,6,9,14,21,32,45,60      },
  { 2,4,6,10,15,22,33,44,55     },
  { 2,4,6,11,17,24,32,48,60     },
  { 3,6,10,15,21,28,42,70,100   },
  { 3,6,10,15,21,28,36,72,100   }
};

int gTicketsDispensed = 0;
int gGameState = GAMESTATE_IDLE;



void StartNewGame() {
    gGameState = GAMESTATE_GAME;
    gMachineOut.switches &= ~(1 << SWITCH_IDLELIGHT);
    gMachineOut.switches |=  (1 << SWITCH_SOLENOID);
    gMachineOut.score = 0;
    gMachineOut.ballCount = 0;
}

void EndGame() {

    int score = gMachineOut.score;

    gGameState = GAMESTATE_IDLE;
    gMachineOut.switches |= (1 << SWITCH_IDLELIGHT);
    gMachineOut.score = 0;
    gMachineOut.ballCount = 0;
    gTicketsDispensed = 0;

    if (score >= gOptionValues[SETUP_OPTION_FREEGAME_SCORE]) 
      StartNewGame();
}

void LoadSounds() {
  FreeSoundSlots();

  const char* basePath = "assets/audio";
  char fullPath[512];

  sprintf(fullPath, "%s/%d/%s", basePath, gOptionValues[SETUP_OPTION_SOUND_SET], "nopoints.wav");
  PreloadSound(fullPath, SFX_NO_POINTS);

  sprintf(fullPath, "%s/%d/%s", basePath, gOptionValues[SETUP_OPTION_SOUND_SET], "10points.wav");
  PreloadSound(fullPath, SFX_10_POINTS);

  sprintf(fullPath, "%s/%d/%s", basePath, gOptionValues[SETUP_OPTION_SOUND_SET], "20points.wav");
  PreloadSound(fullPath, SFX_20_POINTS);

  sprintf(fullPath, "%s/%d/%s", basePath, gOptionValues[SETUP_OPTION_SOUND_SET], "30points.wav");
  PreloadSound(fullPath, SFX_30_POINTS);

  sprintf(fullPath, "%s/%d/%s", basePath, gOptionValues[SETUP_OPTION_SOUND_SET], "40points.wav");
  PreloadSound(fullPath, SFX_40_POINTS);

  sprintf(fullPath, "%s/%d/%s", basePath, gOptionValues[SETUP_OPTION_SOUND_SET], "50points.wav");
  PreloadSound(fullPath, SFX_50_POINTS);

  sprintf(fullPath, "%s/%d/%s", basePath, gOptionValues[SETUP_OPTION_SOUND_SET], "100points.wav");
  PreloadSound(fullPath, SFX_100_POINTS);

  sprintf(fullPath, "%s/%d/%s", basePath, gOptionValues[SETUP_OPTION_SOUND_SET], "winner.wav");
  PreloadSound(fullPath, SFX_WINNER_SONG);

  sprintf(fullPath, "%s/%d/%s", basePath, gOptionValues[SETUP_OPTION_SOUND_SET], "attendant.wav");
  PreloadSound(fullPath, SFX_CALL_ATTENDANT);

  sprintf(fullPath, "%s/%d/%s", basePath, gOptionValues[SETUP_OPTION_SOUND_SET], "freegame.wav");
  PreloadSound(fullPath, SFX_FREE_GAME);

  sprintf(fullPath, "%s/%d/%s", basePath, gOptionValues[SETUP_OPTION_SOUND_SET], "attract.wav");
  PreloadSound(fullPath, SFX_ATTRACT_SONG);
}

void InitLongshot() {
  LoadSounds();
  EndGame();
}

void UpdateLongshot() {

    if (gMachineIn.coinClicks >= gOptionValues[SETUP_OPTION_COINCOUNT]) {
          StartNewGame();
          return;
    }

    // clear the ball solenoid
    if (gMachineOut.switches & (1 << SWITCH_SOLENOID))
      gMachineOut.switches &= ~(1 << SWITCH_SOLENOID);

    // score up
    if (gMachineInPrev.hundredClicks < gMachineIn.hundredClicks) {
        PlaySound(SFX_100_POINTS);
        gMachineOut.score += (50 * (gMachineIn.hundredClicks - gMachineInPrev.hundredClicks));
    }

    // score up
    if (gMachineInPrev.scoreClicks < gMachineIn.scoreClicks) {

        gMachineOut.score += (10 * (gMachineIn.scoreClicks - gMachineInPrev.scoreClicks));
    
        // play the appropriate
        int newScore = gMachineOut.score - gMachineOutPrev.score;
        if (newScore <= 10) PlaySound(SFX_10_POINTS);
        else if (newScore <= 20)  PlaySound(SFX_20_POINTS);
        else if (newScore <= 30)  PlaySound(SFX_30_POINTS);
        else if (newScore <= 40)  PlaySound(SFX_40_POINTS);
        else if (newScore <= 50)  PlaySound(SFX_50_POINTS);
        else if (newScore <= 100) PlaySound(SFX_100_POINTS);
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
            if (ticketsEarned > gTicketsDispensed) {
                int diff = ticketsEarned - gTicketsDispensed;
                
                if (diff > 0 && gTicketsDispensed == 0) {
                  PlaySound(SFX_WINNER_SONG);
                }

                gMachineOut.dispense = diff;
                gTicketsDispensed += diff;
            }
        }
    }

    // balls played
    if (gMachineInPrev.ballClicks < gMachineIn.ballClicks)
    {
        gMachineOut.ballCount += (gMachineIn.ballClicks - gMachineInPrev.ballClicks);
        if (gMachineOut.ballCount >= gOptionValues[SETUP_OPTION_BALLCOUNT]) 
          EndGame();
    }
}
