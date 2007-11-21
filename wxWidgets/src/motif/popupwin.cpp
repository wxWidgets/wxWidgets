/////////////////////////////////////////////////////////////////////////////
// Name:        src/motif/popupwin.cpp
// Purpose:     wxPopupWindow implementation
// Author:      Mattia barbon
// Modified by:
// Created:     28.08.03
// RCS-ID:      $Id$
// Copyright:   (c) Mattia barbon
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/popupwin.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/Xm.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

#include "wx/motif/private.h"

bool wxPopupWindow::Create( wxWindow *parent, int flags )
{
    if( !wxPopupWindowBase::Create( parent, flags ) )
        return false;

    SetParent( parent );
    if( parent )
        parent->AddChild( this );

    Widget popup = XtVaCreatePopupShell( "shell",
                                         overrideShellWidgetClass,
                                         (Widget)wxTheApp->GetTopLevelWidget(),
                                         NULL );

    m_mainWidget = (WXWidget)popup;

    SetSize( 100, 100 ); // for child creation to work

    XtSetMappedWhenManaged( popup, False );
    XtRealizeWidget( popup );

    return true;
}

bool wxPopupWindow::Show( bool show )
{
    if( !wxWindowBase::Show( show ) )
        return false;

    if( show )
    {
        XtPopup( (Widget)GetMainWidget(), XtGrabNone );
    }
    else
    {
        XtPopdown( (Widget)GetMainWidget() );
    }

    return true;
}
