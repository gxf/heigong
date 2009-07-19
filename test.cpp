#include "inc/hg.h"
#include <iostream>

int main(int argc, char** argv){
    // Argument check
    if (argc > 3 || argc < 2){
        std::cout << std::endl
            << "Usage: " << std::endl
            << "    hgTest <filename>" << std::endl
            << std::endl
            << " <filename> - .doc file to open." << std::endl
            << std::endl
            << std::endl;

        return 0;
    }
    hHgMaster hHG;
    if(!(hHG = HG_Init(argv[1], 600, 800))){
        std::cout << "Fail to init engine."
            << std::endl;
        return 0;
    }
    if(!(HG_StartParse(hHG))){
        std::cout << "Fail to start parsing." 
            << std::endl;
        return 0;
    }
    p_page_info pPage;
    uint32 page_num = 0;
    while(NULL != (pPage = HG_GetPage(hHG, page_num))){
        // Replace me to do whatever you want
        std::cout << "Got page " << page_num
            << std::endl;
        ++page_num;
        HG_FreePage(hHG, pPage);
        if (page_num > 5)
            break;
    }
    HG_Term(hHG);
    return 0;
}
