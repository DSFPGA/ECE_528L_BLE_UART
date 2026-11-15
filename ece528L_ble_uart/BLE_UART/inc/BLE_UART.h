/**
 * @file BLE_UART.h
 *
 * @brief Header file for the BLE_UART driver.
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

#ifndef INC_BLE_UART_H_
#define INC_BLE_UART_H_

/**
 * @brief Specifies the size of the buffer used for the BLE UART module
 */
#define BLE_UART_BUFFER_SIZE 128

/**
 * @brief Carriage return character
 */
#define CR 0x0D

/**
 * @brief Line Feed character
 */
#define LF 0x0A
/**
 * @brief Back Space character
 */
#define BS 0x08

/**
 * @brief Escape character
 */
#define ESC 0x1B

/**
 * @brief Space character
 */
#define SP 0x20

/**
 * @brief Delete character
 */
#define DEL 0x7F

/*
 * @brief ! character
 */
#define EX 0x21

/*
 * @brief B character
 */
#define BCHAR 0x42

#include <stdint.h>
#include <string.h>
#include "msp.h"
#include "Clock.h"
#include "GPIO.h"
#include "Motor.h"

void BLE_UART_Init();

uint8_t BLE_UART_InChar();

void BLE_UART_OutChar(uint8_t data);

/**
 * @brief The BLE_UART_InPacket reads the 5 data bytes used for movement
 *
 * This function will first wait for a ! data byte to appear. Then, it stores
 * this byte and the following four data bytes into an array
 *
 * @param None
 *
 * @return None
 */
void BLE_UART_InPacket(char *buffer_pointer);

/**
 * @brief The Decode_Controller_Packet moves the robot based on the value
 * of the data packets received
 *
 * This function first confirms that a ! and B symbol have been received
 * Then, it checks if the controller button has been pressed or released
 * If pressed, move the robot based on the symbol provided
 * If released, stop moving the robot
 *
 * @param None
 *
 * @return None
 */
void Decode_Controller_Packet(char *buffer_pointer);

int BLE_UART_InString(char *buffer_pointer, uint16_t buffer_size);

void BLE_UART_OutString(char *pt);

uint8_t Check_BLE_UART_Data(char BLE_UART_Data_Buffer[], char *data_string);

void BLE_UART_Reset();

#endif /* INC_BLE_UART_H_ */
