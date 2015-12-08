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

#include <QtWidgets/QScrollArea>

#ifndef WX_PRECOMP
    #include "wx/menu.h"
    #include "wx/toolbar.h"
#endif // WX_PRECOMP

#include "wx/frame.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"


class wxQtMainWindow : public wxQtEventSignalHandler< QMainWindow, wxFrame >
{
public:
    wxQtMainWindow( wxWindow *parent, wxFrame *handler );

private:
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
    QtStoreWindowPointer( GetHandle()->centralWidget(), NULL );
}

bool wxFrame::Create( wxWindow *parent, wxWindowID id, const wxString& title,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name )
{
    m_qtWindow = new wxQtMainWindow( parent, this );

    // TODO: Could we use a wxPanel as the central widget? If so then we could
    // remove wxWindow::QtReparent.

    GetHandle()->setCentralWidget( new wxQtCentralWidget( parent, this ) );

    PostCreation();

    return wxFrameBase::Create( parent, id, title, pos, size, style, name );
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
        GetHandle()->setMenuBar( menuBar->GetHandle() );
    }
    else
    {
        // Creating an empty menu bar should hide it and free the previous:
        QMenuBar *qmenubar = new QMenuBar(GetHandle());
        GetHandle()->setMenuBar( qmenubar );
    }
    wxFrameBase::SetMenuBar( menuBar );
}

void wxFrame::SetStatusBar( wxStatusBar *statusBar )
{
    // The current status bar could be deleted by Qt when dereferencing it
    // TODO: add a mechanism like Detach in menus to avoid issues
    if ( statusBar != NULL )
    {
        GetHandle()->setStatusBar( statusBar->GetHandle() );
        // Update statusbar sizes now that it has a size
        statusBar->Refresh();
    }
    else
    {
        // Remove the current status bar
        GetHandle()->setStatusBar(NULL);
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

        GetHandle()->addToolBar((Qt::ToolBarArea)area, toolbar->GetHandle());
    }
    else if ( m_frameToolBar != NULL )
    {
        GetHandle()->removeToolBar(m_frameToolBar->GetHandle());
    }
    wxFrameBase::SetToolBar( toolbar );
}

void wxFrame::SetWindowStyleFlag( long style )
{
    wxWindow::SetWindowStyleFlag( style );

    Qt::WindowFlags qtFlags = GetHandle()->windowFlags();

    if ( HasFlag( wxFRAME_TOOL_WINDOW ) )
    {
        qtFlags &= ~Qt::WindowType_Mask;
        qtFlags |= Qt::Tool;
    }

    GetHandle()->setWindowFlags( qtFlags );
}

void wxFrame::AddChild( wxWindowBase *child )
{
    // Make sure all children are children of the central widget:

    QtReparent( child->GetHandle(), GetHandle()->centralWidget() );

    wxFrameBase::AddChild( child );
}

void wxFrame::RemoveChild( wxWindowBase *child )
{
    wxFrameBase::RemoveChild( child );
}

QAbstractScrollArea *wxFrame::QtGetScrollBarsContainer() const
{
    return dynamic_cast <QAbstractScrollArea *> (GetHandle()->centralWidget() );
}

void wxFrame::DoGetClientSize(int *width, int *height) const
{
    wxWindow::DoGetClientSize(width, height);

    // for a status bar, we must subtract it's height here
    wxStatusBar *sb = GetStatusBar();
    if (height && sb)
    {
        int sbh = 0;
        sb->GetSize(NULL, &sbh);
        *height -= sbh;
    }
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
