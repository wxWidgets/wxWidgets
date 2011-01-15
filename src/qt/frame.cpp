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
#include "wx/qt/utils.h"
#include "wx/qt/converter.h"
#include "wx/qt/window_qt.h"

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
    SendDestroyEvent();
}

bool wxFrame::Create( wxWindow *parent, wxWindowID id, const wxString& title,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name )
{
    m_qtMainWindow = new wxQtMainWindow( parent, this );

    // TODO: Could we use a wxPanel as the central widget? If so then we could
    // remove wxWindow::QtReparent.

    m_qtMainWindow->setCentralWidget( new QWidget() );

    return wxFrameBase::Create( parent, id, title, pos, size, style, name );
}

void wxFrame::SetMenuBar( wxMenuBar *menuBar )
{
    m_qtMainWindow->setMenuBar( menuBar->GetHandle() );

    wxFrameBase::SetMenuBar( menuBar );
}

void wxFrame::SetStatusBar( wxStatusBar *statusBar )
{
    m_qtMainWindow->setStatusBar( statusBar->GetHandle() );

    // Update statusbar sizes now that it has a size
    statusBar->Refresh();

    wxFrameBase::SetStatusBar( statusBar );
}

void wxFrame::SetWindowStyleFlag( long style )
{
//    wxWindow::SetWindowStyleFlag( style );
//
//    Qt::WindowFlags qtFlags = GetHandle()->windowFlags();
//
//    wxCHECK_RET( !HasFlag( wxFRAME_FLOAT_ON_PARENT ) && !HasFlag( wxTINY_CAPTION ) ,
//                 "wxFRAME_FLOAT_ON_PARENT, wxTINY_CAPTION not supported. Use wxFRAME_TOOL_WINDOW instead." );
//    wxCHECK_RET( !HasFlag( wxFRAME_TOOL_WINDOW ) || HasFlag( wxFRAME_NO_TASKBAR ) ,
//                 "wxFRAME_TOOL_WINDOW without wxFRAME_NO_TASKBAR not supported." );
//
//    if ( HasFlag( wxFRAME_TOOL_WINDOW ) )
//    {
//        qtFlags &= ~Qt::WindowType_Mask;
//        qtFlags = Qt::Tool;
//    }
//    else if ( HasFlag( wxFRAME_NO_TASKBAR ) )
//    {
//        qtFlags &= ~Qt::WindowType_Mask;
//        qtFlags = Qt::Dialog;
//    }
//
//    GetHandle()->setWindowFlags( qtFlags );
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


QMainWindow *wxFrame::GetHandle() const
{
    return m_qtMainWindow;
//    return m_qtMainWindow->centralWidget();
}

//=============================================================================

wxQtMainWindow::wxQtMainWindow( wxWindow *parent, wxFrame *handler )
    : wxQtEventSignalHandler< QMainWindow, wxFrame >( parent, handler )
{
//    setCentralWidget( new wxQtWidget( parent, handler ));
}
