
int dispense_tickets(){
  if((dispense - ticketsDispensed) > 0){
      if(ticketTimer < 10000){
        digitalWrite(ticketDispenser, HIGH);
        checkButtonInput(ticketDebounce);
        ticketsDispensed = ticketsDispensed + ticketDebounce.getClicks();
        if(ticketsDispensed == (ticketsDispensed + ticketDebounce.getClicks())){
          ticketTimer++;
        }
        else{
          ticketTimer = 0;
        }
        ticketDebounce.setClicks(0);
        
            }
      else{
        digitalWrite(ticketDispenser,LOW);
        checkButtonInput(ticketDebounce);
        if(ticketsDispensed == (ticketsDispensed + ticketDebounce.getClicks())){
          ticketTimer++;
          ticketError = 1; //let Pi know there is a problem with the tixdisp
        }
        else{
          dispense++; //I think because we need to pass at least one ticket by the sensor
          //before we actually have a ticket in the "dispense" position we need to increment here
          //maybe even by two?
          ticketTimer = 0;
          ticketError = 0; //clear error flag
          digitalWrite(ticketDispenser,HIGH);  
        }
      }
    }
   if((dispense - ticketsDispensed) == 0){
        digitalWrite(ticketDispenser, LOW);
        ticketTimer = 0;
                  }
    
}

  
