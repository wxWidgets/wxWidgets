/////////////////////////////////////////////////////////////////////////////
// Name:        helpchm.h
// Purpose:     Help system: MS HTML Help implementation
// Author:      Julian Smart
// Modified by:
// Created:     16/04/2000
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_HELPCHM_H_
#define _WX_HELPCHM_H_

#ifdef __GNUG__
#pragma interface "helpchm.h"
#endif

#include "wx/wx.h"

#if wxUSE_HELP

#include "wx/helpbase.h"

class WXDLLEXPORT wxCHMHelpController: public wxHelpControllerBase
{
    DECLARE_CLASS(wxCHMHelpController)
        
public:
    wxCHMHelpController() {}
    ~wxCHMHelpController() {}
    
    // Must call this to set the filename
    virtual bool Initialize(const wxString& file);
    
    // If file is "", reloads file given in Initialize
    virtual bool LoadFile(const wxString& file = wxEmptyString);
    virtual bool DisplayContents();
    virtual bool DisplaySection(int sectionNo);
    virtual bool DisplaySection(const wxString& section);
    virtual bool DisplayBlock(long blockNo);
    virtual bool KeywordSearch(const wxString& k);
    virtual bool Quit();

    inline wxString GetHelpFile() const { return m_helpFile; }
    
protected:
    // Append extension if necessary.
    wxString GetValidFilename(const wxString& file) const;
    
protected:
    wxString m_helpFile;
};

#endif // wxUSE_HELP
#endif
// _WX_HELPCHM_H_
