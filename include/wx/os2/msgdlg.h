/////////////////////////////////////////////////////////////////////////////
// Name:        msgdlg.h
// Purpose:     wxMessageDialog class. Use generic version if no
//              platform-specific implementation.
// Author:      David Webster
// Modified by:
// Created:     10/12/99
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSGBOXDLG_H_
#define _WX_MSGBOXDLG_H_

#include "wx/setup.h"
#include "wx/dialog.h"

/*
 * Message box dialog
 */

WXDLLEXPORT_DATA(extern const char*) wxMessageBoxCaptionStr;

class WXDLLEXPORT wxMessageDialog : public wxDialog
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
    long                            m_lDialogStyle;
    wxWindow*                       m_pParent;
}; // end of CLASS wxMessageDialog


int WXDLLEXPORT wxMessageBox( const wxString& rsMessage
                             ,const wxString& rsCaption = wxMessageBoxCaptionStr
                             ,long            lStyle = wxOK|wxCENTRE
                             ,wxWindow*       pParent = NULL
                             ,int             nX = -1
                             ,int             nY = -1
                            );

#endif
    // _WX_MSGBOXDLG_H_
