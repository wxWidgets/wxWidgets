/////////////////////////////////////////////////////////////////////////////
// Name:        xh_gdctl.cpp
// Purpose:     XRC resource for wxGenericDirCtrl
// Author:      Markus Greither
// Created:     2002/01/20
// RCS-ID:
// Copyright:   (c) 2002 Markus Greither
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "xh_gdctl.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/defs.h"
#if wxUSE_DIRDLG

#include "wx/textctrl.h"
#include "wx/xrc/xh_gdctl.h"
#include "wx/dirctrl.h"

wxGenericDirCtrlXmlHandler::wxGenericDirCtrlXmlHandler()
: wxXmlResourceHandler()
{
    XRC_ADD_STYLE(wxDIRCTRL_DIR_ONLY);
    XRC_ADD_STYLE(wxDIRCTRL_3D_INTERNAL);
    XRC_ADD_STYLE(wxDIRCTRL_SELECT_FIRST);
    XRC_ADD_STYLE(wxDIRCTRL_SHOW_FILTERS);
    AddWindowStyles();
}

wxObject *wxGenericDirCtrlXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(ctrl, wxGenericDirCtrl)
 
    ctrl->Create(m_parentAsWindow,
                 GetID(),
                 GetText(wxT("defaultfolder")),
                 GetPosition(), GetSize(),
                 GetStyle(),
                 GetText(wxT("filter")),
                 (int)GetLong(wxT("defaultfilter")),
                 GetName());

    SetupWindow(ctrl);

    return ctrl;
}

bool wxGenericDirCtrlXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxGenericDirCtrl"));
}

#endif
