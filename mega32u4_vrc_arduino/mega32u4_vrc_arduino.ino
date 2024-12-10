//MIT license, check LICENSE for more information

#include "RC_pwm.h"
#include <Joystick.h>
#include <Arduino.h>

// Joystick object (make sure you have the Joystick library installed)
Joystick_ joystick;

enum pwm_state {low = 0, mid = 1, high = 2};

void setup() {
  Serial.begin(9600);
  joystick.begin();
  setup_PWM();
}

void loop() {

  unsigned long width[4];
  static unsigned long lastPrintTime = millis(); // To track the last time we printed
  static uint8_t test_flags = 0;

  //wait until at least one of the channels measures PWM width
  while (changeflag == 0) {
    //if no PWM for 1s -> print "No signal"
    if (millis() - lastPrintTime >= 2000) {
      Serial.println("No signal");
      lastPrintTime = millis();
    }
  }
  
  //get the values; no interrupt
  noInterrupts();
  for(uint8_t pin = 0; pin < 4; ++pin){
    width[pin] = pulseWidth[pin];
  }
  test_flags |= changeflag;
  changeflag = 0;
  interrupts();

  //perform max min on the measured signals
  for(uint8_t pin = 0; pin < 4; ++pin){
    width[pin] = max(1000, min(2000, width[pin]));
  }
  
  //go over the inputs and calculate outputs - Circuit Superstars specific
  joystick.setXAxis(map(width[1],1000,2000,0,1023));
  joystick.setYAxis(1024/2);
  
  if(width[0] > 1500){
    joystick.setZAxis(map(width[0],1500,2000,1024/2,1023));
    joystick.setRxAxis(1024/2);
  }
  else{
    joystick.setRxAxis(map(width[0],1500,1000,1024/2,1023));
    joystick.setZAxis(1024/2);    
  }

  static pwm_state ch4_state = mid;
  static unsigned long ch4_timer = 0;

  if(millis() - ch4_timer > 50){
    joystick.releaseButton(1);
    joystick.releaseButton(9);
    switch(ch4_state){
      case low:
        if(width[3] >= 1250 && width[3] < 1750){
          //+ -> menu
          joystick.pressButton(9);
          ch4_state = mid;
          ch4_timer = millis();
        }
        else if(width[3] >= 1750){
          //- -> back
          joystick.pressButton(1);
          ch4_state = high;
          ch4_timer = millis();
        }
        break;

      case mid:
        if(width[3] >= 1750){
          //+ -> menu
          joystick.pressButton(9);
          ch4_state = high;
          ch4_timer = millis();
        }
        else if(width[3] < 1250){
          //- -> back
          joystick.pressButton(1);
          ch4_state = low;
          ch4_timer = millis();
        }
        break;

      case high:
        if(width[3] < 1250){
          //+ -> menu
          joystick.pressButton(9);
          ch4_state = low;
          ch4_timer = millis();
        }
        else if(width[3] >= 1250 && width[3] < 1750){
          //- -> back
          joystick.pressButton(1);
          ch4_state = mid;
          ch4_timer = millis();
        }
        break;
    }
  }

  if(width[2] > 1500){ //CH3 controls button 4 (reset position + optionally opponent names)
    joystick.pressButton(4);
  }
  else{
    joystick.releaseButton(4);
  }

  joystick.sendState();

  //debug printing
  if (millis() - lastPrintTime >= 500) {
    Serial.print("CH1 ");
    Serial.println(width[0]);
    Serial.print("CH2 ");
    Serial.println(width[1]);
    Serial.print("CH3 ");
    Serial.println(width[2]);
    Serial.print("CH4 ");
    Serial.println(width[3]);
    Serial.print("flags ");
    Serial.println(test_flags, HEX);    
    test_flags = 0;
    lastPrintTime = millis(); // Update the last print time
  }
}