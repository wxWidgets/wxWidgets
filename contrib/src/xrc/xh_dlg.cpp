/////////////////////////////////////////////////////////////////////////////
// Name:        xh_dlg.cpp
// Purpose:     XRC resource for dialogs
// Author:      Vaclav Slavik
// Created:     2000/03/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_dlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xrc/xh_dlg.h"
#include "wx/dialog.h"
#include "wx/log.h"
#include "wx/intl.h"


wxDialogXmlHandler::wxDialogXmlHandler() : wxXmlResourceHandler()
{
    XRC_ADD_STYLE(wxSTAY_ON_TOP);
    XRC_ADD_STYLE(wxCAPTION);
    XRC_ADD_STYLE(wxDEFAULT_DIALOG_STYLE);
    XRC_ADD_STYLE(wxTHICK_FRAME);
    XRC_ADD_STYLE(wxSYSTEM_MENU);
    XRC_ADD_STYLE(wxRESIZE_BORDER);
    XRC_ADD_STYLE(wxRESIZE_BOX);
    XRC_ADD_STYLE(wxDIALOG_MODAL);
    XRC_ADD_STYLE(wxDIALOG_MODELESS);

    XRC_ADD_STYLE(wxNO_3D);
    XRC_ADD_STYLE(wxTAB_TRAVERSAL);
    XRC_ADD_STYLE(wxWS_EX_VALIDATE_RECURSIVELY);
    XRC_ADD_STYLE(wxCLIP_CHILDREN);

    AddWindowStyles();
}

wxObject *wxDialogXmlHandler::DoCreateResource()
{ 
    wxDialog *dlg = wxDynamicCast(m_instance, wxDialog);
    
    wxASSERT_MSG(dlg, _("XRC resource: Cannot create dialog without instance."));
    
    dlg->Create(m_parentAsWindow,
                GetID(),
                GetText(wxT("title")),
                wxDefaultPosition, wxDefaultSize,
                GetStyle(wxT("style"), wxDEFAULT_DIALOG_STYLE),
                GetName());

    if (HasParam(wxT("size")))
        dlg->SetClientSize(GetSize());
    if (HasParam(wxT("pos")))
        dlg->Move(GetPosition());

    SetupWindow(dlg);

    CreateChildren(dlg);
    
    if (GetBool(wxT("centered"), FALSE))
        dlg->Centre();
    
    return dlg;
}

bool wxDialogXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxDialog"));
}
