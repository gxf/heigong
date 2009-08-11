#ifndef __INCLUDEIPADTYPES__
#define __INCLUDEIPADTYPES__

typedef int INT;
typedef char INT8;
typedef short int INT16;
typedef long INT32;
//typedef int INT32;
typedef unsigned int UINT;
typedef unsigned char UINT8;
typedef unsigned short int UINT16;
typedef unsigned int UINT32;
typedef short BOOL;
typedef int STATUS ;

#ifdef WIN32
	typedef unsigned __int64 UINT64;
	typedef __int64 INT64;
#else
	typedef unsigned long long UINT64;
	typedef long long INT64;
#endif // WIN32

#ifndef LOCAL
#define LOCAL  static
#endif
#ifndef FAST
#define FAST register
#endif

typedef int (*FUNCPTR)();
typedef void (* VOIDFUNCPTR)(); 

//#define NULL (void*)0
#define OK    0
#define ERROR (-1)  
#define FALSE 0
#define TRUE 1
#define EOF  (-1)


#ifndef NULL
#define NULL 0
#endif

#ifndef NONE
#define NONE 0
#endif

#endif
  
