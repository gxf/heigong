#include <wv/wv.h>
//#include <gsf/gsf-infile.h>
//#include <gsf/gsf-infile-msole.h>
//#include <gsf/gsf-msole-utils.h>
//#include <gsf/gsf-docprop-vector.h>
//#include <gsf/gsf-meta-names.h>

#include <iostream>

// Forward decls. to wv's callbacks
static int charProc (wvParseStruct *ps, U16 eachchar, U8 chartype, U16 lid);
static int specCharProc (wvParseStruct *ps, U16 eachchar, CHP* achp);
static int eleProc (wvParseStruct *ps, wvTag tag, void *props, int dirty);
static int docProc (wvParseStruct *ps, wvTag tag);

// Register WV Callback functions
static void regWVCallBacks()
{
    wvSetElementHandler (&ps, eleProc);
    wvSetCharHandler (&ps, charProc);
    wvSetSpecialCharHandler(&ps, specCharProc);
    wvSetDocumentHandler (&ps, docProc);
}

static int charProc (wvParseStruct *ps, U16 eachchar, U8 chartype, U16 lid)
{
    std::cout << "Entering into char Proc" << std::endl;
}

static int specCharProc (wvParseStruct *ps, U16 eachchar, CHP* achp)
{
    std::cout << "Entering into specChar Proc" << std::endl;
}

static int eleProc (wvParseStruct *ps, wvTag tag, void *props, int dirty)
{
    std::cout << "Entering into ele Proc" << std::endl;
}

static int docProc (wvParseStruct *ps, wvTag tag)
{
    std::cout << "Entering into doc Proc" << std::endl;
}

int main(int argc, char** argv)
{
    GsfInput * fp;
    wvParseStruct ps;

    int ret = wvInitParser_gsf(&ps, fp);
    const char * password = NULL;


}
