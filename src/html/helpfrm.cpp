/////////////////////////////////////////////////////////////////////////////
// Name:        src/html/helpfrm.cpp
// Purpose:     wxHtmlHelpFrame
// Notes:       Based on htmlhelp.cpp, implementing a monolithic
//              HTML Help controller class,  by Vaclav Slavik
// Author:      Harm van der Heijden and Vaclav Slavik
// Copyright:   (c) Harm van der Heijden and Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h"

#include "wx/wxprec.h"


#if wxUSE_WXHTML_HELP

#ifndef WX_PRECOMP
    #include "wx/object.h"
    #include "wx/dynarray.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #if wxUSE_STREAMS
        #include "wx/stream.h"
    #endif

    #include "wx/sizer.h"

    #include "wx/bmpbuttn.h"
    #include "wx/statbox.h"
    #include "wx/radiobox.h"
    #include "wx/menu.h"
    #include "wx/settings.h"
    #include "wx/msgdlg.h"
    #include "wx/textctrl.h"
    #include "wx/toolbar.h"
    #include "wx/choicdlg.h"
    #include "wx/filedlg.h"
#endif // WX_PRECOMP

#include "wx/html/helpfrm.h"
#include "wx/html/helpctrl.h"
#include "wx/notebook.h"
#include "wx/imaglist.h"
#include "wx/treectrl.h"
#include "wx/tokenzr.h"
#include "wx/wfstream.h"
#include "wx/html/htmlwin.h"
#include "wx/busyinfo.h"
#include "wx/progdlg.h"
#include "wx/fontenum.h"
#include "wx/artprov.h"
#include "wx/spinctrl.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxHtmlHelpFrame, wxFrame);

wxBEGIN_EVENT_TABLE(wxHtmlHelpFrame, wxFrame)
    EVT_ACTIVATE(wxHtmlHelpFrame::OnActivate)
    EVT_CLOSE(wxHtmlHelpFrame::OnCloseWindow)
wxEND_EVENT_TABLE()

wxHtmlHelpFrame::wxHtmlHelpFrame(wxWindow* parent, wxWindowID id, const wxString& title,
                                 int style, wxHtmlHelpData* data
#if wxUSE_CONFIG
                                 , wxConfigBase *config, const wxString& rootpath
#endif // wxUSE_CONFIG
                                 )
{
    Init(data);
    Create(parent, id, title, style
#if wxUSE_CONFIG
        , config, rootpath
#endif // wxUSE_CONFIG
        );
}

void wxHtmlHelpFrame::Init(wxHtmlHelpData* data)
{
    // Simply pass the pointer on to the help window
    m_Data = data;
    m_HtmlHelpWin = NULL;
    m_helpController = NULL;
    m_shouldPreventAppExit = false;
}

void wxHtmlHelpFrame::SetController(wxHtmlHelpController* controller)
{
    m_helpController = controller;
    if ( m_HtmlHelpWin )
        m_HtmlHelpWin->SetController(controller);
}

// Create: builds the GUI components.
bool wxHtmlHelpFrame::Create(wxWindow* parent, wxWindowID id,
                             const wxString& WXUNUSED(title), int style
#if wxUSE_CONFIG
                             , wxConfigBase *config, const wxString& rootpath
#endif // wxUSE_CONFIG
                             )
{
    m_HtmlHelpWin = new wxHtmlHelpWindow(m_Data);
    m_HtmlHelpWin->SetController(m_helpController);
#if wxUSE_CONFIG
    if ( config )
        m_HtmlHelpWin->UseConfig(config, rootpath);
#endif // wxUSE_CONFIG

    wxFrame::Create(parent, id, _("Help"),
                    wxPoint(m_HtmlHelpWin->GetCfgData().x, m_HtmlHelpWin->GetCfgData().y),
                    wxSize(m_HtmlHelpWin->GetCfgData().w, m_HtmlHelpWin->GetCfgData().h),
                    wxDEFAULT_FRAME_STYLE, wxT("wxHtmlHelp"));
#if wxUSE_STATUSBAR
    CreateStatusBar();
#endif
    m_HtmlHelpWin->Create(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxTAB_TRAVERSAL|wxNO_BORDER, style);

    GetPosition(& (m_HtmlHelpWin->GetCfgData().x), & (m_HtmlHelpWin->GetCfgData()).y);

    SetIcons(wxArtProvider::GetIconBundle(wxART_HELP, wxART_FRAME_ICON));

    m_HtmlHelpWin->GetHtmlWindow()->SetRelatedFrame(this, m_TitleFormat);
#if wxUSE_STATUSBAR
    m_HtmlHelpWin->GetHtmlWindow()->SetRelatedStatusBar(0);
#endif
    return true;
}

wxHtmlHelpFrame::~wxHtmlHelpFrame()
{
}

void wxHtmlHelpFrame::SetTitleFormat(const wxString& format)
{
    if (GetHelpWindow() && GetHelpWindow()->GetHtmlWindow())
        GetHelpWindow()->GetHtmlWindow()->SetRelatedFrame(this, format);
    m_TitleFormat = format;
}

/*
EVENT HANDLING :
*/


void wxHtmlHelpFrame::OnActivate(wxActivateEvent& event)
{
    // This saves one mouse click when using the
    // wxHTML for context sensitive help systems
#ifndef __WXGTK__
    // NB: wxActivateEvent is a bit broken in wxGTK
    //     and is sometimes sent when it should not be
    if (event.GetActive() && m_HtmlHelpWin)
        m_HtmlHelpWin->GetHtmlWindow()->SetFocus();
#endif

    event.Skip();
}

void wxHtmlHelpFrame::OnCloseWindow(wxCloseEvent& evt)
{
    if (!IsIconized())
    {
        GetSize(& (m_HtmlHelpWin->GetCfgData().w), &(m_HtmlHelpWin->GetCfgData().h));
        GetPosition(& (m_HtmlHelpWin->GetCfgData().x), & (m_HtmlHelpWin->GetCfgData().y));
    }

#ifdef __WXGTK__
    if (IsGrabbed())
    {
        RemoveGrab();
    }
#endif

    if (m_HtmlHelpWin->GetSplitterWindow() && m_HtmlHelpWin->GetCfgData().navig_on)
        m_HtmlHelpWin->GetCfgData().sashpos = m_HtmlHelpWin->GetSplitterWindow()->GetSashPosition();

    if (m_helpController && wxDynamicCast(m_helpController, wxHtmlHelpController))
    {
        ((wxHtmlHelpController*) m_helpController)->OnCloseFrame(evt);
    }

    evt.Skip();
}

// Make the help controller's frame 'modal' if
// needed
void wxHtmlHelpFrame::AddGrabIfNeeded()
{
    // So far, wxGTK only
#ifdef __WXGTK__
    bool needGrab = false;

    // Check if there are any modal windows present,
    // in which case we need to add a grab.
    for ( wxWindowList::iterator it = wxTopLevelWindows.begin();
          it != wxTopLevelWindows.end();
          ++it )
    {
        wxWindow *win = *it;
        wxDialog *dialog = wxDynamicCast(win, wxDialog);

        if (dialog && dialog->IsModal())
            needGrab = true;
    }

    if (needGrab)
        AddGrab();
#endif // __WXGTK__
}

#if wxUSE_CONFIG
// For compatibility
void wxHtmlHelpFrame::UseConfig(wxConfigBase *config, const wxString& rootPath)
{
    if (m_HtmlHelpWin)
        m_HtmlHelpWin->UseConfig(config, rootPath);
}
#endif // wxUSE_CONFIG

void wxHtmlHelpFrame::SetShouldPreventAppExit(bool enable)
{
    m_shouldPreventAppExit = enable;
}

#endif // wxUSE_WXHTML_HELP
