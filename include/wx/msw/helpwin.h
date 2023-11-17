/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/helpwin.h
// Purpose:     Help system: WinHelp implementation
// Author:      Julian Smart
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_HELPWIN_H_
#define _WX_HELPWIN_H_

#include "wx/wx.h"

#if wxUSE_HELP

#include "wx/helpbase.h"

class WXDLLIMPEXP_CORE wxWinHelpController: public wxHelpControllerBase
{
    wxDECLARE_DYNAMIC_CLASS(wxWinHelpController);

public:
    wxWinHelpController(wxWindow* parentWindow = nullptr): wxHelpControllerBase(parentWindow) {}
    virtual ~wxWinHelpController() = default;

    // Must call this to set the filename
    virtual bool Initialize(const wxString& file) override;
    virtual bool Initialize(const wxString& file, int WXUNUSED(server) ) override { return Initialize( file ); }

    // If file is "", reloads file given in Initialize
    virtual bool LoadFile(const wxString& file = wxEmptyString) override;
    virtual bool DisplayContents() override;
    virtual bool DisplaySection(int sectionNo) override;
    virtual bool DisplaySection(const wxString& section) override { return KeywordSearch(section); }
    virtual bool DisplayBlock(long blockNo) override;
    virtual bool DisplayContextPopup(int contextId) override;
    virtual bool KeywordSearch(const wxString& k,
                               wxHelpSearchMode mode = wxHELP_SEARCH_ALL) override;
    virtual bool Quit() override;

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
