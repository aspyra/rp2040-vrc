//MIT license, check LICENSE for more information

#include "USB_and_HID_report.h"
#include "RC_pwm.h"
#include "status_LED.h"
#include "boot_button.h"

#include "game_specific_settings.h" //requires "RC_pwm.h"

enum pwm_state_t {low = 0, mid = 1, high = 2};
enum adapter_mode_t {running, binding} mode = running;
bool signal_ok = true;

void setup() {
  setup_LED();
  LED_set_constant(CRGB::Red); //mark beginning of setup

  Serial.begin(115200);

  setup_gp();

  setup_PWM();

  LED_set_blink(); //mark end of setup
}

void loop() {
  static unsigned long last_gp_time = 0;
  loop_gp();
  //check if signal from any channel was received
  if(update_PWM_values()){
    if(mode == binding){
      update_bindData();
    }
    update_gp_by_game(change_flags, gp, pwm_in, false);
    //Serial.printf("CH1:%i,CH2:%i,CH3:%i,CH4:%i,axis1:%i,axis2:%i,axis3:%i\n", pwm_in[0], pwm_in[1], pwm_in[2], pwm_in[3], gp.axis[0], gp.axis[1], gp.axis[2]);
    send_gp();
    last_gp_time = millis();
    signal_ok = true;
  }
  else{
    //if no input signal, do all the stuff that isn't critical
    process_button_and_LED();
    if(millis() - last_gp_time > 3000*CLK_MLTP){
      update_gp_by_game(change_flags, gp, pwm_in, true);
      send_gp();
      last_gp_time = millis();
      signal_ok = false;
      Serial.println("No signal!");
    }
  }
}

void process_button_and_LED(){
  static bool long_press = false;
  switch(updateButtonState()){
    case LONG_PRESSED:
      long_press = true;
      if(mode == running){
        LED_set_constant(CRGB::Blue);
      }else if(mode == binding){
        LED_set_constant(CRGB::Red);
      }
      break;
    case SHORT_PRESSED:
      long_press = false;
      if(mode == running){
        LED_set_constant(CRGB::Yellow);
      }else if(mode == binding){
        LED_set_constant(CRGB::Yellow);
      }
      break;
    case RELEASED:
      if(mode == running){
        if(long_press){
          mode = binding;
          LED_set_blink();
        }
        else{ //short press
          game = static_cast<game_t>(game + 1);
          if (game >= wrap) {
            game = static_cast<game_t>(1);
          }
          LED_set_blink();
        }
      }else if(mode == binding){
        mode = running;
        if(!long_press){
          save_new_bindData();
        }else{
          reset_new_bindData();
        }
        LED_set_blink();
      }
      break;
    case NOT_CHANGED:
      break;
  }

  if(mode == binding){
      debug_bindData();
      CRGB colors[4] = {CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue};
      for(uint8_t ch = 0; ch < 4; ++ch){
        if(pwm_in[ch] <= bindData.pwm_in_min[ch]){
          colors[ch] += CRGB::Green;
        }
        if(pwm_in[ch] >= bindData.pwm_in_max[ch]){
          colors[ch] += CRGB::Red;
        }
        if((pwm_in[ch] >= (bindData.pwm_in_mid[ch] - bind_offset)) && (pwm_in[ch] <= (bindData.pwm_in_mid[ch] + bind_offset))){
          colors[ch] = CRGB::White;
        }
      }
      loop_LED(4, colors, 4);
    }
    else{
      CRGB colors[1] = {CRGB::Green};
      if(!signal_ok)
      colors[0] = CRGB::Red;
      loop_LED(game, colors, 1);
    }
}

