/*
 * seal_MSG.h
 *
 * Created: 30-Apr-18 22:47:57
 *  Author: Ethan
 */
#include "seal_RTOS.h"
#include "seal_USB.h"
#include "sealPrint.h"

#ifndef SEAL_MSG_H_
#define SEAL_MSG_H_

#define MSG_STACK_SIZE                  (750 / sizeof(portSTACK_TYPE))
#define MSG_TASK_PRI                    (tskIDLE_PRIORITY + 1)

typedef enum {
    DEV_ENV  = 0x10,
    DEV_IMU  = 0x20,
    DEV_GPS  = 0x30,
    DEV_MOD  = 0x40,
    DEV_MEM  = 0x50,
    DEV_CPU  = 0x60,
    DEV_MASK = 0xF0
} DEV_ID_t;

typedef enum {
    ERROR_NONE    = 0x00,
    ERROR_TEMP    = 0x01,
    ERROR_LIGHT   = 0x02,
    ERROR_CRC     = 0x03,
    ERROR_I2C     = 0x04,
    ERROR_TIMEOUT = 0x05
} DEV_ERR_CODES_t;

// 24-bit system wide event group
typedef enum {
    EVENT_VBUS,
    EVENT_LOW_BATTERY,
    EVENT_MOTION_SURGE,
    EVENT_MOTION_SWAY,
    EVENT_MOTION_HEAVE,
    EVENT_POSITION_1,
    EVENT_POSITION_2,
    EVENT_POSITION_3
} SYSTEM_EVENT_FLAGS_t;

typedef struct __attribute__((__packed__)){
    uint16_t srtSym;    // symbol to indicate start of packet
    uint16_t id;	    // Upper four bits is the device ID, lower four are device specific event flags
    uint32_t timestamp; // timestamp. how many bits?
    uint16_t size;		// size of data packet to follow. in bytes or samples? (worst case IMU size in bytes would need a uint16 :( )
} DATA_HEADER_t;

extern TaskHandle_t       xMSG_th;      // Message accumulator for USB/MEM
extern EventGroupHandle_t xMSG_eg;      // IMU event group
extern SemaphoreHandle_t  disp_mutex;   // Mutex to control access to USB terminal
extern QueueHandle_t      xMSG_q;       // a message Queue for collecting all logged data

int32_t byteQ_write(uint8_t* buff, const uint32_t LEN);

int32_t MSG_task_init(uint32_t qLength);

void MSG_task(void* pvParameters);

#endif /* SEAL_MSG_H_ */