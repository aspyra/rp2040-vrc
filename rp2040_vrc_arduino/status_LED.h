#include <FastLED.h>

const uint8_t RGB_pin = 16;
const uint8_t LED_pin = 25;

// Global variables

const unsigned long blinkInterval = 250*CLK_MLTP; // 250 ms on/off
const unsigned long gapInterval = 1000*CLK_MLTP; // 1 second gap
const uint8_t RGB_brightness = 50;
int blinkCount = 0; // Counts completed on/off cycles
bool ledState = false; // Tracks the current state of the LED

CRGB RGB_LED;

void LED_set(CRGB color){
  RGB_LED = color;
  FastLED.show();
}

void setup_LED(){
  FastLED.setBrightness(RGB_brightness);
  FastLED.addLeds<WS2812, RGB_pin, GRB>(&RGB_LED, 1);  // GRB ordering is typical
  LED_set(CRGB::Black);
}

void LED_update(bool reset_blink = false){
  static unsigned long previousMillis = 0;  // Tracks time for LED blinking
  static int blinkCount = 0; // Counts completed on/off cycles
  static bool ledState = false; // Tracks the current state of the LED
  static unsigned long gapStartMillis = 0; // Tracks time during the gap

  if (!reset_blink && (blinkCount < game)) {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= blinkInterval) {
      previousMillis = currentMillis; // Update the last time LED was toggled
      ledState = !ledState; // Toggle LED state
      if(ledState)  LED_set(CRGB::Green);
      else          LED_set(CRGB::Black);

      if (!ledState) { // Count only complete on/off cycles
          blinkCount++;
      }
    }
  } else {
    if(reset_blink){
      blinkCount = game; // Reset for the next cycle
      gapStartMillis = 0; // Reset gap timer      
    }
    // Start the 1-second gap if blinking is complete
    unsigned long currentMillis = millis();

    if (gapStartMillis == 0) {
        gapStartMillis = currentMillis;
        LED_set(CRGB::Black);
    }

    if (currentMillis - gapStartMillis >= gapInterval) {
        blinkCount = 0; // Reset for the next cycle
        gapStartMillis = 0; // Reset gap timer
    }
  }
}