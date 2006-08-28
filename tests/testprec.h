#include "wx/wxprec.h"
#include "wx/cppunit.h"

// define wxHAVE_U_ESCAPE if the compiler supports \uxxxx character constants
#if (defined(__VISUALC__) && (__VISUALC__ >= 1300)) || \
    (defined(__GNUC__) && (__GNUC__ >= 3))
    #define wxHAVE_U_ESCAPE
#endif
