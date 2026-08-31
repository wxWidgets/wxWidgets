/////////////////////////////////////////////////////////////////////////////
// Private, non-installed access to static control test operations.
// Available only in an explicitly test-enabled WinUI library build.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_STATIC_TEST_ACCESS_H
#define WX_WINUI_STATIC_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
    #error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/statbmp.h"
#include "wx/statbox.h"
#include "wx/stattext.h"

struct wxWinUIAppearanceSnapshot;

#if wxUSE_STATTEXT || wxUSE_STATBMP || wxUSE_STATBOX
class WXDLLIMPEXP_CORE wxWinUIStaticTestAccess final
{
public:
#if wxUSE_STATTEXT
    static bool GetAppearance(const wxStaticText& text,
                              wxWinUIAppearanceSnapshot* snapshot);
    static wxString GetVisibleLabel(const wxStaticText& text);
    static wxString GetRenderedText(const wxStaticText& text);
    static int GetTextTrimming(const wxStaticText& text);
    static bool HasLocalBoldInline(const wxStaticText& text);
    static bool HasLocalUnderlineInline(const wxStaticText& text);
#endif
#if wxUSE_STATBMP
    static bool RefreshForScale(wxStaticBitmap& bitmap, double scale);
    static bool GetPeerImageState(const wxStaticBitmap& bitmap,
                                  wxSize* pixelSize, wxSize* dipSize,
                                  int* stretch, unsigned* generation,
                                  bool* hasSource);
#endif
#if wxUSE_STATBOX
    static bool GetAppearance(const wxStaticBox& box,
                              wxWinUIAppearanceSnapshot* snapshot);
    static bool GetLayout(const wxStaticBox& box,
                          double* frameTopDIP, double* titleGapDIP,
                          bool* usesThemeBrush);
#endif
};
#endif

#endif
