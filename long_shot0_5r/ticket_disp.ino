
int dispense_tickets(){
  if(dispense > 0){
   //Serial.print("Dispense");
   Serial.println(dispense);
   digitalWrite(ticketDispenser, HIGH);
    
    if(checkButtonInput(ticketDebounce) == 1){
      dispense--;
      ticketsDispensed++;
            }
    }
      if(dispense == 0){
        digitalWrite(ticketDispenser, LOW);
                  }

  }
