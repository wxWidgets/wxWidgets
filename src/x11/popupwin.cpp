/////////////////////////////////////////////////////////////////////////////
// Name:        popupwin.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "popupwin.h"
#endif

#include "wx/defs.h"

#if wxUSE_POPUPWIN

#include "wx/popupwin.h"
#include "wx/frame.h"
#include "wx/app.h"
#include "wx/cursor.h"

#include "wx/x11/private.h"

//-----------------------------------------------------------------------------
// wxPopupWindow
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxPopupWindow,wxPopupWindowBase)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxPopupWindow, wxWindow)

bool wxPopupWindow::Create( wxWindow *parent, int style )
{
    m_needParent = FALSE;

    if (!CreateBase( parent, -1, wxDefaultPosition, wxDefaultSize, style, wxDefaultValidator, "popup" ))
    {
        wxFAIL_MSG( wxT("wxPopupWindow creation failed") );
        return FALSE;
    }

    // All dialogs should really have this style
    m_windowStyle |= wxTAB_TRAVERSAL;

    if (m_parent) m_parent->AddChild( this );

    // TODO: implementation

    return TRUE;
}

void wxPopupWindow::DoMoveWindow(int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(width), int WXUNUSED(height) )
{
    wxFAIL_MSG( wxT("DoMoveWindow called for wxPopupWindow") );
}

void wxPopupWindow::DoSetSize( int x, int y, int width, int height, int sizeFlags )
{
    // TODO
}
bool wxPopupWindow::Show( bool show )
{
    // TODO?
    bool ret = wxWindow::Show( show );

    return ret;
}

#endif // wxUSE_POPUPWIN
