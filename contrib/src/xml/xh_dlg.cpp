/////////////////////////////////////////////////////////////////////////////
// Name:        xh_dlg.cpp
// Purpose:     XML resource for dialogs
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

#include "wx/xml/xh_dlg.h"
#include "wx/dialog.h"
#include "wx/log.h"
#include "wx/intl.h"


wxDialogXmlHandler::wxDialogXmlHandler() : wxXmlResourceHandler()
{
    ADD_STYLE(wxSTAY_ON_TOP);
    ADD_STYLE(wxCAPTION);
    ADD_STYLE(wxDEFAULT_DIALOG_STYLE);
    ADD_STYLE(wxTHICK_FRAME);
    ADD_STYLE(wxSYSTEM_MENU);
    ADD_STYLE(wxRESIZE_BORDER);
    ADD_STYLE(wxRESIZE_BOX);
    ADD_STYLE(wxDIALOG_MODAL);
    ADD_STYLE(wxDIALOG_MODELESS);

    ADD_STYLE(wxNO_3D);
    ADD_STYLE(wxTAB_TRAVERSAL);
    ADD_STYLE(wxWS_EX_VALIDATE_RECURSIVELY);
    ADD_STYLE(wxCLIP_CHILDREN);
    AddWindowStyles();
}



wxObject *wxDialogXmlHandler::DoCreateResource()
{ 
    wxDialog *dlg = wxDynamicCast(m_Instance, wxDialog);
    
    wxASSERT_MSG(dlg, _("XML resource: Cannot create dialog without instance."));
    
    dlg->Create(m_ParentAsWindow,
                GetID(),
                GetText(_T("title")),
                wxDefaultPosition, wxDefaultSize,
                GetStyle(_T("style"), wxDEFAULT_DIALOG_STYLE),
                GetName());
    dlg->SetClientSize(GetSize());
    dlg->Move(GetPosition());
    SetupWindow(dlg);

    CreateChildren(dlg);
    
    if (GetBool(_("centered"), FALSE))
        dlg->Centre();
    
    return dlg;
}



bool wxDialogXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, _T("wxDialog"));
}


