/**
 * STM8S Configuration Header
 * Enable peripherals used in the project
 */

#ifndef __STM8S_CONF_H
#define __STM8S_CONF_H

// Uncomment the peripherals you're using
#define _GPIO  (1)
#define _CLK   (1)
#define _I2C   (1)

// Include peripheral headers
#include "stm8s_gpio.h"
#include "stm8s_clk.h"
#include "stm8s_i2c.h"

// Disable full assertions - define assert_param as empty macro
#ifdef USE_FULL_ASSERT
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif

#endif /* __STM8S_CONF_H */