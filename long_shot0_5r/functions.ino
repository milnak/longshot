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
      if(Serial.readBytes((char *)state,16)){
        sendGameStatus();
        parseGameState(state);
      }
    //state needs to return tickets that need to be dispensed, current score, current ball count, solenoid status, lamps status, beacon status,meter status
  }
  
}

void parseGameState(byte* state){
   
    score = 0;
    switches=0;
    ballCount = 0;
    score =  state[0] << 24 | state[1] << 16 | state[2] << 8 | state[3];
    switches = state[4] << 24 | state[5] << 16 | state[6] << 8 | state[7];
    int disp_byte = state[8] << 24 | state[9] << 16 | state[10] << 8 | state[11];
    ballCount = state[12] << 24 | state[13] << 16 | state[14] << 8 | state[15];
    dispense = dispense + disp_byte;

    if(bitRead(switches,0) == 1){
   //turn free game lamp on
   digitalWrite(freeGameLight,LOW);
   }
   else{
   digitalWrite(freeGameLight,HIGH);
   //turn free game lamp off
   }
   if(bitRead(switches,1) == 1){
   //turn game over lamp on
   digitalWrite(gameOverLight, LOW);
   }
   else{
   //turn game over lamp off
   digitalWrite(gameOverLight,HIGH);
   
   }
   if(bitRead(switches,2) == 1){
   //turn winner lamp on
   digitalWrite(winLight,LOW);
   }
   else{
   //turn winner lamp off
   digitalWrite(winLight,HIGH);
   
   }
   if(bitRead(switches,3) == 1){
   //turn beacon lamp on
   digitalWrite(beacon, HIGH);
   }
   else{
   //turn beacon lamp off
   digitalWrite(beacon,LOW);
   }
   if(bitRead(switches,4) == 1){
   //turn coin meter on
   digitalWrite(coinMeter,LOW);
   }
   else{
   //turn coin meter off
   digitalWrite(coinMeter,HIGH);
   
   }
   if(bitRead(switches,5) == 1){
   //turn ticket meter on
   digitalWrite(ticketMeter,LOW);
   }
   else{
   //turn ticket meter off
   digitalWrite(ticketMeter,HIGH);
   }
   if(bitRead(switches,6) == 1){
     //turn solenoid on 
     digitalWrite(solenoid,HIGH);
     solenoidTimer.reset();
     solenoidTimer.enable();
   }
   else{
   //turn solenoid off
   
   }
   if(bitRead(switches,7) == 1){
    if(gameState == true){
       idle.enable();
     }
     gameState = false;
  }
   else{
     
     coinDebounce.setClicks(0);
     if(gameState == false){
       scoreDebounce.setClicks(0);
       hundredDebounce.setClicks(0);
       ballCountDebounce.setClicks(0);
     }
     gameState = true;
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

void solenoidOff(){
  digitalWrite(solenoid, LOW);
  solenoidTimer.disable();
}

void gameOverLightOn(){
  digitalWrite(gameOverLight, LOW);
  gameOverLightOnTimer.disable();
}

void gameOverLightOff(){
  digitalWrite(gameOverLight, HIGH);
  gameOverLightOffTimer.disable();
}

void freeGameLightOn(){
  digitalWrite(freeGameLight, LOW);
  freeGameLightOnTimer.disable();
}

void freeGameLightOff(){
  digitalWrite(freeGameLight, HIGH);
  freeGameLightOffTimer.disable();
}

void winLightOn(){
  digitalWrite(winLight, LOW);
  winLightOnTimer.disable();
}

void winLightOff(){
  digitalWrite(winLight, HIGH);
  winLightOffTimer.disable();
}

void beaconOff(){
  digitalWrite(beacon, HIGH);
  beaconTimer.disable();
}

void ticketMeterClick(){
   digitalWrite(ticketMeter, LOW);
   ticketMeterTimer.disable(); 
}

void coinMeterClick(){
   digitalWrite(coinMeter, LOW);
   coinMeterTimer.disable(); 
}
