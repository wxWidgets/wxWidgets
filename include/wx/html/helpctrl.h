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

#ifdef __GNUG__
#pragma interface
#endif

#if wxUSE_HTML

#include "wx/html/helpfrm.h"

class WXDLLEXPORT wxHtmlHelpController : public wxEvtHandler
{
        DECLARE_DYNAMIC_CLASS(wxHtmlHelpController)

    public:
        wxHtmlHelpController(int style = wxHF_DEFAULTSTYLE);
        virtual ~wxHtmlHelpController();

        void SetTitleFormat(const wxString& format);
        void SetTempDir(const wxString& path) { m_helpData.SetTempDir(path); }
        bool AddBook(const wxString& book, bool show_wait_msg = FALSE);
        bool Display(const wxString& x)
        {
            CreateHelpWindow(); return m_helpFrame->Display(x);
        }
        bool Display(int id)
        {
            CreateHelpWindow(); return m_helpFrame->Display(id);
        }
        bool DisplayContents()
        {
            CreateHelpWindow(); return m_helpFrame->DisplayContents();
        }
        bool DisplayIndex()
        {
            CreateHelpWindow(); return m_helpFrame->DisplayIndex();
        }
        bool KeywordSearch(const wxString& keyword)
        {
            CreateHelpWindow(); return m_helpFrame->KeywordSearch(keyword);
        }
        wxHtmlHelpFrame* GetFrame() { return m_helpFrame; }
        void UseConfig(wxConfigBase *config, const wxString& rootpath = wxEmptyString);
	
        // Assigns config object to the Ctrl. This config is then
        // used in subsequent calls to Read/WriteCustomization of both help
        // Ctrl and it's wxHtmlWindow
        virtual void ReadCustomization(wxConfigBase *cfg, const wxString& path = wxEmptyString);
        virtual void WriteCustomization(wxConfigBase *cfg, const wxString& path = wxEmptyString);

    protected:
        virtual wxHtmlHelpFrame* CreateHelpFrame(wxHtmlHelpData *data);
    
        virtual void CreateHelpWindow();
        virtual void DestroyHelpWindow();

        void OnCloseFrame(wxCloseEvent& evt);
        wxHtmlHelpData m_helpData;
        wxHtmlHelpFrame* m_helpFrame;
        wxConfigBase *m_Config;
        wxString m_ConfigRoot;
        wxString m_titleFormat;
        int m_FrameStyle;
        DECLARE_EVENT_TABLE()
};

#endif

#endif // _WX_HELPCTRL_H_
