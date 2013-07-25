/*void integerToBytes(int val, byte b[4]) {
 b[0] = (byte )((val >> 24) & 0xff);
 b[1] = (byte )((val >> 16) & 0xff);
 b[2] = (byte )((val >> 8) & 0xff);
 b[3] = (byte )(val & 0xff);
}*/

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
  commandByte = 1;
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
    /*score =  state[0] << 24 | state[1] << 16 | state[2] << 8 | state[3];
    switches = state[4] << 24 | state[5] << 16 | state[6] << 8 | state[7];
    int disp_byte = state[8] << 24 | state[9] << 16 | state[10] << 8 | state[11];
    ballCount = state[12] << 24 | state[13] << 16 | state[14] << 8 | state[15];
    */
     score =   state[2] << 8 | state[3];
    switches = state[6] << 8 | state[7];
    int disp_byte =  state[10] << 8 | state[11];
    ballCount =  state[14] << 8 | state[15];
    dispense = dispense + disp_byte;
    Serial3.print("disp_byte:");
    Serial3.println(disp_byte);
    Serial3.print("Dispense + disp_byte:");
    Serial3.println(dispense);

    if(bitRead(switches,0) == 1 ){ //0 is off
     //turn free game lamp on
     //freeGameLightTimer.reset();
     digitalWrite(freeGameLight,HIGH);
     freeGameLightTimer.enable();
   }
   if(bitRead(switches,0) == 0 && gameState ==true){
     digitalWrite(freeGameLight,LOW);
     freeGameLightTimer.disable();
   }
   
   if(bitRead(switches,1) == 1){
     //turn game over lamp on
     //gameOverLightTimer.reset();
     digitalWrite(gameOverLight,HIGH);
     gameOverLightTimer.enable();
   }
     if(bitRead(switches,1) == 0 && gameState ==true){
     digitalWrite(gameOverLight, LOW);
     gameOverLightTimer.disable();
   }
     
   if(bitRead(switches,2) == 1){
     //turn winner lamp on
     //winLightTimer.reset();
     digitalWrite(winLight,HIGH);
     winLightTimer.enable();
   }
     if(bitRead(switches,2) == 0 && gameState ==true){
     digitalWrite(winLight,LOW);
     winLightTimer.disable();
   }
  
   if(bitRead(switches,3) == 0){
     //turn beacon lamp on
     beaconTimer.reset();
     beaconTimer.enable();
   }
  
   if(bitRead(switches,4) == 1){
     //turn coin meter on
     digitalWrite(coinMeter,HIGH);
     coinMeterTimer.reset();
     coinMeterTimer.enable();
   }
   
   if(bitRead(switches,5) == 1){
     //turn ticket meter on
     digitalWrite(ticketMeter,HIGH);
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
      if(dispense > 0 && ticketsOwed == 0){
           ticketsOwed = dispense;
           dispense = 0;
           
       }
       if (dispense > 0 && ticketsOwed > 0){
         Serial3.print("Dispense:");
         Serial3.println(dispense);
         Serial3.print("TicketsOwed:");
         Serial3.println(ticketsOwed);
          ticketsOwed += dispense;
          Serial3.print("TicketsOwed+dispense:");
          Serial3.println(ticketsOwed);
          dispense = 0;
          
       }
       
    }
     
    gameState = false;
 }
   else{
     
     coinDebounce.setClicks(0);
     if(gameState == false){  //this might be bad...we might be restarting the game and still be in gameState=true
       ticketsDispensed = 0;
      
       coinMeterTimer.reset();
       coinMeterTimer.enable();
       gameOverLightTimer.disable();
       freeGameLightTimer.disable();
       winLightTimer.disable();
       digitalWrite(winLight,LOW);
       digitalWrite(freeGameLight,LOW);
       digitalWrite(gameOverLight,LOW);
       idleFlash.disable();
       idleOff.disable();
       scoreDebounce.setClicks(0);
       hundredDebounce.setClicks(0);
       ballCountDebounce.setClicks(0);
       
      }
     gameState = true;
    }
  }



