/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dialog.h
// Purpose:     wxDialogBase class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29.06.99
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DIALOG_H_BASE_
#define _WX_DIALOG_H_BASE_

#ifdef __GNUG__
    #pragma interface "dialogbase.h"
#endif

#include "wx/defs.h"
#include "wx/panel.h"

class WXDLLEXPORT wxDialogBase : public wxPanel
{
public:
    // the modal dialogs have a return code - usually the id of the last
    // pressed button
    void SetReturnCode(int returnCode) { m_returnCode = returnCode; }
    int GetReturnCode() const { return m_returnCode; }

protected:
    // splits text up at newlines and places the
    // lines into a vertical wxBoxSizer
    wxSizer *CreateTextSizer( const wxString &message );
    
    // places buttons into a horizontal wxBoxSizer
    wxSizer *CreateButtonSizer( long flags );

    // the return code from modal dialog
    int m_returnCode;
};

#if defined(__WXMSW__)
    #include "wx/msw/dialog.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/dialog.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/dialog.h"
#elif defined(__WXQT__)
    #include "wx/qt/dialog.h"
#elif defined(__WXMAC__)
    #include "wx/mac/dialog.h"
#elif defined(__WXPM__)
    #include "wx/os2/dialog.h"
#elif defined(__WXSTUBS__)
    #include "wx/stubs/dialog.h"
#endif

#endif
    // _WX_DIALOG_H_BASE_
