/*
 * et6226m.h
 * ET6226M 7-Segment Display Driver for STM8S
 * I2C Interface: PB4 (SCL), PB5 (SDA)
 */

#ifndef ET6226M_H
#define ET6226M_H

#include <stdint.h>

// ET6226M I2C addresses (7-bit addresses, will be shifted left by I2C driver)
#define ET6226M_STATUS_ADDRESS      0x48
#define ET6226M_DATA0_ADDRESS       0x68
#define ET6226M_DATA1_ADDRESS       0x6A
#define ET6226M_DATA2_ADDRESS       0x6C
#define ET6226M_DATA3_ADDRESS       0x6E
#define ET6226M_KEY_CODE_ADDRESS    0x4F

#define SEGMENT_ROWS 3  // Number of physical 7-segment display rows

// 7-segment font table (0-9, A-Z)
extern const uint8_t seven_segment_font[36];

/**
 * @brief Initialize the ET6226M display
 * @note Must be called after I2C is initialized
 */
void ET6226M_Init(void);

/**
 * @brief Display a single character on specified row
 * @param row: Display row (0-2)
 * @param character: Character to display ('0'-'9', 'A'-'Z', '-')
 */
void ET6226M_DisplayChar(uint8_t row, char character);

/**
 * @brief Display a string across multiple rows
 * @param str: Null-terminated string to display
 */
void ET6226M_DisplayString(const char *str);

/**
 * @brief Clear all displays
 */
void ET6226M_Clear(void);

/**
 * @brief Read key code from ET6226M
 * @return Key character ('0'-'9', 'B', 'C') or 0 if no key pressed
 */
uint8_t ET6226M_GetKeyCode(void);

#endif /* ET6226M_H */