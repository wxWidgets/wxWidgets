/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/filedlg.h
// Purpose:     wxFileDialog class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
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

protected:

#if !(defined(__SMARTPHONE__) && defined(__WXWINCE__))
    virtual void DoMoveWindow(int x, int y, int width, int height);
    virtual void DoGetSize( int *width, int *height ) const;
    virtual void DoGetPosition( int *x, int *y ) const;
#endif // !(__SMARTPHONE__ && __WXWINCE__)

private:
    wxArrayString m_fileNames;
    bool m_bMovedWindow;

    DECLARE_DYNAMIC_CLASS(wxFileDialog)
    DECLARE_NO_COPY_CLASS(wxFileDialog)
};

#endif // _WX_FILEDLG_H_

