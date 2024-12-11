#include "class\hid\hid_device.h"

typedef struct TU_ATTR_PACKED
{
  uint16_t axis[6];
  uint16_t buttons;          ///< Buttons mask for currently pressed buttons
}hid_custom_gamepad_report_t;

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
    HID_LOGICAL_MAX_N  ( CLK_MLTP*1000, 2                       ) ,\
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
