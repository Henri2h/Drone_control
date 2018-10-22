#include <Navio2/Led_Navio2.h>
#include <Navio+/Led_Navio.h>
#include <Common/Util.h>
#include <unistd.h>
#include <memory>

class LED
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
    auto led = get_led();

  public:
    LED()
    {
        if (!led->initialize())
        {
            std::cout << "[ LED ] : Cannot initialize ...\n";
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
        led->setColor(Colors::Blue)
    }

    void setArming()
    {
        led->setColor(Colors::Cyan)
    }

    void setColor()
    {

        printf("LED is green\n");
        sleep(1);

        led->setColor(Colors::Cyan);
        printf("LED is cyan\n");
        sleep(1);

        led->setColor(Colors::Blue);
        printf("LED is blue\n");
        sleep(1);

        led->setColor(Colors::Magenta);
        printf("LED is magenta\n");
        sleep(1);

        led->setColor(Colors::Red);
        printf("LED is red\n");
        sleep(1);

        led->setColor(Colors::Yellow);
        printf("LED is yellow\n");
        sleep(1);
    }
};