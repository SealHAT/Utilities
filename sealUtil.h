/*
 * SerialPrint.h
 *
 * Created: 3/7/2018 2:39:48 PM
 *  Author: eslatter
 */


#ifndef SEALUTIL_H_
#define SEALUTIL_H_

#include "atmel_start_pins.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <hal_atomic.h>
#include <hal_delay.h>
#include <hal_gpio.h>
#include <hal_init.h>
#include <hal_io.h>
#include <hal_sleep.h>

#include <hal_i2c_m_sync.h>

#include <hal_i2c_m_sync.h>

#include <hal_i2c_m_sync.h>
#include <hal_spi_m_sync.h>

#include "hal_usb_device.h"

typedef enum {
    I2C_STATE_UNKNOWN = 0x00,
    I2C_STATE_IDLE    = 0x01,
    I2C_STATE_OWNER   = 0x02,
    I2C_STATE_BUSY    = 0x03
} I2C_STATE_t;

/**
 * This function unblocks an I2C bus where the slave is holding
 * data low because out an unexpected restart or bus error where
 * all the clock pulses were not detected. It will first check to
 * see if the line is actually help low and if it is toggle the
 * clock 16 times to complete whatever stalled transaction is present.
 *
 * @param SDA [IN] pin for the I2C devices data line
 * @param SCL [IN] pin for the I2C devices clock line
 */
void I2C_UNBLOCK_BUS(const uint8_t SDA, const uint8_t SCL);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* SEALUTIL_H_ */
