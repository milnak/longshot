void integerToBytes(long val, byte b[4]) {
 b[0] = (byte )((val >> 24) & 0xff);
 b[1] = (byte )((val >> 16) & 0xff);
 b[2] = (byte )((val >> 8) & 0xff);
 b[3] = (byte )(val & 0xff);
}


void sendGameState(){
  int b[10] = { commandByte,ticketsDispensed,scoreDebounce.getClicks(),hundredDebounce.getClicks(),
                  ballCountDebounce.getClicks(),coinDebounce.getClicks(),
                  upDebounce.getClicks(),downDebounce.getClicks(),
                  selectDebounce.getClicks(),setupDebounce.getClicks()  };
  
  for(int x = 0; x<=9; x++){
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
     digitalWrite(coinMeter,LOW);
     coinMeterTimer.reset();
     coinMeterTimer.enable();
   }
   
   if(bitRead(switches,5) == 1){
     //turn ticket meter on
     digitalWrite(ticketMeter,LOW);
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
       coinMeterTimer.reset();
       coinMeterTimer.enable();
       scoreDebounce.setClicks(0);
       hundredDebounce.setClicks(0);
       ballCountDebounce.setClicks(0);
     }
     gameState = true;
   }
   
}



