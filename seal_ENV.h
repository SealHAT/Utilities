/*
 * seal_ENV.h
 *
 * Created: 30-Apr-18 22:40:35
 *  Author: Ethan
 */
#include "seal_RTOS.h"
#include "seal_MSG.h"
#include "max44009/max44009.h"
#include "si705x/si705x.h"

#ifndef SEAL_ENV_H_
#define SEAL_ENV_H_

#define ENV_STACK_SIZE                  (500 / sizeof(portSTACK_TYPE))
#define ENV_TASK_PRI                    (tskIDLE_PRIORITY + 2)

typedef struct __attribute__((__packed__)){
    DATA_HEADER_t header;
    uint16_t      temp;
    uint16_t      light;
} ENV_MSG_t;

extern TaskHandle_t xENV_th;        // environmental sensors task (light and temp)

void ENV_task(void* pvParameters);

#endif /* SEAL_ENV_H_ */