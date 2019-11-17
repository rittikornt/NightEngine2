// Local Headers
#include "Core/World.hpp"

int main(int argc, char * argv[])
{
    World::Initialize();

    World::MainLoop();

    World::Terminate();
    
    return 0;
}