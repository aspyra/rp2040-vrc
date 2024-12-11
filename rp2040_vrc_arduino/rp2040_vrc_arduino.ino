//MIT license, check LICENSE for more information

#include "Adafruit_TinyUSB.h"
#include "RC_pwm.h"
#include "hid_report_desc.h"

enum pwm_state {low = 0, mid = 1, high = 2};

// USB HID object, descriptor & gamepad report (data sent to PC)
Adafruit_USBD_HID usb_hid;
uint8_t const desc_hid_report[] = {TUD_HID_REPORT_DESC_CUSTOM_GAMEPAD()};
hid_custom_gamepad_report_t gp;

void setup() {
  Serial.begin(115200);
  // Manual begin() is required on core without built-in support e.g. mbed rp2040
  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }

  // Setup HID
  usb_hid.setPollInterval(1);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.begin();

  // If already enumerated, additional class driver begin() e.g msc, hid, midi won't take effect until re-enumeration
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

  setup_PWM();
}

void loop() {

  #ifdef TINYUSB_NEED_POLLING_TASK
  // Manual call tud_task since it isn't called by Core's background
  TinyUSBDevice.task();
  #endif

  // not enumerated()/mounted() yet: nothing to do
  if (!TinyUSBDevice.mounted()) {
    return;
  }

  if (!usb_hid.ready()) return;

  uint32_t width[4];
  static unsigned long lastPrintTime = millis(); // To track the last time we printed
  static uint8_t test_flags = 0;

  //wait until at least one of the channels measures PWM width
  while (changeflag == 0) {
    //if no PWM for 1s -> print "No signal"
    if (millis() - lastPrintTime >= CLK_MLTP*2000) {
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
    width[pin] = max(CLK_MLTP*1000, min(CLK_MLTP*2000, width[pin]))-CLK_MLTP*1000;
  }
  
  //go over the inputs and calculate outputs - Circuit Superstars specific
  gp.axis[0] = width[0];
  gp.axis[1] = CLK_MLTP*500;
  if(width[1] > CLK_MLTP*500){
    gp.axis[2] = width[1];
    gp.axis[3] = CLK_MLTP*500;
  }
  else{
    gp.axis[3] = CLK_MLTP*1000-width[1];
    gp.axis[2] = CLK_MLTP*500;    
  }
  gp.axis[4] = CLK_MLTP*500;
  gp.axis[5] = CLK_MLTP*500;

  static pwm_state ch4_state = mid;
  static uint32_t ch4_timer = 0;

  const uint32_t low_th = CLK_MLTP*250;
  const uint32_t hi_th = CLK_MLTP*750;

  if(micros() - ch4_timer > CLK_MLTP*50000){
    gp.buttons = 0;
    switch(ch4_state){
      case low:
        if(width[3] >= low_th && width[3] < hi_th){
          //+ -> menu
          gp.buttons = (1<<9);
          ch4_state = mid;
          ch4_timer = time_us_32();
        }
        else if(width[3] >= hi_th){
          //- -> back
          gp.buttons = (1<<1);
          ch4_state = high;
          ch4_timer = time_us_32();
        }
        break;

      case mid:
        if(width[3] >= hi_th){
          //+ -> menu
          gp.buttons = (1<<9);
          ch4_state = high;
          ch4_timer = time_us_32();
        }
        else if(width[3] < low_th){
          //- -> back
          gp.buttons = (1<<1);
          ch4_state = low;
          ch4_timer = time_us_32();
        }
        break;

      case high:
        if(width[3] < low_th){
          //+ -> menu
          gp.buttons = (1<<9);
          ch4_state = low;
          ch4_timer = time_us_32();
        }
        else if(width[3] >= low_th && width[3] < hi_th){
          //- -> back
          gp.buttons = (1<<1);
          ch4_state = mid;
          ch4_timer = time_us_32();
        }
        break;
    }
  }

  if(width[2] > 500){ //CH3 controls button 4 (reset position + optionally opponent names)
    gp.buttons |= (1<<4);
  }
  else{
    gp.buttons &= ~(1<<4);
  }

  //uint16_t((bool(random(0,2))<<9)|(bool(random(0,2))<<3)|(bool(random(0,2))<<5));
  usb_hid.sendReport(0, &gp, sizeof(gp));

  //debug printing
  if (millis() - lastPrintTime >= CLK_MLTP*500) {
    Serial.printf("CH1 %i CH2 %i CH3 %i CH4 %i, flags 0x%x\n", width[0], width[1], width[2], width[3], test_flags);
    test_flags = 0;
    lastPrintTime = millis(); // Update the last print time
  }
  //*/
}