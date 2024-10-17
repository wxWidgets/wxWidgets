/////////////////////////////////////////////////////////////////////////////
// Name:        xh_propgrid.cpp
// Purpose:     XRC resource for wxPropertyGrid
// Author:      Jaakko Salli
// Created:     May-16-2007
// Copyright:   (c) 2007 Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_XRC && wxUSE_PROPGRID

#include "wx/xrc/xh_propgrid.h"

#include "wx/propgrid/manager.h"
#include "wx/propgrid/propgrid.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
#endif

IMPLEMENT_DYNAMIC_CLASS(wxPropertyGridXmlHandler, wxXmlResourceHandler)

wxPropertyGridXmlHandler::wxPropertyGridXmlHandler()
                     :wxXmlResourceHandler()
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
        m_xrcHandler->CreateChildrenPrivately(m_pg, nullptr);
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
    m_populator->SetGrid( m_pg );
}

void wxPropertyGridXmlHandler::PopulatePage( wxPropertyGridPageState* state )
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
        m_pg->SetVirtualWidth(GetDimension(sVW));
    }
}

wxObject *wxPropertyGridXmlHandler::DoCreateResource()
{
    const wxXmlNode* node = m_node;
    wxString nodeName = GetNodeName(node);

    if ( nodeName == wxT("property") )
    {
        // property
        wxString clas = GetNodeAttribute(node, "class");

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
        wxString* pValue = nullptr;
        if ( HasParam(sValue) )
        {
            value = GetText(sValue);
            pValue = &value;
        }

        wxXmlNode* choicesNode = GetParamNode(wxT("choices"));
        wxPGChoices choices;
        if ( choicesNode )
        {
            choices = m_populator->ParseChoices( GetNodeContent(choicesNode),
                                                 GetNodeAttribute(choicesNode, "id"));
        }

        wxPGProperty* property = m_populator->Add( clas, label, name, pValue, &choices );

        if ( !property )
            return nullptr;

        wxString sFlags(wxT("flags"));
        if ( HasParam(sFlags) )
            property->SetFlagsFromString( GetText(sFlags) );

        wxString sTip(wxT("tip"));
        if ( HasParam(sTip) )
            property->SetHelpString(GetText(sTip));

        if ( property->GetChildCount() )
        {
            wxString sExpanded(wxT("expanded"));
            if ( HasParam(sExpanded) )
                property->SetExpanded(GetBool(sExpanded));
        }

        // Need to call AddChildren even for non-parent properties for attributes and such
        m_populator->AddChildren(property);
    }
    else if ( nodeName == wxT("attribute") )
    {
        // attribute
        wxString s1 = GetNodeAttribute(node, "name");
        if ( s1.length() )
        {
            wxPGPropertyValuesFlags flags = GetNodeAttribute(node, "recurse") == "1"
                ? wxPGPropertyValuesFlags::Recurse
                : wxPGPropertyValuesFlags::DontRecurse;

            m_populator->AddAttribute( s1, GetNodeAttribute(node, "type"),
                                       GetNodeContent(node), flags );
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
        m_populator->ParseChoices( GetNodeContent(node),
                                   GetNodeAttribute(node, "id"));
    }
    else if ( nodeName == wxT("splitterpos") )
    {
        // splitterpos
        wxASSERT(m_populator);
        wxString sIndex = GetNodeAttribute(node, "index");

        long index;
        if ( !sIndex.ToLong(&index, 10) )
            index = 0;

        wxString s = GetNodeContent(node);
        long pos;
        if ( wxPropertyGridPopulator::ToLongPCT(s, &pos, m_pg->GetClientSize().x) )
            m_populator->GetState()->DoSetSplitter( pos, index );
    }
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
        wxPropertyGridPageState* state = m_manager->GetPage(m_manager->GetPageCount()-1);
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
        CreateChildrenPrivately(control, nullptr);
        DonePopulator();

        m_manager = oldManager;
        SetupWindow(control);

        return control;
    }
    else
    {
        wxFAIL_MSG( "unreachable" );
    }

    return nullptr;
}

bool wxPropertyGridXmlHandler::CanHandle(wxXmlNode *node)
{
    const wxString name = GetNodeName(node);

    return (
            (
             m_populator && ( name == wxT("property") ||
                              name == wxT("attribute") ||
                              name == wxT("choices") ||
                              name == wxT("splitterpos")
                            )
            ) ||
            (m_manager && name == wxT("page")) ||
            (!m_populator && IsOfClass(node, wxT("wxPropertyGrid"))) ||
            (!m_populator && IsOfClass(node, wxT("wxPropertyGridManager")))
           );
}

#endif // wxUSE_XRC && wxUSE_PROPGRID
