
int dispense_tickets(){
  if(dispense > 0){
    digitalWrite(ticketDispenser, HIGH);
    
    if(checkButtonInput(ticketDebounce) > 0){
      dispense = dispense - ticketDebounce.getClicks();
      ticketsDispensed = ticketsDispensed + ticketDebounce.getClicks();
            }
    }
      if(dispense == 0){
        digitalWrite(ticketDispenser, LOW);
                  }

  }
