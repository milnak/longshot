
int dispense_tickets(){
   if(gameState == false && ticketError == 1){
     //if the game is over and there is a ticket dispenser error
        idle.disable(); //turn off spinning zeros
        score = (dispense - ticketsDispensed); 
         //set the score to the difference of the number of tickets to
        //be dispensed and the number of tickets dispensed so far
        ballCount = 0;
        //doesn't have to be
        shifter.display(score,ballCount);
        //ok so we stuffed the number of tickets owed and we're displaying it instead of
        //idle (only active when there isn't a game going on so it doesn't interfere with
        //normal gameplay)
     }
     if(gameState == false && ticketError ==0){
       //if the game is over and there is no ticket errors
       idle.enable();
       //display the normal idle display
     }
  if((dispense - ticketsDispensed) > 0){
    //if we still owe tickets
      if(ticketTimer < 4000){
        //and the ticket dispensor hasn't timed out
        digitalWrite(ticketDispenser, HIGH);
        //turn the ticket dispensor on
        checkButtonInput(ticketDebounce);
        //check the ticket sensor for ticket insertion
          if(ticketDebounce.getClicks()==0){
            //if there still aren't any tickets
           // ticketTimer++;
            //increment the timer? wait...maybe not? once we are in time out we are timed out right?
            //just don't reset until we have tickets and we will stay in timeout state...
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

  
