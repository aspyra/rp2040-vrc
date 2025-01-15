#include <Arduino.h>
#include "hardware/timer.h"
#include <EEPROM.h>

typedef struct{
  uint16_t pwm_in_max[4];
  uint16_t pwm_in_min[4];
  uint16_t pwm_in_mid[4];
  uint8_t check_byte;
}eeprom_data_t;

// Global variables and functions -------------------------------------------------------------------

#define CLK_MLTP 12
uint8_t change_flags = 0;
uint32_t pwm_in[4];
eeprom_data_t bindData;

// -------------------------------------------------------------------

volatile uint32_t pulseStartTime[4]; // Support multiple pins if needed
volatile uint32_t pulseWidth[4];
volatile uint8_t _change_flags = 0;

void handleRCPWM(int pin) {
  // Read the pin state and decide
  uint32_t current_time = time_us_32();
  if (digitalRead(PWM_pins[pin])) {
    // Rising edge: record the start time
    pulseStartTime[pin] = current_time;
  } else {
    // Falling edge: calculate the pulse width
    pulseWidth[pin] = current_time - pulseStartTime[pin];
    _change_flags |= (1<<pin);
  }
}

void RC_PWM_INT0(){handleRCPWM(0);}
void RC_PWM_INT1(){handleRCPWM(1);}
void RC_PWM_INT2(){handleRCPWM(2);}
void RC_PWM_INT3(){handleRCPWM(3);}

void update_bindData(){
  ;
}

void save_new_bindData(){
  ;
}

void discard_new_bindData(){
  ;
}

void setup_PWM(){
  for(uint8_t pin = 0; pin < 4; ++pin){
    pinMode(PWM_pins[pin], INPUT_PULLDOWN);
  }
  //This magic line increases Watchdog tick 12x effectively speeding up the timer clock 12x giving sub 0.1us accuracy
  *(uint32_t *)(WATCHDOG_BASE + 0x2c) = 0x0000201;

  EEPROM.begin(256);
  EEPROM.get(0, bindData);
  if(bindData.check_byte != 0xAA){
    bindData.check_byte = 0xAA;
    for(uint8_t ch = 0; ch < 4; ++ch){
      bindData.pwm_in_max[ch] = 2000 * CLK_MLTP;
      bindData.pwm_in_min[ch] = 1000 * CLK_MLTP;
      bindData.pwm_in_mid[ch] = 1500 * CLK_MLTP;
    }
    EEPROM.commit();
  }

  attachInterrupt(digitalPinToInterrupt(PWM_pins[0]), RC_PWM_INT0, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PWM_pins[1]), RC_PWM_INT1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PWM_pins[2]), RC_PWM_INT2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PWM_pins[3]), RC_PWM_INT3, CHANGE);
}

bool update_PWM_values(){
  if(_change_flags){
    noInterrupts();
    for(uint8_t pin = 0; pin < 4; ++pin){
      if((_change_flags>>pin) & 0x01){
        pwm_in[pin] = pulseWidth[pin];
        //perform max min on the measured signals
        pwm_in[pin] = max(bindData.pwm_in_min[pin], min(bindData.pwm_in_max[pin], pwm_in[pin]))-bindData.pwm_in_min[pin];
      }
    }
    change_flags = _change_flags;
    _change_flags = 0;
    interrupts();
    return true;
  }
  return false;
}