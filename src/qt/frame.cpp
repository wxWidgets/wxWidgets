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


wxFrame::wxFrame()
{
}

wxFrame::wxFrame( wxWindow *parent, wxWindowID id, const wxString& title,
        const wxPoint& pos, const wxSize& size, long style, const wxString& name )
{
    Create( parent, id, title, pos, size, style, name );
}

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
    int area = 0;
    if ( toolbar != NULL )
    {
        if (toolbar->HasFlag(wxTB_LEFT))   area |= Qt::LeftToolBarArea;
        if (toolbar->HasFlag(wxTB_RIGHT))  area |= Qt::RightToolBarArea;
        if (toolbar->HasFlag(wxTB_TOP))    area |= Qt::TopToolBarArea;
        if (toolbar->HasFlag(wxTB_BOTTOM)) area |= Qt::BottomToolBarArea;

        GetQMainWindow()->addToolBar((Qt::ToolBarArea)area, toolbar->GetQToolBar());
    }
    else if ( m_frameToolBar != NULL )
    {
        GetQMainWindow()->removeToolBar(m_frameToolBar->GetQToolBar());
    }
    wxFrameBase::SetToolBar( toolbar );
}

void wxFrame::SetWindowStyleFlag( long style )
{
    wxWindow::SetWindowStyleFlag( style );

    Qt::WindowFlags qtFlags = GetQMainWindow()->windowFlags();

    if ( HasFlag( wxFRAME_TOOL_WINDOW ) )
    {
        qtFlags &= ~Qt::WindowType_Mask;
        qtFlags |= Qt::Tool;
    }

    GetQMainWindow()->setWindowFlags( qtFlags );
}

void wxFrame::AddChild( wxWindowBase *child )
{
    // Make sure all children are children of the central widget:

    QtReparent( child->GetHandle(), GetQMainWindow()->centralWidget() );

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
}
