
int dispense_tickets(){
  if((dispense - ticketsDispensed) > 0){
      if(ticketTimer < 10000){
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
          dispense++; //I think because we need to pass at least one ticket by the sensor
          //before we actually have a ticket in the "dispense" position we need to increment here
          //maybe even by two?
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

  
