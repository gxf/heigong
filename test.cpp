#include "inc/hg.h"
#include <iostream>
#include <unistd.h>
#include <sys/unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

static char * work_d = (char*)("/tmp/");
static char * html_d = NULL;
static const uint32 page_w = 600;
//static const uint32 page_h = 800;
static const uint32 page_h = 770;
static uint32 total_page;
static bool g_start = false;
static int pipefd[2];

#define DPI_L   120
#define DPI_M   96
#if 0
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
#endif

static unsigned char head_data[17] = {
	0x50, 0x35, 0x0A, 0x30, 0x36, 0x30, 0x30, 0x20, 0x30, 0x38, 0x30, 0x30, 0x0A, 0x32, 0x35, 0x35, 0x0A
};

static bool BasicRoutine(const char* filename, bool async, bool serialized, bool notdoc){
    bool render_only = false;
    hHgMaster hHG;
	unsigned int w,h;
	char name[100];
	
    if (true == notdoc){
        async = false;
        render_only = true;
    }
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

	if( page_w >= 10000 || page_h >= 10000 ){
		return false;
	}
	w = page_w;
	h = page_h;
	head_data[3] = w / 1000 + '0';
	w %= 1000;
	head_data[4] = w / 100 + '0';
	w %= 100;
	head_data[5] = w / 10 + '0';
	w %= 10;
	head_data[6] = w + '0';

	head_data[8] = h / 1000 + '0';
	h %= 1000;
	head_data[9] = h / 100 + '0';
	h %= 100;
	head_data[10] = h / 10 + '0';
	h %= 10;
	head_data[11] = h + '0';
	
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
    page_num--;     // This is the max page num
    if(!HG_Term(hHG)){
        std::cout << "Fail to term engine" << std::endl;
        return false; 
    }

	printf("now use the old inof\n");
	getchar();

    // 
    //
    // This time, use the info generated.
    //
    //
    serialized = true;
    render_only = false;
    if (true == notdoc){
        async = false;
        render_only = true;
    }
    if(!(hHG = HG_Init(filename, work_d, html_d, async, render_only, serialized, false, page_w, page_h, DPI_L))){
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
	sprintf(name, "%s-%05d.pgm", p,0);
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
        --page_num;
        HG_FreePage(hHG, pPage);
    }
    if(!HG_Term(hHG)){
        std::cout << "Fail to term engine" << std::endl;
        return false; 
    }
    return true;
}

#if 0
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
#endif

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
	
    write(pipefd[1], "0", 1);

    total_page = HG_GetMaxPage(hHG);

    if(!HG_Term(hHG)){
        std::cout << "Fail to term engine" << std::endl;
        return 0; 
    }

    return total_page;
}

static bool PgBasedRoutine(const char* filename, uint32 pg_num){
    hHgMaster hHG;
    if(!(hHG = HG_PB_Init(filename, html_d, work_d, page_w, page_h, DPI_L))){
        std::cout << "Fail to init engine."
            << std::endl;
        return false;
    }
    if(!(HG_PB_StartParse(hHG))){
        std::cout << "Fail to start parsing." << std::endl;
        HG_Term(hHG);
        return false;
    }
	
    if(!(HG_PB_SetPage(hHG, pg_num))){
        std::cout << "Fail to set page for parsing." << std::endl;
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
    sprintf(name, "%s-%d-%05d.pgm", p, pg_num, 0);
    printf("open file:%s\n",name);

    while(NULL != (pPage = HG_PB_GetReRenderedPage(hHG, page_num))){
		memset(name, 0, sizeof(name));
		sprintf(name, "%s_%u_%05d.pgm", p, pg_num, page_num);
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
        HG_PB_FreePage(hHG, pPage);
    }
    page_num--;     // This is the max page num
    if(!HG_PB_Term(hHG)){
        std::cout << "Fail to term engine" << std::endl;
        return false; 
    }
    return true;
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

#if 0
        std::cout << "=========================" << std::endl;
        if(false == BasicRoutine(argv[1], true, serialized, notdoc)){
            std::cout << "Basic Routine fails." << std::endl;
            return 0;
        }
        else{
            std::cout << "Basic Routine passes." << std::endl;
        }
#endif
//	printf("|||||||||||||||||||||||||||||||||\\\\\\\\\\\\\\\\\%d\n", FastPageRoutine(argv[1], true, serialized, notdoc) );
    
    char * filen = new char[strlen(argv[1]) + 1];
    std::strcpy(filen, argv[1]);

#if 0
    if ((pid = fork()) < 0){
        perror("fork failed");
        return 2;
    }
    else if (0 == pid){ //Child
        std::cout << "This is child" << std::endl;
        std::cout << "Total Page: " 
            << FastPageRoutine(argv[1], true, serialized, notdoc) << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
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
#else
//#if 0
    if (pipe(pipefd) < 0){
        perror("pipe fails.");
        return 7;
    }
    if ((pid = fork()) < 0){
        perror("fork failed");
        return 2;
    }
    else if (0 == pid){ //Child
        close(pipefd[0]);
        std::cout << "This is child" << std::endl;
        std::cout << "Total Page: " 
            << FastPageRoutine(argv[1], true, serialized, notdoc) << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
            << std::endl;
    }
    else{
        close(pipefd[1]);
        FILE* fd;
        char cmd[200];
        int buf;
        int page_num = 0;
        while(page_num < 5){
            memset(cmd,0,200);
            sprintf(cmd,"%s%d.pg",work_d, page_num);
            printf("try to get:%s----------------------------------------------------------------\n",cmd);
            if(( fd = fopen(cmd, "rb"))){
                fread(&buf,1,4,fd);
                if(buf!=(~0)){
                    fclose(fd);
                    usleep(1000*10);
                    continue;
                }
                printf("get a pg file ok:%d\n",page_num);
                char buf[10];

                read(pipefd[0], buf, 1);
                if (notdoc == false){
                    html_d = work_d;
                    PgBasedRoutine("tmp.hg", page_num);
                }
                else{
                    PgBasedRoutine(filen, page_num);
                }
                page_num++;
            }
            usleep(1000*10);
        }
    }
#endif
//#endif
    wait(&status);
//    PgBasedRoutine("/tmp/tmp.hg", 0);
#if 0
        if(false == BasicRoutine(filen, true, serialized, notdoc)){
            std::cout << "Basic Routine fails." << std::endl;
            return 0;
        }
        else{
            std::cout << "Basic Routine passes." << std::endl;
        }
#endif
    delete [] filen;
    return 0;
}

