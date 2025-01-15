#include "Adafruit_TinyUSB.h"
#include "class\hid\hid_device.h"
#define MAX_HID_SIGNAL 0xFFFF

typedef struct TU_ATTR_PACKED{
  uint16_t axis[6];
  uint16_t buttons;
}hid_custom_gamepad_report_t;

// Global variables and functions -------------------------------------------------------------------

hid_custom_gamepad_report_t gp;
void loop_gp();
void send_gp();

// -------------------------------------------------------------------

#define TUD_HID_REPORT_DESC_CUSTOM_GAMEPAD(...)                    \
  HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP     )                   ,\
  HID_USAGE      ( HID_USAGE_DESKTOP_GAMEPAD  )                   ,\
  HID_COLLECTION ( HID_COLLECTION_APPLICATION )                   ,\
    HID_USAGE_PAGE     ( HID_USAGE_PAGE_DESKTOP                 ) ,\
    HID_USAGE          ( HID_USAGE_DESKTOP_X                    ) ,\
    HID_USAGE          ( HID_USAGE_DESKTOP_Y                    ) ,\
    HID_USAGE          ( HID_USAGE_DESKTOP_Z                    ) ,\
    HID_USAGE          ( HID_USAGE_DESKTOP_RX                   ) ,\
    HID_USAGE          ( HID_USAGE_DESKTOP_RY                   ) ,\
    HID_USAGE          ( HID_USAGE_DESKTOP_RZ                   ) ,\
    HID_LOGICAL_MIN_N  ( 0x0000, 2                              ) ,\
    HID_LOGICAL_MAX_N  ( MAX_HID_SIGNAL, 2                      ) ,\
    HID_REPORT_COUNT   ( 6                                      ) ,\
    HID_REPORT_SIZE    ( 16                                     ) ,\
    HID_INPUT          ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ) ,\
    /* 8bit Button Map */                                          \
    HID_USAGE_PAGE     ( HID_USAGE_PAGE_BUTTON                  ) ,\
    HID_USAGE_MIN      ( 1                                      ) ,\
    HID_USAGE_MAX      ( 16                                     ) ,\
    HID_LOGICAL_MIN    ( 0                                      ) ,\
    HID_LOGICAL_MAX    ( 1                                      ) ,\
    HID_REPORT_COUNT   ( 16                                     ) ,\
    HID_REPORT_SIZE    ( 1                                      ) ,\
    HID_INPUT          ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ) ,\
  HID_COLLECTION_END                                               \

// USB HID object, descriptor & gamepad report (data sent to PC)
Adafruit_USBD_HID usb_hid;
uint8_t const desc_hid_report[] = {TUD_HID_REPORT_DESC_CUSTOM_GAMEPAD()};

void setup_gp(){ //TinyUSB stuff
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
}

void loop_gp(){ //TinyUSB stuff
  #ifdef TINYUSB_NEED_POLLING_TASK
  // Manual call tud_task since it isn't called by Core's background
  TinyUSBDevice.task();
  #endif
  /*
  // not enumerated()/mounted() yet: nothing to do
  if (!TinyUSBDevice.mounted()) {
    return;
  }

  if (!usb_hid.ready()) return;*/
}

void send_gp(){
  usb_hid.sendReport(0, &gp, sizeof(gp));
}
