/////////////////////////////////////////////////////////////////////////////
// Name:        xh_listc.cpp
// Purpose:     XML resource for wxListCtrl
// Author:      Brian Gavin
// Created:     2000/09/09
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Brian Gavin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_listc.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/textctrl.h"
#include "wx/xml/xh_listc.h"
#include "wx/listctrl.h"


wxListCtrlXmlHandler::wxListCtrlXmlHandler() 
: wxXmlResourceHandler() 
{
    ADD_STYLE(wxLC_LIST);
    ADD_STYLE(wxLC_REPORT);
    ADD_STYLE(wxLC_ICON);
    ADD_STYLE(wxLC_SMALL_ICON);
    ADD_STYLE(wxLC_ALIGN_TOP);
    ADD_STYLE(wxLC_ALIGN_LEFT);
    ADD_STYLE(wxLC_AUTOARRANGE);
    ADD_STYLE(wxLC_USER_TEXT);
    ADD_STYLE(wxLC_EDIT_LABELS);
    ADD_STYLE(wxLC_NO_HEADER);
    ADD_STYLE(wxLC_SINGLE_SEL);
    ADD_STYLE(wxLC_SORT_ASCENDING);
    ADD_STYLE(wxLC_SORT_DESCENDING);
    AddWindowStyles();
}


wxObject *wxListCtrlXmlHandler::DoCreateResource()
{ 
    wxListCtrl *list = new wxListCtrl(m_ParentAsWindow,
                                    GetID(),
                                    GetPosition(), GetSize(),
                                    GetStyle(),
                                    wxDefaultValidator,
                                    GetName());
    /* TODO: columns definition */
    
    SetupWindow(list);
    
    return list;
}



bool wxListCtrlXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxListCtrl"));
}
