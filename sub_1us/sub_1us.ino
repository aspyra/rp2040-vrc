#include "Adafruit_TinyUSB.h"
#include <hardware/pwm.h>
#include <hardware/irq.h>

uint slice_num = 0;

// Function to handle the interrupt
void onPWM0Overflow() {
    // Clear the interrupt
    pwm_clear_irq(slice_num);
    static uint8_t counter = 0;
    // Your custom code
    if(++counter == 10){
      counter = 0;
      Serial.println(time_us_32());
    }
}

void setup() {
  slice_num = pwm_gpio_to_slice_num(0);
  pwm_set_wrap(slice_num, 9999);       // Set the period (max value before overflow)
  pwm_set_clkdiv(slice_num, 12.0f);     // Set clock divider to 1

  pwm_set_irq_enabled(slice_num, true);           // Enable IRQ for this slice
  irq_set_exclusive_handler(PWM_IRQ_WRAP, onPWM0Overflow); // Set handler
  irq_set_enabled(PWM_IRQ_WRAP, true);            // Enable PWM interrupt globally

  pwm_set_enabled(slice_num, true);
}

void loop() {
  // put your main code here, to run repeatedly:

}
