/////////////////////////////////////////////////////////////////////////////
// Name:        xh_unkwn.cpp
// Purpose:     XML resource for unknown widget
// Author:      Vaclav Slavik
// Created:     2000/09/09
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_unkwn.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xml/xh_unkwn.h"
#include "wx/window.h"
#include "wx/log.h"


wxUnknownWidgetXmlHandler::wxUnknownWidgetXmlHandler() 
: wxXmlResourceHandler() 
{
    AddWindowStyles();
}

wxObject *wxUnknownWidgetXmlHandler::DoCreateResource()
{ 
    long id = GetLong(_T("id"), -1);
    wxString name = GetParamValue(_T("name"));
    
    wxWindow *wnd = NULL;
    
    if (id != -1)
        wnd = m_ParentAsWindow->FindWindow(id);
    if (wnd == NULL && !name.IsEmpty())
        wnd = m_ParentAsWindow->FindWindow(name);
        
    if (wnd == NULL)
        wxLogError(_T("Cannot find specified window for class 'unknown' (id=%li, name='%s')."), id, name.mb_str());
    else
    {
        if (wnd->GetParent() != m_ParentAsWindow)
            wnd->Reparent(m_ParentAsWindow);
        SetupWindow(wnd);
    }

    return wnd;
}

bool wxUnknownWidgetXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, _T("unknown"));
}

