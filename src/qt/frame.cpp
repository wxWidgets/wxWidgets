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
    // central widget should be deleted by qt when the main window is destroyed
    QtStoreWindowPointer( GetQMainWindow()->centralWidget(), nullptr );
}

bool wxFrame::Create( wxWindow *parent, wxWindowID id, const wxString& title,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name )
{
    m_qtWindow = new wxQtMainWindow( parent, this );

    // QMainWindow takes ownership of the central widget pointer.
    // Not using QScrollArea or wxPanel is intentional here as it makes the
    // implementation simpler and more manageable.
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
        // The current menu bar could be deleted by Qt when dereferencing it so
        // then that QMenuBar will raise a segmentation fault when using it again
        wxCHECK_RET( menuBar->GetHandle(),
                     "Using a replaced menu bar is not supported in wxQT");
        // Warning: Qt main window takes ownership of the QMenuBar pointer:
        GetQMainWindow()->setMenuBar( menuBar->GetQMenuBar() );
    }
    else
    {
        // Creating an empty menu bar should hide it and free the previous:
        QMenuBar *qmenubar = new QMenuBar(GetHandle());
        GetQMainWindow()->setMenuBar( qmenubar );
    }
    wxFrameBase::SetMenuBar( menuBar );
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

QWidget* wxFrame::QtGetParentWidget() const
{
    return GetQMainWindow()->centralWidget();
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
    return wxQtConvertPoint( GetQMainWindow()->centralWidget()->pos() );
}

QMainWindow *wxFrame::GetQMainWindow() const
{
    return static_cast<QMainWindow*>(m_qtWindow);
}
