# GN1640T 16-Segment LED Display Driver for STM8

A complete, production-ready driver for controlling 16-segment LED displays using the GN1640T controller chip on STM8 microcontrollers.

## 📋 Features

- ✅ Support for 6 digits (3× FJ3291BH 2-digit 16-segment displays)
- ✅ Complete alphanumeric font (A-Z, 0-9, symbols)
- ✅ High-level API for easy text/number display
- ✅ Low-level API for custom segment control
- ✅ Brightness control (8 levels)
- ✅ Optimized for STM8S003
- ✅ Well-documented and example-rich

## 🔌 Hardware Setup

### Components
- **MCU:** STM8S003F3P6 (or compatible)
- **Display Controller:** GN1640T
- **Displays:** 3× FJ3291BH (16-segment, 2-digit displays)

### Pin Connections

| STM8 Pin | GN1640T Pin | Function |
|----------|-------------|----------|
| PB4      | CLK         | Clock    |
| PB5      | DATA        | Data     |

### GN1640T to Display Wiring

```
GN1640T GRID1-16  → All 16 segments (shared across all displays)
GN1640T SEG1-2    → Display 1 (Digits 1-2)
GN1640T SEG3-4    → Display 2 (Digits 3-4)
GN1640T SEG5-6    → Display 3 (Digits 5-6)
```

### 16-Segment Layout

```
       1,2
  8  09,10,11  3
      16 12    
  7  15 14 13   4
       6,5
```

## 📁 Project Structure

```
your_project/
├── src/
│   ├── main.c          # Your application code (examples included)
│   └── gn1640t.c       # Driver implementation
├── inc/
│   └── gn1640t.h       # Driver header
└── README.md           # This file
```

## 🚀 Quick Start

### 1. Add Files to Your Project

Copy `gn1640t.c` and `gn1640t.h` to your STM8 project.

### 2. Configure Hardware Pins (if different)

Edit `gn1640t.h` if your pins differ:

```c
#define GN1640_CLK_PIN    GPIO_PIN_4  // Change if needed
#define GN1640_DATA_PIN   GPIO_PIN_5  // Change if needed
#define GN1640_PORT       GPIOB       // Change if needed
```

### 3. Initialize the Driver

```c
#include "stm8s.h"
#include "gn1640t.h"

void main(void) {
    // Initialize system clock
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    
    // Initialize display driver
    GN1640_Init();
    
    // Your code here...
}
```

### 4. Display Something!

```c
// Display text
GN1640_DisplayString(0, "HELLO");

// Display number
GN1640_DisplayNumber(1, 123, 0);  // Display "123" starting at digit 1

// Display single character
GN1640_DisplayChar(0, 'A');

// Update display to show changes
GN1640_UpdateDisplay();
```

## 📚 API Reference

### Core Functions

#### `void GN1640_Init(void)`
Initialize the driver and display. **Call once during startup.**

#### `void GN1640_Clear(void)`
Clear display buffer and turn off all segments.

#### `void GN1640_UpdateDisplay(void)`
Send buffer contents to display. **Call after making changes.**

#### `void GN1640_SetBrightness(uint8_t brightness)`
Set display brightness (0-7, where 7 is brightest).

### High-Level Display Functions

#### `uint8_t GN1640_DisplayChar(uint8_t digit, char ch)`
Display a character on a specific digit.
- **digit:** Position (0-5)
- **ch:** Character to display (A-Z, 0-9, :, =, -, +, space)
- **Returns:** 1 if successful, 0 if character not found

```c
GN1640_DisplayChar(0, 'A');  // Show 'A' on first digit
GN1640_UpdateDisplay();
```

#### `void GN1640_DisplayString(uint8_t start_digit, const char* str)`
Display a string starting from specified digit.

```c
GN1640_DisplayString(0, "HELLO");  // Show "HELLO" from digit 0
GN1640_UpdateDisplay();
```

#### `void GN1640_DisplayNumber(uint8_t start_digit, int16_t number, uint8_t leading_zeros)`
Display a number.
- **start_digit:** Starting position
- **number:** Number to display (-999 to 9999)
- **leading_zeros:** 1 = show leading zeros, 0 = suppress

```c
GN1640_DisplayNumber(1, 42, 0);    // Show "42"
GN1640_DisplayNumber(1, 42, 1);    // Show "00042"
GN1640_UpdateDisplay();
```

### Low-Level Functions

#### `void GN1640_SetDigitSegments(uint8_t digit, uint16_t segment_mask)`
Set specific segments on a digit using a 16-bit mask.

```c
// Turn on segments 1, 5, 9, 13 on digit 0
uint16_t pattern = SEG(1) | SEG(5) | SEG(9) | SEG(13);
GN1640_SetDigitSegments(0, pattern);
GN1640_UpdateDisplay();
```

#### `void GN1640_SetGrid(uint8_t grid, uint8_t seg_mask)`
Directly set a grid (segment) value in the buffer.

#### `uint8_t GN1640_GetCharMask(char ch, uint16_t* mask)`
Get segment pattern for a character from font table.

## 💡 Usage Examples

### Example 1: Display Static Text

```c
void main(void) {
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    GN1640_Init();
    
    GN1640_Clear();
    GN1640_DisplayString(0, "STM8");
    
    while(1) {
        // Text stays on display
    }
}
```

### Example 2: Counter

```c
void main(void) {
    int16_t counter = 0;
    
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    GN1640_Init();
    
    while(1) {
        GN1640_Clear();
        GN1640_DisplayNumber(1, counter, 0);
        
        delay_ms(100);  // Your delay function
        
        counter++;
        if (counter > 999) counter = 0;
    }
}
```

### Example 3: Time Display (HH:MM)

```c
void DisplayTime(uint8_t hours, uint8_t minutes) {
    GN1640_Clear();
    
    // Hours
    GN1640_DisplayChar(0, '0' + (hours / 10));
    GN1640_DisplayChar(1, '0' + (hours % 10));
    
    // Colon
    GN1640_DisplayChar(2, ':');
    
    // Minutes
    GN1640_DisplayChar(3, '0' + (minutes / 10));
    GN1640_DisplayChar(4, '0' + (minutes % 10));
    
    GN1640_UpdateDisplay();
}

void main(void) {
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    GN1640_Init();
    
    DisplayTime(12, 34);  // Show 12:34
    
    while(1) {
        // Update time periodically
    }
}
```

### Example 4: Scrolling Text

```c
void main(void) {
    const char* text = "HELLO WORLD    ";
    uint8_t offset = 0;
    uint8_t i;
    
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    GN1640_Init();
    
    while(1) {
        GN1640_Clear();
        
        for (i = 0; i < 6; i++) {
            GN1640_DisplayChar(i, text[(offset + i) % 16]);
        }
        
        GN1640_UpdateDisplay();
        delay_ms(300);
        
        offset++;
        if (offset >= 16) offset = 0;
    }
}
```

### Example 5: Custom Segment Patterns

```c
void main(void) {
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    GN1640_Init();
    
    GN1640_Clear();
    
    // Create custom pattern (star shape)
    uint16_t star = SEG(1) | SEG(5) | SEG(9) | SEG(11) | SEG(13) | SEG(15);
    GN1640_SetDigitSegments(0, star);
    
    GN1640_UpdateDisplay();
    
    while(1) {}
}
```

## ⚙️ Advanced Configuration

### Adjust Timing for Different Clock Speeds

If using a clock frequency other than 16MHz, adjust the `delay_us()` function in `gn1640t.c`:

```c
void delay_us(uint16_t us) {
    uint16_t i;
    // Adjust loop iterations based on your clock:
    // 16MHz: 4 nop() per microsecond
    // 8MHz:  2 nop() per microsecond
    // 2MHz:  1 nop() per 2 microseconds
    for (i = 0; i < us; i++) {
        nop();
        nop();
        nop();
        nop();
    }
}
```

### Extend Font Table

Add custom characters to the font table in `gn1640t.c`:

```c
const glyph_t GN1640_Font[] = {
    // ... existing characters ...
    
    // Add your custom characters
    {'&', SEG(1)|SEG(2)|SEG(5)|SEG(8)|SEG(11)|SEG(13)},
    {'*', SEG(9)|SEG(10)|SEG(11)|SEG(12)|SEG(13)|SEG(14)|SEG(15)|SEG(16)},
};
```

## 🐛 Troubleshooting

### Display not lighting up
1. Check power supply to displays and GN1640T
2. Verify pin connections (CLK, DATA)
3. Check that `GN1640_Init()` is called
4. Ensure `GN1640_UpdateDisplay()` is called after setting content

### Wrong characters displayed
1. Verify segment wiring matches your display pinout
2. Check if your display uses common anode or cathode
3. Review segment bit mapping in hardware setup

### Flickering display
1. Increase delay in `delay_us()` function
2. Add decoupling capacitors near displays
3. Check power supply stability

### Only some digits work
1. Verify SEG1-6 connections to display common pins
2. Check that all displays share GRID1-16 connections
3. Test each display individually

## 📝 License

This project is open source and available for free use in commercial and non-commercial projects.

## 🤝 Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

## 📧 Support

For questions or issues, please open an issue on GitHub.

---

**Made with ❤️ for the STM8 community**
