// Local Headers
#include "World.hpp"

int main(int argc, char * argv[])
{
    World::Initialize();
    World::MainLoop();
    World::Terminate();

    return 0;
}