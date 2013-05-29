void integerToBytes(long val, byte b[4]) {
 b[0] = (byte )((val >> 24) & 0xff);
 b[1] = (byte )((val >> 16) & 0xff);
 b[2] = (byte )((val >> 8) & 0xff);
 b[3] = (byte )(val & 0xff);
}


void sendGameStatus(){
  int b[9] = {ticketsDispensed,scoreDebounce.getClicks(),hundredDebounce.getClicks(),
                  ballCountDebounce.getClicks(),coinDebounce.getClicks(),
                  upDebounce.getClicks(),downDebounce.getClicks(),
                  selectDebounce.getClicks(),setupDebounce.getClicks()  };
  
  for(int x = 0; x<=8; x++){
    byte testByte[4];
    integerToBytes(b[x], testByte);
    Serial.write(testByte,sizeof(testByte));
  }
}

void getGameStatus(){
  if(Serial.available()){
     if(Serial.readBytesUntil('\n',(char *)state,7)==7){
      //had to do a cast here even though docs say it can take byte[]
      sendGameStatus();
      parseGameState(state);
      }
    //state needs to return tickets that need to be dispensed, current score, current ball count, solenoid status, lamps status, beacon status,meter status
  }
}

void parseGameState(byte* state){
    dispense = dispense + state[5]; //this is the number of tickets, on/off is in the packed byte
    score = 0;
    score |=  state[3] << 24;
    score |=  state[2] << 16;
    score |=  state[1] << 8;
    score |=  state[0];

    
    ballCount = state[6];
    if(bitRead(state[4],0) == 1){
   //turn free game lamp on
   digitalWrite(freeGameLight,LOW);
   }
   else{
   digitalWrite(freeGameLight,HIGH);
   //turn free game lamp off
   }
   if(bitRead(state[state[4]],1) == 1){
   //turn game over lamp on
   digitalWrite(gameOverLight, LOW);
   }
   else{
   //turn game over lamp off
   digitalWrite(gameOverLight,HIGH);
   
   }
   if(bitRead(state[4],2) == 1){
   //turn winner lamp on
   digitalWrite(winLight,LOW);
   }
   else{
   //turn winner lamp off
   digitalWrite(winLight,HIGH);
   
   }
   if(bitRead(state[4],3) == 1){
   //turn beacon lamp on
   digitalWrite(beacon, HIGH);
   }
   else{
   //turn beacon lamp off
   digitalWrite(beacon,LOW);
   }
   if(bitRead(state[4],4) == 1){
   //turn coin meter on
   digitalWrite(coinMeter,LOW);
   }
   else{
   //turn coin meter off
   digitalWrite(coinMeter,HIGH);
   
   }
   if(bitRead(state[4],5) == 1){
   //turn ticket meter on
   digitalWrite(ticketMeter,LOW);
   }
   else{
   //turn ticket meter off
   digitalWrite(ticketMeter,HIGH);
   }
   if(bitRead(state[4],6) == 1){
   //turn solenoid on 
   digitalWrite(solenoid, HIGH);
   }
   else{
   //turn solenoid off
   digitalWrite(solenoid,LOW);
   }
   if(bitRead(state[4],7) == 1){
   //turn on ticket dispenser 
   digitalWrite(ticketDispenser, HIGH);
   }
   else{
   //turn off ticket dispenser
   digitalWrite(ticketDispenser,LOW);
   }
   
}

int checkButtonInput(Bounce &theButton){
   buttonDebounce(theButton);
    }

void clearClicks(Bounce &theButton){
  theButton.setClicks(0);
}

void buttonDebounce(Bounce &theButton){
  if( theButton.update()){
    if(theButton.read() == HIGH && theButton.getLastState() == 0){
      theButton.setClicks((theButton.getClicks() + 1));
      theButton.setLastState(1);
    }
    else{
      theButton.setLastState(0);
    }
  }
}

void idler(){
  shifter.idle();
}



