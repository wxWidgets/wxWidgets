/////////////////////////////////////////////////////////////////////////////
// Name:        filedlg.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKFILEDLGH__
#define __GTKFILEDLGH__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface
#endif

//-------------------------------------------------------------------------
// wxFileDialog
//-------------------------------------------------------------------------

class wxFileDialog: public wxFileDialogBase
{
public:
    wxFileDialog() { }

    wxFileDialog(wxWindow *parent,
                 const wxString& message = wxFileSelectorPromptStr,
                 const wxString& defaultDir = wxEmptyString,
                 const wxString& defaultFile = wxEmptyString,
                 const wxString& wildCard = wxFileSelectorDefaultWildcardStr,
                 long style = 0,
                 const wxPoint& pos = wxDefaultPosition);
    
    virtual void GetPaths(wxArrayString& paths) const;
    virtual void GetFilenames(wxArrayString& files) const;
    
    virtual void SetMessage(const wxString& message);
    virtual void SetPath(const wxString& path);
    virtual void SetDirectory(const wxString& dir);
    virtual void SetFilename(const wxString& name);
    virtual void SetWildcard(const wxString& wildCard);
    virtual void SetFilterIndex(int filterIndex);
            void UpdateFromDialog();
            void UpdateDialog();
            void DoSetPath(const wxString& path);
private:
    DECLARE_DYNAMIC_CLASS(wxFileDialog)
};

#endif // __GTKFILEDLGH__
