#include "inc/hg.h"
#include <iostream>
#include <unistd.h>
#include <sys/unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

static char * work_d = (char*)("/tmp/5/");
static char * html_d = NULL;
static const uint32 page_w = 600;
static const uint32 page_h = 800;
static uint32 total_page;

#define DPI_L   120
#define DPI_M   96
#define DPI_S   80

static unsigned char head_data[15] = {
	0x50, 0x35, 0x0A, 0x36, 0x30, 0x30, 0x20, 0x38, 0x30, 0x30, 0x0A, 0x32, 0x35, 0x35, 0x0A
};

static bool BasicRoutine(const char* filename, bool async, bool serialized, bool notdoc){
    bool render_only = false;
    if (true == notdoc){
        async = false;
        render_only = true;
    }
    hHgMaster hHG;
    if(!(hHG = HG_Init(filename, work_d, html_d, async, render_only, serialized, false, page_w, page_h, DPI_L))){
        std::cout << "Fail to init engine."
            << std::endl;
        return false;
    }
    if(!(HG_StartParse(hHG))){
        std::cout << "Fail to start parsing." << std::endl;
        HG_Term(hHG);
        return false;
    }
	
    p_page_info pPage;
    uint32 page_num = 0;

    char tmp[strlen(filename) + 1];
    memcpy(tmp, filename, strlen(filename) + 1);
    char* p = (char*)tmp + strlen(tmp);
    int i = strlen(tmp);
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

    char name[100];
    memset(name, 0, sizeof(name));
    sprintf(name, "%s-%05d.pgm", p, 0);
    printf("open file:%s\n",name);

    while(NULL != (pPage = HG_GetPage(hHG, page_num))){
		memset(name, 0, sizeof(name));
		sprintf(name, "%s_%05d.pgm", p, page_num);
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
    page_num--;     // This is the max page num
    if(!HG_Term(hHG)){
        std::cout << "Fail to term engine" << std::endl;
        return false; 
    }

    // 
    //
    // This time, use the info generated.
    //
    //
    serialized = true;
    render_only = false;
    if(!(hHG = HG_Init(filename, work_d, html_d, async, render_only, serialized, false, page_w, page_h, DPI_S))){
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
	
    p = ( char* )filename + strlen( filename );
    i = strlen(filename);
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
	sprintf(name, "%s-%05d.pgm", p, 0);
	printf("open file:%s\n",name);
    while(page_num >= 0 && (NULL != (pPage = HG_GetPage(hHG, page_num)))){
		memset(name, 0, sizeof(name));
		sprintf(name,"%s_%05d_restart.pgm",p,page_num);
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
        HG_FreePage(hHG, pPage);
        if (page_num == 0)
            break;
        else
            --page_num;
    }
    if(!HG_Term(hHG)){
        std::cout << "Fail to term engine" << std::endl;
        return false; 
    }
    return true;
}

static int FastPageRoutine(const char* filename, bool async, bool serialized, bool notdoc){
    bool render_only = false;
    if (true == notdoc){
        async = false;
        render_only = true;
    }
    hHgMaster hHG;
    if(!(hHG = HG_Init(filename, work_d, html_d, async, render_only, serialized, true, page_w, page_h, DPI_L))){
        std::cout << "Fail to init engine."
            << std::endl;
        return 0;
    }
    if(!(HG_StartParse(hHG))){
        std::cout << "Fail to start parsing." << std::endl;
        HG_Term(hHG);
        return 0;
    }
	
    total_page = HG_GetMaxPage(hHG);

    if(!HG_Term(hHG)){
        std::cout << "Fail to term engine" << std::endl;
        return 0; 
    }

    return total_page;
}

int main(int argc, char** argv){
    bool notdoc         = false;
    bool serialized     = false;

    // Argument check
    if (argc > 7 || argc < 2){
        std::cout << std::endl
            << "Usage: " << std::endl
//            << "    hgBin <filename> [-not-doc] [-render-only | -serialized] [-work-dir DIR]" << std::endl
            << "    hgBin <filename> [-not-doc] [-work-dir DIR] [-html-dir HDIR]" << std::endl
            << std::endl
            << " <filename> - file to open. DOC, HTML are supported." << std::endl
            << " [-not-doc] - file is not DOC. (it is html) " << std::endl
#if 0
            << " [-render-only] - Render only. " << std::endl
            << " [-serialized] - serialize information is prepared. " << std::endl
#endif
            << " [-work-dir DIR] - Set working directory in parameter." << std::endl
            << " [-html-dir DIR] - Set html root directory in parameter." << std::endl
            << std::endl
            << std::endl;

        return 0;
    }
    int i = 2;
    while (i < argc){
        if (0 == strcmp(argv[i], "-not-doc")){
            notdoc = true;
        }
        if (0 == strcmp(argv[i], "-render-only") || 
           (0 == strcmp(argv[i], "-serialized"))){
            serialized = true;
        }
        else if (0 == strcmp(argv[i], "-work-dir")){
            work_d = argv[i + 1];
        }
        else if (0 == strcmp(argv[i], "-html-dir")){
            html_d = argv[i + 1];
        }
        i++;
    }

    pid_t pid;
    int status;
    
    if ((pid = fork()) < 0){
        perror("fork failed");
        return 2;
    }

    else if (0 == pid){ //Child
        std::cout << "This is child" << std::endl;
        std::cout << "Total Page: " 
            << FastPageRoutine(argv[1], true, serialized, notdoc)
            << std::endl;
    }
    else{
        std::cout << "=========================" << std::endl;
        if(false == BasicRoutine(argv[1], true, serialized, notdoc)){
            std::cout << "Basic Routine fails." << std::endl;
            return 0;
        }
        else{
            std::cout << "Basic Routine passes." << std::endl;
        }
    }

    wait(&status);
    return 0;
}

