#include <Navio2/Led_Navio2.h>
#include <Navio+/Led_Navio.h>
#include <Common/Util.h>
#include <unistd.h>
#include <memory>

class LEDManager
{
  private:
    std::unique_ptr<Led> get_led()
    {
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
 
    int rate = 300;
    int mode = 0;
    int updateIter = 0;

  public:
    LEDManager()
    {
        if (!led->initialize())
        {
            std::cout << "[ LEDManager ] : Cannot initialize ...\n";
            throw EXIT_FAILURE;
        }
    }
    /* mode :
     0 : KO or black
     1 : OK
     2 : Arming
     3 : Armed
    */
    void update()
    {
        if (mode == 1 && updateIter == 1)
        {
            led->setColor(Colors::Green);
        }
        if (mode == 2 && updateIter == 1)
        {
            led->setColor(Colors::Cyan);
        }
        else if ((mode == 1 || mode == 2) && updateIter == rate)
        {
            led->setColor(Colors::Black);
        }
        else if (updateIter > 2*rate)
        {
            updateIter = 0;
        }

        updateIter++;
    }

    void setKO()
    {
        mode = 0;
        led->setColor(Colors::Red);
    }

    void setOK()
    {
        mode = 1; // blinking state
    }

    void setArmed()
    {
        mode = 3;
        led->setColor(Colors::Blue);
    }

    void setBlack()
    {
        mode = 0;
        led->setColor(Colors::Black);
    }

    void setArming()
    {
        mode = 2;  // blinking state
    }
};