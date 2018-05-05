/*
 * seal_MSG.c
 *
 * Created: 30-Apr-18 22:53:41
 *  Author: Ethan
 */

#include "seal_CTRL.h"

TaskHandle_t       xCTRL_th;     // Message accumulator for USB/MEM
EventGroupHandle_t xCTRL_eg;     // IMU event group
SemaphoreHandle_t  USB_mutex;    // mutex to control access to USB terminal
QueueHandle_t      xDATA_q;      // a message Queue for collecting all logged data

void vbus_detection_cb(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t xResult;

    if( gpio_get_pin_level(VBUS_DETECT) ) {
        usb_start();
        xResult = xEventGroupSetBitsFromISR(xCTRL_eg, EVENT_VBUS, &xHigherPriorityTaskWoken);
    }
    else {
        usb_stop();
        xResult = xEventGroupClearBitsFromISR(xCTRL_eg, EVENT_VBUS);
    }

    if(xResult != pdFAIL) {
        /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context switch should be requested. */
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
}

int32_t byteQ_write(uint8_t* buff, const uint32_t LEN)
{
    uint32_t i;

    if(xSemaphoreTake(USB_mutex, ~0)) {

        // bail early if there isn't enough space
        if(uxQueueSpacesAvailable(xDATA_q) < LEN) {
            return ERR_NO_RESOURCE;
        }

        for(i = 0; i < LEN; i++) {
            if(!xQueueSendToBack(xDATA_q, &buff[i], pdMS_TO_TICKS(50))){
                break;
            }
        }

        xSemaphoreGive(USB_mutex);
    }
    else {
        return ERR_FAILURE;
    }

    return i;
}

int32_t MSG_task_init(uint32_t qLength)
{
    struct calendar_date date;
    struct calendar_time time;

    // create 24-bit system event group
    xCTRL_eg = xEventGroupCreate();
    if(xCTRL_eg == NULL) {
        return ERR_NO_MEMORY;
    }

    calendar_enable(&RTC_CALENDAR);

    date.year  = 2018;
    date.month = 5;
    date.day   = 4;

    time.hour = 15;
    time.min  = 33;
    time.sec  = 0;

    calendar_set_date(&RTC_CALENDAR, &date);
    calendar_set_time(&RTC_CALENDAR, &time);

    // initialize (clear all) event group and check current VBUS level
    xEventGroupClearBits(xCTRL_eg, EVENT_MASK_ALL);
    if(gpio_get_pin_level(VBUS_DETECT)) {
        usb_start();
        xEventGroupSetBits(xCTRL_eg, EVENT_VBUS);
    }

    USB_mutex = xSemaphoreCreateMutex();
    if (USB_mutex == NULL) {
        return ERR_NO_MEMORY;
    }

    xDATA_q = xQueueCreate(qLength, 1);
    if(xDATA_q == NULL) {
        return ERR_NO_MEMORY;
    }

    // TODO - add debug guards
    vQueueAddToRegistry(xDATA_q, "BYTE_Q");

    return ( xTaskCreate(MSG_task, "MSG", MSG_STACK_SIZE, NULL, MSG_TASK_PRI, &xCTRL_th) == pdPASS ? ERR_NONE : ERR_NO_MEMORY);
}

void MSG_task(void* pvParameters)
{
    static const uint8_t BUFF_SIZE = 64;
    uint8_t endptBuf[BUFF_SIZE];       // hold the received messages
    uint_fast8_t i;
    (void)pvParameters;

    // register VBUS detection interrupt
    ext_irq_register(VBUS_DETECT, vbus_detection_cb);

    for(;;) {

        for(i = 0; i < BUFF_SIZE; i++) {
            xQueueReceive(xDATA_q, &endptBuf[i], ~0);
        }

        if(usb_state() == USB_Configured && usb_dtr()) {
            usb_write(endptBuf, BUFF_SIZE);
        }
    }
}