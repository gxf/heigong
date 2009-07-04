#include "MayTwelfth.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>

int main(int argc, char** argv)
{
    bool convert = false; // doesn't need to convert file format using wvWare
    // Argument check
    if (argc > 3 || argc < 2){
        std::cout << std::endl
            << "Usage: " << std::endl
            << "    hgMaster <filename>" << std::endl
            << std::endl
            << " <filename> - .doc file to open." << std::endl
            << std::endl
            << std::endl;
        exit(0);
    }

    char *ext = strrchr(argv[1], '.');
    if (strcmp(ext, ".doc") == 0)
        convert = true;
    
    if (argc == 2){ 
        Logger logger;
        May12th may(&logger, argv[1], convert);
        may.MainLoop();
    }
    else if (argc == 3){
        int page = std::atoi(argv[2]);
        Logger logger;
        May12th may(&logger, argv[1], convert);
        may.RenderAll();
        may.Display(page);
        //usleep(20 * 1e6);
    }

    return 0;
}
