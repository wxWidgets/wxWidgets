/////////////////////////////////////////////////////////////////////////////
// Name:        wx/palmos/filedlg.h
// Purpose:     wxFileDialog class
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id: 
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
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
public:
    wxFileDialog(wxWindow *parent,
                 const wxString& message = wxFileSelectorPromptStr,
                 const wxString& defaultDir = wxEmptyString,
                 const wxString& defaultFile = wxEmptyString,
                 const wxString& wildCard = wxFileSelectorDefaultWildcardStr,
                 long style = 0,
                 const wxPoint& pos = wxDefaultPosition);

    virtual void SetPath(const wxString& path);
    virtual void GetPaths(wxArrayString& paths) const;
    virtual void GetFilenames(wxArrayString& files) const;

    virtual int ShowModal();

private:
    wxArrayString m_fileNames;

    DECLARE_DYNAMIC_CLASS(wxFileDialog)
    DECLARE_NO_COPY_CLASS(wxFileDialog)
};

#endif // _WX_FILEDLG_H_

