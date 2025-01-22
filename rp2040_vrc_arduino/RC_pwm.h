#include <Arduino.h>
#include "hardware/timer.h"
#include <EEPROM.h>

//RP2040 pins for RC PWM input (in order)
const uint8_t PWM_pins[4] = {0, 1, 2, 3};

const uint32_t CLK_MLTP = 12; //we trick the timer into running 12x faster, so we need to scale ALL time references

typedef struct{
  uint16_t pwm_in_max[4];
  uint16_t pwm_in_min[4];
  uint16_t pwm_in_mid[4];
  uint8_t check_byte;
}eeprom_data_t;

const uint32_t pwm_in_abs_min = 98 * CLK_MLTP;
const uint32_t pwm_in_abs_max = 2550 * CLK_MLTP;
const uint32_t bind_offset = 5 * CLK_MLTP; //5us of min/max offset that ensures 'saturation' on ends

// Global variables and functions -------------------------------------------------------------------

uint8_t change_flags = 0;
uint32_t pwm_in[4];
uint32_t pwm_in_raw[4];
eeprom_data_t bindData, newBindData;

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
  for(uint8_t ch = 0; ch < 4; ++ch){
    if((pwm_in_raw[ch] >= pwm_in_abs_min) && (pwm_in_raw[ch] <= pwm_in_abs_max)){ //make sure signal makes sense
      if(pwm_in_raw[ch] > newBindData.pwm_in_max[ch]){
        newBindData.pwm_in_max[ch] = pwm_in_raw[ch] - bind_offset;
      }
      if(pwm_in_raw[ch] < newBindData.pwm_in_min[ch]){
        newBindData.pwm_in_min[ch] = pwm_in_raw[ch] + bind_offset;
        printf("Setting min to %i\n", newBindData.pwm_in_min[ch]);
      }
      newBindData.pwm_in_mid[ch] = pwm_in_raw[ch];
    }
  }
}

void debug_bindData(){
  static unsigned long lastDebugTime = 0;
  if((millis() - lastDebugTime) > (2500 * CLK_MLTP)){
    lastDebugTime = millis();
    Serial.printf("Inputs: CH1 %.2fus, CH2 %.2fus, CH3 %.2fus, CH4 %.2fus\n", float(pwm_in_raw[0])/float(CLK_MLTP), float(pwm_in_raw[1])/float(CLK_MLTP), float(pwm_in_raw[2])/float(CLK_MLTP), float(pwm_in_raw[3])/float(CLK_MLTP));
    for(uint8_t ch = 0; ch < 4; ++ch){
      Serial.printf("Previous bind setting for CH%i: min %.2fus, mid %.2fus, max %.2fus  ", ch, float(bindData.pwm_in_min[ch])/float(CLK_MLTP), float(bindData.pwm_in_mid[ch])/float(CLK_MLTP), float(bindData.pwm_in_max[ch])/float(CLK_MLTP));
      Serial.printf("New bind setting for CH%i: min %.2fus, mid %.2fus, max %.2fus\n", ch, float(newBindData.pwm_in_min[ch])/float(CLK_MLTP), float(newBindData.pwm_in_mid[ch])/float(CLK_MLTP), float(newBindData.pwm_in_max[ch])/float(CLK_MLTP));
    }
  }
}

void reset_new_bindData(){
  newBindData.check_byte = 0xAA;
  for(uint8_t ch = 0; ch < 4; ++ch){
      newBindData.pwm_in_max[ch] = pwm_in_abs_min;
      newBindData.pwm_in_min[ch] = pwm_in_abs_max;
      newBindData.pwm_in_mid[ch] = 0;    
  }
}

void set_channel_to_default(eeprom_data_t &data, uint8_t ch){
  data.pwm_in_max[ch] = 2000 * CLK_MLTP;
  data.pwm_in_min[ch] = 1000 * CLK_MLTP;
  data.pwm_in_mid[ch] = 1500 * CLK_MLTP;
}

void save_new_bindData(){
  Serial.println("Saving data to flash:");
  for(uint8_t ch = 0; ch < 4; ++ch){
    if(newBindData.pwm_in_max[ch] < newBindData.pwm_in_mid[ch] || newBindData.pwm_in_mid[ch] < newBindData.pwm_in_min[ch]){
      set_channel_to_default(newBindData, ch);
      Serial.print("Set to default! ");
    }
    Serial.printf("New Ch%i: min %.2fus, mid %.2fus, max %.2fus\n", ch, float(newBindData.pwm_in_min[ch])/float(CLK_MLTP), float(newBindData.pwm_in_mid[ch])/float(CLK_MLTP), float(newBindData.pwm_in_max[ch])/float(CLK_MLTP));
  }
  memcpy(&bindData, &newBindData, sizeof(eeprom_data_t));
  reset_new_bindData();
  EEPROM.put(0, bindData);
  EEPROM.commit();
}

void setup_PWM(){
  for(uint8_t pin = 0; pin < 4; ++pin){
    pinMode(PWM_pins[pin], INPUT_PULLDOWN);
  }
  //This magic line increases Watchdog tick 12x effectively speeding up the timer clock 12x giving sub 0.1us accuracy
  *(uint32_t *)(WATCHDOG_BASE + 0x2c) = 0x0000201;

  EEPROM.begin(256);
  EEPROM.get(0, bindData);
  if(bindData.check_byte != 0xAA){//if check_byte is not correct, restore 'default' values
    bindData.check_byte = 0xAA;
    for(uint8_t ch = 0; ch < 4; ++ch){
      set_channel_to_default(bindData, ch);
    }
    EEPROM.put(0, bindData);
    EEPROM.commit();
  }
  reset_new_bindData();

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
        pwm_in_raw[pin] = pulseWidth[pin];
      }
    }
    change_flags = _change_flags;
    _change_flags = 0;
    interrupts();

    for(uint8_t pin = 0; pin < 4; ++pin){
      if((change_flags>>pin) & 0x01){
        	//perform max min on the measured signals
          pwm_in_raw[pin] = max(pwm_in_abs_min, min(pwm_in_abs_max, pwm_in_raw[pin]));
	        pwm_in[pin] = max(bindData.pwm_in_min[pin], min(bindData.pwm_in_max[pin], pwm_in_raw[pin]));
      }
    }
    return true;
  }
  return false;
}

uint32_t scale_pwm(uint8_t channel, uint32_t out_min, uint32_t out_max){ //extended map() function to include mid-point. Beware of uint32_t overflows
  uint32_t numerator, denominator, output;
  if(pwm_in[channel] >= bindData.pwm_in_mid[channel]){
    numerator = (pwm_in[channel] - bindData.pwm_in_mid[channel])*(out_max - out_min);
    denominator = (bindData.pwm_in_max[channel] - bindData.pwm_in_mid[channel]);
    output = ((numerator / denominator) + (out_max + out_min))/2;
  }
  else{
    numerator = (pwm_in[channel] - bindData.pwm_in_min[channel])*(out_max - out_min);
    denominator = (bindData.pwm_in_mid[channel] - bindData.pwm_in_min[channel]);
    output = (numerator / denominator)/2 + out_min;
  }
  //Serial.printf("PWM%i:%i,N%i:%i,D%i:%i,O%i:%i\n", channel, pwm_in[channel], channel, numerator, channel, denominator, channel, output);
  return output;
}