/**
 * @file main.c
 *
 * @brief Main source code for the BLE_UART program.
 *
 * This file contains the main entry point for the BLE_UART program,
 * which is used to demonstrate the BLE_UART driver.
 *
 * It interfaces with the Adafruit Bluefruit LE UART Friend Bluetooth Low Energy (BLE) module, which uses the UART communication protocol.
 *  - Product Link: https://www.adafruit.com/product/2479
 *
 * @note For more information regarding the Enhanced Universal Serial Communication Interface (eUSCI),
 * refer to the MSP432Pxx Microcontrollers Technical Reference Manual
 *
 * @author
 *
 */

#include "msp.h"
#include <math.h>
#include <stdint.h>

#include "inc/Clock.h"
#include "inc/CortexM.h"
#include "inc/GPIO.h"
#include "inc/EUSCI_A0_UART.h"
#include "inc/Motor.h"
#include "inc/BLE_UART.h"

// Initialize constant PWM duty cycle values for the motors
#define PWM_NOMINAL 3000

void Process_BLE_UART_Data(char BLE_UART_Buffer[])
{
    if (Check_BLE_UART_Data(BLE_UART_Buffer, "RGB LED GREEN"))
    {
        LED2_Output(RGB_LED_GREEN);
    }

    else if (Check_BLE_UART_Data(BLE_UART_Buffer, "RGB LED OFF"))
    {
        LED2_Output(RGB_LED_OFF);
    }

    else
    {
        printf("BLE UART Command Not Found\n");
    }
}

int main(void)
{
    //Ensure the interrupts are disabled during initialization
    DisableInterrupts();

    // Initialize the 48 MHz Clock
    Clock_Init48MHz();

    // Initialize the PMOD Switches
    PMOD_SWT_Init();

    // Iniitalize the RGB LED on the MSP432 microcontroller
    LED1_Init();

    // Iniitalize the RGB LED on the MSP432 microcontroller
    LED2_Init();

    // Initialize the EUSCI_A0_UART module
    EUSCI_A0_UART_Init_Printf();

    // Initialize the Adafruit Bluefruit LE UART Friend module
    BLE_UART_Init();

    // Initialize the DC motors
    Motor_Init();

    // Initialize a buffer that will be used to receive command string from the BLE module
    char BLE_UART_Buffer[BLE_UART_BUFFER_SIZE] = {0};

    // Enable the interrupts used by the modules
    EnableInterrupts();

    // Provide a short delay after initialization and reset the BLE module
    Clock_Delay1ms(1000);
    BLE_UART_Reset();

    // Send a message to the BLE module to check if the connection is stable
    BLE_UART_OutString("BLE UART Active\r\n");
    Clock_Delay1ms(1000);

    // Variable selected during programming to ensure either only Task 1 or Task 2 runs
    // If 0, perform original procedure
    // If 1, perform Task 1 for oscilloscope
    // If 2, perform Task 2 for motors
    uint8_t uint8_Choose_Operation = 2;

    while(uint8_Choose_Operation == 0)
    {
        int string_size = BLE_UART_InString(BLE_UART_Buffer, BLE_UART_BUFFER_SIZE);

        printf("BLE UART Data: ");

        for (int i = 0; i < string_size; i++)
        {
            printf("%c", BLE_UART_Buffer[i]);
        }

        printf("\n");

        Process_BLE_UART_Data(BLE_UART_Buffer);
    }

    while (uint8_Choose_Operation == 1)
    {
        uint8_t uint8_swt_status = Get_PMOD_SWT_Status();

        PMOD_SWT_and_UART_Process(uint8_swt_status);
    }

    while (uint8_Choose_Operation == 2)
    {
        BLE_UART_InPacket(BLE_UART_Buffer);
        Decode_Controller_Packet(BLE_UART_Buffer);
    }
}
