void integerToBytes(long val, byte b[4]) {
 b[0] = (byte )((val >> 24) & 0xff);
 b[1] = (byte )((val >> 16) & 0xff);
 b[2] = (byte )((val >> 8) & 0xff);
 b[3] = (byte )(val & 0xff);
}


void sendGameState(){
  int b[9] = { ticketsDispensed,scoreDebounce.getClicks(),hundredDebounce.getClicks(),
                  ballCountDebounce.getClicks(),coinDebounce.getClicks(),
                  upDebounce.getClicks(),downDebounce.getClicks(),
                  selectDebounce.getClicks(),setupDebounce.getClicks()  };
  
  for(int x = 0; x<=8; x++){
    byte statusByte[4];
    integerToBytes(b[x], statusByte);
    Serial.write(statusByte,sizeof(statusByte));
  }
}

void updateGame(){
  if(Serial.available()){
      if(Serial.readBytes((char *)state,16)){ //read in the 16 byte game status from Pi
        sendGameState(); //send out the current switch states
        parseGameState(state); //act on the game status from Pi
      }
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
     freeGameLightTimer.reset();
     freeGameLightTimer.enable();
   }
   
   if(bitRead(switches,1) == 1){
     //turn game over lamp on
     gameOverLightTimer.reset();
     gameOverLightTimer.enable();
   }
   
   if(bitRead(switches,2) == 1){
     //turn winner lamp on
     winLightTimer.reset();
     winLightTimer.enable();
   }
  
   if(bitRead(switches,3) == 1){
     //turn beacon lamp on
     beaconTimer.reset();
     beaconTimer.enable();
   }
  
   if(bitRead(switches,4) == 1){
     //turn coin meter on
     coinMeterTimer.reset();
     coinMeterTimer.enable();
   }
   
   if(bitRead(switches,5) == 1){
     //turn ticket meter on
     ticketMeterTimer.reset();
     ticketMeterTimer.enable();
   }
   
   if(bitRead(switches,6) == 1){
     //turn solenoid on 
     digitalWrite(solenoid,HIGH);
     solenoidTimer.reset();
     solenoidTimer.enable();
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
  //need to handle the light flashing here too
  idleFlash.enable();
  beaconTimer.enable();
}

void idleFlashOn(){
  gameOverLightTimer.enable();
  freeGameLightTimer.enable();
  winLightTimer.enable();
  idleFlash.disable();
  idleOff.reset();
  idleOff.enable();
}

void idleFlashOff(){
   gameOverLightTimer.disable();
   freeGameLightTimer.disable();
   winLightTimer.disable();
   digitalWrite(gameOverLight, HIGH);
   digitalWrite(freeGameLight,HIGH);
   digitalWrite(winLight,HIGH); //make sure the lights are turned off
   idleOff.disable(); 
   idleFlash.reset();
   idleFlash.enable();
}
void solenoidOff(){
  digitalWrite(solenoid, LOW);
  solenoidTimer.disable();
}

void gameOverLightBlink(){
  if(digitalRead(gameOverLight) == HIGH){
    digitalWrite(gameOverLight, LOW);
  }
  else{
    digitalWrite(gameOverLight,HIGH);
  }
}

void freeGameLightBlink(){
  if(digitalRead(freeGameLight)){
    digitalWrite(freeGameLight, LOW);
  }
  else{
    digitalWrite(freeGameLight,HIGH);
  }
}

void winLightBlink(){
  if(digitalRead(winLight)){
    digitalWrite(winLight, LOW);
  }
  else{
    digitalWrite(winLight,HIGH);
  }
}

void beaconOff(){
  if(digitalRead(beacon)==HIGH){
  digitalWrite(beacon, LOW);
  
  }
  else{
    digitalWrite(beacon,HIGH);
  }
}

void ticketMeterClick(){
   digitalWrite(ticketMeter, LOW);
   ticketMeterTimer.disable(); 
}

void coinMeterClick(){
   digitalWrite(coinMeter, LOW);
   coinMeterTimer.disable(); 
}
