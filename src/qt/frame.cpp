/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/frame.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2009 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/menu.h"
    #include "wx/toolbar.h"
#endif // WX_PRECOMP

#include "wx/frame.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QScrollArea>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>

class wxQtMainWindow : public wxQtEventSignalHandler< QMainWindow, wxFrame >
{
public:
    wxQtMainWindow( wxWindow *parent, wxFrame *handler );

private:
    virtual bool focusNextPrevChild(bool) wxOVERRIDE { return false; }
};

// Central widget helper (container to show scroll bars and receive events):

class wxQtCentralWidget : public wxQtEventSignalHandler< QScrollArea, wxFrame >
{
    public:
        wxQtCentralWidget( wxWindow *parent, wxFrame *handler );
};


wxFrame::~wxFrame()
{
    // central widget should be deleted by qt when the main window is destroyed
    QtStoreWindowPointer( GetQMainWindow()->centralWidget(), NULL );
}

bool wxFrame::Create( wxWindow *parent, wxWindowID id, const wxString& title,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name )
{
    m_qtWindow = new wxQtMainWindow( parent, this );

    // TODO: Could we use a wxPanel as the central widget? If so then we could
    // remove wxWindow::QtReparent.

    GetQMainWindow()->setCentralWidget( new wxQtCentralWidget( parent, this ) );

    if ( !wxFrameBase::Create( parent, id, title, pos, size, style, name ) )
        return false;

    PostCreation();
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

void wxFrame::SetStatusBar( wxStatusBar *statusBar )
{
    // The current status bar could be deleted by Qt when dereferencing it
    // TODO: add a mechanism like Detach in menus to avoid issues
    if ( statusBar != NULL )
    {
        GetQMainWindow()->setStatusBar( statusBar->GetQStatusBar() );
        // Update statusbar sizes now that it has a size
        statusBar->Refresh();
    }
    else
    {
        // Remove the current status bar
        GetQMainWindow()->setStatusBar(NULL);
    }
    wxFrameBase::SetStatusBar( statusBar );
}

void wxFrame::SetToolBar(wxToolBar *toolbar)
{
    if ( toolbar != NULL )
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
    else if ( m_frameToolBar != NULL )
    {
        GetQMainWindow()->removeToolBar(m_qtToolBar);
        m_qtToolBar = NULL;
    }
    wxFrameBase::SetToolBar( toolbar );
}

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

QScrollArea *wxFrame::QtGetScrollBarsContainer() const
{
    return dynamic_cast <QScrollArea *> (GetQMainWindow()->centralWidget() );
}

void wxFrame::DoGetClientSize(int *width, int *height) const
{
    wxWindow::DoGetClientSize(width, height);

    // Adjust the height, taking the status and menu bars into account, if any:
    if ( height )
    {
        if ( wxStatusBar *sb = GetStatusBar() )
        {
            *height -= sb->GetSize().y;
        }


        if ( QWidget *qmb = GetQMainWindow()->menuWidget() )
        {
            *height -= qmb->geometry().height();
        }
    }
}

void wxFrame::DoSetClientSize(int width, int height)
{
    wxWindow::DoSetClientSize(width, height);

    int adjustedWidth, adjustedHeight;
    DoGetClientSize(&adjustedWidth, &adjustedHeight);

    QWidget *centralWidget = GetQMainWindow()->centralWidget();
    QRect geometry = centralWidget->geometry();
    geometry.setSize(QSize(adjustedWidth, adjustedHeight));
    centralWidget->setGeometry(geometry);
}

QMainWindow *wxFrame::GetQMainWindow() const
{
    return static_cast<QMainWindow*>(m_qtWindow);
}

//=============================================================================

wxQtMainWindow::wxQtMainWindow( wxWindow *parent, wxFrame *handler )
    : wxQtEventSignalHandler< QMainWindow, wxFrame >( parent, handler )
{
//    setCentralWidget( new wxQtWidget( parent, handler ));
}

wxQtCentralWidget::wxQtCentralWidget( wxWindow *parent, wxFrame *handler )
    : wxQtEventSignalHandler< QScrollArea, wxFrame >( parent, handler )
{
    setFocusPolicy(Qt::NoFocus);
}
