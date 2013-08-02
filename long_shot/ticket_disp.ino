
int dispense_tickets(){
  checkButtonInput(ticketDebounce);
  
  //lets check this once at first and then just check ticketDebounce.getclicks() after that
   if(gameState != 1 && gameState != 5 && ticketError == 1){
     //if the game is over and there is a ticket dispenser error
        idle.disable(); //turn off spinning zeros
         //set the score to the difference of the number of tickets to
        //be dispensed and the number of tickets dispensed so far
        shifter.display(ticketsOwed,ballCount);
        //ok so we stuffed the number of tickets owed and we're displaying it instead of
        //idle (only active when there isn't a game going on so it doesn't interfere with
        //normal gameplay)
     }
     
     if(ticketTimer < 3000){
        digitalWrite(ticketDispenser, HIGH); //activate motor
        ticketTimer++;
     }
     else{

          ticketError = 1;
          digitalWrite(ticketDispenser, LOW); //no tickets, deactivate motor

           }
      
       if(ticketDebounce.getClicks() > 0){
          if(bitRead(PORTL,7)){//check to see if the motor is active
          if(ticketsOwed > 0){
            ticketsOwed -=(int)ticketDebounce.getClicks();//pay out previously owed tickets first
          }
          else{
            dispense -= (int)ticketDebounce.getClicks(); 
            ticketsDispensed += (int)ticketDebounce.getClicks();
          }
         
             digitalWrite(ticketMeter,HIGH);
             ticketMeterTimer.reset();
             ticketMeterTimer.enable();
          }
           ticketDebounce.setClicks(0);
           ticketTimer = 0; // reset the timer
        }
  if(dispense <= 0 && ticketsOwed <= 0){
     
     ticketError = 0;
     digitalWrite(ticketDispenser,LOW);
     if(gameState == 0 || gameState == 2 || gameState == 3){
       ticketsDispensed = 0;
       idle.enable();
      }  
   }
}
