#ifndef _WX_FONTDLG_H_BASE_
#define _WX_FONTDLG_H_BASE_

#if wxUSE_FONTDLG

#if defined(__WXUNIVERSAL__) || defined(__WXMOTIF__) || defined(__WXMAC__)
#   include "wx/generic/fontdlgg.h"
#   define wxFontDialog wxGenericFontDialog
#   define sm_classwxFontDialog sm_classwxGenericFontDialog
#elif defined(__WXMSW__)
#   include "wx/msw/fontdlg.h"
#elif defined(__WXGTK__)
#   include "wx/gtk/fontdlg.h"
#elif defined(__WXPM__)
#   include "wx/os2/fontdlg.h"
#endif

// get the font from user and return it, returns wxNullFont if the dialog was
// cancelled
wxFont WXDLLEXPORT
wxGetFontFromUser(wxWindow *parent = (wxWindow *)NULL,
                  const wxFont& fontInit = wxNullFont);

#endif // wxUSE_FONTDLG

#endif
    // _WX_FONTDLG_H_BASE_
