#ifndef HTML_FILTER
#define HTML_FILTER

#include "Logger.h"
#include <stdio.h>

class HtmlFilter{
    public:
        HtmlFilter(Logger * log):
            logger(log)
        {}
        ~HtmlFilter();

//        void GetNextDrawable(FILE* fd);

    private:
        Logger* logger;
};

#endif
