/////////////////////////////////////////////////////////////////////////////
// Name:        xh_listc.cpp
// Purpose:     XRC resource for wxListCtrl
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
#include "wx/xrc/xh_listc.h"
#include "wx/listctrl.h"


wxListCtrlXmlHandler::wxListCtrlXmlHandler() 
: wxXmlResourceHandler() 
{
    XRC_ADD_STYLE(wxLC_LIST);
    XRC_ADD_STYLE(wxLC_REPORT);
    XRC_ADD_STYLE(wxLC_ICON);
    XRC_ADD_STYLE(wxLC_SMALL_ICON);
    XRC_ADD_STYLE(wxLC_ALIGN_TOP);
    XRC_ADD_STYLE(wxLC_ALIGN_LEFT);
    XRC_ADD_STYLE(wxLC_AUTOARRANGE);
    XRC_ADD_STYLE(wxLC_USER_TEXT);
    XRC_ADD_STYLE(wxLC_EDIT_LABELS);
    XRC_ADD_STYLE(wxLC_NO_HEADER);
    XRC_ADD_STYLE(wxLC_SINGLE_SEL);
    XRC_ADD_STYLE(wxLC_SORT_ASCENDING);
    XRC_ADD_STYLE(wxLC_SORT_DESCENDING);
    AddWindowStyles();
}

wxObject *wxListCtrlXmlHandler::DoCreateResource()
{ 
    XRC_MAKE_INSTANCE(list, wxListCtrl)

    list->Create(m_parentAsWindow,
                 GetID(),
                 GetPosition(), GetSize(),
                 GetStyle(),
                 wxDefaultValidator,
                 GetName());

    // FIXME: add columns definition
    
    SetupWindow(list);
    
    return list;
}

bool wxListCtrlXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxListCtrl"));
}
