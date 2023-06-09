void idler(){
  shifter.idle();
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
   idleOff.disable(); 
   idleFlash.reset();
   idleFlash.enable();
}
void solenoidOff(){
  digitalWrite(solenoid, HIGH);
  solenoidTimer.disable();
}

void gameOverLightBlink(){
  if(bitRead(PORTC, 5)){
    digitalWrite(gameOverLight, LOW);
  }
  else{
    digitalWrite(gameOverLight,HIGH);
  }
}

void freeGameLightBlink(){
  if(bitRead(PORTC, 6)==HIGH){
    digitalWrite(freeGameLight, LOW);
  }
  else{
    digitalWrite(freeGameLight,HIGH);
  }
}

void winLightBlink(){
  if(bitRead(PORTC, 7)){
    digitalWrite(winLight, LOW);
  }
  else{
    digitalWrite(winLight,HIGH);
  }
}

void beaconOff(){
  if(digitalRead(beacon)==HIGH){//why does this work? confused
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
