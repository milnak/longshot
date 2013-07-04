
int dispense_tickets(){
   if(gameState == false && ticketError == 1){
        idle.disable();
        score = (dispense - ticketsDispensed);
        ballCount = 0;
        shifter.display(score,ballCount);
     }
     if(gameState == false && ticketError ==0){
       idle.enable();
     }
  if((dispense - ticketsDispensed) > 0){
      if(ticketTimer < 4000){
        digitalWrite(ticketDispenser, HIGH);
        checkButtonInput(ticketDebounce);
          if(ticketDebounce.getClicks()==0){
            ticketTimer++;
          }
        else{
          ticketTimer = 0;
        }
        ticketsDispensed += ticketDebounce.getClicks();
        ticketDebounce.setClicks(0);
     }
      else{
        digitalWrite(ticketDispenser,LOW); //no tickets/timeout turn off TD
        checkButtonInput(ticketDebounce); //keep checking TD for ticket insertion
        if(ticketDebounce.getClicks()==0){
          ticketTimer++;
          ticketError = 1; //let Pi know there is a problem with the tixdisp
        }
        else{
          //dispense++; //I think because we need to pass at least one ticket by the sensor
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

  
