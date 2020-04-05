#ifndef LEDManager_H
#define LEDManager_H

#include <Navio2/Led_Navio2.h>
#include <Navio+/Led_Navio.h>
#include <Common/Util.h>
#include <unistd.h>
#include <memory>
#include <iostream>

class LEDManager
{
  private:
    std::unique_ptr<Led> get_led();
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

    LEDManager();
    void update();
    void setKO();
    void setOK();
    void setArmed();
    void setBlack();
    void setArming();    
    void backToPrevious();// prevent deleting KO state
};

#endif
