/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/mdi.cpp
// Author:      Mariano Reingart, Peter Most
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_MDI

#include "wx/mdi.h"
#include "wx/stockitem.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMdiSubWindow>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>

namespace
{
static QMdiSubWindow* gs_qtActiveSubWindow = nullptr;
}

/*static*/
wxMDIParentFrame::Layout wxMDIParentFrame::ms_layout =
#if defined(__WINDOWS__)
    Layout::MDI;
#else // !__WINDOWS__
    Layout::Tabbed;
#endif // __WINDOWS__

// Central widget helper (provides an area in which MDI windows are displayed):

class wxQtMdiArea : public wxQtEventSignalHandler< QMdiArea, wxMDIClientWindow >
{
public:
    wxQtMdiArea(wxWindow *parent, wxMDIClientWindow *handler);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxMDIParentFrame, wxFrame);

wxMDIParentFrame::wxMDIParentFrame(wxWindow *parent,
                 wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style,
                 const wxString& name)
{
    Create(parent, id, title, pos, size, style, name);
}

wxMDIParentFrame::~wxMDIParentFrame()
{
    // Avoid double deletion by preventing the parent window from deleting
    // m_windowMenu which will be deleted by this frame's menubar.
    m_windowMenu = nullptr;

    delete m_clientWindow;
}

bool wxMDIParentFrame::Create(wxWindow *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
{
    if ( !wxFrame::Create(parent, id, title, pos, size, style, name) )
        return false;

    wxMDIClientWindow* client = OnCreateClient();
    m_clientWindow = client;

    if ( !m_clientWindow->CreateClient(this, GetWindowStyleFlag()) )
        return false;

    // Replace the central widget set in wxFrame with our wxQtMdiArea.
    GetQMainWindow()->setCentralWidget( client->GetHandle() );

    QtSetPreferredDILayout(ms_layout);

    return true;
}

void wxMDIParentFrame::QtSetPreferredDILayout(Layout layout)
{
    ms_layout = layout;

    QMdiArea::ViewMode viewMode;

    switch ( layout )
    {
    case Layout::MDI:
        viewMode = QMdiArea::SubWindowView;
        break;

    case Layout::Tabbed:
        viewMode = QMdiArea::TabbedView;
        break;

    default:
        viewMode = QMdiArea::SubWindowView;
        wxFAIL_MSG("Unknown layout passed to QtSetPreferredDILayout()");
        break;
    }

    GetQtMdiArea()->setViewMode(viewMode);
}

QMdiArea* wxMDIParentFrame::GetQtMdiArea() const
{
    return static_cast<QMdiArea*>(m_clientWindow->GetHandle());
}

void wxMDIParentFrame::Cascade()
{
    GetQtMdiArea()->cascadeSubWindows();
}

void wxMDIParentFrame::Tile(wxOrientation WXUNUSED(orient))
{
    GetQtMdiArea()->tileSubWindows();
}

void wxMDIParentFrame::ActivateNext()
{
    GetQtMdiArea()->activateNextSubWindow();
}

void wxMDIParentFrame::ActivatePrevious()
{
    GetQtMdiArea()->activatePreviousSubWindow();
}

void wxMDIParentFrame::OnMDICommand(wxCommandEvent& event)
{
    switch ( event.GetId() )
    {
    case wxID_MDI_WINDOW_CASCADE:
        Cascade();
        break;

    case wxID_MDI_WINDOW_TILE_HORZ:
        wxFALLTHROUGH;
    case wxID_MDI_WINDOW_TILE_VERT:
        Tile();
        break;

    case wxID_MDI_WINDOW_NEXT:
        ActivateNext();
        break;

    case wxID_MDI_WINDOW_PREV:
        ActivatePrevious();
        break;

    default:
        wxFAIL_MSG("unknown MDI command");
        break;
    }
}

int wxMDIParentFrame::GetChildFramesCount() const
{
    const auto parent = GetClientWindow(); // The parent of our wxMDIChildFrames

    int count = 0;
    for ( wxWindowList::const_iterator i = parent->GetChildren().begin();
          i != parent->GetChildren().end();
          ++i )
    {
        if ( wxDynamicCast(*i, wxMDIChildFrame) )
            count++;
    }

    return count;
}

#if wxUSE_MENUS
void wxMDIParentFrame::AddMDIChild(wxMDIChildFrame* WXUNUSED(child))
{
    switch ( GetChildFramesCount() )
    {
        case 1:
            // first MDI child was just added, we need to insert the window
            // menu now if we have it
            AddWindowMenu();

            // and disable the items which can't be used until we have more
            // than one child
            UpdateWindowMenu(false);
            break;

        case 2:
            // second MDI child was added, enable the menu items which were
            // disabled because they didn't make sense for a single window
            UpdateWindowMenu(true);
            break;
    }
}

void wxMDIParentFrame::RemoveMDIChild(wxMDIChildFrame* WXUNUSED(child))
{
    switch ( GetChildFramesCount() )
    {
        case 1:
            // last MDI child is being removed, remove the now unnecessary
            // window menu too
            RemoveWindowMenu();

            // wxWidgets just deletes the windows contained in m_qtSubWindows
            // when closing all frames from the "clear" menu. So we need to
            // close them explicitly with closeAllSubWindows().
            GetQtMdiArea()->closeAllSubWindows();

            gs_qtActiveSubWindow = nullptr;

            // there is no need to call UpdateWindowMenu(true) here so this is
            // not quite symmetric to AddMDIChild() above
            break;

        case 2:
            // only one MDI child is going to remain, disable the menu commands
            // which don't make sense for a single child window
            UpdateWindowMenu(false);
            break;
    }
}

void wxMDIParentFrame::AddWindowMenu()
{
    // this style can be used to prevent a window from having the standard MDI
    // "Window" menu
    if ( !(GetWindowStyleFlag() & wxFRAME_NO_WINDOW_MENU) && !m_windowMenu )
    {
        m_windowMenu = new wxMenu;

        // Qt offers only "Tile" without specifying any direction, so just
        // reuse one of the predifined ids.

        if ( ms_layout == Layout::MDI )
        {
            // Qt offers only "Tile" without specifying any direction, so just
            // reuse one of the predifined ids.

            m_windowMenu->Append(wxID_MDI_WINDOW_CASCADE, _("&Cascade"));
            m_windowMenu->Append(wxID_MDI_WINDOW_TILE_HORZ, _("&Tile"));
            m_windowMenu->AppendSeparator();
        }

        m_windowMenu->Append(wxID_MDI_WINDOW_NEXT, _("&Next"));
        m_windowMenu->Append(wxID_MDI_WINDOW_PREV, _("&Previous"));

        Bind(wxEVT_MENU, &wxMDIParentFrame::OnMDICommand, this,
             wxID_MDI_WINDOW_FIRST, wxID_MDI_WINDOW_LAST);
    }
}

void wxMDIParentFrame::RemoveWindowMenu()
{
    if ( m_windowMenu )
    {
        auto pos = GetMenuBar()->FindMenu(m_windowMenu->GetTitle());
        if ( pos != wxNOT_FOUND )
        {
            GetMenuBar()->Remove(pos);
        }

        delete m_windowMenu;
        m_windowMenu = nullptr;
    }
}

void wxMDIParentFrame::UpdateWindowMenu(bool enable)
{
    if ( m_windowMenu )
    {
        m_windowMenu->Enable(wxID_MDI_WINDOW_NEXT, enable);
        m_windowMenu->Enable(wxID_MDI_WINDOW_PREV, enable);
    }
}
#endif // wxUSE_MENUS

//##############################################################################

wxIMPLEMENT_DYNAMIC_CLASS(wxMDIChildFrame, wxMDIChildFrameBase)

wxMDIChildFrame::wxMDIChildFrame(wxMDIParentFrame *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos,
                const wxSize& size,
                long style,
                const wxString& name)
{
    Create(parent, id, title, pos, size, style, name);
}

wxMDIChildFrame::~wxMDIChildFrame()
{
    if ( gs_qtActiveSubWindow == m_qtSubWindow )
    {
        // wxWidgets just deletes the window contained in m_qtSubWindow
        // when Closing this frame from the menu or by Ctrl+W. So we need
        // to close it explicitly with closeActiveSubWindow().
        m_mdiParent->GetQtMdiArea()->closeActiveSubWindow();
        m_mdiParent->SetActiveChild(nullptr);
        gs_qtActiveSubWindow = nullptr;
    }

    m_mdiParent->RemoveMDIChild(this);

    delete m_menuBar;
}

bool wxMDIChildFrame::Create(wxMDIParentFrame *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
{
    if ( !wxFrameBase::Create(parent->GetClientWindow(), id,
                              title, pos, size, style, name) )
        return false;

    m_mdiParent = parent;

    // Add this child frame to the internal MDI client area:
    m_qtSubWindow = m_mdiParent->GetQtMdiArea()->addSubWindow(GetHandle());

    if ( size != wxDefaultSize )
        m_qtSubWindow->setMinimumSize(wxQtConvertSize(size));

    QObject::connect(m_qtSubWindow, &QMdiSubWindow::aboutToActivate, [this]()
        {
            // m_qtSubWindow will be the active window, so we need to restore m_mdiParent's
            // menubar stored in the previouse active one "gs_qtActiveSubWindow".
            if ( m_qtSubWindow != gs_qtActiveSubWindow && gs_qtActiveSubWindow )
            {
                auto win = wxWindowQt::QtRetrieveWindowPointer(gs_qtActiveSubWindow->widget());
                static_cast<wxMDIChildFrame*>(win)->InternalSetMenuBar();
            }
        });

    QObject::connect(m_mdiParent->GetQtMdiArea(), &QMdiArea::subWindowActivated,
        [this](QMdiSubWindow* window)
        {
            QWidget* qtWidget = nullptr;

            bool activated = false;

            if ( window )
            {
                if ( window != gs_qtActiveSubWindow )
                {
                    // window is the active window and it's menubar will be attached to m_mdiParent
                    // with InternalSetMenuBar() call below.
                    qtWidget = window->widget();

                    activated = true;
                }
            }
            else if ( gs_qtActiveSubWindow )
            {
                // the old active window "gs_qtActiveSubWindow" should restore the m_mdiParent's
                // menubar with InternalSetMenuBar() call below.
                qtWidget = gs_qtActiveSubWindow->widget();
            }

            if ( qtWidget )
            {
                const auto win = wxWindowQt::QtRetrieveWindowPointer(qtWidget);
                const auto childFrame = static_cast<wxMDIChildFrame*>(win);

                childFrame->InternalSetMenuBar();

                childFrame->GetMDIParent()->SetActiveChild(activated ? childFrame : nullptr);

                wxActivateEvent event(wxEVT_ACTIVATE, activated, win->GetId());
                event.SetEventObject(win);

                win->HandleWindowEvent(event);
            }

            gs_qtActiveSubWindow = window;
        });

    m_mdiParent->AddMDIChild(this);

    return true;
}

void wxMDIChildFrame::SetWindowStyleFlag(long style)
{
    wxWindow::SetWindowStyleFlag(style);
}

void wxMDIChildFrame::Activate()
{
    m_mdiParent->GetQtMdiArea()->setActiveSubWindow(m_qtSubWindow);
    m_mdiParent->SetActiveChild(this);
}

void wxMDIChildFrame::SetMenuBar(wxMenuBar* menuBar)
{
    if ( m_menuBar )
    {
        if ( m_mdiParent->GetQtMdiArea()->activeSubWindow() == m_qtSubWindow )
        {
            // This window is the active one and it's menubar is attached to
            // m_mdiParent window. Restore the parent's menubar by calling
            // InternalSetMenuBar() before deleting the old m_menuBar.

            InternalSetMenuBar();

            delete m_menuBar;
        }
    }

    m_menuBar = menuBar;

    // Don't call wxFrameBase::SetMenuBar() here because m_menuBar will be
    // attached to m_mdiParent later when this child frame becomes active.
}

void wxMDIChildFrame::InternalSetMenuBar()
{
    wxCHECK_RET(m_mdiParent, "Invalid MDI parent window");

    if ( !m_menuBar )
    {
        // This child frame doesn't have a menubar.
        return;
    }

    auto oldMenuBar = m_mdiParent->GetMenuBar();

    if ( m_menuBar != oldMenuBar )
    {
        m_mdiParent->SetMenuBar(m_menuBar);

        AttachWindowMenuTo(m_menuBar, oldMenuBar);

        m_menuBar->Show(); // Show the attached menubar
        m_menuBar = oldMenuBar;
        m_menuBar->Hide(); // Hide the detached menubar
    }
}

void wxMDIChildFrame::AttachWindowMenuTo(wxMenuBar* attachedMenuBar,
                                         wxMenuBar* detachedMenuBar)
{
    const wxString windowMenuTitle = _("&Window");

    if ( detachedMenuBar )
    {
        auto pos = detachedMenuBar->FindMenu(windowMenuTitle);
        if ( pos != wxNOT_FOUND )
        {
            detachedMenuBar->Remove(pos);
        }
    }

    auto windowMenu = m_mdiParent->GetWindowMenu();

    if ( windowMenu && attachedMenuBar )
    {
        auto pos = attachedMenuBar->FindMenu(wxGetStockLabel(wxID_HELP));
        if ( pos != wxNOT_FOUND )
        {
            attachedMenuBar->Insert(pos, windowMenu, windowMenuTitle);
        }
    }
}

//##############################################################################

wxIMPLEMENT_DYNAMIC_CLASS(wxMDIClientWindow, wxMDIClientWindowBase)

bool wxMDIClientWindow::CreateClient(wxMDIParentFrame *parent, long WXUNUSED(style))
{
    // create the MDI client area where the children window are displayed:
    m_qtWindow = new wxQtMdiArea(parent, this);
    // The cast is safe and returns a non-null pointer because wxQtMdiArea derives
    // from QMdiArea which in turn derives from QAbstractScrollArea.
    m_qtContainer = static_cast<QAbstractScrollArea*>(m_qtWindow);
    return true;
}

// Helper implementation:

wxQtMdiArea::wxQtMdiArea(wxWindow *parent, wxMDIClientWindow *handler )
    : wxQtEventSignalHandler< QMdiArea, wxMDIClientWindow >( parent, handler )
{
}

#endif // wxUSE_MDI
