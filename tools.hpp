// -----------------------------------------------------------------------------
// file: tools.hpp -------------------------------------------------------------
// header file for the tools library in C++.                                  --
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
#ifndef TOOLSPP
#define TOOLSPP

#define NAME    "Stephen Fernandez"
#define CLASS   "ITS-MCA 2016"

#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

#include <stdio.h>     // for NULL    
#include <stdlib.h>    // for malloc() and calloc()     
#include <string.h>   
#include <math.h>      
#include <time.h>      // for time_t, time() and ctime()        
#include <ctype.h>     // for isspace() and isdigit() 
#include <limits.h>    // for INT_MAX    
#include <float.h>     // for DOUBLE_MAX, etc. 
#include <stdarg.h>    // for functions with variable # of arguments 
#include <sys/stat.h>  //Stat function
#include <sys/types.h>
#include <cstring>
#include <dirent.h>
#include <algorithm>
#include <sstream>
#include <regex>		//requires c++ 11


typedef char* cstring;
typedef const char* nstring;
typedef FILE* cstream;

// -----------------------------------------------------------------------------
// Macros for debugging.                                                      --
// -----------------------------------------------------------------------------
#define DUMPp(p) "\n" <<"    " #p " @ " <<&p <<"    value = " <<p <<"    " #p " --> " <<*p 
#define DUMPv(k) "\n" <<"    " #k " @ " <<&k <<"    value = " <<k 

// -----------------------------------------------------------------------------
// Routine screen and process management.                                     --
// -----------------------------------------------------------------------------
void     fbanner( ostream& fout );
void     banner();
void 	 delay( int );
void     say( cstring format, ... );
void     clearscreen( void );             
void     bye( void );                                    
void     hold( void );                                    
istream& flush( istream& is ); 		// Used in cin >> x >> flush;

// -----------------------------------------------------------------------------
// Error handling.															  --
// -----------------------------------------------------------------------------
void    fatal( const string format, ... );
void    cleanline( cstream ff );     

// -----------------------------------------------------------------------------
// Time and date.															  --
// -----------------------------------------------------------------------------
void     when( cstring date, cstring hour );
cstring  today( cstring date );
cstring  oclock( cstring hour );
const string currentDateTime();
const string sysResponse(string command); 
string findReplace(string source, string search, string replace);
#endif // TOOLSPP

