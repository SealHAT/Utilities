/*
 * seal_ENV.c
 *
 * Created: 30-Apr-18 22:53:14
 *  Author: Ethan
 */

#include "seal_ENV.h"

TaskHandle_t      xENV_th;      // environmental sensors task (light and temp)

void ENV_task(void* pvParameters)
{
    //    UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
    int32_t     err = 0;        // for catching API errors
    ENV_MSG_t   msg;            // reads the data
    (void)pvParameters;

    // initialize the temperature sensor
    si705x_init(&I2C_ENV);

    // initialize the light sensor
    max44009_init(&I2C_ENV, LIGHT_ADD_GND);

    //    uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );

    // set the header data
    msg.header.srtSym    = 0xDEAD;
    msg.header.size      = 4;   // four bytes of data in an env packet
    msg.header.timestamp = 0;   // use timestamp as an index for now.

    for(;;) {
        // reset the message header
        msg.header.id           = DEV_ENV;
        msg.header.timestamp++;

        // start an asynchronous temperature reading
        portENTER_CRITICAL();
        err = si705x_measure_asyncStart();
        portEXIT_CRITICAL();

        //  read the light level
        portENTER_CRITICAL();
        msg.light = max44009_read_uint16();
        portEXIT_CRITICAL();

        //        uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );

        // wait for the temperature sensor to finish
        os_sleep(pdMS_TO_TICKS(5));

        // get temp
        portENTER_CRITICAL();
        msg.temp  = si705x_measure_asyncGet(&err);
        portEXIT_CRITICAL();
        if(err < 0) {
            msg.header.id |= ERROR_TEMP;
        }

        byteQ_write((uint8_t*)&msg, sizeof(ENV_MSG_t));

        // sleep till the next sample
        os_sleep(pdMS_TO_TICKS(975));
    }
}