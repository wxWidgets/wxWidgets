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

    return true;
}

QMdiArea* wxMDIParentFrame::GetQtMdiArea() const
{
    return static_cast<QMdiArea*>(m_clientWindow->GetHandle());
}

void wxMDIParentFrame::ActivateNext()
{
}

void wxMDIParentFrame::ActivatePrevious()
{
}

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
        m_mdiParent->GetQtMdiArea()->closeActiveSubWindow();
        m_mdiParent->SetActiveChild(nullptr);
        gs_qtActiveSubWindow = nullptr;
    }

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
            }

            gs_qtActiveSubWindow = window;
        });

    return true;
}

void wxMDIChildFrame::SetWindowStyleFlag(long style)
{
    wxWindow::SetWindowStyleFlag(style);
}

void wxMDIChildFrame::Activate()
{
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

    auto oldMenuBar = m_mdiParent->GetMenuBar();

    if ( m_menuBar != oldMenuBar )
    {
        m_mdiParent->SetMenuBar(m_menuBar);

        m_menuBar->Show(); // Show the attached menubar
        m_menuBar = oldMenuBar;
        m_menuBar->Hide(); // Hide the detached menubar
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
