#ifndef _WX_STATLINE_H_BASE_
#define _WX_STATLINE_H_BASE_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// this defines wxUSE_STATLINE
#include "wx/defs.h"

#if wxUSE_STATLINE

// the base class declaration
#include "wx/control.h"

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

// the default name for objects of class wxStaticLine
WXDLLEXPORT_DATA(extern const wxChar*) wxStaticTextNameStr;

// ----------------------------------------------------------------------------
// wxStaticLine - a line in a dialog
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxStaticLineBase : public wxControl
{
public:
    // constructor
    wxStaticLineBase() { }

    // is the line vertical?
    bool IsVertical() const { return (GetWindowStyle() & wxLI_VERTICAL) != 0; }

    // get the default size for the "lesser" dimension of the static line
    static int GetDefaultSize() { return 2; }

protected:
    // set the right size for the right dimension
    wxSize AdjustSize(const wxSize& size)
    {
        wxSize sizeReal(size);
        if ( IsVertical() )
        {
            if ( size.x == -1 )
                sizeReal.x = GetDefaultSize();
        }
        else
        {
            if ( size.y == -1 )
                sizeReal.y = GetDefaultSize();
        }

        return sizeReal;
    }
};

// ----------------------------------------------------------------------------
// now include the actual class declaration
// ----------------------------------------------------------------------------

#if defined(__WXMSW__)
    #include "wx/msw/statline.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/statline.h"
#elif defined(__WXPM__)
    #include "wx/os2/statline.h"
#else // use generic implementation for all other platforms
    #include "wx/generic/statline.h"
#endif

#endif // wxUSE_STATLINE

#endif
    // _WX_STATLINE_H_BASE_
