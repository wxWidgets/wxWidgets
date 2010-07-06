/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/frame.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/frame.h"
#include "wx/menu.h"
#include "wx/qt/converter.h"
#include <QtGui/QResizeEvent>

wxFrame::wxFrame()
{
}

wxFrame::wxFrame( wxWindow *parent, wxWindowID id, const wxString& title,
        const wxPoint& pos, const wxSize& size, long style, const wxString& name )
{
    Create( parent, id, title, pos, size, style, name );
}

bool wxFrame::Create( wxWindow *parent, wxWindowID id, const wxString& title,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name )
{
    // Create the Qt frame first so that GetHandle() works in the base classes:

    QWidget *qtParent = NULL;
    if ( parent != NULL ) {
        qtParent = parent->GetContainer();
        parent->AddChild(this);
    }

    m_qtFrame = new wxQtFrame( this, qtParent );

    return wxFrameBase::Create( parent, id, title, pos, size, style, name );
}

void wxFrame::SetMenuBar( wxMenuBar *menuBar )
{
    m_qtFrame->setMenuBar( menuBar->GetHandle() );

    wxFrameBase::SetMenuBar( menuBar );
}

void wxFrame::SetStatusBar( wxStatusBar *statusBar )
{
    m_qtFrame->setStatusBar( statusBar->GetHandle() );

    // Update statusbar sizes now that it has a size
    statusBar->Refresh();
    
    wxFrameBase::SetStatusBar( statusBar );
}

QMainWindow *wxFrame::GetHandle() const
{
    return m_qtFrame;
}

QWidget *wxFrame::GetContainer() const
{
    return m_qtFrame->centralWidget();
}

//=============================================================================

wxQtFrame::wxQtFrame( wxFrame *frame, QWidget *parent )
    : WindowEventForwarder< QMainWindow >( parent )
{
    m_frame = frame;
    setCentralWidget(new QWidget());
}

wxWindow *wxQtFrame::GetEventReceiver()
{
    return m_frame;
}
