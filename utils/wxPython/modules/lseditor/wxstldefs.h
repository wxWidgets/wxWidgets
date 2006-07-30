#ifndef __WXSTLDEFS_G__
#define __WXSTLDEFS_G__


#include "config.h"

// defines some very commonly used container types
// for both template and macro-based configurations

#if defined( wxUSE_TEMPLATE_STL )

	#include <vector>
        using namespace std;
    #ifdef WIN32xxx
	#include <bstring.h>
	#else

	//#include <strclass.h>
	//#include <string.h>
    // For now
    #include "wx/string.h"
    #define string wxString

	#endif

#else

	#include "wx/string.h"
	#include "wxstlvec.h"

	// FOR NOW:: quick n' dirty:

	#define  string wxString

#endif

#if defined( wxUSE_TEMPLATE_STL )

	typedef vector<string>     StrListT;
	typedef vector<int>        IntListT;

#else

	typedef WXSTL_VECTOR(string)           StrListT;
	typedef WXSTL_VECTOR_SHALLOW_COPY(int) IntListT;

#endif

#endif
