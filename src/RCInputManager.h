#ifndef RCInputManager_H
#define RCInputManager_H

#include <unistd.h>
#include <cstdio>

#include <Navio2/RCInput_Navio2.h>
#include <Navio+/RCInput_Navio.h>
#include <Common/Util.h>
#include <memory>

#include "Ref.h"

#define READ_FAILED -1

class RCInputManager
{
  private:
    std::unique_ptr<RCInput> get_rcin();
    std::unique_ptr<RCInput> rcin = get_rcin();

  public:
    RCInputManager();
    int read(int commands[]);
};

#endif
