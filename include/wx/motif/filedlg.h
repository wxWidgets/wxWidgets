/////////////////////////////////////////////////////////////////////////////
// Name:        filedlg.h
// Purpose:     wxFileDialog class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FILEDLG_H_
#define _WX_FILEDLG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "filedlg.h"
#endif

//-------------------------------------------------------------------------
// wxFileDialog
//-------------------------------------------------------------------------

class WXDLLEXPORT wxFileDialog: public wxFileDialogBase
{
    DECLARE_DYNAMIC_CLASS(wxFileDialog)
public:
    
    // For Motif
    static wxString m_fileSelectorAnswer;
    static bool m_fileSelectorReturned;
    
public:
    wxFileDialog(wxWindow *parent,
                 const wxString& message = wxFileSelectorPromptStr,
                 const wxString& defaultDir = wxEmptyString,
                 const wxString& defaultFile = wxEmptyString,
                 const wxString& wildCard = wxFileSelectorDefaultWildcardStr,
                 long style = 0,
                 const wxPoint& pos = wxDefaultPosition);
    
    virtual int ShowModal();
};

#endif // _WX_FILEDLG_H_
