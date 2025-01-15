#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/sync.h"
#include "hardware/structs/ioqspi.h"
#include "hardware/structs/sio.h"

// Global variables and functions -------------------------------------------------------------------

enum ButtonState_t {NOT_CHANGED, NOT_PRESSED, SHORT_PRESSED, LONG_PRESSED} buttonState = NOT_PRESSED;
ButtonState_t updateButtonState();

// -------------------------------------------------------------------

const unsigned long LONG_PRESS_THRESHOLD = 2000 * CLK_MLTP;
const unsigned long BUTTON_CHECK_INTERVAL = 100 * CLK_MLTP;

// Function to detect button press on flash CS pin
bool __no_inline_not_in_flash_func(isBootselButtonPressed)() {
    const uint CS_PIN_INDEX = 1;

    uint32_t flags = save_and_disable_interrupts();

    hw_write_masked(&ioqspi_hw->io[CS_PIN_INDEX].ctrl,
                    GPIO_OVERRIDE_LOW << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
                    IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);

    for (volatile int i = 0; i < 1000; ++i);

#if PICO_RP2040
    #define CS_BIT (1u << 1)
#else
    #define CS_BIT SIO_GPIO_HI_IN_QSPI_CSN_BITS
#endif

    bool isPressed = !(sio_hw->gpio_hi_in & CS_BIT);

    hw_write_masked(&ioqspi_hw->io[CS_PIN_INDEX].ctrl,
                    GPIO_OVERRIDE_NORMAL << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
                    IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);

    restore_interrupts(flags);

    return isPressed;
}

ButtonState_t updateButtonState() {
    static unsigned long lastCheckTime = 0;
    static unsigned long pressStartTime = 0;
    static bool previousButtonState = false;

    unsigned long currentTime = millis();

    if (currentTime - lastCheckTime >= BUTTON_CHECK_INTERVAL) {
        lastCheckTime = currentTime;
        bool currentButtonState = isBootselButtonPressed();

        if (!previousButtonState && currentButtonState) {
          buttonState = SHORT_PRESSED;
          pressStartTime = currentTime;
        } else if (currentButtonState && buttonState == SHORT_PRESSED && (currentTime - pressStartTime > LONG_PRESS_THRESHOLD)) {
          buttonState = LONG_PRESSED;
        } else if (previousButtonState && !currentButtonState) {
          buttonState = NOT_PRESSED;
        }
        previousButtonState = currentButtonState;
        return buttonState;
    }
    return NOT_CHANGED;
}
