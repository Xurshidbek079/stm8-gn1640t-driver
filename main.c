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
    GN1640_DisplayString(0, "HELLO");

    while (1) {
        /* Text stays on display */
    }
}

/**
 * @brief Example 3: Display counter
 */
void Example_Counter(void) {
    int16_t counter = 0;

    while (1) {
        GN1640_Clear();
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
    GN1640_DisplayChar(0, '0' + (hours / 10));
    GN1640_DisplayChar(1, '0' + (hours % 10));
    GN1640_DisplayChar(2, ':');
    GN1640_DisplayChar(3, '0' + (minutes / 10));
    GN1640_DisplayChar(4, '0' + (minutes % 10));
    GN1640_UpdateDisplay();

    while (1) {
        /* Update time here */
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
    uint16_t pattern;

    GN1640_Clear();

    pattern = SEG(1) | SEG(5) | SEG(9) | SEG(13);
    GN1640_SetDigitSegments(0, pattern);

    GN1640_UpdateDisplay();

    while (1) {
        /* Pattern stays on display */
    }
}

/**
 * @brief Example 7: Scrolling text
 */
void Example_ScrollingText(void) {
    const char* text = "HELLO WORLD    ";
    uint8_t offset = 0;
    uint8_t i;

    while (1) {
        GN1640_Clear();

        for (i = 0; i < 6; i++) {
            uint8_t ch_idx;
            ch_idx = (offset + i) % 16;
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

/* ==================================================================
 * Example 8: Keypad UART Receiver
 * ==================================================================
 * Receives keypad presses from a separate keypad MCU via UART.
 * Uses the stm8-keypad-driver protocol:
 *   https://github.com/Xurshidbek079/stm8-keypad-driver
 *
 * Protocol: [0xAA][TYPE][LEN][DATA...][XOR_CHK]
 * Keypad packet: [0xAA][0x01][0x01][key_char][CHK]
 *
 * Requires UART1 configured at 9600 baud with RX interrupt enabled.
 * Connect keypad MCU TX -> this MCU RX (PD6).
 *
 * Keys: '0'-'9' display on screen, 'C' = clear, 'B' = backspace
 *
 * NOTE: This example requires your own UART1 ring buffer driver.
 *       Replace UART1_available() and UART1_getc() with your
 *       UART implementation.
 * ================================================================== */
/*
void Example_KeypadDisplay(void) {
    uint8_t rx_state = 0;
    uint8_t rx_type = 0;
    uint8_t rx_len = 0;
    uint8_t rx_key = 0;
    uint8_t rx_chk = 0;
    uint8_t digit_pos = 0;
    uint16_t byte;

    GN1640_Clear();
    GN1640_DisplayString(0, "READY");

    while (1) {
        if (!UART1_available()) {
            continue;
        }

        byte = UART1_getc();

        switch (rx_state) {
            case 0:
                if (byte == 0xAA) {
                    rx_chk = 0xAA;
                    rx_state = 1;
                }
                break;

            case 1:
                rx_type = (uint8_t)byte;
                rx_chk ^= rx_type;
                rx_state = 2;
                break;

            case 2:
                rx_len = (uint8_t)byte;
                rx_chk ^= rx_len;
                rx_state = (rx_len == 0) ? 4 : 3;
                break;

            case 3:
                rx_key = (uint8_t)byte;
                rx_chk ^= rx_key;
                rx_state = 4;
                break;

            case 4:
                if ((uint8_t)byte == rx_chk && rx_type == 0x01) {
                    if (rx_key == 'C') {
                        GN1640_Clear();
                        GN1640_UpdateDisplay();
                        digit_pos = 0;
                    } else if (rx_key == 'B') {
                        if (digit_pos > 0) {
                            digit_pos--;
                            GN1640_DisplayChar(digit_pos, ' ');
                            GN1640_UpdateDisplay();
                        }
                    } else if (digit_pos < GN1640_DIGITS) {
                        GN1640_DisplayChar(digit_pos, rx_key);
                        GN1640_UpdateDisplay();
                        digit_pos++;
                    }
                }
                rx_state = 0;
                break;

            default:
                rx_state = 0;
                break;
        }
    }
}
*/

/**
 * @brief Main function - Select your example here
 */
void main(void) {
    /* Initialize system clock (16MHz HSI) */
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);

    /* Initialize GN1640T driver */
    GN1640_Init();

    /* ============================================================
     * SELECT EXAMPLE TO RUN (uncomment one)
     * ============================================================ */

    Example_ScrollCharacters();     /* Example 1 */
    /* Example_DisplayText();       */ /* Example 2 */
    /* Example_Counter();           */ /* Example 3 */
    /* Example_TimeDisplay();       */ /* Example 4 */
    /* Example_BrightnessControl(); */ /* Example 5 */
    /* Example_CustomPattern();     */ /* Example 6 */
    /* Example_ScrollingText();     */ /* Example 7 */
    /* Example_KeypadDisplay();     */ /* Example 8: UART Keypad -> GN1640T */
}
