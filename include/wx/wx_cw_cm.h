#define MSL_USE_PRECOMPILED_HEADERS	1

#if !defined( __MWERKS__ )
    #error "this file is only for builds with Metrowerks CodeWarrior"
#endif 

#define WX_COMP_INLINE_NO_CLASS // defined if the compiler does not want the classname repeated for inlines within a class definition

#if __MWERKS__ >= 0x2400
#pragma old_argmatch on
#endif

#if (__MWERKS__ < 0x0900) || macintosh
    #define __MAC__
    #define __WXMAC__
    #if TARGET_CARBON
	    #define	USE_PRECOMPILED_MAC_HEADERS	0  /*Set to 0 if you don't want to use precompiled MacHeaders*/
		#include "MacHeaders.c"
    	/*
	    #define	USE_PRECOMPILED_MAC_HEADERS	0 
	   		#ifdef __cplusplus
					#include <MacHeadersPPCX++>
			#else
					#include <MacHeadersPPCX>
			#endif
			*/
	#else
        #define	USE_PRECOMPILED_MAC_HEADERS	1  /*Set to 0 if you don't want to use precompiled MacHeaders*/		
    #endif
// automatically includes MacHeaders
#elif (__MWERKS__ >= 0x0900) && __INTEL__
    #define __WXMSW__
    #define __WINDOWS__
    #define __WIN95__
    #define STRICT
    #define NOMINMAX
#elif __BEOS__
    #include <ansi_prefix.be.h>
    #include <Be.h>
#else	
    #error unknown MW compiler
#endif

#ifdef __WXMSW__
    #include <ansi_prefix.win32.h>
    #include <ansi_parms.h>
    #ifdef __MWERKS__
    #if defined( __MSL__ ) && __MSL__ >= 0x5012
			#define	fileno	_fileno
			#define	fdopen	_fdopen
			#define	tell	_tell
    #endif
    #endif
#elif defined( __WXMAC__)
    #include <ansi_prefix.mac.h>
#endif

#define USE_DEFINE

// in order to avoid problems further down in wxWindows

#ifdef __cplusplus

extern "C" 
{
#endif
	char *strdup(const char *s) ;
	int	isascii( int c ) ;
#ifdef __cplusplus
} ;
#endif

