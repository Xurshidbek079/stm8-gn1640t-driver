/**
  ******************************************************************************
  * @file    gn1640t.c
  * @brief   GN1640T LED Display Driver Implementation
  * @author  STM8 GN1640T Driver
  * @version 1.1.0
  * @date    2026
  ******************************************************************************
  */

  #include "gn1640t.h"
  #include <string.h>

  /*============================================================================*/
  /* GLOBAL VARIABLES                                                           */
  /*============================================================================*/

  /* Display buffer: 16 bytes, one per grid row.
   * Each byte controls which digit columns are lit for that segment row.
   * Bit 0 = digit 0, Bit 1 = digit 1, ..., Bit 5 = digit 5 */
  uint8_t displayBuffer[GN1640_GRIDS] = {0};

  /*============================================================================*/
  /* FONT TABLE - 16-SEGMENT CHARACTER DEFINITIONS                              */
  /*============================================================================*/

  const glyph_t GN1640_Font[] = {
      /* Uppercase letters */
      {'A', SEG(1)|SEG(2)|SEG(3)|SEG(4)|SEG(7)|SEG(8)|SEG(12)|SEG(16)},
      {'B', SEG(1)|SEG(2)|SEG(3)|SEG(4)|SEG(5)|SEG(6)|SEG(10)|SEG(12)|SEG(14)},
      {'C', SEG(1)|SEG(2)|SEG(5)|SEG(6)|SEG(7)|SEG(8)},
      {'D', SEG(1)|SEG(2)|SEG(3)|SEG(4)|SEG(5)|SEG(6)|SEG(10)|SEG(14)},
      {'E', SEG(1)|SEG(2)|SEG(5)|SEG(6)|SEG(7)|SEG(8)|SEG(16)},
      {'F', SEG(1)|SEG(2)|SEG(7)|SEG(8)|SEG(16)},
      {'G', SEG(1)|SEG(2)|SEG(4)|SEG(5)|SEG(6)|SEG(7)|SEG(8)|SEG(12)},
      {'H', SEG(3)|SEG(4)|SEG(7)|SEG(8)|SEG(12)|SEG(16)},
      {'I', SEG(1)|SEG(2)|SEG(5)|SEG(6)|SEG(10)|SEG(14)},
      {'J', SEG(3)|SEG(4)|SEG(5)|SEG(6)|SEG(7)},
      {'K', SEG(7)|SEG(8)|SEG(11)|SEG(13)|SEG(16)},
      {'L', SEG(5)|SEG(6)|SEG(7)|SEG(8)},
      {'M', SEG(3)|SEG(4)|SEG(7)|SEG(8)|SEG(9)|SEG(11)},
      {'N', SEG(3)|SEG(4)|SEG(7)|SEG(8)|SEG(9)|SEG(13)},
      {'O', SEG(1)|SEG(2)|SEG(3)|SEG(4)|SEG(5)|SEG(6)|SEG(7)|SEG(8)},
      {'P', SEG(1)|SEG(2)|SEG(3)|SEG(7)|SEG(8)|SEG(12)|SEG(16)},
      {'Q', SEG(1)|SEG(2)|SEG(3)|SEG(4)|SEG(5)|SEG(6)|SEG(7)|SEG(8)|SEG(13)},
      {'R', SEG(1)|SEG(2)|SEG(3)|SEG(7)|SEG(8)|SEG(12)|SEG(13)|SEG(16)},
      {'S', SEG(1)|SEG(2)|SEG(4)|SEG(5)|SEG(6)|SEG(8)|SEG(12)|SEG(16)},
      {'T', SEG(1)|SEG(2)|SEG(10)|SEG(14)},
      {'U', SEG(3)|SEG(4)|SEG(5)|SEG(6)|SEG(7)|SEG(8)},
      {'V', SEG(7)|SEG(8)|SEG(11)|SEG(15)},
      {'W', SEG(3)|SEG(4)|SEG(7)|SEG(8)|SEG(13)|SEG(15)},
      {'X', SEG(9)|SEG(11)|SEG(13)|SEG(15)},
      {'Y', SEG(3)|SEG(4)|SEG(5)|SEG(6)|SEG(8)|SEG(12)|SEG(16)},
      {'Z', SEG(1)|SEG(2)|SEG(5)|SEG(6)|SEG(11)|SEG(15)},

      /* Numbers */
      {'0', SEG(1)|SEG(2)|SEG(3)|SEG(4)|SEG(5)|SEG(6)|SEG(7)|SEG(8)|SEG(11)|SEG(15)},
      {'1', SEG(3)|SEG(4)|SEG(11)},
      {'2', SEG(1)|SEG(2)|SEG(3)|SEG(5)|SEG(6)|SEG(7)|SEG(12)|SEG(16)},
      {'3', SEG(1)|SEG(2)|SEG(3)|SEG(4)|SEG(5)|SEG(6)|SEG(12)},
      {'4', SEG(3)|SEG(4)|SEG(8)|SEG(12)|SEG(16)},
      {'5', SEG(1)|SEG(2)|SEG(5)|SEG(6)|SEG(8)|SEG(13)|SEG(16)},
      {'6', SEG(1)|SEG(2)|SEG(4)|SEG(5)|SEG(6)|SEG(7)|SEG(8)|SEG(12)|SEG(16)},
      {'7', SEG(1)|SEG(2)|SEG(3)|SEG(4)},
      {'8', SEG(1)|SEG(2)|SEG(3)|SEG(4)|SEG(5)|SEG(6)|SEG(7)|SEG(8)|SEG(12)|SEG(16)},
      {'9', SEG(1)|SEG(2)|SEG(3)|SEG(4)|SEG(5)|SEG(6)|SEG(8)|SEG(12)|SEG(16)},

      /* Symbols */
      {':', SEG(10)|SEG(14)},
      {'=', SEG(5)|SEG(6)|SEG(12)|SEG(16)},
      {'-', SEG(12)|SEG(16)},
      {'+', SEG(10)|SEG(12)|SEG(14)|SEG(16)},
      {' ', 0x0000},
  };

  const uint8_t GN1640_FontCount = sizeof(GN1640_Font) / sizeof(GN1640_Font[0]);

  /*============================================================================*/
  /* PHYSICAL BIT-BANG LAYER (internal)                                         */
  /*============================================================================*/

  static void gn1640_delay_us(uint16_t us)
  {
      uint16_t i;
      for (i = 0; i < us; i++) {
          nop(); nop(); nop(); nop();
      }
  }

  /* Pull CLK+DATA high, then pull DATA low while CLK is high = START */
  static void GN1640_Start(void)
  {
      /* Borrow PB4/PB5 from the I2C peripheral */
      I2C_Cmd(DISABLE);
      GPIO_Init(GN1640_PORT, GN1640_CLK_PIN,  GPIO_MODE_OUT_PP_HIGH_FAST);
      GPIO_Init(GN1640_PORT, GN1640_DATA_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);

      GPIO_WriteHigh(GN1640_PORT, GN1640_DATA_PIN);
      GPIO_WriteHigh(GN1640_PORT, GN1640_CLK_PIN);
      gn1640_delay_us(2);
      GPIO_WriteLow(GN1640_PORT, GN1640_DATA_PIN);
      gn1640_delay_us(2);
  }

  /* Pull DATA low, then bring CLK high, then DATA high = STOP.
   * Restores PB4/PB5 to open-drain and re-enables I2C. */
  static void GN1640_Stop(void)
  {
      GPIO_WriteLow(GN1640_PORT, GN1640_CLK_PIN);
      gn1640_delay_us(2);
      GPIO_WriteLow(GN1640_PORT, GN1640_DATA_PIN);
      gn1640_delay_us(2);
      GPIO_WriteHigh(GN1640_PORT, GN1640_CLK_PIN);
      gn1640_delay_us(2);
      GPIO_WriteHigh(GN1640_PORT, GN1640_DATA_PIN);
      gn1640_delay_us(2);

      /* Return PB4/PB5 to the I2C peripheral */
      GPIO_Init(GN1640_PORT, GN1640_CLK_PIN,  GPIO_MODE_OUT_OD_HIZ_FAST);
      GPIO_Init(GN1640_PORT, GN1640_DATA_PIN, GPIO_MODE_OUT_OD_HIZ_FAST);
      I2C_Cmd(ENABLE);
  }

  /* Shift out one byte, LSB first (GN1640T is not standard I2C - no ACK) */
  static void GN1640_WriteByte(uint8_t data)
  {
      uint8_t i;

      GPIO_WriteLow(GN1640_PORT, GN1640_CLK_PIN);

      for (i = 0; i < 8; i++) {
          GPIO_WriteLow(GN1640_PORT, GN1640_CLK_PIN);
          gn1640_delay_us(1);

          if (data & 0x01) {
              GPIO_WriteHigh(GN1640_PORT, GN1640_DATA_PIN);
          } else {
              GPIO_WriteLow(GN1640_PORT, GN1640_DATA_PIN);
          }

          gn1640_delay_us(1);
          GPIO_WriteHigh(GN1640_PORT, GN1640_CLK_PIN);
          gn1640_delay_us(1);

          data >>= 1;
      }
  }

  /*============================================================================*/
  /* FRAME LAYER - core communication primitive                                 */
  /*============================================================================*/

  /*
   * Send one GN1640T transaction: START + data[0..len-1] + STOP.
   *
   * GN1640T protocol requires each command in its own frame:
   *   Frame 1 (data set):   GN1640_WriteFrame({0x40}, 1)
   *   Frame 2 (addr+data):  GN1640_WriteFrame({0xC0, grid[0..15]}, 17)
   *   Frame 3 (disp ctrl):  GN1640_WriteFrame({0x8F}, 1)
   */
  void GN1640_WriteFrame(uint8_t *data, uint8_t len)
  {
      uint8_t i;
      GN1640_Start();
      for (i = 0; i < len; i++) {
          GN1640_WriteByte(data[i]);
      }
      GN1640_Stop();
  }

  /* Single-byte command frame */
  void GN1640_SendCommand(uint8_t cmd)
  {
      GN1640_WriteFrame(&cmd, 1);
  }

  /*============================================================================*/
  /* CORE DRIVER FUNCTIONS                                                      */
  /*============================================================================*/

  void GN1640_Init(void)
  {
      gn1640_delay_us(100);
      GN1640_SendCommand(CMD_DATA_SET | 0x00);   /* Frame 1: auto address increment */
      GN1640_Clear();                            /* Frame 2: clear all grids        */
      GN1640_SetBrightness(BRIGHTNESS_MAX);      /* Frame 3: display ON, max bright */
  }

  void GN1640_Clear(void)
  {
      uint8_t i;
      for (i = 0; i < GN1640_GRIDS; i++) {
          displayBuffer[i] = 0;
      }
      GN1640_UpdateDisplay();
  }

  /* Build the address+data frame and send it in one transaction */
  void GN1640_UpdateDisplay(void)
  {
      uint8_t frame[GN1640_GRIDS + 1];
      uint8_t i;

      frame[0] = CMD_ADDR_SET;                   /* 0xC0 - start at grid address 0 */
      for (i = 0; i < GN1640_GRIDS; i++) {
          frame[1 + i] = displayBuffer[i];
      }
      GN1640_WriteFrame(frame, GN1640_GRIDS + 1);
  }

  void GN1640_SetBrightness(uint8_t brightness)
  {
      if (brightness > BRIGHTNESS_MAX) {
          brightness = BRIGHTNESS_MAX;
      }
      GN1640_SendCommand((uint8_t)(CMD_DISP_CTRL | DISP_ON | brightness));
  }

  void GN1640_SetDisplayState(uint8_t state)
  {
      if (state) {
          GN1640_SendCommand(CMD_DISP_CTRL | DISP_ON | BRIGHTNESS_MAX);
      } else {
          GN1640_SendCommand(CMD_DISP_CTRL | DISP_OFF);
      }
  }

  /*============================================================================*/
  /* BUFFER MANIPULATION FUNCTIONS                                              */
  /*============================================================================*/

  void GN1640_SetGrid(uint8_t grid, uint8_t seg_mask)
  {
      if (grid < GN1640_GRIDS) {
          displayBuffer[grid] = (uint8_t)(seg_mask & 0x3F);
      }
  }

  uint8_t GN1640_GetGrid(uint8_t grid)
  {
      if (grid < GN1640_GRIDS) {
          return displayBuffer[grid];
      }
      return 0;
  }

  /*============================================================================*/
  /* HIGH-LEVEL DISPLAY FUNCTIONS                                               */
  /*============================================================================*/

  uint8_t GN1640_GetCharMask(char ch, uint16_t *mask)
  {
      uint8_t i;
      for (i = 0; i < GN1640_FontCount; i++) {
          if (GN1640_Font[i].ch == ch) {
              *mask = GN1640_Font[i].mask;
              return 1;
          }
      }
      return 0;
  }

  void GN1640_SetDigitSegments(uint8_t digit, uint16_t segment_mask)
  {
      uint8_t seg;

      if (digit >= GN1640_DIGITS) {
          return;
      }

      for (seg = 0; seg < 16; seg++) {
          if (segment_mask & (1U << seg)) {
              displayBuffer[seg] |=  (uint8_t)(1 << digit);
          } else {
              displayBuffer[seg] &= (uint8_t)~(1 << digit);
          }
      }
  }

  uint8_t GN1640_DisplayChar(uint8_t digit, char ch)
  {
      uint16_t mask;

      if (digit >= GN1640_DIGITS) {
          return 0;
      }
      if (!GN1640_GetCharMask(ch, &mask)) {
          return 0;
      }
      GN1640_SetDigitSegments(digit, mask);
      return 1;
  }

  void GN1640_DisplayString(uint8_t start_digit, const char *str)
  {
      uint8_t pos = start_digit;

      while (*str && pos < GN1640_DIGITS) {
          GN1640_DisplayChar(pos, *str);
          str++;
          pos++;
      }
      GN1640_UpdateDisplay();
  }

  void GN1640_DisplayNumber(uint8_t start_digit, int16_t number, uint8_t leading_zeros)
  {
      char buffer[7];
      uint8_t i, len;
      int16_t temp = number;
      uint8_t is_negative = 0;

      if (number < 0) {
          is_negative = 1;
          temp = -number;
          start_digit++;
      }

      i = 0;
      do {
          buffer[i++] = (char)('0' + (temp % 10));
          temp /= 10;
      } while (temp > 0 && i < 6);

      len = i;

      if (leading_zeros) {
          while (i < (GN1640_DIGITS - start_digit) && i < 6) {
              buffer[i++] = '0';
          }
          len = i;
      }

      if (is_negative && start_digit > 0) {
          GN1640_DisplayChar((uint8_t)(start_digit - 1), '-');
      }

      for (i = 0; i < len && (start_digit + i) < GN1640_DIGITS; i++) {
          GN1640_DisplayChar((uint8_t)(start_digit + i), buffer[len - 1 - i]);
      }

      GN1640_UpdateDisplay();
  }
