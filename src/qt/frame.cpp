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
    if ( parent != NULL )
        qtParent = parent->GetHandle();

    m_qtFrame = new wxQtFrame( qtParent );

    return wxTopLevelWindow::Create( parent, id, title, pos, size, style, name );
}

wxQtFrame *wxFrame::GetHandle() const
{
    return m_qtFrame;
}

