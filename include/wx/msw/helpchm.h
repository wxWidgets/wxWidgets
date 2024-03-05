/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/helpchm.h
// Purpose:     Help system: MS HTML Help implementation
// Author:      Julian Smart
// Created:     16/04/2000
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_HELPCHM_H_
#define _WX_MSW_HELPCHM_H_

#if wxUSE_MS_HTML_HELP

#include "wx/helpbase.h"

class WXDLLIMPEXP_CORE wxCHMHelpController : public wxHelpControllerBase
{
public:
    wxCHMHelpController(wxWindow* parentWindow = nullptr): wxHelpControllerBase(parentWindow) { }

    // Must call this to set the filename
    virtual bool Initialize(const wxString& file) override;
    virtual bool Initialize(const wxString& file, int WXUNUSED(server) ) override { return Initialize( file ); }

    // If file is "", reloads file given in Initialize
    virtual bool LoadFile(const wxString& file = wxEmptyString) override;
    virtual bool DisplayContents() override;
    virtual bool DisplaySection(int sectionNo) override;
    virtual bool DisplaySection(const wxString& section) override;
    virtual bool DisplayBlock(long blockNo) override;
    virtual bool DisplayContextPopup(int contextId) override;
    virtual bool DisplayTextPopup(const wxString& text, const wxPoint& pos) override;
    virtual bool KeywordSearch(const wxString& k,
                               wxHelpSearchMode mode = wxHELP_SEARCH_ALL) override;
    virtual bool Quit() override;

    wxString GetHelpFile() const { return m_helpFile; }

    // helper of DisplayTextPopup(), also used in wxSimpleHelpProvider::ShowHelp
    static bool ShowContextHelpPopup(const wxString& text,
                                     const wxPoint& pos,
                                     wxWindow *window);

protected:
    // get the name of the CHM file we use from our m_helpFile
    wxString GetValidFilename() const;

    // Call HtmlHelp() with the provided parameters (both overloads do the same
    // thing but allow to avoid casts in the calling code) and return false
    // (but don't crash) if HTML help is unavailable
    static bool CallHtmlHelp(wxWindow *win, const wxChar *str,
                             unsigned cmd, WXWPARAM param);
    static bool CallHtmlHelp(wxWindow *win, const wxChar *str,
                             unsigned cmd, const void *param = nullptr)
    {
        return CallHtmlHelp(win, str, cmd, reinterpret_cast<WXWPARAM>(param));
    }

    // even simpler wrappers using GetParentWindow() and GetValidFilename() as
    // the first 2 HtmlHelp() parameters
    bool CallHtmlHelp(unsigned cmd, WXWPARAM param)
    {
        return CallHtmlHelp(GetParentWindow(), GetValidFilename().t_str(),
                            cmd, param);
    }

    bool CallHtmlHelp(unsigned cmd, const void *param = nullptr)
    {
        return CallHtmlHelp(cmd, reinterpret_cast<WXWPARAM>(param));
    }

    // wrapper around CallHtmlHelp(HH_DISPLAY_TEXT_POPUP): only one of text and
    // contextId parameters can be non-null/non-zero
    static bool DoDisplayTextPopup(const wxChar *text,
                                   const wxPoint& pos,
                                   int contextId,
                                   wxWindow *window);


    wxString m_helpFile;

    wxDECLARE_DYNAMIC_CLASS(wxCHMHelpController);
};

#endif // wxUSE_MS_HTML_HELP

#endif // _WX_MSW_HELPCHM_H_
