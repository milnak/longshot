
int dispense_tickets(){
  if((dispense - ticketsDispensed) > 0){
    digitalWrite(ticketDispenser, HIGH);
    checkButtonInput(ticketDebounce);
    ticketsDispensed = ticketsDispensed + ticketDebounce.getClicks();
    ticketMeterTimer.reset();
    ticketMeterTimer.enable();
    ticketDebounce.setClicks(0);
            }
   if((dispense - ticketsDispensed) == 0){
        digitalWrite(ticketDispenser, LOW);
                  }
    }
      

  
