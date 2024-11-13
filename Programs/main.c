#include "main.h"
#include "usb_device.h"  // Include USB device initialization for DFU

// ADC Handle and value storage
ADC_HandleTypeDef hadc1;
uint32_t adc_values[8];  // Store 8 ADC values

// Function prototypes
void ADC_Config(void);
void JumpToDFU(void);
int IsUSBConnected(void);

// Main function
int main(void) {
    HAL_Init();  // HAL initialization
    SystemInit();  // Custom system initialization (ADC, USB, etc.)

    // Check if USB is connected (PA11 and PA12 data lines)
    if (IsUSBConnected()) {
        JumpToDFU();  // Jump to DFU mode if USB is connected
    }

    // Configure ADC channels (PA0-3, PA6-7, PB0-1)
    ADC_Config();

    // Main loop: continuously read ADC values and store in the array
    while (1) {
        // Start ADC conversion and poll for results
        for (int i = 0; i < 8; i++) {
            HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
            adc_values[i] = HAL_ADC_GetValue(&hadc1);
        }
    }
}

// Function to check if USB is connected (on PA11 and PA12)
int IsUSBConnected() {
    // Check if PA11 and PA12 are connected to USB data lines (this is a simple check)
    return (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12));
}

// Jump to DFU mode (This function may involve more low-level code for actual DFU entry)
void JumpToDFU(void) {
    // Reset system to initiate DFU mode entry
    HAL_NVIC_SystemReset();
}

// Configure the ADC peripheral (PA0-3, PA6-7, PB0-1 for analog input)
void ADC_Config(void) {
    ADC_ChannelConfTypeDef sConfig = {0};
    
    // ADC initialization
    __HAL_RCC_ADC1_CLK_ENABLE();
    
    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.ContinuousConvMode = ENABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 8;  // 8 channels (PA0-3, PA6-7, PB0-1)
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    HAL_ADC_Init(&hadc1);

    // Configure ADC channels (PA0-3, PA6-7, PB0-1)
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.Channel = ADC_CHANNEL_0; // PA0
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    sConfig.Channel = ADC_CHANNEL_1; // PA1
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    sConfig.Channel = ADC_CHANNEL_2; // PA2
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    sConfig.Channel = ADC_CHANNEL_3; // PA3
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    sConfig.Channel = ADC_CHANNEL_6; // PA6
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    sConfig.Channel = ADC_CHANNEL_7; // PA7
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    sConfig.Channel = ADC_CHANNEL_8; // PB0
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    sConfig.Channel = ADC_CHANNEL_9; // PB1
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}
