void integerToBytes(long val, byte b[4]) {
 b[0] = (byte )((val >> 24) & 0xff);
 b[1] = (byte )((val >> 16) & 0xff);
 b[2] = (byte )((val >> 8) & 0xff);
 b[3] = (byte )(val & 0xff);
}

void sendGameStatus(){
  int b[9] = {ticketsDispensed,scoreClicks,hundredClicks,ballClicks,
                  coinClicks,upClicks,downClicks,selectClicks,setupClicks  };
   
 // int numBytes = sizeof(int) * 9;
 // byte* data = (byte*)malloc(numBytes);               
  //byte* ptr = data;
  
  for(int x = 0; x < 9; x++){
    byte testByte[4];
    integerToBytes(b[x], testByte);
    Serial.write(testByte,sizeof(testByte));
  }
 // int testInt = 57;
  
//integerToBytes(scoreClicks,testByte);
  
  //int count = Serial.write(testByte,sizeof(testByte));
 
  //Serial.write(data, numBytes);
 // free(data);
  
  //ticketsDispensed = 0; //this is me passing back the number of tickets dispensed since last
  //scoreClicks = 0;
// coinClicks = 0;
  //hundredClicks = 0;
 // ballClicks = 0;
  //upClicks = 0
 // downClicks = 0;
 // selectClicks = 0;
 // setupClicks = 0;

}

void getGameStatus(){
  if(Serial.available()){
    if(Serial.readBytesUntil('\0',(char *)state,4) == 4){
    //had to do a cast here even though docs say it can take byte[]
    
    sendGameStatus();
    parseGameState(state);
    }
    //state needs to return tickets that need to be dispensed, current score, current ball count, solenoid status, lamps status, beacon status,meter status
  }
}

void parseGameState(byte* state){
    //dispense = state[1]; //this is the number of tickets, on/off is in the packed byte
    score = state[2];//this isn't going to work, this is a 3 digit number
    //also how are we going to convert 3 separate chars back into the int?
    //ex: int someInt = someChar - '0';
    //or: int number = atoi(input);
    //looks like atoi will convert a char array
    ballCount = state [3];
    if(bitRead(state[0],0) == 1){
   //turn free game lamp on
   digitalWrite(freeGameLight,LOW);
   }
   else{
   digitalWrite(freeGameLight,HIGH);
   //turn free game lamp off
   }
   if(bitRead(state[0],1) == 1){
   //turn game over lamp on
   digitalWrite(gameOverLight, LOW);
   }
   else{
   //turn game over lamp off
   digitalWrite(gameOverLight,HIGH);
   
   }
   if(bitRead(state[0],2) == 1){
   //turn winner lamp on
   digitalWrite(winLight,LOW);
   }
   else{
   //turn winner lamp off
   digitalWrite(winLight,HIGH);
   
   }
   if(bitRead(state[0],3) == 1){
   //turn beacon lamp on
   digitalWrite(beacon, HIGH);
   }
   else{
   //turn beacon lamp off
   digitalWrite(beacon,LOW);
   }
   if(bitRead(state[0],4) == 1){
   //turn coin meter on
   digitalWrite(coinMeter,LOW);
   }
   else{
   //turn coin meter off
   digitalWrite(coinMeter,HIGH);
   
   }
   if(bitRead(state[0],5) == 1){
   //turn ticket meter on
   digitalWrite(ticketMeter,LOW);
   }
   else{
   //turn ticket meter off
   digitalWrite(ticketMeter,HIGH);
   }
   if(bitRead(state[0],6) == 1){
   //turn solenoid on 
   digitalWrite(solenoid, HIGH);
   }
   else{
   //turn solenoid off
   digitalWrite(solenoid,LOW);
   }
   if(bitRead(state[0],7) == 1){
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
  if(theButton.getClicks() == 1){
    clearClicks(theButton);   
    return 1;
    //do we want to ditch this and instead call getClicks when we pass
    //it up to the Pi to handle multiple clicks?
  }
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



