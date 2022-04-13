#include "../include/Game.h"
#include <comutil.h>

int main(int argc, char** argv)
{
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    Game game;
    game.run();

    return 1;
}