/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_wizrd.cpp
// Purpose:     XRC resource for wxWizard
// Author:      Vaclav Slavik
// Created:     2003/03/01
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_XRC && wxUSE_WIZARDDLG

#include "wx/xrc/xh_wizrd.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
#endif

#include "wx/wizard.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxWizardXmlHandler, wxXmlResourceHandler);

wxWizardXmlHandler::wxWizardXmlHandler() : wxXmlResourceHandler()
{
    m_wizard = NULL;
    m_lastSimplePage = NULL;

    XRC_ADD_STYLE(wxSTAY_ON_TOP);
    XRC_ADD_STYLE(wxCAPTION);
    XRC_ADD_STYLE(wxDEFAULT_DIALOG_STYLE);
    XRC_ADD_STYLE(wxSYSTEM_MENU);
    XRC_ADD_STYLE(wxRESIZE_BORDER);
    XRC_ADD_STYLE(wxCLOSE_BOX);
    XRC_ADD_STYLE(wxDIALOG_NO_PARENT);

    XRC_ADD_STYLE(wxTAB_TRAVERSAL);
    XRC_ADD_STYLE(wxWS_EX_VALIDATE_RECURSIVELY);
    XRC_ADD_STYLE(wxDIALOG_EX_METAL);
    XRC_ADD_STYLE(wxMAXIMIZE_BOX);
    XRC_ADD_STYLE(wxMINIMIZE_BOX);
    XRC_ADD_STYLE(wxFRAME_SHAPED);
    XRC_ADD_STYLE(wxDIALOG_EX_CONTEXTHELP);

    XRC_ADD_STYLE(wxWIZARD_EX_HELPBUTTON);
    AddWindowStyles();
}

wxObject *wxWizardXmlHandler::DoCreateResource()
{
    if (m_class == wxT("wxWizard"))
    {
        XRC_MAKE_INSTANCE(wiz, wxWizard)

        long style = GetStyle(wxT("exstyle"), 0);
        if (style != 0)
            wiz->SetExtraStyle(style);
        wiz->Create(m_parentAsWindow,
                    GetID(),
                    GetText(wxT("title")),
                    GetBitmapBundle(),
                    GetPosition(),
                    GetStyle(wxT("style"), wxDEFAULT_DIALOG_STYLE));
        SetupWindow(wiz);

        wxWizard *old = m_wizard;
        m_wizard = wiz;
        m_lastSimplePage = NULL;
        CreateChildren(wiz, true /*this handler only*/);
        m_wizard = old;
        return wiz;
    }
    else
    {
        wxWizardPage *page;

        if (m_class == wxT("wxWizardPageSimple"))
        {
            XRC_MAKE_INSTANCE(p, wxWizardPageSimple)
            p->Create(m_wizard, NULL, NULL, GetBitmapBundle());
            if (m_lastSimplePage)
                wxWizardPageSimple::Chain(m_lastSimplePage, p);
            page = p;
            m_lastSimplePage = p;
        }
        else /*if (m_class == wxT("wxWizardPage"))*/
        {
            if ( !m_instance )
            {
                ReportError("wxWizardPage is abstract class and must be subclassed");
                return NULL;
            }

            page = wxStaticCast(m_instance, wxWizardPage);
            page->Create(m_wizard, GetBitmapBundle());
        }

        page->SetName(GetName());
        page->SetId(GetID());

        SetupWindow(page);
        CreateChildren(page);
        return page;
    }
}

bool wxWizardXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxWizard")) ||
           (m_wizard != NULL &&
                (IsOfClass(node, wxT("wxWizardPage")) ||
                 IsOfClass(node, wxT("wxWizardPageSimple")))
           );
}

#endif // wxUSE_XRC && wxUSE_WIZARDDLG
