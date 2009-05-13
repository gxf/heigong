
#ifndef DOC_EXTRACTOR_H
#define DOC_EXTRACTOR_H

#include <fstream>

class docExtractor{
    public:
        docExtrator(const char* file);
        ~docExtrator();

    public:
        // For demo
        const char* GetString();
    private:
        const char*     filename;
        std::ifstream   filestream;
};

#endif
