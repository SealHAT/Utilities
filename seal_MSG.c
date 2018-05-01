/*
 * seal_MSG.c
 *
 * Created: 30-Apr-18 22:53:41
 *  Author: Ethan
 */

#include "seal_MSG.h"

TaskHandle_t      xMSG_th;      // Message accumulator for USB/MEM
SemaphoreHandle_t disp_mutex;   // mutex to control access to USB terminal
QueueHandle_t     xMSG_q;       // a message Queue for collecting all logged data

int32_t byteQ_write(uint8_t* buff, const uint32_t LEN)
{
    uint32_t i;

    if(xSemaphoreTake(disp_mutex, ~0)) {

        // bail early if there isn't enough space
        if(uxQueueSpacesAvailable(xMSG_q) < LEN) {
            return ERR_NO_RESOURCE;
        }

        for(i = 0; i < LEN; i++) {
            if(!xQueueSendToBack(xMSG_q, &buff[i], pdMS_TO_TICKS(50))){
                break;
            }
        }

        xSemaphoreGive(disp_mutex);
    }
    else {
        return ERR_FAILURE;
    }

    return i;
}

void MSG_task(void* pvParameters)
{
    static const uint8_t BUFF_SIZE = 64;
    uint8_t endptBuf[BUFF_SIZE];       // hold the received messages
    uint_fast8_t i;
    (void)pvParameters;

    for(;;) {

        for(i = 0; i < BUFF_SIZE; i++) {
            xQueueReceive(xMSG_q, &endptBuf[i], ~0);
        }

        usb_write(endptBuf, BUFF_SIZE);
    }
}