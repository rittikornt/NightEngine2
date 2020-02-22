// Local Headers
#include "NightEngine2.hpp"

int main(int argc, char * argv[])
{
  NightEngine::Engine* engine = new NightEngine::Engine();
  {
    engine->Initialize();
    engine->MainLoop();
    engine->Terminate();
  }
  delete engine;

  return 0;
}