/////////////////////////////////////////////////////////////////////////////
// Name:        msgdlg.h
// Purpose:     wxMessageDialog class. Use generic version if no
//              platform-specific implementation.
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSGBOXDLG_H_
#define _WX_MSGBOXDLG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "msgdlg.h"
#endif

#include "wx/setup.h"
#include "wx/dialog.h"

// ----------------------------------------------------------------------------
// Message box dialog
// ----------------------------------------------------------------------------

WXDLLEXPORT_DATA(extern const char*) wxMessageBoxCaptionStr;

class WXDLLEXPORT wxMessageDialog: public wxDialog
{
    DECLARE_DYNAMIC_CLASS(wxMessageDialog)
        
public:
    wxMessageDialog(wxWindow *parent,
        const wxString& message,
        const wxString& caption = wxMessageBoxCaptionStr,
        long style = wxOK | wxCENTRE,
        const wxPoint& pos = wxDefaultPosition);
    
    int ShowModal();
    
    // implementation only from now on
    // called by the Motif callback
    void SetResult(long result) { m_result = result; }
    
protected:
    wxString    m_caption;
    wxString    m_message;
    long        m_dialogStyle;
    wxWindow *  m_parent;
    long        m_result;
};

#endif
// _WX_MSGBOXDLG_H_
