//MIT license, check LICENSE for more information

#include "USB_and_HID_report.h"
#include "game_specific_settings.h"

#include "RC_pwm.h"
#include "status_LED.h"
#include "boot_button.h"

enum pwm_state_t {low = 0, mid = 1, high = 2};
enum adapter_mode_t {running, binding} mode = running;

void setup() {
  setup_LED();
  LED_set_constant(CRGB::Red); //mark beginning of setup

  Serial.begin(115200);

  setup_gp();

  setup_PWM();

  LED_set_blink(); //mark end of setup
}

void loop() {
  loop_gp();
  //check if signal from any channel was received
  if(update_PWM_values()){
    if(mode == binding){
      update_bindData();
    }
    update_gp_by_game(change_flags, gp, pwm_in);
    send_gp();
  }
  else{
    //if no input signal, do all the stuff that isn't critical

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
      case NOT_PRESSED:
        if(mode == running){
          if(long_press){
            mode = binding;
            LED_set_blink();
          }
          else{ //short press
            game = static_cast<game_t>(game + 1);
            if (game >= wrap) {
              game = static_cast<game_t>(0);
            }
          }
        }else if(mode == binding){
          mode = running;
          if(!long_press){
            save_new_bindData();
          }else{
            discard_new_bindData();
          }
          LED_set_blink();
        }
      case NOT_CHANGED:
        break;
    }
    loop_LED(false);
    //Serial.printf("CH1 %i CH2 %i CH3 %i CH4 %i\n", pwm_in[0], pwm_in[1], pwm_in[2], pwm_in[3]);
  }
}

