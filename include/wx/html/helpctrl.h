/////////////////////////////////////////////////////////////////////////////
// Name:        helpctrl.h
// Purpose:     wxHtmlHelpController
// Notes:       Based on htmlhelp.cpp, implementing a monolithic
//              HTML Help controller class,  by Vaclav Slavik
// Author:      Harm van der Heijden and Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) Harm van der Heijden and Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_HELPCTRL_H_
#define _WX_HELPCTRL_H_

#include "wx/defs.h"

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "helpctrl.h"
#endif

#if wxUSE_WXHTML_HELP

#include "wx/helpbase.h"
#include "wx/html/helpfrm.h"

#define wxID_HTML_HELPFRAME   (wxID_HIGHEST + 1)

class WXDLLIMPEXP_HTML wxHtmlHelpController : public wxHelpControllerBase // wxEvtHandler
{
    DECLARE_DYNAMIC_CLASS(wxHtmlHelpController)

public:
    wxHtmlHelpController(int style = wxHF_DEFAULT_STYLE);
    virtual ~wxHtmlHelpController();

    void SetTitleFormat(const wxString& format);
    void SetTempDir(const wxString& path) { m_helpData.SetTempDir(path); }
    bool AddBook(const wxString& book_url, bool show_wait_msg = false);
    bool AddBook(const wxFileName& book_file, bool show_wait_msg = false);

    bool Display(const wxString& x);
    bool Display(int id);
    bool DisplayContents();
    bool DisplayIndex();
    bool KeywordSearch(const wxString& keyword,
                       wxHelpSearchMode mode = wxHELP_SEARCH_ALL);

    wxHtmlHelpFrame* GetFrame() { return m_helpFrame; }
    void UseConfig(wxConfigBase *config, const wxString& rootpath = wxEmptyString);

    // Assigns config object to the Ctrl. This config is then
    // used in subsequent calls to Read/WriteCustomization of both help
    // Ctrl and it's wxHtmlWindow
    virtual void ReadCustomization(wxConfigBase *cfg, const wxString& path = wxEmptyString);
    virtual void WriteCustomization(wxConfigBase *cfg, const wxString& path = wxEmptyString);

    //// Backward compatibility with wxHelpController API

    virtual bool Initialize(const wxString& file, int WXUNUSED(server) ) { return Initialize(file); }
    virtual bool Initialize(const wxString& file);
    virtual void SetViewer(const wxString& WXUNUSED(viewer), long WXUNUSED(flags) = 0) {}
    virtual bool LoadFile(const wxString& file = wxT(""));
    virtual bool DisplaySection(int sectionNo);
    virtual bool DisplaySection(const wxString& section) { return Display(section); }
    virtual bool DisplayBlock(long blockNo) { return DisplaySection(blockNo); }
    virtual bool DisplayTextPopup(const wxString& text, const wxPoint& pos);

    virtual void SetFrameParameters(const wxString& title,
                               const wxSize& size,
                               const wxPoint& pos = wxDefaultPosition,
                               bool newFrameEachTime = false);
    /// Obtains the latest settings used by the help frame and the help
    /// frame.
    virtual wxFrame *GetFrameParameters(wxSize *size = NULL,
                               wxPoint *pos = NULL,
                               bool *newFrameEachTime = NULL);

    // Get direct access to help data:
    wxHtmlHelpData *GetHelpData() { return &m_helpData; }

    virtual bool Quit() ;
    virtual void OnQuit() {}

    void OnCloseFrame(wxCloseEvent& evt);

    // Make the help controller's frame 'modal' if
    // needed
    void AddGrabIfNeeded();

protected:
    virtual wxHtmlHelpFrame* CreateHelpFrame(wxHtmlHelpData *data);

    virtual void CreateHelpWindow();
    virtual void DestroyHelpWindow();

    wxHtmlHelpData      m_helpData;
    wxHtmlHelpFrame*    m_helpFrame;
    wxConfigBase *      m_Config;
    wxString            m_ConfigRoot;
    wxString            m_titleFormat;
    int                 m_FrameStyle;
    // DECLARE_EVENT_TABLE()

    DECLARE_NO_COPY_CLASS(wxHtmlHelpController)
};

#endif // wxUSE_WXHTML_HELP

#endif // _WX_HELPCTRL_H_
