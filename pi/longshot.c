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

int gTicketsDispensed = 0;
int gTicketTableSelection = 0;

void InitLongshot() {
    gTicketsDispensed = 0;
}

void UpdateLongshot() {

    // score up
    if (gMachineInPrev.hundredClicks < gMachineIn.hundredClicks)
        gMachineOut.score += (50 * (gMachineIn.hundredClicks - gMachineInPrev.hundredClicks));
    
    // score up
    if (gMachineInPrev.scoreClicks < gMachineIn.scoreClicks)
        gMachineOut.score += (10 * (gMachineIn.scoreClicks - gMachineInPrev.scoreClicks));

    // balls played
    if (gMachineInPrev.ballClicks < gMachineIn.ballClicks)
        gMachineOut.ballCount += (gMachineIn.ballClicks - gMachineInPrev.ballClicks);
    
    
    // we haz points! we can haz tix?
    gMachineOut.dispense = 0;
    if (gMachineOut.score > gMachineOutPrev.score) {
        int tableIndex = 0;
        int ticketsEarned = 0;

        if (gMachineOut.score <= 50) tableIndex = 0;
        if (gMachineOut.score >= 60  && gMachineOut.score <= 100) tableIndex++;
        if (gMachineOut.score >= 110 && gMachineOut.score <= 200) tableIndex++;
        if (gMachineOut.score >= 210 && gMachineOut.score <= 300) tableIndex++;
        if (gMachineOut.score >= 310 && gMachineOut.score <= 400) tableIndex++;
        if (gMachineOut.score >= 410 && gMachineOut.score <= 500) tableIndex++;
        if (gMachineOut.score >= 510 && gMachineOut.score <= 600) tableIndex++;
        if (gMachineOut.score >= 610 && gMachineOut.score <= 700) tableIndex++;
        if (gMachineOut.score > 700) tableIndex++;

        if (tableIndex < 9) {
            ticketsEarned = gTickMatrix[gTicketTableSelection][tableIndex];
            if (ticketsEarned > gTicketsDispensed) {
                int diff = ticketsEarned - gTicketsDispensed;
                gMachineOut.dispense = diff;
                gTicketsDispensed += diff;
            
                printf("## LONGSHOT: Total Tickets Earned: %d Dispense Request: %d Total Dispensed: %d", 
                    ticketsEarned, diff, gTicketsDispensed);
            }
        }

    }
}
