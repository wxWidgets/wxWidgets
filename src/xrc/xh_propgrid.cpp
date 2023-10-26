/////////////////////////////////////////////////////////////////////////////
// Name:        xh_propgrid.cpp
// Purpose:     XRC resource for wxPropertyGrid
// Author:      Jaakko Salli
// Modified by:
// Created:     May-16-2007
// RCS-ID:      $Id:
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

/*

  NOTE: This source file is *not* included in the wxPropertyGrid library
    (to prevent xrc-lib dependency). To use this code, you will need to
    separately add src/xh_propgrid.cpp to your application.

*/

#if wxUSE_XRC

#include <wx/propgrid/propgrid.h>

#include <wx/propgrid/xh_propgrid.h>

#ifndef WX_PRECOMP
    #include "wx/intl.h"
#endif

#if wxCHECK_VERSION(3, 0, 0)
    #define wxXML_GetAttribute(A,B,C)   (A->GetAttribute(B,C))
#else
    #define wxXML_GetAttribute(A,B,C)   (A->GetPropVal(B,C))
#endif

IMPLEMENT_DYNAMIC_CLASS(wxPropertyGridXmlHandler, wxXmlResourceHandler)

wxPropertyGridXmlHandler::wxPropertyGridXmlHandler()
                     :wxXmlResourceHandler(), m_manager(NULL), m_populator(NULL)
{
    XRC_ADD_STYLE(wxTAB_TRAVERSAL);
    XRC_ADD_STYLE(wxPG_AUTO_SORT);
    XRC_ADD_STYLE(wxPG_HIDE_CATEGORIES);
    XRC_ADD_STYLE(wxPG_BOLD_MODIFIED);
    XRC_ADD_STYLE(wxPG_SPLITTER_AUTO_CENTER);
    XRC_ADD_STYLE(wxPG_TOOLTIPS);
    XRC_ADD_STYLE(wxPG_HIDE_MARGIN);
    XRC_ADD_STYLE(wxPG_STATIC_SPLITTER);
    XRC_ADD_STYLE(wxPG_LIMITED_EDITING);
    XRC_ADD_STYLE(wxPG_TOOLBAR);
    XRC_ADD_STYLE(wxPG_DESCRIPTION);
    XRC_ADD_STYLE(wxPG_EX_INIT_NOCAT);
    XRC_ADD_STYLE(wxPG_EX_HELP_AS_TOOLTIPS);
    XRC_ADD_STYLE(wxPG_EX_AUTO_UNSPECIFIED_VALUES);
    XRC_ADD_STYLE(wxPG_EX_WRITEONLY_BUILTIN_ATTRIBUTES);
    XRC_ADD_STYLE(wxPG_EX_NO_FLAT_TOOLBAR);
    XRC_ADD_STYLE(wxPG_EX_MODE_BUTTONS);
#if wxPG_COMPATIBILITY_1_2_0
    XRC_ADD_STYLE(wxPG_EX_TRADITIONAL_VALIDATORS);
#endif

    AddWindowStyles();
}

class wxPropertyGridXrcPopulator : public wxPropertyGridPopulator
{
public:
    wxPropertyGridXrcPopulator( wxPropertyGridXmlHandler* handler )
        : wxPropertyGridPopulator()
    {
        m_xrcHandler = handler;
        m_prevPopulator = m_xrcHandler->m_populator;
    }

    ~wxPropertyGridXrcPopulator() override
    {
        m_xrcHandler->m_populator = m_prevPopulator;
    }

    void DoScanForChildren() override
    {
        m_xrcHandler->CreateChildrenPrivately(m_pg, NULL);
    }

protected:
    wxPropertyGridXmlHandler*   m_xrcHandler;
    wxPropertyGridPopulator*    m_prevPopulator;
};


void wxPropertyGridXmlHandler::InitPopulator()
{
    wxPropertyGridXrcPopulator* populator
        = new wxPropertyGridXrcPopulator(this);
    m_populator = populator;
}

void wxPropertyGridXmlHandler::PopulatePage( wxPropertyGridState* state )
{
    wxString sColumns(wxT("columns"));
    if ( HasParam(sColumns) )
        state->SetColumnCount( GetLong(sColumns) );

    m_populator->SetState( state );
    m_populator->AddChildren( state->DoGetRoot() );
}

void wxPropertyGridXmlHandler::DonePopulator()
{
    delete m_populator;
}

void wxPropertyGridXmlHandler::HandlePropertyGridParams()
{
    wxString sVW(wxT("virtualwidth"));
    if ( HasParam(sVW) )
    {
        int vw = GetLong(sVW);
        m_pg->SetVirtualWidth(vw);
    }
}

wxObject *wxPropertyGridXmlHandler::DoCreateResource()
{
    const wxXmlNode* node = m_node;
    wxString nodeName = node->GetName();
    wxString emptyString;

    if ( nodeName == wxT("property") )
    {
        // property
        wxString clas = wxXML_GetAttribute(node, wxT("class"), emptyString);

        wxString label;
        wxString sLabel(wxT("label"));
        if ( HasParam(sLabel) )
            label = GetText(sLabel);

        wxString name;
        wxString sName(wxT("name"));
        if ( HasParam(sName) )
            name = GetText(sName);
        else
            name = label;

        wxString sValue(wxT("value"));
        wxString value;
        wxString* pValue = NULL;
        if ( HasParam(sValue) )
        {
            value = GetText(sValue);
            pValue = &value;
        }

        wxXmlNode* choicesNode = GetParamNode(wxT("choices"));
        wxPGChoices choices;
        if ( choicesNode )
        {
            choices = m_populator->ParseChoices( choicesNode->GetNodeContent(),
                                                 wxXML_GetAttribute(choicesNode, wxT("id"), emptyString));
        }

        wxPGProperty* property = m_populator->Add( clas, label, name, pValue, &choices );

        if ( !property )
            return NULL;

        wxString sFlags(wxT("flags"));
        wxString flags;
        if ( HasParam(sFlags) )
            property->SetFlagsFromString( GetText(sFlags) );

        wxString sTip(wxT("tip"));
        if ( HasParam(sTip) )
            property->SetHelpString(GetText(sTip));

        if ( property->GetChildCount() )
        {
            wxPGProperty* pwc = property;

            // FIXME
            wxString sExpanded(wxT("expanded"));
            if ( HasParam(sExpanded) )
                pwc->SetExpanded(GetBool(sExpanded));
        }

        // Need to call AddChildren even for non-parent properties for attributes and such
        m_populator->AddChildren(property);
    }
    else if ( nodeName == wxT("attribute") )
    {
        // attribute
        wxString s1 = wxXML_GetAttribute(node, wxT("name"), emptyString);
        if ( s1.length() )
        {
            m_populator->AddAttribute( s1, wxXML_GetAttribute(node, wxT("type"), emptyString),
                                       node->GetNodeContent() );
        }
    }
    else if( m_class == wxT("wxPropertyGrid"))
    {
        XRC_MAKE_INSTANCE(control, wxPropertyGrid)

        control->Create(m_parentAsWindow,
                        GetID(),
                        GetPosition(), GetSize(),
                        GetStyle(),
                        GetName());

        m_pg = control;
        HandlePropertyGridParams();

        InitPopulator();
        PopulatePage(control->GetState());
        DonePopulator();

        SetupWindow(control);

        return control;
    }
    else if ( nodeName == wxT("choices") )
    {
        // choices

        //
        // Add choices list outside of a property
        m_populator->ParseChoices( node->GetNodeContent(),
                                   wxXML_GetAttribute(node, wxT("id"), emptyString));
    }
    else if ( nodeName == wxT("splitterpos") )
    {
        // splitterpos
        wxASSERT(m_populator);
        wxString sIndex = wxXML_GetAttribute(node, wxT("index"), emptyString);

        long index;
        if ( !sIndex.ToLong(&index, 10) )
            index = 0;

        wxString s = node->GetNodeContent();
        long pos;
        if ( wxPropertyGridPopulator::ToLongPCT(s, &pos, m_pg->GetClientSize().x) )
            m_populator->GetState()->DoSetSplitterPosition( pos, index, false );
    }
#if wxPG_INCLUDE_MANAGER
    else if ( nodeName == wxT("page") )
    {
        // page
        wxASSERT(m_manager);

        wxString label;
        wxString sLabel(wxT("label"));
        if ( HasParam(sLabel) )
            label = GetText(sLabel);
        else
            label = wxString::Format(_("Page %i"),(int)(m_manager->GetPageCount()+1));

        m_manager->AddPage(label);
        wxPropertyGridState* state = m_manager->GetPage(m_manager->GetPageCount()-1);
        PopulatePage(state);
    }
    else if( m_class == wxT("wxPropertyGridManager"))
    {
        XRC_MAKE_INSTANCE(control, wxPropertyGridManager)

        control->Create(m_parentAsWindow,
                        GetID(),
                        GetPosition(), GetSize(),
                        GetStyle(),
                        GetName());

        wxPropertyGridManager* oldManager = m_manager;
        m_manager = control;
        m_pg = control->GetGrid();
        HandlePropertyGridParams();

        InitPopulator();
        CreateChildrenPrivately(control, NULL);
        DonePopulator();

        m_manager = oldManager;
        SetupWindow(control);

        return control;
    }
#endif
    else
    {
        wxASSERT( false );
    }

    return NULL;
}

bool wxPropertyGridXmlHandler::CanHandle(wxXmlNode *node)
{
    #define fOurClass(A) IsOfClass(node, A)

    wxString name = node->GetName();

    return (
            (
             m_populator && ( name == wxT("property") ||
                              name == wxT("attribute") ||
                              name == wxT("choices") ||
                              name == wxT("splitterpos")
                            )
            ) ||
            (m_manager && name == wxT("page")) ||
            (!m_populator && fOurClass(wxT("wxPropertyGrid")))
#if wxPG_INCLUDE_MANAGER
            ||
            (!m_populator && fOurClass(wxT("wxPropertyGridManager")))
#endif
           );
}

#endif // wxUSE_XRC
