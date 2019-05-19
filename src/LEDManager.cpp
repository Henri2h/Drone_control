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

  /* mode :
     0 : KO or black
     1 : OK
     2 : Arming
     3 : Armed
    */

    int updateIter = 0;

  public:
    LEDManager()
    {
        if (!led->initialize())
        {
            std::cout << "[ LEDManager ] : Cannot initialize ...\n";
            throw EXIT_FAILURE;
        }
        setKO();
        std::cout << "[ LEDManager ] : Starting ...\n";
    }
  
    void update()
    {
        if (mode == 1 && updateIter == 0) // mode ok and start of the loop
        {
            led->setColor(Colors::Green);
        }

        if (mode == 2 && updateIter == 0) // mode arming and start of the loop
        {
            led->setColor(Colors::Cyan);
        }
        if ((mode == 1 || mode == 2) && updateIter == rate)
        {
            led->setColor(Colors::Black);
        }

        updateIter++;

        if (updateIter > 2 * rate)
        {
            updateIter = 0;
        }
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
        mode = 2; // blinking state
    }
    
    void backToPrevious(){ // prevent deleting KO state
        if(mode > 0){
            mode = 1;
        }
    }
};