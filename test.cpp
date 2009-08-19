#include "inc/hg.h"
#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

static const char * work_dir = "/tmp/";

static bool TestStartTerm(const char* filename){
    hHgMaster hHG;
    uint32 i = 0;
    while (i < 5){
        if(!(hHG = HG_Init(filename, work_dir, false, 600, 800))){
            std::cout << "Fail to init engine at time." << i
                << std::endl;
            return false;
        }
	printf("start parse\n");
        if(!(HG_StartParse(hHG))){
            std::cout << "Fail to start parsing at time." << i
                << std::endl;
            HG_Term(hHG);
            return false;
        }
	printf("stop term\n");
        if(!HG_Term(hHG)){
            std::cout << "Fail to term engine at time." << i
                << std::endl;
            return false;
            
        }
        i++;
        usleep(2000);
    }
    return true;
}

static unsigned char head_data[15] = {
	0x50, 0x35, 0x0A, 0x36, 0x30, 0x30, 0x20, 0x38, 0x30, 0x30, 0x0A, 0x32, 0x35, 0x35, 0x0A
};
static bool TestBasicRoutine(const char* filename){
    hHgMaster hHG;
	char name[100];
    if(!(hHG = HG_Init(filename, work_dir, true, 600, 800))){
        std::cout << "Fail to init engine."
            << std::endl;
        return false;
    }
    if(!(HG_StartParse(hHG))){
        std::cout << "Fail to start parsing." 
            << std::endl;
        HG_Term(hHG);
        return false;
    }
	
    p_page_info pPage;
    uint32 page_num = 0;
    char* p = ( char* )filename + strlen( filename );
    int i = strlen(filename);
	FILE* fd;
	while(i--){
		if( *p == '/' ){
			p++;
			break;
		}
		if( *p == '.' ){
			*p = 0;
		}
		p--;
	}
	
	memset(name, 0, sizeof(name));
	sprintf(name, "%s-%05d.pgm", p,0);
	printf("open file:%s\n",name);
    while(NULL != (pPage = HG_GetPage(hHG, page_num))){
		memset(name, 0, sizeof(name));
		sprintf(name,"%s_%05d.pgm",p,page_num);
        // Replace me to do whatever you want
        std::cout << "***************************************Got page**************************************** " << page_num
            << std::endl;
		fd = fopen(name, "wb");
		if( fd == NULL ){
			printf("file open error:%s\n",name);
			break;
		}
		fwrite(head_data, 1, sizeof(head_data), fd);
		fwrite((char*)pPage->img, 1, pPage->height*pPage->width, fd);
		fclose(fd);
        ++page_num;
        HG_FreePage(hHG, pPage);
    }
    if(!HG_Term(hHG)){
        std::cout << "Fail to term engine" << std::endl;
        return false; 
    }
    return true;
}

static bool TestBookMarkRoutine(const char* filename){
    hHgMaster hHG;
    if(!(hHG = HG_Init(filename, work_dir, false, 600, 800))){
        std::cout << "Fail to init engine."
            << std::endl;
        return false;
    }
    if(!(HG_StartParse(hHG))){
        std::cout << "Fail to start parsing." 
            << std::endl;
        HG_Term(hHG);
        return false;
    }
    p_page_info pPage;
    uint32 page_num = 5;
    while(NULL != (pPage = HG_GetPage(hHG, page_num))){
        // Replace me to do whatever you want
        std::cout << "Got page " << page_num
            << std::endl;
        ++page_num;
        HG_FreePage(hHG, pPage);
        if (page_num > 10)
            break;
    }
    page_num = 4;
    while(NULL != (pPage = HG_GetPage(hHG, page_num))){
        // Replace me to do whatever you want
        std::cout << "Got page " << page_num
            << std::endl;
        --page_num;
        HG_FreePage(hHG, pPage);
        if (page_num == 0)
            break;
    }
    if(!HG_Term(hHG)){
        std::cout << "Fail to term engine" << std::endl;
        return false; 
    }
    return true;
}

static bool TestLargePageRangeRoutine(const char* filename){
    hHgMaster hHG;
    if(!(hHG = HG_Init(filename, work_dir, false, 600, 800))){
        std::cout << "Fail to init engine."
            << std::endl;
        return false;
    }
    if(!(HG_StartParse(hHG))){
        std::cout << "Fail to start parsing." 
            << std::endl;
        HG_Term(hHG);
        return false;
    }
    p_page_info pPage;
    uint32 page_nums[8] = {5, 1, 10, 2, 20, 3, 40, 4};
    uint32 i = 0;
    while(NULL != (pPage = HG_GetPage(hHG, page_nums[i]))){
        // Replace me to do whatever you want
        std::cout << "Got page " << page_nums[i]
            << std::endl;
        ++i;
        HG_FreePage(hHG, pPage);
    }
    if(!HG_Term(hHG)){
        std::cout << "Fail to term engine" << std::endl;
        return false; 
    }
    return true;
}

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

#if 0
    std::cout << "=========================" << std::endl;
    if(false == TestStartTerm(argv[1])){
        std::cout << "Start & Term test fails." << std::endl;
        return 0;
    }
    else{
        std::cout << "Start & Term test passes." << std::endl;
    }
    return 0;
#endif

    std::cout << "=========================" << std::endl;
    if(false == TestBasicRoutine(argv[1])){
        std::cout << "Basic Routine test fails." << std::endl;
        return 0;
    }
    else{
        std::cout << "Basic Routine test passes." << std::endl;
    }

#if 0
    std::cout << "=========================" << std::endl;
    if(false == TestBookMarkRoutine(argv[1])){
        std::cout << "Bookmark Routine test fails." << std::endl;
        return 0;
    }
    else{
        std::cout << "Bookmark Routine test passes." << std::endl;
    }

    std::cout << "=========================" << std::endl;
    if(false == TestLargePageRangeRoutine(argv[1])){
        std::cout << "Large Page Range Access Routine test fails." << std::endl;
        return 0;
    }
    else{
        std::cout << "Large Page Range Access Routine test passes." << std::endl;
    }
#endif

    return 0;
}

