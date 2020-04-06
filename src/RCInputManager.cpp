#include "RCInputManager.h"

std::unique_ptr<RCInput> RCInputManager::get_rcin()
{
    if (get_navio_version() == NAVIO2)
    {
        auto ptr = std::unique_ptr<RCInput>{new RCInput_Navio2()};
        return ptr;
    }
    else
    {
        auto ptr = std::unique_ptr<RCInput>{new RCInput_Navio()};
        return ptr;
    }
}

RCInputManager::RCInputManager()
{
    /*if (check_apm()) {
            return EXIT_FAILURE;
        }*/
    rcin = get_rcin();
    rcin->initialize();
}

int RCInputManager::read(int commands[])
{

    for (size_t i = 0; i < cmd_size; i++)
    {
        commands[i] = rcin->read(i);
        // reverse yaw command
        /* if(i == cmd_yaw){
                commands[i] = -commands[i];
            }*/
        if (commands[i] == READ_FAILED)
            return EXIT_FAILURE;
    }

    return 1;
}