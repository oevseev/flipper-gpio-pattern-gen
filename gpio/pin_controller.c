#include "pin_controller.h"

#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>

const GpioPin* pins[] = {
    &gpio_ext_pa7,
    &gpio_ext_pa6,
    &gpio_ext_pa4,
    &gpio_ext_pb3,
    &gpio_ext_pb2,
    &gpio_ext_pc3,
    &gpio_ext_pc1,
    &gpio_ext_pc0};

void PinController_init() {
    for(size_t i = 0; i < sizeof(pins) / sizeof(pins[0]); i++) {
        const GpioPin* pin = pins[i];
        furi_hal_gpio_write(pin, false);
        furi_hal_gpio_init(pin, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    }
}

bool PinController_setPins(char value) {
    for(size_t i = 0; i < CHAR_BIT; i++) {
        bool pinValue = ((value >> i) & 1) != 0;
        furi_hal_gpio_write(pins[i], pinValue);
    }

    return true;
}
