#include "Servo.h"


Servo::Servo()
{
    parameters.CHANNEL_NUM = 0;
    parameters.CLOCK_FREQUENCY = 0;
    parameters.MAX_PULSE_WIDTH = 0;
    parameters.MIN_PULSE_WIDTH = 0;
    parameters.PWM_Frequency = 50;
    parameters.TIMER_HANDLE = NULL;


    _InitFlag = false;
    _channelAddress = 0;
    _period = 0;
    _resolution = 0;
}

bool Servo::init(void)
{
    if(setPwmFrequency(parameters.PWM_Frequency) == false)
    {
        return false;
    }

    if(_checkParameters() == false)
    {
        return false;
    }

    _period = (1000000.0 / (float)parameters.PWM_Frequency);

    switch(parameters.CHANNEL_NUM)
    {
        case 1:
            _channelAddress = TIM_CHANNEL_1;
        break;
        case 2:
            _channelAddress = TIM_CHANNEL_2;
        break;
        case 3:
            _channelAddress = TIM_CHANNEL_3;
        break;
        case 4:
            _channelAddress = TIM_CHANNEL_4;
        break;
        default:
            errorMessage = "Error Servo: Channel number is not valid.";
            return false;
    }

    _InitFlag = true;
    return true;
}

bool Servo::start(void)
{
    if(_InitFlag == false)
    {
        errorMessage = "Error Servo: start() is not succeeded.";
        return false;
    }

    if(HAL_TIM_PWM_Start(parameters.TIMER_HANDLE, _channelAddress) != HAL_OK)
    {
        errorMessage = "Error Servo: HAL_TIM_PWM_Start() is not succeeded.";
        return false;
    }

    write(0);

    return true;
}

bool Servo::stop(void)
{
    if(_InitFlag == false)
    {
        errorMessage = "Error Servo: start() is not succeeded.";
        return false;
    }

    write(0);

    if(HAL_TIM_PWM_Stop(parameters.TIMER_HANDLE, _channelAddress) != HAL_OK)
    {
        errorMessage = "Error Servo: HAL_TIM_PWM_Stop() is not succeeded.";
        return false;
    }

    return true;   
}

bool Servo::setPwmFrequency(uint32_t value)
{
    bool state = ((parameters.PWM_Frequency == 50) || (parameters.PWM_Frequency == 100) || 
                 (parameters.PWM_Frequency == 200) || (parameters.PWM_Frequency == 400));

    if(state == false)
    {
        errorMessage = "Error Servo: PWM frequency is not correct value. Please select a value of 50, 100, 200 or 400.";
        return false;
    }

    parameters.TIMER_HANDLE->Instance->PSC = (parameters.CLOCK_FREQUENCY / 1000000.0) - 1;

    parameters.TIMER_HANDLE->Instance->ARR = (1000000.0 / value) - 1;

    parameters.PWM_Frequency = value;

    _period = (1000000.0 / (float)parameters.PWM_Frequency);
    _resolution = _period / (float)parameters.TIMER_HANDLE->Instance->ARR;

    return true;
}

void Servo::write(uint32_t value)
{
    if(_InitFlag == false)
    {
        return;
    }

    if(value > (parameters.TIMER_HANDLE->Instance->ARR + 1))
    {
        value = (parameters.TIMER_HANDLE->Instance->ARR + 1);
    }

    if(parameters.MAX_PULSE_WIDTH > 0)
    {
        if(value > parameters.MAX_PULSE_WIDTH)
        {
            value = parameters.MAX_PULSE_WIDTH;
        }
    }

    if(parameters.MIN_PULSE_WIDTH > 0)
    {
        if(value < parameters.MIN_PULSE_WIDTH)
        {
            value = parameters.MIN_PULSE_WIDTH;
        }
    }

    if(value != 0)
    {
        value = value - 1;
    }
    
    __HAL_TIM_SET_COMPARE(parameters.TIMER_HANDLE, _channelAddress, value);
}

// void Servo::writeMicroseconds(uint32_t value)
// {
//     uint32_t temp = value / _resolution;
//     write(temp);
// }

uint32_t Servo::read(void)
{
    if(_InitFlag == false)
    {
        return 0;
    }

    return (__HAL_TIM_GET_COMPARE(parameters.TIMER_HANDLE, _channelAddress) + 1);
}

// uint32_t Servo::readMicroseconds(void)
// {
//     return read();
// }

bool Servo::_checkParameters(void)
{
    bool state = (parameters.CHANNEL_NUM >= 1) && (parameters.CHANNEL_NUM <= 4) &&
                 (parameters.CLOCK_FREQUENCY > 0) && 
                 (parameters.TIMER_HANDLE != NULL) && 
                 ((parameters.PWM_Frequency == 50) || (parameters.PWM_Frequency == 100) || 
                 (parameters.PWM_Frequency == 200) || (parameters.PWM_Frequency == 400));

    if(state == false)
    {
        errorMessage = "Error Servo: One or some parameters are not correct.";
        return false;
    }

    if(parameters.MAX_PULSE_WIDTH > 0)
    {
        if(parameters.MAX_PULSE_WIDTH <= parameters.MIN_PULSE_WIDTH)
        {
            errorMessage = "Error Servo:- MAX_PULSE_WIDTH can not be smaller or equal MIN_PULS_WIDTH.";
            return false;
        }
    }

    // _resolution = _period / (float)parameters.TIMER_HANDLE->Instance->ARR;
    
    // if( ((float)parameters.CLOCK_FREQUENCY / (float)(parameters.TIMER_HANDLE->Instance->PSC + 1)) != 1000000.0)
    // {
    //     errorMessage = "Error Servo: Timer handle prescaler value not valid for CLOCK_FREQUENCY. The PWM resolution must be 1 microseconds.";
    //     return false;
    // }

    if(parameters.MIN_PULSE_WIDTH > 0)
    {
        if(parameters.MIN_PULSE_WIDTH >= (parameters.TIMER_HANDLE->Instance->ARR + 1))
        {
            errorMessage = "Error Servo: Timer handle auto reload value can not be less than MIN_PULSE_WIDTH.";
            return false;
        }
    }

    if(parameters.MAX_PULSE_WIDTH > 0)
    {
        if(parameters.MAX_PULSE_WIDTH >= (parameters.TIMER_HANDLE->Instance->ARR + 1))
        {
            errorMessage = "Error Servo: Timer handle auto reload value can not be less than MAX_PULSE_WIDTH.";
            return false;
        }
    }

    // float _frq = (float)parameters.CLOCK_FREQUENCY / (float)( (parameters.TIMER_HANDLE->Instance->ARR + 1) * (parameters.TIMER_HANDLE->Instance->PSC + 1) );

    // if(_frq != parameters.PWM_Frequency)
    // {
    //     errorMessage = "Error Servo: Timer frequency not matched with desired PWM_FREQUENCY value.";
    //     return false;
    // }

    return true;
}