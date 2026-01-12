/**
 * ET6226M Display Test Program
 * MCU: STM8S003F3
 * I2C Pins: PB4 (SCL), PB5 (SDA)
 */

#include "stm8s.h"
#include "et6226m.h"

// Function prototypes
void CLK_Config(void);
void I2C_Config(void);
void Delay_ms(uint16_t ms);

int main(void)
{
    uint8_t counter = 0;
    uint8_t key;
    
    // Initialize system
    CLK_Config();
    I2C_Config();
    
    // Initialize ET6226M display
    ET6226M_Init();
    Delay_ms(100);
    
    // Show startup message
    ET6226M_DisplayString("888");
    Delay_ms(1000);
    
    ET6226M_Clear();
    Delay_ms(500);
    
    // Main loop
    while (1)
    {
        // Display counter (0-9)
        ET6226M_DisplayChar(0, '0' + (counter % 10));
        ET6226M_DisplayChar(1, 'A' + (counter % 26));
        ET6226M_DisplayChar(2, '-');
        
        counter++;
        if (counter >= 100) counter = 0;
        
        Delay_ms(500);
        
        // Check for key press
        key = ET6226M_GetKeyCode();
        if (key != 0) {
            // Display pressed key
            ET6226M_Clear();
            ET6226M_DisplayChar(1, key);
            Delay_ms(300);
        }
    }
}

/**
 * @brief Configure system clock (16 MHz internal RC)
 */
void CLK_Config(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
}

/**
 * @brief Configure I2C peripheral
 * PB4: SCL (I2C_SCL)
 * PB5: SDA (I2C_SDA)
 */
void I2C_Config(void)
{
    // Enable I2C clock
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, ENABLE);
    
    // Initialize I2C
    I2C_DeInit();
    
    I2C_Init(
        100000,                    // I2C clock speed: 100 kHz
        0x00,                      // Own address (not used in master mode)
        I2C_DUTYCYCLE_2,           // I2C duty cycle
        I2C_ACK_CURR,              // ACK current byte
        I2C_ADDMODE_7BIT,          // 7-bit addressing
        16                         // Input clock in MHz (HSI = 16 MHz)
    );
    
    // Enable I2C
    I2C_Cmd(ENABLE);
}

/**
 * @brief Simple delay function (approximately ms)
 * @param ms: Delay in milliseconds
 * @note Not accurate, just for demonstration
 */
void Delay_ms(uint16_t ms)
{
    uint16_t i;
    uint16_t j;
    
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 1600; j++) {  // Tuned for ~16MHz
            nop();
        }
    }
}
