//MIT license, check LICENSE for more information

#include "Adafruit_TinyUSB.h"
#include "hid_report_desc.h"
uint8_t const desc_hid_report[] = {
    TUD_HID_REPORT_DESC_CUSTOM_GAMEPAD()
};

// USB HID object
Adafruit_USBD_HID usb_hid;

// Report payload defined in src/class/hid/hid.h
// - For Gamepad Button Bit Mask see  hid_gamepad_button_bm_t
// - For Gamepad Hat    Bit Mask see  hid_gamepad_hat_t
hid_custom_gamepad_report_t gp;

void setup() {
  // Manual begin() is required on core without built-in support e.g. mbed rp2040
  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }

  Serial.begin(115200);

  // Setup HID
  usb_hid.setPollInterval(1);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.begin();

  // If already enumerated, additional class driverr begin() e.g msc, hid, midi won't take effect until re-enumeration
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

  Serial.println("Adafruit TinyUSB HID Gamepad example");

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

//  // Remote wakeup
//  if ( TinyUSBDevice.suspended() && btn )
//  {
//    // Wake up host if we are in suspend mode
//    // and REMOTE_WAKEUP feature is enabled by host
//    TinyUSBDevice.remoteWakeup();
//  }

  if (!usb_hid.ready()) return;

  // Random touch
  Serial.println("Random touch");
  gp.x = 0xFFFF/2;
  gp.y = 0xFFFF/2;
  gp.z = 0xFFFF/2;
  gp.rx = 0xFFFF/2;
  gp.ry = 0xFFFF/2;
  gp.rz = 0xFFFF/2;
  gp.buttons = 0;//uint16_t((bool(random(0,2))<<9)|(bool(random(0,2))<<3)|(bool(random(0,2))<<5));
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(1000);
  gp.z = 0xFFFF;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(1000);
  gp.z = 0xFFFF/2;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(1000);
  gp.rz = 0xFFFF;
    usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(1000);
  gp.buttons = 1<<9;
    usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(1000);
  
  // */
}