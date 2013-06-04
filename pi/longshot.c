#include "machine.h"
#include "longshot.h"


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

enum GameState {
  GAMESTATE_IDLE,
  GAMESTATE_GAME
};

int gTicketsDispensed = 0;
int gTicketTableSelection = 0;
int gGameState = GAMESTATE_IDLE;
int gRequiredCoins = 4;
int gMaxBallCount = 9;

void InitLongshot() {
    EndGame();
}

void StartNewGame() {
    gGameState = GAMESTATE_GAME;
    gMachineOut.switches &= ~(1 << SWITCH_IDLELIGHT);
    gMachineOut.score = 0;
    gMachineOut.ballCount = 0;
}

void EndGame() {
    gGameState = GAMESTATE_IDLE;
    gMachineOut.switches |= (1 << SWITCH_IDLELIGHT);
    gMachineOut.score = 0;
    gMachineOut.ballCount = 0;
    gTicketsDispensed = 0;
}

void UpdateLongshot() {

    if (gGameState == GAMESTATE_IDLE) {
      gMachineOut.ballCount = gMachineIn.coinClicks;
      if (gMachineIn.coinClicks > gRequiredCoins) {
        StartNewGame();
      }
    }

    if (gGameState != GAMESTATE_GAME)
      return;

    // score up
    if (gMachineInPrev.hundredClicks < gMachineIn.hundredClicks)
        gMachineOut.score += (50 * (gMachineIn.hundredClicks - gMachineInPrev.hundredClicks));
    
    // score up
    if (gMachineInPrev.scoreClicks < gMachineIn.scoreClicks)
        gMachineOut.score += (10 * (gMachineIn.scoreClicks - gMachineInPrev.scoreClicks));
    
    // we haz points! we can haz tix?
    gMachineOut.dispense = 0;
    if (gMachineOut.score > gMachineOutPrev.score) {
        int tableIndex    = 0;
        int ticketsEarned = 0;

        if (gMachineOut.score >= 50)  tableIndex = 0;
        if (gMachineOut.score >= 60)  tableIndex++;
        if (gMachineOut.score >= 110) tableIndex++;
        if (gMachineOut.score >= 210) tableIndex++;
        if (gMachineOut.score >= 310) tableIndex++;
        if (gMachineOut.score >= 410) tableIndex++;
        if (gMachineOut.score >= 510) tableIndex++;
        if (gMachineOut.score >= 610) tableIndex++;
        if (gMachineOut.score >= 700) tableIndex++;

        if (tableIndex < 9) {
            ticketsEarned = gTickMatrix[gTicketTableSelection][tableIndex];
            if (ticketsEarned > gTicketsDispensed) {
                int diff = ticketsEarned - gTicketsDispensed;
                gMachineOut.dispense = diff;
                gTicketsDispensed += diff;
            
                printf("###LONGSHOT:\n \tTotal Tickets Earned: %d\n \tDispense Request: %d\n \tTotal Dispensed: %d\n", 
                    ticketsEarned, diff, gTicketsDispensed);
            }
        }
    }

    // balls played
    if (gMachineInPrev.ballClicks < gMachineIn.ballClicks)
    {
        gMachineOut.ballCount += (gMachineIn.ballClicks - gMachineInPrev.ballClicks);
        if (gMachineOut.ballCount > gMaxBallCount) 
          EndGame();
    }
}
