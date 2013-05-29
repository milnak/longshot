
int dispense_tickets(){
  if((dispense - ticketsDispensed) > 0){
    digitalWrite(ticketDispenser, HIGH);
    checkButtonInput(ticketDebounce);
    ticketsDispensed = ticketsDispensed + ticketDebounce.getClicks();
            }
   if((dispense - ticketsDispensed) == 0){
        digitalWrite(ticketDispenser, LOW);
                  }
    }
      

  
