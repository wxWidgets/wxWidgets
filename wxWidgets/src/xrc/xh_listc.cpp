/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_listc.cpp
// Purpose:     XRC resource for wxListCtrl
// Author:      Brian Gavin
// Created:     2000/09/09
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Brian Gavin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_XRC && wxUSE_LISTCTRL

#include "wx/xrc/xh_listc.h"

#ifndef WX_PRECOMP
    #include "wx/textctrl.h"
#endif

#include "wx/listctrl.h"


IMPLEMENT_DYNAMIC_CLASS(wxListCtrlXmlHandler, wxXmlResourceHandler)

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
    XRC_ADD_STYLE(wxLC_VIRTUAL);
    XRC_ADD_STYLE(wxLC_HRULES);
    XRC_ADD_STYLE(wxLC_VRULES);
    XRC_ADD_STYLE(wxLC_NO_SORT_HEADER);
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

#endif // wxUSE_XRC && wxUSE_LISTCTRL
