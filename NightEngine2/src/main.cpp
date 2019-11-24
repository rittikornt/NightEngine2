// Local Headers
#include "NightEngine2.hpp"

int main(int argc, char * argv[])
{
  NightEngine2::Engine* engine = new NightEngine2::Engine();
  {
    engine->Initialize();
    engine->MainLoop();
    engine->Terminate();
  }
  delete engine;

  return 0;
}