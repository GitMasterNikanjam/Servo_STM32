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
// Defin Global Macros:


// ######################################################################################
// Define Classes:

/**
 * @class Servo
 * @brief The class for advanced PWM generation by timers.
 * @note PWM signal can set with 1 microseconds resolution.
 */
class Servo
{
public:

  /// @brief Stores the last error message encountered by the object.
  std::string errorMessage;

  /**
   * @struct ParametersStructure
   * @brief Parameters structure.
   * @note Any changes to these parameters require Servo object re-initialization afterward.
   */
  struct ParametersStructure
  {
    /**
     * @brief Hal timer handle pointer. eg: htim1, htim2, ...
     * @note - Set this timer handler carefully because the Servo object generate PWM by this handler.
     * 
     * @note - The Timer handler must be configured and initialized before and outside of the Servo class.
     * 
     * @note - The PWM frequency can be set dynamically using the setPwmFrequency() method.
     */
    TIM_HandleTypeDef *TIMER_HANDLE;

    /**
     * @brief The Timer base clock frequency. [Hz]
     * @note This value must equal the specified peripheral clock frequency; otherwise, the Servo object will not generate the correct PWM signal.
     */
    uint32_t CLOCK_FREQUENCY;

    /**
     * @brief Timer channel number for PWM output. this value can be: 1, 2, 3 or 4.
     */
    uint8_t CHANNEL_NUM;

    /**
     * @brief Minimum allowable PWM active pulse width. [us]
     * @note A value of 0 means it is disabled.
     */
    uint32_t MIN_PULSE_WIDTH;

    /**
     * @brief Maximum allowable PWM active pulse width. [us]
     * @note A value of 0 means it is disabled.
     */
    uint32_t MAX_PULSE_WIDTH;

    /**
     * @brief The desired PWM Frequency. [Hz]
     * @note - Its value is acceptable between: 50, 100, 200, 400.
     * 
     * @note - Default value is 50 Hz.
     *  */ 
    uint32_t PWM_Frequency;

  }parameters;

  /**
   * @brief Constructor. Init some variables and parameters.
   */
  Servo();

  /**
   * @brief Init Servo object. check parameters validation. Init some variables.
   * @return true if succeeded.
   * @note This method automatically sets the timer prescaler register and auto-reload register. It is also possible to change their previously set values.
   * @warning The Timer handler must be initialized before Servo.init(). Otherwise, PWM generation may not work correctly.
   */
  bool init(void);

  /**
   * @brief Start PWM generation at zero value.
   * @return true if succeeded.
   */
  bool start(void);

  /**
   * @brief Set PWM to zero and stop pwm generation.
   * @return true if succeeded.
   */
  bool stop(void);

  /**
   * @brief Set PWM frequency. [Hz]
   * @note - It is possible that set pwm frequency after/before init() or start().
   * 
   * @note - Its value is acceptable between: 50, 100, 200, 400.
   * 
   * @note - Default value is 50 Hz.
   * @return true if succeeded.
   */
  bool setPwmFrequency(uint32_t value = 50);

  /**
   * @brief Set Current PWM pulse width. [us]
   * @param value: PWM pulse width value. [us]
   */
  void write(uint32_t value);             

  /**
   * @brief Get Current PWM pulse width. [us]
   */
  uint32_t read(void);                         

private:

  /**
   * @brief PWM period time. [us]
   * @note It is calculated from 1/PWM_FREQUENCY.
   */
  float _period;

  /**
   * @brief PWM signal control resolution. [us]
   */
  float _resolution;

  /**
   * @brief Timer channel address value for Servo object. eg: TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3 or TIM_CHANNEL_4
   */
  uint32_t _channelAddress;

  /// @brief The init flag is true if initialization succeeded. If it is false, the Servo object needs to be initialized correctly.
  bool _InitFlag;

  /**
   * @brief check parameters validation.
   * @return true if succeeded.
   */
  bool _checkParameters(void);
};




