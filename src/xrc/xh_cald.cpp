/////////////////////////////////////////////////////////////////////////////
// Name:        xh_cald.cpp
// Purpose:     XRC resource for wxCalendarCtrl
// Author:      Brian Gavin
// Created:     2000/09/09
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Brian Gavin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_cald.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xrc/xh_cald.h"
#include "wx/event.h"
#include "wx/calctrl.h"


wxCalendarCtrlXmlHandler::wxCalendarCtrlXmlHandler() 
: wxXmlResourceHandler() 
{
    XRC_ADD_STYLE(wxCAL_SUNDAY_FIRST);
    XRC_ADD_STYLE(wxCAL_MONDAY_FIRST);
    XRC_ADD_STYLE(wxCAL_SHOW_HOLIDAYS);
    XRC_ADD_STYLE(wxCAL_NO_YEAR_CHANGE);
    XRC_ADD_STYLE(wxCAL_NO_MONTH_CHANGE);
    AddWindowStyles();
}


wxObject *wxCalendarCtrlXmlHandler::DoCreateResource()
{ 
    XRC_MAKE_INSTANCE(calendar, wxCalendarCtrl);

    calendar->Create(m_parentAsWindow,
                     GetID(),
                     wxDefaultDateTime,
                     /*TODO: take it from resource*/
                     GetPosition(), GetSize(),
                     GetStyle(),
                     GetName());
    
    SetupWindow(calendar);
    
    return calendar;
}

bool wxCalendarCtrlXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxCalendarCtrl"));
}
