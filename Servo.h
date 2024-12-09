#pragma once

// Define the target MCU family here
// Uncomment the desired MCU family definition below:

// #define STM32F4
#define STM32F1
// #define STM32H7

// ##################################################################################
// Include libraries:

#if defined(STM32F1)
#include "stm32f1xx_hal.h"      // HAL library for STM32F1 series
#elif defined(STM32F4)
#include "stm32f4xx_hal.h"      // HAL library for STM32F4 series
#elif defined(STM32H7)
#include "stm32h7xx_hal.h"      // HAL library for STM32H7 series
#else
#error "Unsupported MCU family. Please define a valid target (e.g., STM32F1, STM32F4, STM32H7)."
#endif

#include <string>       // Include the standard string library for error handling and messages

// ##################################################################################


class Servo
{
public:

  /// @brief Stores the last error message encountered by the object.
  std::string errorMessage;

  struct ParametersStructure
  {

    TIM_HandleTypeDef *TIMER_HANDLE;

    float CLOCK_FREQUENCY;

    uint8_t CHANNEL_NUM;

    uint32_t MIN_PULSE_WIDTH;

    uint32_t MAX_PULSE_WIDTH;

    /// @brief Frequency of the timer in Hz.
    double PWM_Frequency;

  }parameters;

  Servo();

  bool init(void);

  bool start(void);

  bool stop(void);

  bool setPwmFrequency(uint32_t value);

  void setDutyCycle(float value);

  void write(uint32_t value);             

  void writeMicroseconds(uint32_t value); 

  uint32_t read(void);               

  uint32_t readMicroseconds(void);            

private:

  /**
   * @brief Specifies the period value to be loaded into the active 
   * Auto-Reload Register at the next update event. [us]
   */
  float _period;

  uint32_t _channelAddress;

  bool _checkParameters(void);
};




