#include <Shifter.h>
#include <avr/pgmspace.h>
#include <EEPROM.h> //not sure if we still need this
#include <TimedAction.h>
#include <Wire.h> //not sure if we still need this
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
boolean gameStateChange=false;
byte state[4];
int score = 0;
int ballCount = 0;
Shifter shifter(4, SRCK,SIN,RCK); //these are the shift registers that contol the 7 seg displays

///////////////////////////////////////////////////////////

TimedAction idle = TimedAction(100,idler);

//////////////ticket dispensor variables///////////////////
int dispense = 3;
int notchRead = 111;
int ticketsDispensed = 0;

/////////////////////////////////////////////////////////// 

//////////////////Score Vars////////////////////////////
int scoreClicks = 0;
int hundredClicks = 0;
int ballClicks = 0;
int coinClicks = 0;
int upClicks = 0;
int downClicks = 0;
int selectClicks = 0;
int setupClicks = 0;
int setState = 0;
int lastSetState = 0;

Bounce scoreDebounce = Bounce(scoreSwitch,50);
Bounce coinDebounce = Bounce(coinSwitch,50);
Bounce hundredDebounce = Bounce(hundredSwitch,50);
Bounce ballCountDebounce = Bounce(ballCountSwitch,50);
Bounce ticketDebounce = Bounce(ticketSensor,5);
Bounce selectDebounce = Bounce(selectButton,20);
Bounce setupDebounce = Bounce(setupButton,20);
Bounce upDebounce = Bounce(upButton,20);
Bounce downDebounce = Bounce(downButton,20);

///////////////////////////////////

void setup(){
  Serial.begin(57600);
  Serial.println("Hi!");
  idle.enable();
 
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
  poll_inputs();
  getGameStatus();
  
  idle.check();

 if (dispense>0){
   dispense_tickets();
  }

 if(gameState ==false){
      idle.check();
     }
 if(gameState==true){
        shifter.display(score,ballCount);
     }

 }
  

