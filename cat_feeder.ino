// include libraries
#include <LiquidCrystal.h>
#include <ThreeWire.h> 
#include <RtcDS1302.h>
#include <Servo.h>


// initialize library for LCD with corresponding pin numbers
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
// create rtc instance with correspoinding pin numbers
ThreeWire myWire(6,8,7);
RtcDS1302 <ThreeWire> Rtc(myWire);
Servo feedServo;


// create global variables
const int pinCLK  = A3; // Clock signal
const int pinDT =  A4; // Data signal
const int pinSW  = A5; // Rotary encoder switch
const int servoPin = 9; // Servo Pin
int clkState;
int clkLastState;
int counterMin = 0;
int counterHour = 0;
int feed1Hour = 7;
int feed1Min = 00;
int feed2Hour = 13;
int feed2Min = 00;
int feed3Hour = 17;
int feed3Min = 30;


void setup() {
  // setup LCD
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("Turning on");
  lcd.setCursor(0,1);
  lcd.print("Automatic Feeder");
  // setup rotary encoder
  pinMode(pinCLK, INPUT);
  pinMode(pinDT, INPUT);
  pinMode(pinSW, INPUT);
  feedServo.attach(servoPin);
  clkLastState = digitalRead(pinCLK);
  // initialize rtc
  Rtc.Begin(); 
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  if (!Rtc.IsDateTimeValid())  // copied from rtc library
    {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

        Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiled);
    }
   RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled) 
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
    }
    else if (now == compiled) 
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }

  delay(2000);
  lcd.clear();
}

void loop() {
  lcd.setCursor(0,0);
  RtcDateTime now = Rtc.GetDateTime();
  
  // display current time in top left of lcd
  
  printDigits(now.Hour());
  lcd.print(":");
  printDigits(now.Minute());
  lcd.print(":");
  printDigits(now.Second());
  
  // displays all 3 feeding times
  
  lcd.print(" 1)");
  printDigits(feed1Hour);
  lcd.print(":");
  printDigits(feed1Min);
  lcd.setCursor(0,1);
  lcd.print("2)");
  printDigits(feed2Hour);
  lcd.print(":");
  printDigits(feed2Min);
  lcd.print("  3)");
  printDigits(feed3Hour);
  lcd.print(":");
  printDigits(feed3Min);
   
 // main if section that monitors push button on rotary encoder

 //change feed time 1 hours
 
 if (digitalRead(pinSW) == LOW) { // when button on rotary encoder is pushed
  lcd.blink(); // sets up blinking cursor
  counterHour = feed1Hour; // needed or feed time always resets to 0
  delay(500); // delay needed so arduino registers each button press only once
  do {
    lcd.setCursor(12,0); // sets cursor on feed hour 1 position
    changeTime("hour"); // created method

    feed1Hour = counterHour; // line of code that actually changes the feed time
    lcd.setCursor(11,0); // needed so that the hour can be printed in the correct spot
    printDigits(feed1Hour);
  } while (digitalRead(pinSW) == HIGH); // peform previous do syntax until button is pushed again
  lcd.noBlink(); // turns off blinking cursor
  delay(1000);
  
  // change feed time 1 minutes
  
  lcd.blink();
  counterMin = feed1Min;
  delay(500);
  do {
    lcd.setCursor(15,0);
    changeTime("minute");

    feed1Min = counterMin;
    lcd.setCursor(14,0);
    printDigits(feed1Min);
  } while(digitalRead(pinSW) == HIGH);
  lcd.noBlink();
  delay(1000);
  
  // change feed time 2 hours
  
  lcd.blink();
  counterHour = feed2Hour;
  delay(500);
  do {
    lcd.setCursor(3,1);
    changeTime("hour");

    feed2Hour = counterHour;
    lcd.setCursor(2,1);
    printDigits(feed2Hour);
  } while(digitalRead(pinSW) == HIGH);
  lcd.noBlink();
  delay(1000);
  
  // change feed time 2 minutes
  
  lcd.blink();
  counterMin = feed2Min;
  delay(500);
  do {
    lcd.setCursor(6,1);
    changeTime("minute");

    feed2Min = counterMin;
    lcd.setCursor(5,1);
    printDigits(feed2Min);
  } while(digitalRead(pinSW) == HIGH);
  lcd.noBlink();
  delay(1000);
  
  // change feed time 3 hours
  
  lcd.blink();
  counterHour = feed3Hour;
  delay(500);
  do {
    lcd.setCursor(12,1);
    changeTime("hour");

    feed3Hour = counterHour;
    lcd.setCursor(11,1);
    printDigits(feed3Hour);
  } while(digitalRead(pinSW) == HIGH);
  lcd.noBlink();
  delay(1000);
  
  // change feed time 3 minutes
  
  lcd.blink();
  counterMin = feed3Min;
  delay(500);
  do {
    lcd.setCursor(15,1);
    changeTime("minute");

    feed3Min = counterMin;
    lcd.setCursor(14,1);
    printDigits(feed3Min);
  } while(digitalRead(pinSW) == HIGH);
  lcd.noBlink();
  delay(1000);
 }

 //feed section
 
 if (feed1Hour == now.Hour() && feed1Min == now.Minute() && now.Second() == 0) {
  feed();
  }

 if (feed2Hour == now.Hour() && feed2Min == now.Minute() && now.Second() == 0) {
  feed();
  }

 if (feed3Hour == now.Hour() && feed3Min == now.Minute() && now.Second() == 0) {
   feed();
  }
} 



//created methods
 

void printDigits(int digits) {
 // function for digital clock display: prints leading 0 on lcd if digit is < 10
 if (digits < 10)
 lcd.print('0');
 lcd.print(digits);
}

void changeTime(String time) {
  // function to keep a placeholder counter so that the rotary encoder can change the time for the feed times
  clkState = digitalRead(pinCLK); // reads the "current" state of the clock pin
  if (clkState != clkLastState) { // if the previous and the current state of the clock pin are different that means a rotation has occurred
    if (digitalRead(pinDT) != clkState && time == "minute") { // if the Data pin state is different to the clock pin state, that means the encoder is rotating clockwise
        if (counterMin == 59) counterMin = -1; // resets minutes to 0 when rotating CW after 59
        counterMin++; // increments counter
        //lcd.clear();
        } else if (digitalRead(pinDT) == clkState && time == "minute") {
        if (counterMin == 0) counterMin = 60; //resets minutes to 59 when rotating CCW from 0
        counterMin --; // decrements counter
        //lcd.clear();
        }
    if (digitalRead(pinDT) != clkState && time == "hour") {
        if (counterHour == 23) counterHour = -1; // resets hours to 0 when rotating CW after 23
        counterHour++;
        //lcd.clear();
        } else if (digitalRead(pinDT) == clkState && time == "hour") {
        if (counterHour == 0) counterHour = 24; // resets hours to 23 when rotating CCW from 0
        counterHour --;
        //lcd.clear();
        }
       }
  clkLastState = clkState; // updates the previous state of the pinCLK with the current state
}


void feed() {
  // function to turn motor on for designated period of time
    feedServo.write(0); //turns auger counter clockwise; put 180 for clockwise
    delay(10000); // keeps auger turning for 10 seconds
    feedServo.write(90); //stops auger
}
