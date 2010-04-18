/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/toplevel.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/toplevel.h"
#include "wx/qt/converter.h"

wxTopLevelWindowNative::wxTopLevelWindowNative()
{
}

wxTopLevelWindowNative::wxTopLevelWindowNative(wxWindow *parent,
           wxWindowID winId,
           const wxString &title,
           const wxPoint &pos,
           const wxSize &size,
           long style,
           const wxString &name )
{
    Create( parent, winId, title, pos, size, style, name );
}

bool wxTopLevelWindowNative::Create( wxWindow *parent, wxWindowID winId,
    const wxString &title, const wxPoint &pos, const wxSize &size,
    long style, const wxString &name )
{
    SetTitle( title );

    return wxTopLevelWindowBase::Create( parent, winId, pos, size, style, name );
}

void wxTopLevelWindowNative::Maximize(bool maximize) 
{
}

void wxTopLevelWindowNative::Restore()
{
}

void wxTopLevelWindowNative::Iconize(bool iconize )
{
}

bool wxTopLevelWindowNative::IsMaximized() const
{
    return false;
}

bool wxTopLevelWindowNative::IsIconized() const
{
    return false;
}


bool wxTopLevelWindowNative::ShowFullScreen(bool show, long style)
{
    return false;
}

bool wxTopLevelWindowNative::IsFullScreen() const
{
    return false;
}

void wxTopLevelWindowNative::SetTitle(const wxString& title)
{
    GetHandle()->setWindowTitle( wxQtConvertString( title ));
}

wxString wxTopLevelWindowNative::GetTitle() const
{
    return ( wxQtConvertString( GetHandle()->windowTitle() ));
}

