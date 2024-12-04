#include "class\hid\hid_device.h"

#ifdef __cplusplus
 extern "C" {
#endif

typedef struct TU_ATTR_PACKED
{
  uint16_t  x;         ///< Delta x  movement of left analog-stick
  uint16_t  y;         ///< Delta y  movement of left analog-stick
  uint16_t  z;         ///< Delta z  movement of right analog-joystick
  uint16_t  rx;        ///< Delta Rz movement of right analog-joystick
  uint16_t  ry;        ///< Delta Rx movement of analog left trigger
  uint16_t  rz;        ///< Delta Ry movement of analog right trigger
  //uint8_t hat;       ///< Buttons mask for currently pressed buttons in the DPad/hat
  uint16_t buttons;  ///< Buttons mask for currently pressed buttons
}hid_custom_gamepad_report_t;

#define TUD_HID_REPORT_DESC_CUSTOM_GAMEPAD2(...)\
  HID_USAGE_PAGE(HID_USAGE_PAGE_DESKTOP),\
  HID_USAGE(HID_USAGE_DESKTOP_GAMEPAD),\
  /* X, Y */\
  HID_COLLECTION(HID_COLLECTION_APPLICATION),\
    HID_COLLECTION(HID_COLLECTION_PHYSICAL),\
      HID_USAGE(HID_USAGE_DESKTOP_X),\
      HID_USAGE(HID_USAGE_DESKTOP_Y),\
      HID_LOGICAL_MIN(0x00),\
      HID_LOGICAL_MAX_N(0xffff, 2),\
      HID_REPORT_COUNT(2),\
      HID_REPORT_SIZE(16),\
      HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE),\
    HID_COLLECTION_END,\
    /* RX, RY */\
    HID_COLLECTION(HID_COLLECTION_PHYSICAL),\
      HID_USAGE(HID_USAGE_DESKTOP_RX),\
      HID_USAGE(HID_USAGE_DESKTOP_RY),\
      HID_LOGICAL_MIN(0x00),\
      HID_LOGICAL_MAX_N(0xffff, 2),\
      HID_REPORT_COUNT(2),\
      HID_REPORT_SIZE(16),\
      HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE),\
    HID_COLLECTION_END,\
    /* Z */\
    HID_USAGE_PAGE(HID_USAGE_PAGE_DESKTOP),\
    HID_USAGE(HID_USAGE_DESKTOP_Z),\
    HID_LOGICAL_MIN(0x00),\
    HID_LOGICAL_MAX_N(0xff03, 2),\
    HID_REPORT_COUNT(1),\
    HID_REPORT_SIZE(10),\
    HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE),\
    /* padding */\
    HID_LOGICAL_MIN(0x00),\
    HID_LOGICAL_MAX(0x00),\
    HID_REPORT_SIZE(6),\
    HID_REPORT_COUNT(1),\
    HID_INPUT(HID_CONSTANT | HID_VARIABLE | HID_ABSOLUTE),\
    /* RZ */\
    HID_USAGE_PAGE(HID_USAGE_PAGE_DESKTOP),\
    HID_USAGE(HID_USAGE_DESKTOP_RZ),\
    HID_LOGICAL_MIN(0x00),\
    HID_LOGICAL_MAX_N(0xff03, 2),\
    HID_REPORT_COUNT(1),\
    HID_REPORT_SIZE(10),\
    HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE),\
    /* padding */\
    HID_LOGICAL_MIN(0x00),\
    HID_LOGICAL_MAX(0x00),\
    HID_REPORT_SIZE(6),\
    HID_REPORT_COUNT(1),\
    HID_INPUT(HID_CONSTANT | HID_VARIABLE | HID_ABSOLUTE),\
    /* Buttons */\
    HID_USAGE_PAGE(HID_USAGE_PAGE_BUTTON),\
    HID_USAGE_MIN(1),\
    HID_USAGE_MAX(10),\
    HID_REPORT_COUNT(10),\
    HID_REPORT_SIZE(10),\
    HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE),\
    /* padding */\
    HID_LOGICAL_MIN(0x00),\
    HID_LOGICAL_MAX(0x00),\
    HID_REPORT_SIZE(6),\
    HID_REPORT_COUNT(1),\
    HID_INPUT(HID_CONSTANT | HID_VARIABLE | HID_ABSOLUTE),\
    /* hat switch */\
    HID_USAGE_PAGE(HID_USAGE_PAGE_DESKTOP),\
    HID_USAGE(HID_USAGE_DESKTOP_HAT_SWITCH),\
    HID_LOGICAL_MIN(0x01),\
    HID_LOGICAL_MAX(0x08),\
    HID_PHYSICAL_MIN(0x00),\
    HID_PHYSICAL_MAX_N(0x013B, 2),\
    0x66, 0x14, 0x00,\
    HID_REPORT_SIZE(4),\
    HID_REPORT_COUNT(1),\
    HID_INPUT(HID_DATA | HID_VARIABLE | HID_NULL_STATE),\
    HID_REPORT_SIZE(4),\
    HID_REPORT_COUNT(1),\
    HID_LOGICAL_MIN(0x00),\
    HID_LOGICAL_MAX(0x00),\
    HID_PHYSICAL_MIN(0x00),\
    HID_PHYSICAL_MAX(0x00),\
    HID_REPORT_SIZE(6),\
    HID_REPORT_COUNT(1),\
    0x65, 0x00,\
    HID_INPUT(HID_CONSTANT | HID_VARIABLE | HID_ABSOLUTE),\
  HID_COLLECTION_END \

#define TUD_HID_REPORT_DESC_CUSTOM_GAMEPAD(...) \
  HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP     )                 ,\
  HID_USAGE      ( HID_USAGE_DESKTOP_GAMEPAD  )                 ,\
  HID_COLLECTION ( HID_COLLECTION_APPLICATION )                 ,\
    HID_USAGE_PAGE     ( HID_USAGE_PAGE_DESKTOP                 ),\
    HID_USAGE          ( HID_USAGE_DESKTOP_X                    ) ,\
    HID_USAGE          ( HID_USAGE_DESKTOP_Y                    ) ,\
    HID_USAGE          ( HID_USAGE_DESKTOP_Z                    ) ,\
    HID_USAGE          ( HID_USAGE_DESKTOP_RX                   ) ,\
    HID_USAGE          ( HID_USAGE_DESKTOP_RY                   ) ,\
    HID_USAGE          ( HID_USAGE_DESKTOP_RZ                   ) ,\
    HID_LOGICAL_MIN    ( 0x00                                   ) ,\
    HID_LOGICAL_MAX_N  ( 0xffff, 2                              ) ,\
    HID_REPORT_COUNT   ( 6                                     ) ,\
    HID_REPORT_SIZE    ( 16                                      ) ,\
    HID_INPUT          ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ) ,\
    /* 8 bit DPad/Hat Button Map  */ \
    /*HID_USAGE_PAGE     ( HID_USAGE_PAGE_DESKTOP                 ) ,*/\
    /*HID_USAGE          ( HID_USAGE_DESKTOP_HAT_SWITCH           ) ,*/\
    /*HID_LOGICAL_MIN    ( 1                                      ) ,*/\
    /*HID_LOGICAL_MAX    ( 8                                      ) ,*/\
    /*HID_PHYSICAL_MIN   ( 0                                      ) ,*/\
    /*HID_PHYSICAL_MAX_N ( 315, 2                                 ) ,*/\
    /*HID_REPORT_COUNT   ( 1                                      ) ,*/\
    /*HID_REPORT_SIZE    ( 8                                      ) ,*/\
    /*HID_INPUT          ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ) ,*/\
    /* 32 bit Button Map */ \
    HID_USAGE_PAGE     ( HID_USAGE_PAGE_BUTTON                  ) ,\
    HID_USAGE_MIN      ( 1                                      ) ,\
    HID_USAGE_MAX      ( 16                                     ) ,\
    HID_LOGICAL_MIN    ( 0                                      ) ,\
    HID_LOGICAL_MAX    ( 1                                      ) ,\
    HID_REPORT_COUNT   ( 16                                     ) ,\
    HID_REPORT_SIZE    ( 1                                      ) ,\
    HID_INPUT          ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ) ,\
  HID_COLLECTION_END \

#ifdef __cplusplus
 }
#endif