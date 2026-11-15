/**
 * @file BLE_UART.c
 *
 * @brief Source code for the BLE_UART driver.
 *
 * This file contains the function definitions for the BLE_UART driver.
 *
 * It interfaces with the Adafruit Bluefruit LE UART Friend Bluetooth Low Energy (BLE) module, which uses the UART communication protocol.
 *  - Product Link: https://www.adafruit.com/product/2479
 *
 * The following connections must be made:
 *  - BLE UART MOD  (Pin 1)     <-->  MSP432 LaunchPad Pin P1.6
 *  - BLE UART CTS  (Pin 2)     <-->  MSP432 LaunchPad GND
 *  - BLE UART TXO  (Pin 3)     <-->  MSP432 LaunchPad Pin P9.6 (PM_UCA3RXD)
 *  - BLE UART RXI  (Pin 4)     <-->  MSP432 LaunchPad Pin P9.7 (PM_UCA3TXD)
 *  - BLE UART VIN  (Pin 5)     <-->  MSP432 LaunchPad VCC (3.3V)
 *  - BLE UART RTS  (Pin 6)     <-->  Not Connected
 *  - BLE UART GND  (Pin 7)     <-->  MSP432 LaunchPad GND
 *  - BLE UART DFU  (Pin 8)     <-->  Not Connected
 *
 * @note For more information regarding the Enhanced Universal Serial Communication Interface (eUSCI),
 * refer to the MSP432Pxx Microcontrollers Technical Reference Manual
 *
 * @author
 *
 */

#include "../inc/BLE_UART.h"

void BLE_UART_Init()
{
    //Configure pins P9.6 (PM_UCA3RXD) and P9.7 (PM_UCA3TXD) to use the primary module function
    // by setting Bits 7 and 6 in the SEL0 register for P9
    // and clearing Bits 7 and 6 in the SEL1 register for P9
    P9->SEL0 |= 0xC0;
    P9->SEL1 &= ~0xC0;

    // Configure P1.6 as an output GPIO (Mode Select)
    // Clear Bit 6 of the SEL0 and SEL1 registers for P1
    // Set Bit 6 of the DIR register for P1
    P1->SEL0 &= ~0x40;
    P1->SEL1 &= ~0x40;
    P1->DIR |= 0x40;

    // Hold the EUSCI_A3 module in the reset state by setting the
    // UCSWRST bit (Bit 0) in the CTLW0 register
    EUSCI_A3->CTLW0 |= 0x01;

    // Clear all of the bits in the Module Control Word (MCTLW) register
    EUSCI_A3->MCTLW &= ~0xFF;

    // Disable the parity bit by clearing the UCPEN bit in the CTLW0 register
    EUSCI_A3->CTLW0 &= ~0x8000;

    // Select odd parity for the parity bit by clearing the UCPAR bit in the CTLW0 register
    // Note that the UCPAR bit is not used when parity is disabled
    EUSCI_A3->CTLW0 &= ~0x4000;

    // Set the bit order to LSB first by clearing the UCMSB bit in the CTLW0 register
    EUSCI_A3->CTLW0 &= ~0x2000;

    // Select 8-bit character length by clearing the UC7BIT bit in the CTLW0 register
    EUSCI_A3->CTLW0 &= ~0x1000;

    //Select one stop bit by clearing the UCSPB bit in the CTLW0 register
    EUSCI_A3->CTLW0 &= ~0x0800;

    // Enable UART mode by writing 00b to the UCMODEx field in the CTLW0 register
    EUSCI_A3->CTLW0 &= ~0x0600;

    // Disable synchronous mode by clearing the UCSYNC bit in the CTLW0 register
    EUSCI_A3->CTLW0 &= ~0x0100;

    // Configure the EUSCI_A3 module to use SMCLK as the clock source by
    // writing a value of 11b to the UCSELx field in the CTLW0 register
    EUSCI_A3->CTLW0 |= 0x00C0;

    // Set the baud rate by writing to the UCBRx field
    // in the BRW register
    // N (clock frequency) / (baud rate) = (12,000,000 / 9600) = 1250
    EUSCI_A3->BRW = 1250;

    // Disable the following interrupts by clearing the
    // corresponding bits in the IE register:
    // Transmit Complete Interrupt UCTxCPTIE
    // Start Bit Interrupt UCSTTIE
    EUSCI_A3->IE &= ~0x0C;

    // Enable the following interrupts by setting the
    // corresponding bits in the IE register
    // Transmit interrupt UCTXIE
    // Receive Interrupt UCRXIE
    EUSCI_A3->IE |= 0x03;

    // Release the EUSCI_A3 module from the reset state by clearing the
    // UCSWRST bit in the CTLW0 register
    EUSCI_A3->CTLW0 &= ~0x01;
}

uint8_t BLE_UART_InChar()
{
    // Check the Receive Interrupt flag UCRXIFG
    // in the IFG register and wait if the the flag is not set
    // If the UCRXIFG is set, then the receiver buffer UCAxRXBUF has
    // reecive a complete character
    while ((EUSCI_A3->IFG & 0x01) == 0);

    //Return the data from the Receive Buffer (UCAxRXBUF)
    // Reading the UCAxRXBUF will reset the UCRXIFG flag
    return EUSCI_A3->RXBUF;
}

void BLE_UART_OutChar(uint8_t data)
{
    // Check the Transmit Interrupt flag UCTXIFG
    // in the IFG register and wait if the flag is not set
    // If the UCTXIFG is set, then the Transmit Buffer UCAxTXBUF is empty
    while ((EUSCI_A3->IFG & 0x02) == 0);

    // Write the data to the transmit buffer UCAxTXBUF
    // Writing to the UCAxTXBUF will clear the UCTXIFG flag
    EUSCI_A3->TXBUF = data;
}

// Task 6.2
void BLE_UART_InPacket(char *buffer_pointer)
{
    char character = '0';

    // Wait for the symbol to be received
    while (character != '!')
    {
        character = BLE_UART_InChar();
    }

    // Store 5 data bytes into the BLE_UART_Buffer array
    for (int i = 0; i < 5; i++)
    {
        *buffer_pointer = character;
        buffer_pointer++;

        if (i == 4)
        {
            continue;
        }

        else {
            character = BLE_UART_InChar();
        }
    }

    *buffer_pointer = 0;
}

// Task 6.2
void Decode_Controller_Packet(char *buffer_pointer)
{

    // Confirm that the first data byte is ! before proceeding
    if (*buffer_pointer != '!')
    {
        LED1_Output(RED_LED_ON);
        LED2_Output(RGB_LED_RED);
        buffer_pointer = 0;
        return;
    }

    buffer_pointer++;

    // Confirm that the second data byte is B before proceeding
    if (*buffer_pointer != 'B')
    {
        LED1_Output(RED_LED_ON);
        LED2_Output(RGB_LED_WHITE);
        buffer_pointer = 0;
        return;
    }

    buffer_pointer++;
    buffer_pointer++;

    // If button is pressed, move the robot
    // If button is released, stop the robot
    // Use LEDs to determine what state the robot is in for debugging
    if (*buffer_pointer == '1')
    {
        buffer_pointer--;

        if (*buffer_pointer == '5')
        {
            Motor_Forward(3000, 3000);
            LED2_Output(RGB_LED_GREEN);
        }

        else if (*buffer_pointer == '6')
        {
            Motor_Backward(3000, 3000);
            LED2_Output(RGB_LED_PINK);
        }

        else if (*buffer_pointer == '7')
        {
            Motor_Left(3000, 3000);
            LED2_Output(RGB_LED_YELLOW);
        }

        else if (*buffer_pointer == '8')
        {
            Motor_Right(3000, 3000);
            LED2_Output(RGB_LED_SKY_BLUE);
        }

        else
        {
            LED2_Output(RGB_LED_RED);
        }
    }

    else
    {
        Motor_Stop();
        LED2_Output(RGB_LED_WHITE);
    }

    buffer_pointer = 0;

}

int BLE_UART_InString(char *buffer_pointer, uint16_t buffer_size)
{
    int length = 0;
    int string_size = 0;

    // Read the last received data from the UART receive buffer
    char character = BLE_UART_InChar();

    // Check if the received character is a carriage return. Otherwise
    // if it is a valid character, then store it in BLE_UART buffer in main.
    // For each valid character, increment the string_size variable
    // which will indicate how many characters have been detected from UART BLE module
    while(character != LF)
    {
        // Remove character from buffer if the received character is a backspace character
        if (character == BS)
        {
            buffer_pointer--;
            length--;
            BLE_UART_OutChar(BS);
        }

        // Otherwise if there are more characters to be read, store them in the buffer
        else if (length < buffer_size)
        {
            *buffer_pointer = character;
            buffer_pointer++;
            length++;
            string_size++;
        }

        character = BLE_UART_InChar();
    }

    *buffer_pointer = 0;

    return string_size;
}

void BLE_UART_OutString(char *pt)
{
    while(*pt)
    {
        BLE_UART_OutChar(*pt);
        pt++;
    }
}

uint8_t Check_BLE_UART_Data(char BLE_UART_Data_Buffer[], char *data_string)
{
    if (strstr(BLE_UART_Data_Buffer, data_string) != NULL)
    {
        return 0x01;
    }

    else
    {
        return 0x00;
    }
}

void BLE_UART_Reset()
{
    // Switch to CMD mode by setting the MOD pin (P1.6) to 1
    P1->OUT |= 0x40;
    Clock_Delay1ms(1000);

    // Send the system reset command by sending the ATZ string
    // to the BLE UART module
    BLE_UART_OutString("ATZ\r\n");
    Clock_Delay1ms(3000);

    // Switch back to DATA mode by clearing the MOD pin (P1.6) to 0
    P1->OUT &= ~0x40;
}
