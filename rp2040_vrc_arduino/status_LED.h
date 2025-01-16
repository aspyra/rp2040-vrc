#include <FastLED.h>

// Global variables and functions -------------------------------------------------------------------

void LED_set_constant(CRGB color);
void LED_set_blink();
void LED_set_blink(CRGB color);
CRGB blinkColor = CRGB::Green;

// -------------------------------------------------------------------

bool blinkMode = false;
bool last_blinkMode = false;
const uint8_t RGB_pin = 16;
const unsigned long blinkInterval = 250*CLK_MLTP; // 250 ms on/off
const unsigned long gapInterval = 1000*CLK_MLTP; // 1 second gap
const uint8_t RGB_brightness = 50;
CRGB RGB_LED;

void LED_set(CRGB color){
  RGB_LED = color;
  FastLED.show();
}

void LED_set_constant(CRGB color){
  blinkMode = false;
  last_blinkMode = false;
  LED_set(color);
}

void LED_set_blink(){
  blinkMode = true;
  LED_set(CRGB::Black);
}

void setup_LED(){
  FastLED.setBrightness(RGB_brightness);
  FastLED.addLeds<WS2812, RGB_pin, GRB>(&RGB_LED, 1);  // GRB ordering is typical
}

void loop_LED(uint8_t target_blink_count, CRGB *colors, uint8_t color_count) {  
  static unsigned long previousMillis = 0;  
  static uint8_t blinkCount = 0;  
  static bool ledState = false;  
  static unsigned long gapStartMillis = 0;  
  unsigned long currentMillis = millis();  
  
  // Check if blinkMode just got activated
  if (blinkMode) {
    if (!last_blinkMode) {  
      previousMillis = currentMillis; // Reset timing  
      blinkCount = 0; // Reset blink counter  
      ledState = false; // Ensure LED starts off  
      gapStartMillis = 0; // Reset gap timer
      last_blinkMode = true;
    }  
    if (blinkCount < target_blink_count) {  
      if (currentMillis - previousMillis >= blinkInterval) {  
        previousMillis = currentMillis; // Update the last time LED was toggled  
        ledState = !ledState; // Toggle LED state  
  
        if (ledState) {  
          uint8_t colorIndex = blinkCount % color_count; // Wrap color index  
          LED_set(colors[colorIndex]);  
        } else {  
          LED_set(CRGB::Black);  
          blinkCount++;  
        }  
      }  
    } else {  
      // Start the 1-second gap if blinking is complete  
      if (gapStartMillis == 0) {  
        gapStartMillis = currentMillis;  
        LED_set(CRGB::Black); // Turn off LED  
      }  
  
      if (currentMillis - gapStartMillis >= gapInterval) {  
        blinkMode = false; // Optionally, stop blinking after one sequence  
        gapStartMillis = 0; // Reset gap timer for the next round  
      }
    }
  }
}