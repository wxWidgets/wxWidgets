/////////////////////////////////////////////////////////////////////////////
// Name:        xh_cald.cpp
// Purpose:     XML resource for wxCalendarCtrl
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

#include "wx/xml/xh_cald.h"
#include "wx/calctrl.h"


wxCalendarCtrlXmlHandler::wxCalendarCtrlXmlHandler() 
: wxXmlResourceHandler() 
{
    ADD_STYLE(wxCAL_SUNDAY_FIRST);
    ADD_STYLE(wxCAL_MONDAY_FIRST);
    ADD_STYLE(wxCAL_SHOW_HOLIDAYS);
    ADD_STYLE(wxCAL_NO_YEAR_CHANGE);
    ADD_STYLE(wxCAL_NO_MONTH_CHANGE);
    AddWindowStyles();
}


wxObject *wxCalendarCtrlXmlHandler::DoCreateResource()
{ 
    wxCalendarCtrl *calendar = new wxCalendarCtrl(m_ParentAsWindow,
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
    return IsOfClass(node, _T("wxCalendarCtrl"));
}


