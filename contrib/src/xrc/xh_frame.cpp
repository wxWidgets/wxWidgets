/////////////////////////////////////////////////////////////////////////////
// Name:        xh_frame.cpp
// Purpose:     XRC resource for dialogs
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

#include "wx/xrc/xh_frame.h"
#include "wx/frame.h"
#include "wx/log.h"
#include "wx/intl.h"


wxFrameXmlHandler::wxFrameXmlHandler() : wxXmlResourceHandler()
{
    XRC_ADD_STYLE(wxSTAY_ON_TOP);
    XRC_ADD_STYLE(wxCAPTION);
    XRC_ADD_STYLE(wxDEFAULT_DIALOG_STYLE);
    XRC_ADD_STYLE(wxDEFAULT_FRAME_STYLE);
    XRC_ADD_STYLE(wxTHICK_FRAME);
    XRC_ADD_STYLE(wxSYSTEM_MENU);
    XRC_ADD_STYLE(wxRESIZE_BORDER);
    XRC_ADD_STYLE(wxRESIZE_BOX);

    XRC_ADD_STYLE(wxFRAME_TOOL_WINDOW);
    XRC_ADD_STYLE(wxFRAME_FLOAT_ON_PARENT);
    XRC_ADD_STYLE(wxMAXIMIZE_BOX);
    XRC_ADD_STYLE(wxMINIMIZE_BOX);
    XRC_ADD_STYLE(wxSTAY_ON_TOP);

    XRC_ADD_STYLE(wxNO_3D);
    XRC_ADD_STYLE(wxTAB_TRAVERSAL);
    XRC_ADD_STYLE(wxWS_EX_VALIDATE_RECURSIVELY);
    XRC_ADD_STYLE(wxCLIP_CHILDREN);

    AddWindowStyles();
}

wxObject *wxFrameXmlHandler::DoCreateResource()
{ 
    wxFrame *frame = wxDynamicCast(m_instance, wxFrame);
    
    wxASSERT_MSG(frame, _("XRC resource: Cannot create dialog without instance."));
    
    frame->Create(m_parentAsWindow,
                  GetID(),
                  GetText(wxT("title")),
                  wxDefaultPosition, wxDefaultSize,
                  GetStyle(wxT("style"), wxDEFAULT_FRAME_STYLE),
                  GetName());

    if (HasParam(wxT("size")))
        frame->SetClientSize(GetSize());
    if (HasParam(wxT("pos")))
        frame->Move(GetPosition());

    SetupWindow(frame);

    CreateChildren(frame);
    
    if (GetBool(wxT("centered"), FALSE))
        frame->Centre();
    
    return frame;
}

bool wxFrameXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxFrame"));
}
