void poll_inputs(){
  if(checkButtonInput(setupDebounce)==1){
    //Serial.println("Setup Button Click");
    setupClicks++;
    
  }
  if(checkButtonInput(selectDebounce)==1){
    //Serial.println("Select Button Click");
    selectClicks++;
    
  }
  if(checkButtonInput(upDebounce)==1){
    //Serial.println("Up Button Click");
    upClicks++;
    
  }
  if(checkButtonInput(downDebounce)==1){
    //Serial.println("Down Button Click");
    downClicks++;
   
  }
  if(checkButtonInput(scoreDebounce)==1){
    //Serial.println("Score Switch +10");
    scoreClicks++;
    //Serial.println(scoreClicks);
   
  }
  if(checkButtonInput(hundredDebounce)==1){
    //Serial.println("Score Switch +50");
    hundredClicks++;
    
  }
  if(checkButtonInput(coinDebounce)==1){
    //Serial.println("Coin Switch Click");
    coinClicks++;
    
  }
  if(checkButtonInput(ballCountDebounce)==1){
    //Serial.println("Ball Count Click");
    ballClicks++;
    
  }
}
