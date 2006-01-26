/////////////////////////////////////////////////////////////////////////////
// Name:        wx/os2/msgdlg.h
// Purpose:     wxMessageDialog class. Use generic version if no
//              platform-specific implementation.
// Author:      David Webster
// Modified by:
// Created:     10/12/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSGBOXDLG_H_
#define _WX_MSGBOXDLG_H_

#include "wx/defs.h"
#include "wx/dialog.h"

/*
 * Message box dialog
 */

class WXDLLEXPORT wxMessageDialog : public wxDialog, public wxMessageDialogBase
{
DECLARE_DYNAMIC_CLASS(wxMessageDialog)
public:
    wxMessageDialog( wxWindow*       pParent
                    ,const wxString& rsMessage
                    ,const wxString& rsCaption = wxMessageBoxCaptionStr
                    ,long            lStyle = wxOK|wxCENTRE
                    ,const wxPoint&  rPos = wxDefaultPosition
                   );

    int ShowModal(void);

protected:
    wxString                        m_sCaption;
    wxString                        m_sMessage;
    wxWindow*                       m_pParent;
}; // end of CLASS wxMessageDialog

#endif
    // _WX_MSGBOXDLG_H_
