#include <Navio2/Led_Navio2.h>
#include <Navio+/Led_Navio.h>
#include <Common/Util.h>
#include <unistd.h>
#include <memory>

class LEDManager
{
  private:
    std::unique_ptr<Led> get_led(){
        if (get_navio_version() == NAVIO2)
        {
            auto ptr = std::unique_ptr<Led>{new Led_Navio2()};
            return ptr;
        }
        else
        {
            auto ptr = std::unique_ptr<Led>{new Led_Navio()};
            return ptr;
        }
    }
    std::unique_ptr<Led> led = get_led(); // save led 

  public:
    LEDManager(){
        if (!led->initialize())
        {
            std::cout << "[ LEDManager ] : Cannot initialize ...\n";
            throw EXIT_FAILURE;
        }
    }

    void setOK()
    {
        led->setColor(Colors::Green);
    }

    void setKO()
    {
        led->setColor(Colors::Red);
    }
    void setArmed()
    {
        led->setColor(Colors::Blue);
    }

    void setBlack(){
        led->setColor(Colors::Black);
    }

    void setArming()
    {
        led->setColor(Colors::Cyan);
    }

};