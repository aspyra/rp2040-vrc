#include <Arduino.h>
#include "hardware/timer.h"

#define CLK_MLTP 12

volatile uint32_t pulseStartTime[4]; // Support multiple pins if needed
volatile uint32_t pulseWidth[4];
volatile uint8_t changeflag = 0;

void handleRCPWM(int pin) {
  // Read the pin state and decide
  uint32_t current_time = time_us_32();
  if (digitalRead(PWM_pins[pin])) {
    // Rising edge: record the start time
    pulseStartTime[pin] = current_time;
  } else {
    // Falling edge: calculate the pulse width
    pulseWidth[pin] = current_time - pulseStartTime[pin];
    changeflag |= (1<<pin);
  }
}

void RC_PWM_INT0(){handleRCPWM(0);}
void RC_PWM_INT1(){handleRCPWM(1);}
void RC_PWM_INT2(){handleRCPWM(2);}
void RC_PWM_INT3(){handleRCPWM(3);}

void setup_PWM(){
  for(uint8_t pin = 0; pin < 4; ++pin){
    pinMode(PWM_pins[pin], INPUT_PULLDOWN);
  }
  //This magic line increases Watchdog tick 12x effectively speeding up the timer clock 12x giving sub 0.1us accuracy
  *(uint32_t *)(WATCHDOG_BASE + 0x2c) = 0x0000201;

  attachInterrupt(digitalPinToInterrupt(PWM_pins[0]), RC_PWM_INT0, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PWM_pins[1]), RC_PWM_INT1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PWM_pins[2]), RC_PWM_INT2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PWM_pins[3]), RC_PWM_INT3, CHANGE);
}
