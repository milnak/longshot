#include <Shifter.h>
#include <TimedAction.h>
#include <Bounce.h>

///////////////////pin defs////////////////////////////////
//inputs
#define coinSwitch  A2
#define scoreSwitch A3 
#define hundredSwitch  A1
#define ballCountSwitch  A4
#define selectButton  26
#define downButton  27
#define upButton  28
#define setupButton  29
#define ticketSensor  A5

//outputs
#define winLight  32
#define gameOverLight  31
#define freeGameLight 30
#define beacon A9
#define coinMeter  37
#define ticketMeter  A11
#define solenoid A8
#define ticketDispenser  A10

//7seg pins
#define SRCK 33 
#define RCK 36
#define SIN 35

////////////////////////////////////////////////////////////

boolean gameState=false;
byte state[16];
int score = 0;
int ballCount = 0;
Shifter shifter(4, SRCK,SIN,RCK); //these are the shift registers that contol the 7 seg displays
int switches = 0;
///////////////////////////////////////////////////////////

TimedAction idle = TimedAction(100,idler);
TimedAction solenoidTimer = TimedAction(10 * 1000,solenoidOff);
TimedAction ticketMeterTimer = TimedAction(25, ticketMeterClick);
TimedAction coinMeterTimer = TimedAction(25, coinMeterClick);
TimedAction gameOverLightTimer = TimedAction(1000, gameOverLightBlink);
TimedAction freeGameLightTimer = TimedAction(1000, freeGameLightBlink);
TimedAction winLightTimer = TimedAction(1000, winLightBlink);
TimedAction beaconTimer = TimedAction(10 * 1000, beaconOff);
TimedAction idleFlash = TimedAction(3 * 1000, idleFlashOn);
TimedAction idleOff = TimedAction (3*1000, idleFlashOff);

//////////////ticket dispensor variables///////////////////
int dispense = 0;
int notchRead = 111;
int ticketsDispensed = 0;
int ticketMeterClicks = 0;
long ticketTimer = 0;
int ticketError = 0;
/////////////////////////////////////////////////////////// 
int commandByte = 255;
//////////////////Score Vars////////////////////////////
int scoreClicks = 0;
int hundredClicks = 0;
int ballClicks = 0;
int coinClicks = 0;
int upClicks = 0;
int downClicks = 0;
int selectClicks = 0;
int setupClicks = 0;

;

Bounce scoreDebounce = Bounce(scoreSwitch,25);
Bounce coinDebounce = Bounce(coinSwitch,50);
Bounce hundredDebounce = Bounce(hundredSwitch,50);
Bounce ballCountDebounce = Bounce(ballCountSwitch,50);
Bounce ticketDebounce = Bounce(ticketSensor,8);
Bounce selectDebounce = Bounce(selectButton,20);
Bounce setupDebounce = Bounce(setupButton,20);
Bounce upDebounce = Bounce(upButton,20);
Bounce downDebounce = Bounce(downButton,20);



///////////////////////////////////

void setup(){
  Serial.begin(57600);
  delay(2000);
  idle.enable();
  solenoidTimer.disable();
  pinMode(gameOverLight, OUTPUT);
  pinMode(winLight, OUTPUT);
  pinMode(solenoid, OUTPUT);
  pinMode(beacon, OUTPUT);
  
  pinMode(ticketDispenser, OUTPUT);
  digitalWrite(ticketDispenser, LOW);
  pinMode(ticketSensor, INPUT);
  
  pinMode(coinSwitch, INPUT);
  pinMode(scoreSwitch, INPUT);
  pinMode(hundredSwitch, INPUT);
  pinMode(ballCountSwitch, INPUT);
  
  pinMode(ticketMeter, OUTPUT);
  pinMode(coinMeter, OUTPUT);
  
  pinMode(upButton, INPUT);
  pinMode(downButton, INPUT);
  pinMode(setupButton, INPUT);
  pinMode(selectButton,INPUT);
 
 }

void loop(){
  poll_inputs(); //get switch states
  updateGame(); //pull the state from Pi, push switches to pi, parse out state
  //TimedAction checks//
  solenoidTimer.check();
  idle.check();
  gameOverLightTimer.check();
  winLightTimer.check();
  freeGameLightTimer.check();
  beaconTimer.check();
  idleFlash.check();
  idleOff.check();
  ticketMeterTimer.check();
  coinMeterTimer.check();
  /////////////////////////////////////
  
  
  
 if (dispense >0){ //temperoarily disable tickets for sanity of coworkers
   dispense_tickets();
  }
  
   
   
 if(gameState==true){
        idle.disable();
        shifter.display(score,ballCount);
     }
 if(gameState == false && ticketError == 0){
       //if the game is over and there are no ticket errors
       idle.enable();
       //display the normal idle display
     }
 }
  

