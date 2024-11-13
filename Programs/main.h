#ifndef MAIN_H
#define MAIN_H

#include "stm32f7xx_hal.h"  // STM32F7 HAL library
#include "usb_device.h"     // USB device library including DFU

// ADC handle and value storage
extern ADC_HandleTypeDef hadc1;  
extern uint32_t adc_values[8];  // Array to store 8 ADC values

// Function Prototypes
void SystemInit(void);
void JumpToDFU(void);
int IsUSBConnected(void);
void ADC_Config(void);
void USB_Config(void);

#endif /* MAIN_H */
