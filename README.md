# GN1640T 16-Segment LED Display Driver for STM8

A complete driver for controlling 16-segment LED displays using the GN1640T controller chip on STM8 microcontrollers.

## Features

- Support for 6 digits (3x FJ3291BH 2-digit 16-segment displays)
- Complete alphanumeric font (A-Z, 0-9, symbols)
- High-level API for easy text/number display
- Low-level API for custom segment control
- Brightness control (8 levels)
- Keypad integration example via UART protocol
- Optimized for STM8S003

## Hardware Setup

### Components
- **MCU:** STM8S003F3P6 (or compatible)
- **Display Controller:** GN1640T
- **Displays:** 3x FJ3291BH (16-segment, 2-digit displays)

### Pin Connections

| STM8 Pin | GN1640T Pin | Function |
|----------|-------------|----------|
| PB4      | CLK         | Clock    |
| PB5      | DATA        | Data     |

### GN1640T to Display Wiring

```
GN1640T GRID1-16  -> All 16 segments (shared across all displays)
GN1640T SEG1-2    -> Display 1 (Digits 1-2)
GN1640T SEG3-4    -> Display 2 (Digits 3-4)
GN1640T SEG5-6    -> Display 3 (Digits 5-6)
```

### 16-Segment Layout

```
                              - -
       1,2                |  \ | /  |
  8  09,10,11  3              _ _
      16 12               |  / | \  |
  7  15 14 13   4             - -
       6,5
```

## Project Structure

```
stm8-gn1640t-driver/
  gn1640t.c       - Driver implementation
  gn1640t.h       - Driver header
  main.c          - Examples 1-8 (including keypad integration)
  README.md       - This file
```

## Quick Start

### 1. Add Files to Your Project

Copy `gn1640t.c` and `gn1640t.h` to your STM8 project.

### 2. Configure Hardware Pins (if different)

Edit `gn1640t.h` if your pins differ:

```c
#define GN1640_CLK_PIN    GPIO_PIN_4  // Change if needed
#define GN1640_DATA_PIN   GPIO_PIN_5  // Change if needed
#define GN1640_PORT       GPIOB       // Change if needed
```

### 3. Initialize and Display

```c
#include "stm8s.h"
#include "gn1640t.h"

void main(void) {
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    GN1640_Init();

    GN1640_DisplayString(0, "HELLO");

    while (1) {}
}
```

## API Reference

### Core Functions

| Function | Description |
|----------|-------------|
| `GN1640_Init()` | Initialize driver and display. Call once at startup. |
| `GN1640_Clear()` | Clear display buffer and turn off all segments. |
| `GN1640_UpdateDisplay()` | Send buffer contents to display. Call after changes. |
| `GN1640_SetBrightness(brightness)` | Set brightness (0-7, where 7 is brightest). |
| `GN1640_SetDisplayState(state)` | Turn display on (1) or off (0). |

### High-Level Display Functions

| Function | Description |
|----------|-------------|
| `GN1640_DisplayChar(digit, ch)` | Display character at digit position (0-5). Returns 1 if found. |
| `GN1640_DisplayString(start, str)` | Display string starting from digit position. |
| `GN1640_DisplayNumber(start, num, zeros)` | Display number with optional leading zeros. |

### Low-Level Functions

| Function | Description |
|----------|-------------|
| `GN1640_SetDigitSegments(digit, mask)` | Set specific segments using 16-bit mask. |
| `GN1640_SetGrid(grid, seg_mask)` | Directly set a grid value in the buffer. |
| `GN1640_GetCharMask(ch, mask)` | Get segment pattern for a character. |

## Examples

### Static Text

```c
GN1640_Init();
GN1640_DisplayString(0, "STM8");
```

### Counter

```c
while (1) {
    GN1640_Clear();
    GN1640_DisplayNumber(1, counter++, 0);
    DelayMs(100);
}
```

### Time Display (HH:MM)

```c
GN1640_DisplayChar(0, '1');
GN1640_DisplayChar(1, '2');
GN1640_DisplayChar(2, ':');
GN1640_DisplayChar(3, '3');
GN1640_DisplayChar(4, '4');
GN1640_UpdateDisplay();
```

### Custom Segment Patterns

```c
uint16_t star = SEG(1) | SEG(5) | SEG(9) | SEG(11) | SEG(13) | SEG(15);
GN1640_SetDigitSegments(0, star);
GN1640_UpdateDisplay();
```

See `main.c` for all 8 runnable examples.

## Keypad Integration

This driver works with the [stm8-keypad-driver](https://github.com/Xurshidbek079/stm8-keypad-driver) for a two-MCU system where one MCU reads keypad input and sends it via UART to the display MCU.

### System Architecture

```
[Keypad MCU]  --UART (9600)-->  [Display MCU]
 STM8S003F3                      STM8S003F3
 ET6226M (I2C)                   GN1640T (GPIO)
 PB4=SCL, PB5=SDA               PB4=CLK, PB5=DATA
 PD5=TX  ----------------------> PD6=RX
```

### UART Protocol

Keypad presses are sent as packets: `[0xAA][0x01][0x01][key][XOR_CHK]`

| Key | Action |
|-----|--------|
| `'0'-'9'` | Display digit on screen |
| `'C'` | Clear display |
| `'B'` | Backspace |

### Display-Side Receiver

See `Example_KeypadDisplay()` in `main.c` (Example 8, commented out). It uses a state machine to parse the UART protocol and display typed characters.

To use it, you need:
- UART1 at 9600 baud with RX interrupt enabled
- A ring buffer UART driver providing `UART1_available()` and `UART1_getc()`

For the keypad MCU firmware, see: https://github.com/Xurshidbek079/stm8-keypad-driver

## Advanced

### Adjust Timing for Different Clocks

If not using 16MHz, adjust `gn1640_delay_us()` in `gn1640t.c`:

```c
// 16MHz: 4 nop() per microsecond
// 8MHz:  2 nop() per microsecond
```

### Extend Font Table

Add custom characters in `gn1640t.c`:

```c
const glyph_t GN1640_Font[] = {
    // ... existing characters ...
    {'&', SEG(1)|SEG(2)|SEG(5)|SEG(8)|SEG(11)|SEG(13)},
};
```

## Troubleshooting

| Problem | Check |
|---------|-------|
| Display not lighting up | Power supply, CLK/DATA wiring, `GN1640_Init()` called |
| Wrong characters | Segment wiring, anode/cathode type |
| Only some digits work | SEG1-6 connections to display common pins |
| Flickering | Increase delay_us, add decoupling caps |

## Compiler Notes

- Tested with COSMIC cxstm8 (C90 standard)
- All variable declarations at top of blocks
- Uses STM8S Standard Peripheral Library

## License

This project is open source and available for free use in commercial and non-commercial projects.
