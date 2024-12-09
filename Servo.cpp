#include "Servo.h"


Servo::Servo()
{
    parameters.CHANNEL_NUM = 0;
    parameters.CLOCK_FREQUENCY = 0;
    parameters.MAX_PULSE_WIDTH = 0;
    parameters.MIN_PULSE_WIDTH = 0;
    parameters.PWM_Frequency = 0;
    parameters.TIMER_HANDLE = NULL;


    _channelAddress = 0;
    _period = 0;
}

bool Servo::init(void)
{
    if(_checkParameters() == false)
    {
        return false;
    }

    _period = (1000000.0 / parameters.PWM_Frequency);

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

    return true;
}

bool Servo::start(void)
{
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
    if(value == 0)
    {
        errorMessage = "Error Servo: PWM frequency can not be 0.";
        return false;
    }

    parameters.TIMER_HANDLE->Instance->PSC = (parameters.CLOCK_FREQUENCY / 1000000.0) - 1;

    parameters.TIMER_HANDLE->Instance->ARR = (1000000.0 / value) - 1;

    if (HAL_TIM_Base_Init(parameters.TIMER_HANDLE) != HAL_OK)
    {
        errorMessage = "Error Servo: HAL_TIM_Base_Init() is not succeeded.";
        return false;
    }

    parameters.PWM_Frequency = value;

    return true;
}

void Servo::write(uint32_t value)
{
    if(value > parameters.TIMER_HANDLE->Instance->ARR)
    {
        value = parameters.TIMER_HANDLE->Instance->ARR;
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

    __HAL_TIM_SET_COMPARE(parameters.TIMER_HANDLE, _channelAddress, value);
}

void Servo::writeMicroseconds(uint32_t value)
{
    write(value);
}

uint32_t Servo::read(void)
{
    return __HAL_TIM_GET_COMPARE(parameters.TIMER_HANDLE, _channelAddress);
}

uint32_t Servo::readMicroseconds(void)
{
    return read();
}

bool Servo::_checkParameters(void)
{
    bool state = (parameters.CHANNEL_NUM > 0) && (parameters.CLOCK_FREQUENCY > 0) && (parameters.TIMER_HANDLE != NULL) && (parameters.PWM_Frequency > 0);

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

    if( ((float)parameters.CLOCK_FREQUENCY / (float)(parameters.TIMER_HANDLE->Instance->PSC + 1)) != 1000000.0)
    {
        errorMessage = "Error Servo: Timer handle prescaler value not valid for CLOCK_FREQUENCY. The PWM resolution must be 1 microseconds.";
        return false;
    }

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

    float _frq = (float)parameters.CLOCK_FREQUENCY / (float)( (parameters.TIMER_HANDLE->Instance->ARR + 1) * (parameters.TIMER_HANDLE->Instance->PSC + 1) );

    if(_frq != parameters.PWM_Frequency)
    {
        errorMessage = "Error Servo: Timer frequency not matched with desired PWM_FREQUENCY value.";
        return false;
    }

    return true;
}