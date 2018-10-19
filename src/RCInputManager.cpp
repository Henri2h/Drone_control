#include <unistd.h>
#include <cstdio>

#include <Navio2/RCInput_Navio2.h>
#include <Navio+/RCInput_Navio.h>
#include <Common/Util.h>
#include <memory>

#define READ_FAILED -1



class RCInputManager {
    private:
    
    std::unique_ptr <RCInput> get_rcin()
    {
        if (get_navio_version() == NAVIO2)
        {
            auto ptr = std::unique_ptr <RCInput>{ new RCInput_Navio2() };
            return ptr;
        } else
        {
            auto ptr = std::unique_ptr <RCInput>{ new RCInput_Navio() };
            return ptr;
        }

    }
    
    std::unique_ptr <RCInput> rcin = get_rcin();

    public:

    RCInputManager(){
        /*if (check_apm()) {
            return EXIT_FAILURE;
        }*/
        rcin = get_rcin();
        rcin->initialize();
    }

    int read(){
        int period = rcin->read(2);
        if (period == READ_FAILED)
            return EXIT_FAILURE;
        return period; 
    }
};