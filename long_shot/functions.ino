void sendGameState(){
int b[11] = { commandByte,ticketsDispensed,scoreDebounce.getClicks(),hundredDebounce.getClicks(),
                  ballCountDebounce.getClicks(),coinDebounce.getClicks(),
                  upDebounce.getClicks(),downDebounce.getClicks(),
                  selectDebounce.getClicks(),setupDebounce.getClicks(),ticketError};
  
  for(int x = 0; x<=10; x++){
    //byte statusByte[4];
    //integerToBytes(b[x], statusByte);
    //Serial.write(statusByte,sizeof(statusByte));
    Serial.write(b[x]);
  }
  if(coinDebounce.getClicks()>0){
     digitalWrite(coinMeter,HIGH);
     coinMeterTimer.reset();
     coinMeterTimer.enable();
  }
  coinDebounce.setClicks(0);
  commandByte = 1;
}

void updateGame(){
  if(Serial.available()){
      if(Serial.readBytes((char *)state,20)){ //read in the 16 byte game status from Pi
        sendGameState(); //send out the current switch states
        parseGameState(state); //act on the game status from Pi
       }
   }
}

void parseGameState(byte* state){
    prevGameState = gameState;
    gameState = 0;
    gameState = state[18] << 8 | state[19];
    
    score = 0;
    switches = 0;
    ballCount = 0;

    score =   state[2] << 8 | state[3];
    switches = state[6] << 8 | state[7];
    ballCount =  state[14] << 8 | state[15];
    if((gameState == 2 || gameState == 5) && prevGameState != 5){
      //catch lastscore and ballcount before they are cleared
      lastScore = score;
      lastBallCount = ballCount;
    }
    if(gameState == 1){ //little hacky, only accept new tickets if we are in active game
      int disp_byte =  state[10] << 8 | state[11];
      dispense += disp_byte;
    }

    if(gameState == 2 || gameState == 0){
      //END_GAME state - clear vars

       scoreDebounce.setClicks(0);
       hundredDebounce.setClicks(0);
       ballCountDebounce.setClicks(0);
       ticketsOwed += dispense;
       dispense = 0;
       ticketsDispensed = 0;
    }

    if(gameState == 1 && (prevGameState == 0 || prevGameState == 2 || prevGameState == 3 || prevGameState == 4)){
      //start a new game
      ticketsDispensed = 0;
      
      //this might be bad...we might be restarting the game and still be in gameState=true
       gameOverLightTimer.disable();
       freeGameLightTimer.disable();
       winLightTimer.disable();
       digitalWrite(winLight,LOW);
       digitalWrite(freeGameLight,LOW);
       digitalWrite(gameOverLight,LOW);
       idleFlash.disable();
       idleOff.disable();
    }
     
    if(bitRead(switches,0) == 1 ){ //0 is off
     //turn free game lamp on
     //freeGameLightTimer.reset();
     digitalWrite(freeGameLight,HIGH);
     freeGameLightTimer.enable();
   }
   if(bitRead(switches,0) == 0 && gameState == 1){
     digitalWrite(freeGameLight,LOW);
     freeGameLightTimer.disable();
   }
   
   if(bitRead(switches,1) == 1){
     //turn game over lamp on
     //gameOverLightTimer.reset();
     digitalWrite(gameOverLight,HIGH);
     gameOverLightTimer.enable();
   }
     if(bitRead(switches,1) == 0 && gameState == 1){
     digitalWrite(gameOverLight, LOW);
     gameOverLightTimer.disable();
   }
     
   if(bitRead(switches,2) == 1){
     //turn winner lamp on
     //winLightTimer.reset();
     digitalWrite(winLight,HIGH);
     winLightTimer.enable();
   }
     if(bitRead(switches,2) == 0 && gameState == 1){
     digitalWrite(winLight,LOW);
     winLightTimer.disable();
   }
  
   if(bitRead(switches,3) == 0){
     //turn beacon lamp on
     beaconTimer.reset();
     beaconTimer.enable();
   }
  
   if(bitRead(switches,4) == 1){
     /* //turn coin meter on
     Serial3.println("COIN METER!!");
     digitalWrite(coinMeter,HIGH);
     coinMeterTimer.reset();
     coinMeterTimer.enable();*/
   }
   
   if(bitRead(switches,5) == 1){
     //turn ticket meter on
     digitalWrite(ticketMeter,HIGH);
     ticketMeterTimer.reset();
     ticketMeterTimer.enable();
   }
   
   if(bitRead(switches,6) == 1){
     //turn solenoid on 
     digitalWrite(solenoid,LOW);
     solenoidTimer.reset();
     solenoidTimer.enable();
   }
   
   if(bitRead(switches,7) == 1){
    if(ticketError == 0 && gameState != 3)
      idle.enable();
   }
   else{
     idle.disable();
     //gameState = 0; //not sure about this...we need to drop out for setup mode
   }
       
  }



