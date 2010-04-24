/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/action_qt.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/qt/action_qt.h"
#include "wx/menuitem.h"

wxQtAction::wxQtAction( wxMenuItem *menuItem, const QString &text, QObject *parent )
    : QAction( text, parent ), wxQtSignalReceiver< wxMenuItem >( menuItem )
{
    connect( this, SIGNAL( triggered( bool ) ), this, SLOT( OnActionTriggered( bool ))); 
}

void wxQtAction::OnActionTriggered( bool checked )
{
    GetSignalReceiver()->OnItemTriggered( checked );
}

