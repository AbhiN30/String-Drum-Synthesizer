#include "main.h"
#include "usb_device.h"
#include <math.h>  // To use sine function

// ADC values array
uint16_t adc_values[8];

// DAC output buffer for the sine wave
#define SINE_WAVE_SIZE 256  // Size of the sine wave buffer (256 samples for smoothness)
uint16_t sine_wave[SINE_WAVE_SIZE];  // Lookup table for sine wave values

// Function declarations
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC_Init(void);
static void MX_USB_DEVICE_Init(void);
static void MX_DAC_Init(void);  // DAC Initialization
void JumpToDFU(void);
void ExitDFUAndReboot(void);
void GenerateSineWave(void);  // Function to generate sine wave samples

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
    MX_DAC_Init();  // Initialize DAC

    // Generate the sine wave samples
    GenerateSineWave();

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

        // Output the sine wave to the DAC
        for (int i = 0; i < SINE_WAVE_SIZE; i++) {
            // Write the sine wave value to the DAC
            HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, sine_wave[i]);
            HAL_DAC_Start(&hdac, DAC_CHANNEL_1);  // Start DAC output
            HAL_Delay(10);  // Adjust delay for desired frequency
        }
    }
}

// Function to generate sine wave samples
void GenerateSineWave(void)
{
    // Generate a sine wave table (using a lookup table for simplicity)
    for (int i = 0; i < SINE_WAVE_SIZE; i++) {
        // Create sine wave with amplitude 4095 (12-bit DAC) and offset to center around 2048
        sine_wave[i] = (uint16_t)(2048 + 2047 * sin(2 * M_PI * i / SINE_WAVE_SIZE));
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
}

// DAC Initialization
static void MX_DAC_Init(void)
{
    DAC_HandleTypeDef hdac;
    DAC_ChannelConfTypeDef sConfig = {0};

    // Enable DAC peripheral clock
    __HAL_RCC_DAC1_CLK_ENABLE();

    // Initialize DAC
    hdac.Instance = DAC1;
    HAL_DAC_Init(&hdac);

    // Configure DAC channel 1 (PA4) for continuous output
    sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1);
}
