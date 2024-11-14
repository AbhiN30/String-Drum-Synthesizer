#include "main.h"
#include "usb_device.h"

// ADC values array
uint16_t adc_values[8];

// Function declarations
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC_Init(void);
static void MX_USB_DEVICE_Init(void);
void JumpToDFU(void);
void ExitDFUAndReboot(void);

// USB device state (handled externally by USB library)
extern USB_HandleTypeDef hUsbDeviceFS;

int main(void)
{
    // Initialize hardware
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_ADC_Init();
    MX_USB_DEVICE_Init();

    while (1)
    {
        // Check if USB is connected
        if (hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED)
        {
            // If USB is connected, jump to DFU mode
            JumpToDFU();
        }
        else
        {
            // If USB is not connected, ensure we are running the main application
            // Regular ADC polling code
            for (int i = 0; i < 8; i++) 
            {
                // Start ADC conversion
                HAL_ADC_Start(&hadc1);
                if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK)
                {
                    adc_values[i] = HAL_ADC_GetValue(&hadc1);  // Store the ADC value in the array
                }
                HAL_ADC_Stop(&hadc1); // Stop ADC conversion
            }
            HAL_Delay(1000); // Delay (1 second or as needed)
        }

        // Check if USB was disconnected after DFU entry
        if (hUsbDeviceFS.dev_state != USBD_STATE_CONFIGURED)
        {
            // USB disconnected, exit DFU mode and reboot to run application code
            ExitDFUAndReboot();
        }
    }
}

// Jump to DFU mode function
void JumpToDFU(void)
{
    // DFU bootloader address for STM32F7 series (0x1FFF0000)
    typedef void (*pFunction)(void);
    uint32_t dfuAddress = 0x1FFF0000; // DFU bootloader address for STM32F7 series

    // Deinitialize all peripherals before jump
    HAL_DeInit();

    // Set the main stack pointer to the bootloader's stack pointer address
    __set_MSP(*((volatile uint32_t*)dfuAddress));

    // Set program counter to bootloader start address
    pFunction JumpToBootloader = (pFunction)(*((volatile uint32_t*) (dfuAddress + 4)));

    // Jump to bootloader (DFU mode)
    JumpToBootloader();
}

// Explicit USB disconnection handler for exiting DFU and rebooting
void ExitDFUAndReboot(void)
{
    __NVIC_SystemReset(); // Force a system reset to exit DFU mode and run main application
}

// System Clock Configuration (auto-generated function based on clock settings)
void SystemClock_Config(void)
{
    // Implementation based on your clock configuration
}

// GPIO Initialization (auto-generated based on your pin configuration)
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable GPIO Ports Clock
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();

    // --- Configure ADC1 Input Pins (Analog Mode) ---
    // PA0-PA3, PA6-PA7 as ADC1_IN#
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                          GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // PB0-PB1 as ADC1_IN#
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // --- Configure DAC Output Pin (Analog Mode) ---
    // PA4 as DAC_OUT1
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // --- Configure USB OTG FS Pins ---
    // PA11 as USB_OTG_FS_DM and PA12 as USB_OTG_FS_DP
    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // --- Configure General Purpose Output Pins ---
    // PB2-PB13 as GPIO_Output
    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |
                          GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 |
                          GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // --- Configure USB OTG HS Pins ---
    // PB14 as USB_OTG_HS_DM and PB15 as USB_OTG_HS_DP
    GPIO_InitStruct.Pin = GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_OTG_HS_FS;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // --- Other Connections ---
    // PH0 and PH1 are connected to each other - No GPIO configuration required in this code
    // BOOT0 is grounded via a 10kΩ resistor - No GPIO configuration required in this code
    // VCAP_1 is connected to ground with a 2.2 μF capacitor - No GPIO configuration required in this code
    // VSS, VSSA (ground), VBAT, VDD, VDDA (3.3V) - No GPIO configuration required in this code
}

// ADC Initialization
static void MX_ADC_Init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    // ADC1 Initialization
    __HAL_RCC_ADC1_CLK_ENABLE();

    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
    hadc1.Init.ContinuousConvMode = ENABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 8; // Number of channels (PA0-3, PA6-7, PB0-1)
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    HAL_ADC_Init(&hadc1);

    // Configure each ADC channel
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    for (int i = 0; i < 8; i++) {
        sConfig.Rank = i + 1; // Set rank for each channel
        sConfig.Channel = i;  // Channel number from PA0-3, PA6-7, PB0-1
        HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    }
}
