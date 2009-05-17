#include "MayTwelfth.h"
#include <cstdlib>
#include <cstring>
#include <iostream>

int main(int argc, char** argv)
{
    // Argument check
    if (argc != 2){
        std::cout << std::endl
            << "Usage: " << std::endl
            << "    hgMaster <filename>" << std::endl
            << std::endl
            << " <filename> - .doc file to open." << std::endl
            << std::endl
            << std::endl;
        exit(0);
    }

    Logger logger;
    May12th may(&logger, argv[1]);
    may.MainLoop();

    return 0;
}
