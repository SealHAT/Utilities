#include "sealUtil.h"
#include <peripheral_clk_config.h>
#include <utils.h>
#include <hal_init.h>

void I2C_UNBLOCK_BUS(const uint8_t SDA, const uint8_t SCL)
{
    uint32_t i, count;

    // Set pin SDA direction to input, pull off
    gpio_set_pin_direction(SDA, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(SDA, GPIO_PULL_OFF);
    gpio_set_pin_function(SDA, GPIO_PIN_FUNCTION_OFF);

    for(i = 0, count = 0; i < 50; i++) {
        count += gpio_get_pin_level(SDA);
    }

    if(count < 10) {
        // Set pin SCL direction to output, pull off
        gpio_set_pin_direction(SCL, GPIO_DIRECTION_OUT);
        gpio_set_pin_pull_mode(SCL, GPIO_PULL_OFF);
        gpio_set_pin_function(SCL, GPIO_PIN_FUNCTION_OFF);

        for(i = 0; i <= 32; i++) {
            gpio_toggle_pin_level(SCL);
        }
    }

}
