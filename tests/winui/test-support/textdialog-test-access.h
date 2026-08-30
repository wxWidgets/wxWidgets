/////////////////////////////////////////////////////////////////////////////
// Private, non-installed access to wxTextEntryDialog test operations.
// Available only in an explicitly test-enabled WinUI library build.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_TEXTDIALOG_TEST_ACCESS_H
#define WX_WINUI_TEXTDIALOG_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
    #error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/textdlg.h"

#if wxUSE_TEXTDLG
class WXDLLIMPEXP_CORE wxWinUITextEntryDialogTestAccess final
{
public:
    // Mutate the active peer while a modal surface is open, without input.
    static bool SetPeerValue(wxTextEntryDialog& dialog, const wxString& value);
    static wxString GetPeerValue(const wxTextEntryDialog& dialog);
};
#endif // wxUSE_TEXTDLG

#endif // WX_WINUI_TEXTDIALOG_TEST_ACCESS_H
