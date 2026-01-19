/**
  ******************************************************************************
  * @file    main.c
  * @brief   Example usage of GN1640T driver
  * @author  Your Name
  * @date    2025
  ******************************************************************************
  */

  #include "stm8s.h"
  #include "gn1640t.h"
  
  /**
   * @brief Simple millisecond delay
   * @param ms: Milliseconds to delay (approximate)
   */
  void DelayMs(uint16_t ms) {
      uint16_t i, j;
      for (i = 0; i < ms; i++) {
          for (j = 0; j < 120; j++) {
              nop();
          }
      }
  }
  
  /**
   * @brief Example 1: Display scrolling characters
   */
  void Example_ScrollCharacters(void) {
      uint8_t idx = 0;
      
      while (1) {
          GN1640_Clear();
          
          // Display character on first digit
          GN1640_DisplayChar(0, GN1640_Font[idx].ch);
          GN1640_UpdateDisplay();
          
          DelayMs(500);
          
          idx++;
          if (idx >= GN1640_FontCount) {
              idx = 0;
          }
      }
  }
  
  /**
   * @brief Example 2: Display static text
   */
  void Example_DisplayText(void) {
      GN1640_Clear();
      
      // Display "HELLO" starting from digit 0
      GN1640_DisplayString(0, "HELLO");
      
      while (1) {
          // Do nothing - text stays on display
      }
  }
  
  /**
   * @brief Example 3: Display counter
   */
  void Example_Counter(void) {
      int16_t counter = 0;
      
      while (1) {
          GN1640_Clear();
          
          // Display counter value starting from digit 1
          GN1640_DisplayNumber(1, counter, 0);
          
          DelayMs(100);
          
          counter++;
          if (counter > 9999) {
              counter = 0;
          }
      }
  }
  
  /**
   * @brief Example 4: Display time format (HH:MM)
   */
  void Example_TimeDisplay(void) {
      uint8_t hours = 12;
      uint8_t minutes = 34;
      
      GN1640_Clear();
      
      // Display hours (digits 0-1)
      GN1640_DisplayChar(0, '0' + (hours / 10));
      GN1640_DisplayChar(1, '0' + (hours % 10));
      
      // Display colon (digit 2)
      GN1640_DisplayChar(2, ':');
      
      // Display minutes (digits 3-4)
      GN1640_DisplayChar(3, '0' + (minutes / 10));
      GN1640_DisplayChar(4, '0' + (minutes % 10));
      
      GN1640_UpdateDisplay();
      
      while (1) {
          // Update time here
      }
  }
  
  /**
   * @brief Example 5: Brightness control
   */
  void Example_BrightnessControl(void) {
      uint8_t brightness = 0;
      
      GN1640_DisplayString(0, "BRIGHT");
      
      while (1) {
          GN1640_SetBrightness(brightness);
          DelayMs(500);
          
          brightness++;
          if (brightness > BRIGHTNESS_MAX) {
              brightness = 0;
          }
      }
  }
  
  /**
   * @brief Example 6: Custom segment patterns
   */
  void Example_CustomPattern(void) {
      GN1640_Clear();
      
      // Turn on specific segments manually
      // Example: Create a custom pattern on digit 0
      uint16_t pattern = SEG(1) | SEG(5) | SEG(9) | SEG(13);
      GN1640_SetDigitSegments(0, pattern);
      
      GN1640_UpdateDisplay();
      
      while (1) {
          // Pattern stays on display
      }
  }
  
  /**
   * @brief Example 7: Scrolling text
   */
  void Example_ScrollingText(void) {
      const char* text = "HELLO WORLD    ";  // Add spaces for smooth scroll
      uint8_t offset = 0;
      uint8_t i;
      
      while (1) {
          GN1640_Clear();
          
          // Display 6 characters starting from offset
          for (i = 0; i < 6; i++) {
              uint8_t ch_idx = (offset + i) % 16;  // Wrap around
              GN1640_DisplayChar(i, text[ch_idx]);
          }
          
          GN1640_UpdateDisplay();
          DelayMs(300);
          
          offset++;
          if (offset >= 16) {
              offset = 0;
          }
      }
  }
  
  /**
   * @brief Main function - Select your example here
   */
  void main(void) {
      // Initialize system clock (16MHz HSI)
      CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
      
      // Initialize GN1640T driver
      GN1640_Init();
      
      // ============================================================
      // SELECT EXAMPLE TO RUN (uncomment one)
      // ============================================================
      
      // Example_ScrollCharacters();     // Example 1
      Example_DisplayText();       // Example 2
      // Example_Counter();           // Example 3
      // Example_TimeDisplay();       // Example 4
      // Example_BrightnessControl(); // Example 5
      // Example_CustomPattern();     // Example 6
      // Example_ScrollingText();     // Example 7
  }