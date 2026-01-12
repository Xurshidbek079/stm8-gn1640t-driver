/*
 * et6226m.c
 * ET6226M 7-Segment Display Driver for STM8S
 */

#include "et6226m.h"
#include "stm8s_i2c.h"
#include <ctype.h>

// 7-segment font table
// Bit mapping: DP g f e d c b a
const uint8_t seven_segment_font[36] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111, // 9
    0b01110111, // A
    0b01111100, // B
    0b00111001, // C
    0b01011110, // D
    0b01111001, // E
    0b01110001, // F
    0b00111101, // G
    0b01110110, // H
    0b00000110, // I
    0b00011110, // J
    0b01110110, // K
    0b00111000, // L
    0b00010101, // M
    0b00110111, // N
    0b00111111, // O
    0b01110011, // P
    0b01100111, // Q
    0b00110001, // R
    0b01101101, // S
    0b01111000, // T
    0b00111110, // U
    0b00111110, // V
    0b00101010, // W
    0b01110110, // X
    0b01101110, // Y
    0b01011011  // Z
};

/**
 * @brief Write one byte to ET6226M via I2C
 * @param address: I2C device address (7-bit)
 * @param data: Data byte to write
 */
static void ET6226M_WriteByte(uint8_t address, uint8_t data)
{
    // Wait until I2C bus is free
    while (I2C_GetFlagStatus(I2C_FLAG_BUSBUSY));
    
    // Generate START condition
    I2C_GenerateSTART(ENABLE);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
    
    // Send device address with write bit
    I2C_Send7bitAddress(address, I2C_DIRECTION_TX);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    
    // Send data byte
    I2C_SendData(data);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    
    // Generate STOP condition
    I2C_GenerateSTOP(ENABLE);
}

/**
 * @brief Read one byte from ET6226M via I2C
 * @param address: I2C device address (7-bit)
 * @return Data byte read from device
 */
static uint8_t ET6226M_ReadByte(uint8_t address)
{
    uint8_t data;
    
    // Wait until I2C bus is free
    while (I2C_GetFlagStatus(I2C_FLAG_BUSBUSY));
    
    // Generate START condition
    I2C_GenerateSTART(ENABLE);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
    
    // Send device address with read bit
    I2C_Send7bitAddress(address, I2C_DIRECTION_RX);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    
    // Disable ACK for single byte read
    I2C_AcknowledgeConfig(DISABLE);
    
    // Wait for data reception
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED));
    
    // Read received data
    data = I2C_ReceiveData();
    
    // Generate STOP condition
    I2C_GenerateSTOP(ENABLE);
    
    // Re-enable ACK for future transfers
    I2C_AcknowledgeConfig(ENABLE);
    
    return data;
}

/**
 * @brief Initialize ET6226M display module
 */
void ET6226M_Init(void)
{
    uint8_t init_data = 0x01;
    ET6226M_WriteByte(ET6226M_STATUS_ADDRESS, init_data);
}

/**
 * @brief Display a character on specified row
 * @param row: Display row number (0-2)
 * @param character: Character to display ('0'-'9', 'A'-'Z', '-', or space)
 */
void ET6226M_DisplayChar(uint8_t row, char character)
{
    if (row >= SEGMENT_ROWS) return;
    
    uint8_t segments = 0;
    uint8_t address;
    
    // Convert to uppercase for consistency
    character = toupper((unsigned char)character);
    
    // Map character to segment pattern
    if (character >= '0' && character <= '9') {
        segments = seven_segment_font[character - '0'];
    } else if (character >= 'A' && character <= 'Z') {
        segments = seven_segment_font[character - 'A' + 10];
    } else if (character == '-') {
        segments = 0b01000000;  // Middle segment only
    } else {
        segments = 0;  // Blank for space or unsupported
    }
    
    // Select I2C address for the row
    switch (row) {
        case 0: address = ET6226M_DATA0_ADDRESS; break;
        case 1: address = ET6226M_DATA1_ADDRESS; break;
        case 2: address = ET6226M_DATA2_ADDRESS; break;
        default: return;
    }
    
    // Send segment data to display
    ET6226M_WriteByte(address, segments);
}

/**
 * @brief Display a string across multiple display rows
 * @param str: Null-terminated string to display
 */
void ET6226M_DisplayString(const char *str)
{
    uint8_t row = 0;
    
    // Display each character on consecutive rows
    while (*str && row < SEGMENT_ROWS) {
        ET6226M_DisplayChar(row, *str);
        str++;
        row++;
    }
    
    // Clear any remaining displays
    while (row < SEGMENT_ROWS) {
        ET6226M_DisplayChar(row, ' ');
        row++;
    }
}

/**
 * @brief Clear all display rows (turn off all segments)
 */
void ET6226M_Clear(void)
{
    for (uint8_t row = 0; row < SEGMENT_ROWS; row++) {
        ET6226M_DisplayChar(row, ' ');
    }
}

/**
 * @brief Read keypad input from ET6226M
 * @return Character representing pressed key ('0'-'9', 'B', 'C') or 0 if no key
 */
uint8_t ET6226M_GetKeyCode(void)
{
    uint8_t key_code = ET6226M_ReadByte(ET6226M_KEY_CODE_ADDRESS);
    
    // Translate raw key codes to characters
    switch (key_code) {
        case 0x5D: return '0';
        case 0x46: return '1';
        case 0x45: return '2';
        case 0x44: return '3';
        case 0x4E: return '4';
        case 0x4D: return '5';
        case 0x4C: return '6';
        case 0x56: return '7';
        case 0x55: return '8';
        case 0x54: return '9';
        case 0x5E: return 'C';
        case 0x5C: return 'B';
        default:   return 0;    // No key pressed
    }
}