/////////////////////////////////////////////////////////////////////////////
// Name:        xh_frame.cpp
// Purpose:     XML resource for dialogs
// Author:      Vaclav Slavik & Aleks.
// Created:     2000/03/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_frame.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xml/xh_frame.h"
#include "wx/frame.h"
#include "wx/log.h"
#include "wx/intl.h"


wxFrameXmlHandler::wxFrameXmlHandler() : wxXmlResourceHandler()
{
    ADD_STYLE(wxSTAY_ON_TOP);
    ADD_STYLE(wxCAPTION);
    ADD_STYLE(wxDEFAULT_DIALOG_STYLE);
    ADD_STYLE(wxTHICK_FRAME);
    ADD_STYLE(wxSYSTEM_MENU);
    ADD_STYLE(wxRESIZE_BORDER);
    ADD_STYLE(wxRESIZE_BOX);

    ADD_STYLE(wxFRAME_TOOL_WINDOW);
    ADD_STYLE(wxFRAME_FLOAT_ON_PARENT);
    ADD_STYLE(wxMAXIMIZE_BOX);
    ADD_STYLE(wxMINIMIZE_BOX);
    ADD_STYLE(wxSTAY_ON_TOP);

    ADD_STYLE(wxNO_3D);
    ADD_STYLE(wxTAB_TRAVERSAL);
    ADD_STYLE(wxWS_EX_VALIDATE_RECURSIVELY);
    ADD_STYLE(wxCLIP_CHILDREN);
    AddWindowStyles();
}



wxObject *wxFrameXmlHandler::DoCreateResource()
{ 
    wxFrame *frame = wxDynamicCast(m_Instance, wxFrame);
    
    wxASSERT_MSG(frame, _("XML resource: Cannot create dialog without instance."));
    
    frame->Create(m_ParentAsWindow,
                GetID(),
                GetText(_T("title")),
                wxDefaultPosition, wxDefaultSize,
                GetStyle(_T("style"), wxDEFAULT_FRAME_STYLE),
                GetName());
    frame->SetClientSize(GetSize());
    frame->Move(GetPosition());
    SetupWindow(frame);

    CreateChildren(frame);
    
    if (GetBool(_("centered"), FALSE))
        frame->Centre();
    
    return frame;
}



bool wxFrameXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, _T("wxFrame"));
}


