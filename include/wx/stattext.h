#ifndef _WX_STATTEXT_H_BASE_
#define _WX_STATTEXT_H_BASE_

#if wxUSE_STATTEXT

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxStaticTextNameStr;

class WXDLLEXPORT wxStaticTextBase : public wxControl
{
public:
    wxStaticTextBase() { }

    // overriden base virtuals
    virtual bool AcceptsFocus() const { return false; }
    virtual void ApplyParentThemeBackground(const wxColour& bg)
        { SetBackgroundColour(bg); }

private:
    DECLARE_NO_COPY_CLASS(wxStaticTextBase)
};

#if defined(__WXUNIVERSAL__)
    #include "wx/univ/stattext.h"
#elif defined(__WXMSW__)
    #include "wx/msw/stattext.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/stattext.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/stattext.h"
#elif defined(__WXMAC__)
    #include "wx/mac/stattext.h"
#elif defined(__WXCOCOA__)
    #include "wx/cocoa/stattext.h"
#elif defined(__WXPM__)
    #include "wx/os2/stattext.h"
#endif

#endif // wxUSE_STATTEXT

#endif
    // _WX_STATTEXT_H_BASE_
