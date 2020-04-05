#include "ServoManager.h"

std::unique_ptr<RCOutput> ServoManager::get_rcout()
{
    if (get_navio_version() == NAVIO2)
    {
        auto ptr = std::unique_ptr<RCOutput>{new RCOutput_Navio2()};
        return ptr;
    }
    else
    {
        auto ptr = std::unique_ptr<RCOutput>{new RCOutput_Navio()};
        return ptr;
    }
}

ServoManager::ServoManager()
{
    if (getuid())
    {
        fprintf(stderr, "Not root. Please launch like this: sudo \n");
        throw "Not root!";
    }

    for (size_t i = 0; i < 4; i++)
    {
        if (!(pwm->initialize(i)))
        {
            fprintf(stderr, "Error\n");
        }
    }
}

int ServoManager::initialize()
{
    for (size_t i = 0; i < 4; i++)
    {
        pwm->set_frequency(i, 50);

        if (!(pwm->enable(i)))
        {
            FileManagement::Log("Servo manager", "could not enable");
            return 1;
        }
    }
    return 0;
}

void ServoManager::setDuty(Data &data)
{

    for (size_t i = 0; i < 4; i++)
    {
        int a = data.motors_output[i];
        if (a < SERVO_MIN)
            a = SERVO_MIN;
        if (a > SERVO_MAX)
            a = SERVO_MAX;
        pwm->set_duty_cycle(i, a);
    }
}

void ServoManager::zero()
{

    for (size_t i = 0; i < 4; i++)
    {
        int a = 100;
        pwm->set_duty_cycle(i, a);
    }
}
