/////////////////////////////////////////////////////////////////////////////
// Name:        tabdocmdi.cpp
// Purpose:     Frame classes for MDI document/view applications
// Author:      Julian Smart
// Modified by: Kinaou Hervé
// Created:     01/02/97
// RCS-ID:      $Id:$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_AUI
#if wxUSE_MDI

#include "wx/aui/tabdocmdi.h"

enum MDI_MENU_ID
{
    wxWINDOWCLOSE = 4001,
    wxWINDOWCLOSEALL,
    wxWINDOWNEXT,
    wxWINDOWPREV
};

/*
 * Docview Aui MDI parent frame
 */

IMPLEMENT_DYNAMIC_CLASS(wxAuiDocMDIParentFrame, wxAuiMDIParentFrame)

BEGIN_EVENT_TABLE(wxAuiDocMDIParentFrame, wxAuiMDIParentFrame)
#if wxUSE_MENUS
    EVT_MENU(wxID_ANY, wxAuiDocMDIParentFrame::DoHandleMenu)
#endif
    EVT_MENU(wxID_EXIT, wxAuiDocMDIParentFrame::OnExit)
    EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, wxAuiDocMDIParentFrame::OnMRUFile)
    EVT_CLOSE(wxAuiDocMDIParentFrame::OnCloseWindow)
END_EVENT_TABLE()

wxAuiDocMDIParentFrame::wxAuiDocMDIParentFrame()
{
    Init();
}

wxAuiDocMDIParentFrame::wxAuiDocMDIParentFrame(wxDocManager *manager,
    wxWindow *parent, wxWindowID winid, const wxString& title,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    Init();
    Create(manager, parent, winid, title, pos, size, style, name);
}

wxAuiDocMDIParentFrame::~wxAuiDocMDIParentFrame()
{}

bool wxAuiDocMDIParentFrame::Create(wxDocManager *manager,
    wxWindow *parent, wxWindowID winid, const wxString& title,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    m_docManager = manager;
    return wxAuiMDIParentFrame::Create(parent, winid, title, pos, size, style, name);
}

void wxAuiDocMDIParentFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void wxAuiDocMDIParentFrame::Init()
{
    wxAuiMDIParentFrame::Init();
    m_docManager = NULL;
}

void wxAuiDocMDIParentFrame::OnMRUFile(wxCommandEvent& event)
{
    wxString f(m_docManager->GetHistoryFile(event.GetId() - wxID_FILE1));
    if (!f.empty())
        (void)m_docManager->CreateDocument(f, wxDOC_SILENT);
}

// Extend event processing to search the view's event table
bool wxAuiDocMDIParentFrame::ProcessEvent(wxEvent& event)
{
    // Try the document manager, then do default processing
    if (!m_docManager || !m_docManager->ProcessEvent(event))
    {
#if wxUSE_MENUS
        // Catch the events of kind wxID_CLOSE[_ALL] to send the appropriate tabmdi event
        if (event.GetEventType() == wxEVT_COMMAND_MENU_SELECTED)
        {
            if (event.GetId() == wxID_CLOSE)
                event.SetId(wxWINDOWCLOSE);
            else if (event.GetId() == wxID_CLOSE_ALL)
                event.SetId(wxWINDOWCLOSEALL);
        }
#endif //wxUSE_MENUS
        return wxAuiMDIParentFrame::ProcessEvent(event);
    }
    else
        return true;
}

void wxAuiDocMDIParentFrame::OnCloseWindow(wxCloseEvent& event)
{
    if (m_docManager->Clear(!event.CanVeto()))
    {
        this->Destroy();
    }
    else
        event.Veto();
}

void wxAuiDocMDIParentFrame::SetArtProvider(wxAuiTabArt* provider)
{
    wxAuiMDIParentFrame::SetArtProvider(provider);
}

wxAuiTabArt* wxAuiDocMDIParentFrame::GetArtProvider()
{
    return wxAuiMDIParentFrame::GetArtProvider();
}

wxAuiNotebook* wxAuiDocMDIParentFrame::GetNotebook() const
{
    return wxAuiMDIParentFrame::GetNotebook();
}

#if wxUSE_MENUS
void wxAuiDocMDIParentFrame::SetWindowMenu(wxMenu* pMenu)
{
    wxAuiMDIParentFrame::SetWindowMenu(pMenu);
}
#endif // wxUSE_MENUS

void wxAuiDocMDIParentFrame::SetChildMenuBar(wxAuiDocMDIChildFrame* pChild)
{
    wxAuiMDIParentFrame::SetChildMenuBar(wxDynamicCast(pChild, wxAuiMDIChildFrame));
}

wxAuiDocMDIChildFrame *wxAuiDocMDIParentFrame::GetActiveChild() const
{
    return wxDynamicCast(wxAuiMDIParentFrame::GetActiveChild(), wxAuiDocMDIChildFrame);
}

void wxAuiDocMDIParentFrame::SetActiveChild(wxAuiDocMDIChildFrame* pChildFrame)
{
    wxAuiMDIParentFrame::SetActiveChild(wxDynamicCast(pChildFrame, wxAuiMDIChildFrame));
}

wxAuiMDIClientWindow *wxAuiDocMDIParentFrame::GetClientWindow() const
{
    return wxAuiMDIParentFrame::GetClientWindow();
}

#if wxUSE_MENUS
void wxAuiDocMDIParentFrame::RemoveWindowMenu(wxMenuBar* pMenuBar)
{
    wxAuiMDIParentFrame::RemoveWindowMenu(pMenuBar);
}

void wxAuiDocMDIParentFrame::AddWindowMenu(wxMenuBar *pMenuBar)
{
    wxAuiMDIParentFrame::AddWindowMenu(pMenuBar);
}

void wxAuiDocMDIParentFrame::DoHandleMenu(wxCommandEvent& event)
{
    wxCloseEvent closeEvent;
    closeEvent.SetCanVeto(true);
    switch (event.GetId())
    {
        case wxWINDOWCLOSE:
            if (GetActiveChild())
                wxDynamicCast(GetActiveChild(), wxAuiDocMDIChildFrame)->OnCloseWindow(closeEvent);
            //if (m_docManager)
            //    m_docManager->OnFileClose(event);
            break;
        case wxWINDOWCLOSEALL:
            while (GetActiveChild())
                wxDynamicCast(GetActiveChild(), wxAuiDocMDIChildFrame)->OnCloseWindow(closeEvent);
            //if (m_docManager)
            //    m_docManager->OnFileCloseAll(event);
            break;
        case wxWINDOWNEXT:
            ActivateNext();
            break;
        case wxWINDOWPREV:
            ActivatePrevious();
            break;
        default:
            event.Skip();
    }
}
#endif // wxUSE_MENUS


/*
 * Default document child frame for Aui MDI children
 */

IMPLEMENT_DYNAMIC_CLASS(wxAuiDocMDIChildFrame, wxAuiMDIChildFrame)

BEGIN_EVENT_TABLE(wxAuiDocMDIChildFrame, wxAuiMDIChildFrame)
    EVT_MENU_HIGHLIGHT_ALL(wxAuiDocMDIChildFrame::OnMenuHighlight)
    EVT_ACTIVATE(wxAuiDocMDIChildFrame::OnActivate)
    EVT_CLOSE(wxAuiDocMDIChildFrame::OnCloseWindow)
END_EVENT_TABLE()

void wxAuiDocMDIChildFrame::Init()
{
    wxAuiMDIChildFrame::Init();
    m_childDocument = (wxDocument*)  NULL;
    m_childView = (wxView*) NULL;
}

wxAuiDocMDIChildFrame::wxAuiDocMDIChildFrame()
{
    Init();
}

// Bricsys change start : add tabPosition
wxAuiDocMDIChildFrame::wxAuiDocMDIChildFrame(wxDocument *doc, wxView *view,
    wxAuiDocMDIParentFrame *parent, wxWindowID winid, const wxString& title,
    const wxPoint& WXUNUSED(pos), const wxSize& size, long style, const wxString& name,
    int tabPosition)
// Bricsys change end : add tabPosition
{
    Init();
    
    // There are two ways to create an tabbed mdi child fram without
    // making it the active document.  Either Show(false) can be called
    // before Create() (as is customary on some ports with wxFrame-type
    // windows), or wxMINIMIZE can be passed in the style flags.  Note that
    // wxAuiMDIChildFrame is not really derived from wxFrame, as wxMDIChildFrame
    // is, but those are the expected symantics.  No style flag is passed
    // onto the panel underneath.
    //if (style & wxMINIMIZE)
    //    m_activate_on_create = false;

    // Bricsys change: do pass style to wxAuiMDIChildFrame (this way we can add a view without activating it)
#if 1
    //style; // suppress 'unused' warning

    // Bricsys change start : add tabPosition
    Create(doc, view, parent, winid, title, wxDefaultPosition, size, style, name, tabPosition);
    // Bricsys change end : add tabPosition
#else
    style; // suppress 'unused' warning

    // Bricsys change start : add tabPosition
    Create(doc, view, parent, winid, title, wxDefaultPosition, size, 0, name, tabPosition);
    // Bricsys change end : add tabPosition
#endif
}

// Bricsys change start : add tabPosition
bool wxAuiDocMDIChildFrame::Create(wxDocument *doc, wxView *view,
    wxAuiDocMDIParentFrame *parent, wxWindowID winid, const wxString& title,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name,
    int tabPosition)
// Bricsys change end : add tabPosition
{
    m_childDocument = doc;
    m_childView = view;
    // Bricsys change start : add tabPosition
    if (wxAuiMDIChildFrame::Create(wxDynamicCast(parent, wxAuiMDIParentFrame),
        winid, title, pos, size, style, name, tabPosition))
    // Bricsys change end : add tabPosition
    {
        if (view)
            view->SetFrame(this);
        return true;
    }

    return false;
}

wxAuiDocMDIChildFrame::~wxAuiDocMDIChildFrame()
{
    m_childView = (wxView *) NULL;
}

// Extend event processing to search the view's event table
bool wxAuiDocMDIChildFrame::ProcessEvent(wxEvent& event)
{
    static wxEvent *ActiveEvent = NULL;

    // Break recursion loops
    if (ActiveEvent == &event)
        return false;

    ActiveEvent = &event;

    bool ret;
    if ( !m_childView || ! m_childView->ProcessEvent(event) )
    {
        // Only hand up to the parent if it's a menu command
        if (!event.IsKindOf(CLASSINFO(wxCommandEvent)) || !GetParent() || !GetParent()->GetEventHandler()->ProcessEvent(event))
            ret = wxEvtHandler::ProcessEvent(event);
        else
            ret = true;
    }
    else
        ret = true;

    ActiveEvent = NULL;
    return ret;
}

void wxAuiDocMDIChildFrame::OnMenuHighlight(wxMenuEvent& event)
{
    wxAuiMDIChildFrame::OnMenuHighlight(event);
}

void wxAuiDocMDIChildFrame::OnActivate(wxActivateEvent& event)
{
    wxAuiMDIChildFrame::OnActivate(event);
    
    if (!event.GetActive() && m_childView)
        m_childView->Activate(event.GetActive());

    if (event.GetActive() && m_childView)
        m_childView->Activate(event.GetActive());
}

void wxAuiDocMDIChildFrame::OnCloseWindow(wxCloseEvent& event)
{
    // Close view but don't delete the frame while doing so!
    // ...since it will be deleted by wxWidgets if we return true.
    if (m_childView)
    {
        bool ans = event.CanVeto()
            ? m_childView->Close(false) // false means don't delete associated window
            : true; // Must delete.

        if (ans)
        {
            m_childView->Activate(false);
            delete m_childView;
            m_childView = (wxView *) NULL;
            m_childDocument = (wxDocument *) NULL;

            wxAuiMDIChildFrame::OnCloseWindow(event);
        }
        else
            event.Veto();
    }
    else
        event.Veto();
}

void wxAuiDocMDIChildFrame::SetMDIParentFrame(wxAuiDocMDIParentFrame* parentFrame)
{
    wxAuiMDIChildFrame::SetMDIParentFrame(wxDynamicCast(parentFrame, wxAuiMDIParentFrame));
}

wxAuiDocMDIParentFrame* wxAuiDocMDIChildFrame::GetMDIParentFrame() const
{
    return wxDynamicCast(wxAuiMDIChildFrame::GetMDIParentFrame(), wxAuiDocMDIParentFrame);
}

#endif
    //wxUSE_AUI
#endif
    // wxUSE_MDI

