#ifdef __UNIX__
#  include <Carbon.r>
#else
#  include <Types.r>
#endif
#include "apprsrc.h"

resource 'STR#' ( 128 , "Simple Alert Messages" )
{
	{
	"This application needs at least a MacPlus" ,
	"This application needs more memory" ,
	"This application is out of memory" ,
	"This application needs at least System 8.6" ,
	"About this wxWindows Application" ,
	"This application needs Appearance extension (built in with System 8) - this restriction will be relieved in the final release"
	}
} ;

resource 'MENU' (1, preload)
{
	1, textMenuProc, 0b11111111111111111111111111111110 , enabled, apple ,
	{
		"About…" , noicon, nokey,nomark,plain ,
		"-" , noicon, nokey,nomark,plain
	}
} ;

resource 'MBAR' (1,preload)
{
	{ 1 } ;
} ;
