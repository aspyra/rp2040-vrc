#include "boot_button.h"
// Define the pins  
const int pins[] = {0, 1, 2, 3};  
   
const float minPulseWidth = 0.5; // Minimum pulse width in milliseconds  
const float maxPulseWidth = 2.5; // Maximum pulse width in milliseconds
const float midPulseWidth = 1.5; // Center pulse width in milliseconds  
const float sweepPeriod = 5500; // Total sweep period in milliseconds
const int numSteps = 100; // Number of steps for the sweep  
const int pwmFrequency = 384; // Approximate desired frequency for RP2040  

// Calculate the time per step in milliseconds  
const float timePerStep = (sweepPeriod) / (numSteps * 2);  
const float pwmPeriod = 1000.0 / pwmFrequency; // Period in milliseconds  
  
// Calculate offsets in milliseconds  


unsigned long previousMillis = 0;  
int currentStep = 0;  

void setPWMPulseWidth(int pin, float pulseWidth) {  
  // Calculate the duty cycle percentage  
  uint32_t maxDutyCycle = 0xFFFF; // 32-bit max value  
  float dutyCycle = (pulseWidth / pwmPeriod) * maxDutyCycle;  
  dutyCycle = constrain(dutyCycle, 0, maxDutyCycle);  
  
  // Set the PWM duty cycle  
  analogWrite(pin, (uint32_t)dutyCycle);  
}

void setup() {  
  // Set pins as output  
  for (int i = 0; i < 4; i++) {  
    pinMode(pins[i], OUTPUT);  
  }
  analogWriteFreq(pwmFrequency);
  analogWriteRange(0xFFFF);
}  
  
void loop() {  
  unsigned long currentMillis = millis();  
  if(isBootselButtonPressed()){
    // Set PWM for each pin with offset  
    for (int i = 0; i < 4; i++) {  
      setPWMPulseWidth(pins[i], midPulseWidth);  
    }      
  }
  else if (currentMillis - previousMillis >= timePerStep) {  
    // Save the last time you updated the PWM  
    previousMillis = currentMillis;  
  
    // Set PWM for each pin with offset  
    for (int i = 0; i < 4; i++) {
      int ch_step = ( currentStep + (i*numSteps)/4 ) % (numSteps*2);
      if(ch_step > numSteps){
        ch_step = numSteps*2 - ch_step;
      }
      float pulseWidth = minPulseWidth + (maxPulseWidth - minPulseWidth) * (ch_step / (float)numSteps);  
      setPWMPulseWidth(pins[i], pulseWidth);  
    }  
    currentStep++;  
    if (currentStep > numSteps*2) {  
      currentStep = 0;  
    }  
  }  
}  
  