/////////////////////////////////////////////////////////////////////////////
// Name:        src/web/popupwin.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id: popupwin.cpp 39310 2006-05-24 07:16:32Z ABX $
// Copyright:   (c) 1998 Robert Roebling, John Wilmes
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_POPUPWIN

#include "wx/popupwin.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/app.h"
    #include "wx/settings.h"
#endif

//-----------------------------------------------------------------------------
// wxPopupWindow
//-----------------------------------------------------------------------------

wxPopupWindow::wxPopupWindow() {
}

wxPopupWindow::wxPopupWindow(wxWindow* parent, int flags) {
    Create(parent, flags);
}

wxPopupWindow::~wxPopupWindow()
{
}

bool wxPopupWindow::Create( wxWindow *parent, int style )
{
    if (!CreateBase(parent, -1, wxDefaultPosition, wxDefaultSize, style, wxDefaultValidator, wxT("popup")))
    {
        wxFAIL_MSG(wxT("wxPopupWindow creation failed"));
        return false;
    }

    // All dialogs should really have this style
    m_windowStyle = style;
    m_windowStyle |= wxTAB_TRAVERSAL;

    wxPoint pos( 20,20 );
    wxSize size( 20,20 );
    wxPoint pos2 = pos;
    wxSize size2 = size;

    m_parent = parent;
    if (m_parent) m_parent->AddChild( this );

    return true;
}

#endif // wxUSE_POPUPWIN
