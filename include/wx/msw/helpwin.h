/////////////////////////////////////////////////////////////////////////////
// Name:        helpwin.h
// Purpose:     Help system: WinHelp implementation
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_HELPWIN_H_
#define _WX_HELPWIN_H_

#ifdef __GNUG__
#pragma interface "helpwin.h"
#endif

#include "wx/wx.h"

#if wxUSE_HELP

#include "wx/helpbase.h"

class WXDLLEXPORT wxWinHelpController: public wxHelpControllerBase
{
    DECLARE_CLASS(wxWinHelpController)
        
public:
    wxWinHelpController() {};
    ~wxWinHelpController() {};

    // Must call this to set the filename
    virtual bool Initialize(const wxString& file);

    // If file is "", reloads file given in Initialize
    virtual bool LoadFile(const wxString& file = wxEmptyString);
    virtual bool DisplayContents();
    virtual bool DisplaySection(int sectionNo);
    virtual bool DisplayBlock(long blockNo);
    virtual bool DisplayContextPopup(int contextId);
    virtual bool KeywordSearch(const wxString& k);
    virtual bool Quit();

    inline wxString GetHelpFile() const { return m_helpFile; }

protected:
    // Append extension if necessary.
    wxString GetValidFilename(const wxString& file) const;
    
private:
    wxString m_helpFile;
};

#endif // wxUSE_HELP
#endif
// _WX_HELPWIN_H_
