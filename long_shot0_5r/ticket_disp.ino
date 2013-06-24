
int dispense_tickets(){
  if((dispense - ticketsDispensed) > 0){
      if(ticketTimer < 100){
        digitalWrite(ticketDispenser, HIGH);
        checkButtonInput(ticketDebounce);
        ticketsDispensed = ticketsDispensed + ticketDebounce.getClicks();
        ticketDebounce.setClicks(0);
        ticketTimer++;
            }
      else{
        digitalWrite(ticketDispenser,LOW);
        checkButtonInput(ticketDebounce);
        if(ticketsDispensed == (ticketsDispensed + ticketDebounce.getClicks())){
          ticketTimer++;
        }
        else{
          ticketTimer = 0;
          digitalWrite(ticketDispenser,HIGH);  
        }
      }
    }
   if((dispense - ticketsDispensed) == 0){
        digitalWrite(ticketDispenser, LOW);
        ticketTimer = 0;
                  }
    
}

  
