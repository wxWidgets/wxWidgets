/////////////////////////////////////////////////////////////////////////////
// Name:        xh_statbar.cpp
// Purpose:     XRC resource for wxStatusBar
// Author:      Brian Ravnsgaard Riis
// Created:     2004/01/21
// RCS-ID:      
// Copyright:   (c) 2004 Brian Ravnsgaard Riis
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "xh_statbar.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/frame.h"
#include "wx/string.h"

#if wxUSE_STATUSBAR

#include "wx/xrc/xh_statbar.h"
#include "wx/statusbr.h"

wxStatusBarXmlHandler::wxStatusBarXmlHandler() : 
        wxXmlResourceHandler()
{
    XRC_ADD_STYLE(wxST_SIZEGRIP);
    AddWindowStyles();
}

wxObject *wxStatusBarXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(statbar, wxStatusBar)

    statbar->Create(m_parentAsWindow,
                    GetID(),
                    GetStyle(),
                    GetName());

    int fields = GetLong(wxT("fields"), 1);
    wxString widths = GetParamValue(wxT("widths"));

    if (fields > 1 && !widths.IsEmpty())
    {
        int *width = new int[fields];
  
        for (unsigned int i = 0; i < fields; ++i)
        {
            width[i] = wxAtoi(widths.BeforeFirst(wxT(',')));
            if(widths.Find(wxT(',')))
                widths.Remove(0, widths.Find(wxT(',')) + 1);
        }
        statbar->SetFieldsCount(fields, width);
        delete[] width;
    }
    else
        statbar->SetFieldsCount(fields);

    if (m_parentAsWindow)
    {
        wxFrame *parentFrame = wxDynamicCast(m_parent, wxFrame);
        if (parentFrame)
            parentFrame->SetStatusBar(statbar);
    }

    return statbar;
}

bool wxStatusBarXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxStatusBar"));
}

#endif

