/**
  ******************************************************************************
  * @file    gn1640t.h
  * @brief   GN1640T LED Display Driver Header
  * @author  STM8 GN1640T Driver
  * @version 1.0.0
  * @date    2026
  ******************************************************************************
  * @description
  * Driver for GN1640T LED controller driving three 16-segment displays.
  * 
  * Hardware Configuration:
  * - MCU: STM8S003
  * - Display: 3x FJ3291BH (2-digit 16-segment displays = 6 digits total)
  * - Controller: GN1640T
  * - Connections:
  *   - CLK:  PB4 -> GN1640T CLK
  *   - DATA: PB5 -> GN1640T DATA
  *   - GN1640T GRID1-GRID16 -> 16 segments (shared across all digits)
  *   - GN1640T SEG1-SEG2 -> Display 1 (DIG1, DIG2)
  *   - GN1640T SEG3-SEG4 -> Display 2 (DIG3, DIG4)
  *   - GN1640T SEG5-SEG6 -> Display 3 (DIG5, DIG6)
  ******************************************************************************
  */

  #ifndef __GN1640T_H
  #define __GN1640T_H
  
  #include "stm8s.h"
  
  /*============================================================================*/
  /* HARDWARE CONFIGURATION                                                      */
  /*============================================================================*/
  
  // Pin Configuration - Modify these to match your hardware
  #define GN1640_CLK_PIN    GPIO_PIN_4  // PB4
  #define GN1640_DATA_PIN   GPIO_PIN_5  // PB5
  #define GN1640_PORT       GPIOB
  
  /*============================================================================*/
  /* GN1640T COMMAND DEFINITIONS                                                */
  /*============================================================================*/
  
  #define CMD_DATA_SET      0x40  // Data setting command
  #define CMD_DISP_CTRL     0x80  // Display control command
  #define CMD_ADDR_SET      0xC0  // Address setting command
  
  /*============================================================================*/
  /* DISPLAY CONTROL SETTINGS                                                   */
  /*============================================================================*/
  
  #define DISP_ON           0x08  // Display ON
  #define DISP_OFF          0x00  // Display OFF
  #define BRIGHTNESS_MIN    0x00  // Minimum brightness (0-7)
  #define BRIGHTNESS_MAX    0x07  // Maximum brightness (0-7)
  
  /*============================================================================*/
  /* DISPLAY PARAMETERS                                                         */
  /*============================================================================*/
  
  #define GN1640_GRIDS      16    // Number of grids (segments)
  #define GN1640_SEGMENTS   6     // Number of segment pins (digits)
  #define GN1640_DIGITS     6     // Total displayable digits (3 displays x 2)
  
  /*============================================================================*/
  /* 16-SEGMENT DEFINITIONS                                                     */
  /*============================================================================*/
  
  // Macro to convert segment number (1-16) to bit position
  #define SEG(n) (1U << ((n)-1))
  
  // 16-Segment layout (typical starburst configuration)
  //      _1_
  //  16 |8|9|2
  //     |_12_|
  //  15|10|11|3
  //  7  |_6_| 4
  //       5
  //
  // Segments: 1-8 outer, 9-16 inner/diagonal
  
  /*============================================================================*/
  /* FONT TABLE STRUCTURE                                                       */
  /*============================================================================*/
  
  typedef struct {
      char ch;          // Character
      uint16_t mask;    // 16-bit segment mask
  } glyph_t;
  
  // Built-in font for A-Z, 0-9, and basic symbols
  extern const glyph_t GN1640_Font[];
  extern const uint8_t GN1640_FontCount;
  
  /*============================================================================*/
  /* GLOBAL DISPLAY BUFFER                                                      */
  /*============================================================================*/
  
  // Display buffer: 16 bytes for 16 grids (segments)
  // Each byte controls 6 segment pins (digits)
  // Bit 0 = SEG1 (Display 1, Digit 1)
  // Bit 1 = SEG2 (Display 1, Digit 2)
  // Bit 2 = SEG3 (Display 2, Digit 3)
  // Bit 3 = SEG4 (Display 2, Digit 4)
  // Bit 4 = SEG5 (Display 3, Digit 5)
  // Bit 5 = SEG6 (Display 3, Digit 6)
  extern uint8_t displayBuffer[GN1640_GRIDS];
  
  /*============================================================================*/
  /* CORE DRIVER FUNCTIONS                                                      */
  /*============================================================================*/
  
  /**
   * @brief Initialize GN1640T driver and display
   * @note Call this once during system initialization
   */
  void GN1640_Init(void);
  
  /**
   * @brief Clear display buffer and turn off all segments
   */
  void GN1640_Clear(void);
  
  /**
   * @brief Update display with current buffer contents
   */
  void GN1640_UpdateDisplay(void);
  
  /**
   * @brief Set display brightness
   * @param brightness: 0-7 (0=dimmest, 7=brightest)
   */
  void GN1640_SetBrightness(uint8_t brightness);
  
  /**
   * @brief Turn display on/off
   * @param state: 1=on, 0=off
   */
  void GN1640_SetDisplayState(uint8_t state);
  
  /*============================================================================*/
  /* HIGH-LEVEL DISPLAY FUNCTIONS                                               */
  /*============================================================================*/
  
  /**
   * @brief Display a single character on specified digit
   * @param digit: Digit position (0-5)
   * @param ch: Character to display (A-Z, 0-9, symbols)
   * @return 1 if successful, 0 if character not found in font
   */
  uint8_t GN1640_DisplayChar(uint8_t digit, char ch);
  
  /**
   * @brief Display a string across multiple digits
   * @param start_digit: Starting digit position (0-5)
   * @param str: Null-terminated string
   * @note Stops at end of string or when display is full
   */
  void GN1640_DisplayString(uint8_t start_digit, const char* str);
  
  /**
   * @brief Display a number (integer)
   * @param start_digit: Starting digit position (0-5)
   * @param number: Number to display
   * @param leading_zeros: 1=show leading zeros, 0=suppress
   */
  void GN1640_DisplayNumber(uint8_t start_digit, int16_t number, uint8_t leading_zeros);
  
  /**
   * @brief Set specific segments on a specific digit
   * @param digit: Digit position (0-5)
   * @param segment_mask: 16-bit mask of segments to turn on
   */
  void GN1640_SetDigitSegments(uint8_t digit, uint16_t segment_mask);
  
  /**
   * @brief Get character segment mask from font table
   * @param ch: Character to look up
   * @param mask: Pointer to store segment mask
   * @return 1 if found, 0 if not found
   */
  uint8_t GN1640_GetCharMask(char ch, uint16_t* mask);
  
  /*============================================================================*/
  /* COMMUNICATION FUNCTIONS                                                    */
  /*============================================================================*/

  /**
   * @brief Send one GN1640T frame: START + data[0..len-1] + STOP (LSB-first)
   * @param data: Pointer to byte array
   * @param len:  Number of bytes to send
   *
   * GN1640T requires separate frames per command:
   *   {0x40}               data set (auto address increment)
   *   {0xC0, grid[0..15]}  address 0 + 16 grid bytes
   *   {0x8F}               display ON, max brightness
   *
   * Note: Start/Stop/WriteByte are internal. Use WriteFrame or SendCommand.
   */
  void GN1640_WriteFrame(uint8_t *data, uint8_t len);

  /**
   * @brief Send a single-byte command frame (convenience wrapper)
   * @param cmd: Command byte
   */
  void GN1640_SendCommand(uint8_t cmd);
  
  /*============================================================================*/
  /* BUFFER MANIPULATION FUNCTIONS                                              */
  /*============================================================================*/
  
  /**
   * @brief Set a specific grid (segment) value
   * @param grid: Grid number (0-15)
   * @param seg_mask: Segment mask (which digits to turn on)
   */
  void GN1640_SetGrid(uint8_t grid, uint8_t seg_mask);
  
  /**
   * @brief Get current grid value
   * @param grid: Grid number (0-15)
   * @return Current segment mask for this grid
   */
  uint8_t GN1640_GetGrid(uint8_t grid);
  
  #endif /* __GN1640T_H */