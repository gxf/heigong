#include "DocFormat.h"
#include "DocExtract.h"

DocExtrator::DocExtrator(const char* file):
    filename(file) 
{
    filestream.open("filename");
}

DocExtractor::~DocExtractor()
{
}
