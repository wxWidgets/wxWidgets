/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/frame.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2009 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/menu.h"
    #include "wx/toolbar.h"
#endif // WX_PRECOMP

#include "wx/frame.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>

class wxQtMainWindow : public wxQtEventSignalHandler< QMainWindow, wxFrame >
{
public:
    wxQtMainWindow( wxWindow *parent, wxFrame *handler )
        : wxQtEventSignalHandler< QMainWindow, wxFrame >( parent, handler )
    {
    }

private:
    virtual bool focusNextPrevChild(bool) override { return false; }
};

// Central widget helper (container which receives events):

class wxQtCentralWidget : public wxQtEventSignalHandler< QWidget, wxFrame >
{
public:
    wxQtCentralWidget( wxWindow *parent, wxFrame *handler )
        : wxQtEventSignalHandler< QWidget, wxFrame >( parent, handler )
    {
        setFocusPolicy(Qt::NoFocus);
    }
};


wxFrame::~wxFrame()
{
    // Always false for MDI children
    if ( GetQMainWindow() && GetQMainWindow()->centralWidget() )
    {
        // central widget should be deleted by qt when the main window is destroyed
        QtStoreWindowPointer(GetQMainWindow()->centralWidget(), nullptr);
    }
}

bool wxFrame::Create( wxWindow *parent, wxWindowID id, const wxString& title,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name )
{
    m_qtWindow = new wxQtMainWindow( parent, this );

    // Not using QScrollArea or wxPanel is intentional here as it makes the
    // implementation simpler and more manageable.
    //
    // Quoting the Qt docs [QMainWindow::setCentralWidget()]:
    // QMainWindow takes ownership of the widget pointer and deletes it at
    // the appropriate time.
    //
    GetQMainWindow()->setCentralWidget( new wxQtCentralWidget( this, this ) );

    if ( !wxFrameBase::Create( parent, id, title, pos, size, style, name ) )
    {
        return false;
    }

    SetWindowStyleFlag(style);

    return true;
}

void wxFrame::SetMenuBar( wxMenuBar *menuBar )
{
    if ( menuBar )
    {
        // To prevent QtMainWindow from deleting the old QMenuBar pointer (so that
        // the old wxMenuBar can be reused, which is the case in the MDI framework
        // for example) we reparent the old QMenuBar before calling setMenuBar().
        // Also note that we call menuWidget() and not menuBar() because the latter
        // creates a new one if one doesn't exist before.

        auto oldMenuBar = GetQMainWindow()->menuWidget();
        if ( oldMenuBar )
            oldMenuBar->setParent(nullptr);

        GetQMainWindow()->setMenuBar(menuBar->GetQMenuBar());
    }
    else
    {
        GetQMainWindow()->setMenuBar(nullptr);
    }

    wxFrameBase::SetMenuBar(menuBar);
}

#if wxUSE_STATUSBAR
void wxFrame::SetStatusBar( wxStatusBar *statusBar )
{
    // The current status bar could be deleted by Qt when dereferencing it
    // TODO: add a mechanism like Detach in menus to avoid issues
    if ( statusBar != nullptr )
    {
        GetQMainWindow()->setStatusBar( statusBar->GetQStatusBar() );
    }
    else
    {
        // Remove the current status bar
        GetQMainWindow()->setStatusBar(nullptr);
    }
    wxFrameBase::SetStatusBar( statusBar );
}
#endif // wxUSE_STATUSBAR

#if wxUSE_TOOLBAR
void wxFrame::SetToolBar(wxToolBar *toolbar)
{
    if ( toolbar != nullptr )
    {
        int area = 0;
        if      (toolbar->HasFlag(wxTB_LEFT))  { area = Qt::LeftToolBarArea;  }
        else if (toolbar->HasFlag(wxTB_RIGHT)) { area = Qt::RightToolBarArea; }
        else if (toolbar->HasFlag(wxTB_BOTTOM)){ area = Qt::BottomToolBarArea;}
        else { area = Qt::TopToolBarArea;   }

        // We keep the current toolbar handle in our own member variable
        // because we can't get it from half-destroyed wxToolBar when it calls
        // this function from wxToolBarBase dtor.
        m_qtToolBar = toolbar->GetQToolBar();

        GetQMainWindow()->addToolBar((Qt::ToolBarArea)area, m_qtToolBar);
    }
    else if ( m_frameToolBar != nullptr )
    {
        GetQMainWindow()->removeToolBar(m_qtToolBar);
        m_qtToolBar = nullptr;
    }
    wxFrameBase::SetToolBar( toolbar );
}
#endif // wxUSE_TOOLBAR

void wxFrame::SetWindowStyleFlag( long style )
{
    wxWindow::SetWindowStyleFlag( style );

    Qt::WindowFlags qtFlags = Qt::CustomizeWindowHint;

    if ( HasFlag( wxFRAME_TOOL_WINDOW ) )
    {
        qtFlags |= Qt::Tool;
    }
    else
    {
        qtFlags |= Qt::Window;
    }

    if ( HasFlag(wxCAPTION) )
    {
        qtFlags |= Qt::WindowTitleHint;
    }

    if ( HasFlag(wxSYSTEM_MENU) )
    {
        qtFlags |= Qt::WindowSystemMenuHint;
    }

    if ( HasFlag(wxSTAY_ON_TOP) )
    {
        qtFlags |= Qt::WindowStaysOnTopHint;
    }

    if ( HasFlag(wxMINIMIZE_BOX) )
    {
        qtFlags |= Qt::WindowMinimizeButtonHint;
    }

    if ( HasFlag(wxMAXIMIZE_BOX) )
    {
        qtFlags |= Qt::WindowMaximizeButtonHint;
    }

    if ( HasFlag(wxCLOSE_BOX) )
    {
        qtFlags |= Qt::WindowCloseButtonHint;
    }

    if ( HasFlag(wxNO_BORDER) )
    {
        // Note any of the other window decoration hints (e.g.
        // Qt::WindowCloseButtonHint, Qt::WindowTitleHint) override this style.
        // It doesn't seem possible to create a QMainWindow with a title bar
        // but without a resize border.
        qtFlags |= Qt::FramelessWindowHint;
    }

    GetQMainWindow()->setWindowFlags(qtFlags);
}

void wxFrame::SetWindowModality(Modality modality)
{
    wxCHECK_RET( !IsShown(),
                 "SetWindowModality() must be called before showing the window" );

    Qt::WindowModality qtModality;

    switch ( modality )
    {
        case Modality::App:
            qtModality = Qt::ApplicationModal;
            break;

        case Modality::Window:
            qtModality = Qt::WindowModal;
            break;

        case Modality::None:
            qtModality = Qt::NonModal;
            break;
    }

    GetHandle()->setWindowModality(qtModality);
}

QWidget* wxFrame::QtGetParentWidget() const
{
    // GetQMainWindow() always returns nullptr for MDI children
    return GetQMainWindow() ? GetQMainWindow()->centralWidget() : GetHandle();
}

void wxFrame::AddChild( wxWindowBase *child )
{
    // Make sure all children are children of the central widget:

    QtReparent( child->GetHandle(), QtGetParentWidget() );

    wxFrameBase::AddChild( child );
}

void wxFrame::RemoveChild( wxWindowBase *child )
{
    wxFrameBase::RemoveChild( child );
}

// get the origin of the client area in the client coordinates
// excluding any menubar and toolbar if any.
wxPoint wxFrame::GetClientAreaOrigin() const
{
    // GetQMainWindow() always returns nullptr for MDI children
    if ( GetQMainWindow() )
    {
        return wxQtConvertPoint( GetQMainWindow()->centralWidget()->pos() );
    }

    return wxWindow::GetClientAreaOrigin();
}

QMainWindow *wxFrame::GetQMainWindow() const
{
    // Notice that we intentionally use qobject_cast<> here (and not static_cast<>)
    // because when this function is called on an object of wxMDIChildFrame (which
    // derives from this class) m_qtWindow is not a QMainWindow, and we want to
    // return nullptr in this case.

    return qobject_cast<QMainWindow*>(m_qtWindow);
}
