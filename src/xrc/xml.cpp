/////////////////////////////////////////////////////////////////////////////
// Name:        xml.cpp
// Purpose:     wxXmlDocument - XML parser & data holder class
// Author:      Vaclav Slavik
// Created:     2000/03/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "xml.h"
#pragma implementation "xmlio.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif


#include "wx/wfstream.h"
#include "wx/datstrm.h"
#include "wx/zstream.h"
#include "wx/log.h"
#include "wx/intl.h"

#include "wx/xrc/xml.h"
#include "wx/xrc/xmlio.h"



wxXmlNode::wxXmlNode(wxXmlNode *parent,wxXmlNodeType type,
                     const wxString& name, const wxString& content,
                     wxXmlProperty *props, wxXmlNode *next)
    : m_type(type), m_name(name), m_content(content),
      m_properties(props), m_parent(parent),
      m_children(NULL), m_next(next)
{
    if (m_parent)
    {
        if (m_parent->m_children)
        {
            m_next = m_parent->m_children;
            m_parent->m_children = this;
        }
        else
            m_parent->m_children = this;
    }
}



wxXmlNode::wxXmlNode(wxXmlNodeType type, const wxString& name,
                     const wxString& content)
    : m_type(type), m_name(name), m_content(content),
      m_properties(NULL), m_parent(NULL),
      m_children(NULL), m_next(NULL)
{}



wxXmlNode::wxXmlNode(const wxXmlNode& node)
{
    m_next = NULL;
    m_parent = NULL;
    DoCopy(node);
}



wxXmlNode::~wxXmlNode()
{
    wxXmlNode *c, *c2;
    for (c = m_children; c; c = c2)
    {
        c2 = c->m_next;
        delete c;
    }

    wxXmlProperty *p, *p2;
    for (p = m_properties; p; p = p2)
    {
        p2 = p->GetNext();
        delete p;
    }
}



wxXmlNode& wxXmlNode::operator=(const wxXmlNode& node)
{
    delete m_properties;
    delete m_children;
    DoCopy(node);
    return *this;
}



void wxXmlNode::DoCopy(const wxXmlNode& node)
{
    m_type = node.m_type;
    m_name = node.m_name;
    m_content = node.m_content;
    m_children = NULL;

    wxXmlNode *n = node.m_children;
    while (n)
    {
        AddChild(new wxXmlNode(*n));
        n = n->GetNext();
    }

    m_properties = NULL;
    wxXmlProperty *p = node.m_properties;
    while (p)
    {
       AddProperty(p->GetName(), p->GetValue());
       p = p->GetNext();
    }
}


bool wxXmlNode::HasProp(const wxString& propName) const
{
    wxXmlProperty *prop = GetProperties();

    while (prop)
    {
        if (prop->GetName() == propName) return TRUE;
        prop = prop->GetNext();
    }

    return FALSE;
}



bool wxXmlNode::GetPropVal(const wxString& propName, wxString *value) const
{
    wxXmlProperty *prop = GetProperties();

    while (prop)
    {
        if (prop->GetName() == propName)
        {
            *value = prop->GetValue();
            return TRUE;
        }
        prop = prop->GetNext();
    }

    return FALSE;
}



wxString wxXmlNode::GetPropVal(const wxString& propName, const wxString& defaultVal) const
{
    wxString tmp;
    if (GetPropVal(propName, &tmp))
        return tmp;
    else
        return defaultVal;
}



void wxXmlNode::AddChild(wxXmlNode *child)
{
    if (m_children == NULL)
        m_children = child;
    else
    {
        wxXmlNode *ch = m_children;
        while (ch->m_next) ch = ch->m_next;
        ch->m_next = child;
    }
    child->m_next = NULL;
    child->m_parent = this;
}



void wxXmlNode::InsertChild(wxXmlNode *child, wxXmlNode *before_node)
{
    wxASSERT_MSG(before_node->GetParent() == this, wxT("wxXmlNode::InsertChild - the node has incorrect parent"));

    if (m_children == before_node)
       m_children = child;
    else
    {
        wxXmlNode *ch = m_children;
        while (ch->m_next != before_node) ch = ch->m_next;
        ch->m_next = child;
    }

    child->m_parent = this;
    child->m_next = before_node;
}



bool wxXmlNode::RemoveChild(wxXmlNode *child)
{
    if (m_children == NULL)
        return FALSE;
    else if (m_children == child)
    {
        m_children = child->m_next;
        child->m_parent = NULL;
        child->m_next = NULL;
        return TRUE;
    }
    else
    {
        wxXmlNode *ch = m_children;
        while (ch->m_next)
        {
            if (ch->m_next == child)
            {
                ch->m_next = child->m_next;
                child->m_parent = NULL;
                child->m_next = NULL;
                return TRUE;
            }
            ch = ch->m_next;
        }
        return FALSE;
    }
}



void wxXmlNode::AddProperty(const wxString& name, const wxString& value)
{
    AddProperty(new wxXmlProperty(name, value, NULL));
}

void wxXmlNode::AddProperty(wxXmlProperty *prop)
{
    if (m_properties == NULL)
        m_properties = prop;
    else
    {
        wxXmlProperty *p = m_properties;
        while (p->GetNext()) p = p->GetNext();
        p->SetNext(prop);
    }
}



bool wxXmlNode::DeleteProperty(const wxString& name)
{
    if (m_properties == NULL)
        return FALSE;

    else if (m_properties->GetName() == name)
    {
        wxXmlProperty *prop = m_properties;
        m_properties = prop->GetNext();
        prop->SetNext(NULL);
        delete prop;
        return TRUE;
    }

    else
    {
        wxXmlProperty *p = m_properties;
        while (p->GetNext())
        {
            if (p->GetNext()->GetName() == name)
            {
                wxXmlProperty *prop = p->GetNext();
                p->SetNext(prop->GetNext());
                prop->SetNext(NULL);
                delete prop;
                return TRUE;
            }
            p = p->GetNext();
        }
        return FALSE;
    }
}








wxList *wxXmlDocument::sm_handlers = NULL;



wxXmlDocument::wxXmlDocument(const wxString& filename, wxXmlIOType io_type)
                          : wxObject(), m_root(NULL)
{
    if (!Load(filename, io_type))
    {
        delete m_root;
        m_root = NULL;
    }
}



wxXmlDocument::wxXmlDocument(wxInputStream& stream, wxXmlIOType io_type)
                          : wxObject(), m_root(NULL)
{
    if (!Load(stream, io_type))
    {
        delete m_root;
        m_root = NULL;
    }
}



wxXmlDocument::wxXmlDocument(const wxXmlDocument& doc)
{
    DoCopy(doc);
}



wxXmlDocument& wxXmlDocument::operator=(const wxXmlDocument& doc)
{
    delete m_root;
    DoCopy(doc);
    return *this;
}



void wxXmlDocument::DoCopy(const wxXmlDocument& doc)
{
    m_version = doc.m_version;
    m_encoding = doc.m_encoding;
    m_root = new wxXmlNode(*doc.m_root);
}



bool wxXmlDocument::Load(const wxString& filename, wxXmlIOType io_type)
{
    wxFileInputStream stream(filename);
    return Load(stream, io_type);
}



bool wxXmlDocument::Load(wxInputStream& stream, wxXmlIOType io_type)
{
    wxNode *n = sm_handlers->GetFirst();
    while (n)
    {
        wxXmlIOHandler *h = (wxXmlIOHandler*) n->GetData();

        if ((io_type == wxXML_IO_AUTO || io_type == h->GetType()) &&
            h->CanLoad(stream))
        {
            return h->Load(stream, *this);
        }
        n = n->GetNext();
    }
    wxLogError(_("Cannot find XML I/O handler capable of loading this format."));
    return FALSE;
}



bool wxXmlDocument::Save(const wxString& filename, wxXmlIOType io_type) const
{
    wxFileOutputStream stream(filename);
    return Save(stream, io_type);
}



bool wxXmlDocument::Save(wxOutputStream& stream, wxXmlIOType io_type) const
{
    wxNode *n = sm_handlers->GetFirst();
    while (n)
    {
        wxXmlIOHandler *h = (wxXmlIOHandler*) n->GetData();
        if (io_type == h->GetType() && h->CanSave())
        {
            return h->Save(stream, *this);
        }
        n = n->GetNext();
    }
    wxLogError(_("Cannot find XML I/O handler capable of saving in this format."));
    return FALSE;
}






void wxXmlDocument::AddHandler(wxXmlIOHandler *handler)
{
    if (sm_handlers == NULL)
    {
        sm_handlers = new wxList;
        sm_handlers->DeleteContents(TRUE);
    }
    sm_handlers->Append(handler);
}


void wxXmlDocument::CleanUpHandlers()
{
    delete sm_handlers;
    sm_handlers = NULL;
}


void wxXmlDocument::InitStandardHandlers()
{
    AddHandler(new wxXmlIOHandlerBin);
#if wxUSE_ZLIB
    AddHandler(new wxXmlIOHandlerBinZ);
#endif
    AddHandler(new wxXmlIOHandlerExpat);
    AddHandler(new wxXmlIOHandlerWriter);
}


#include "wx/module.h"

class wxXmlModule: public wxModule
{
    DECLARE_DYNAMIC_CLASS(wxXmlModule)
    public:
        wxXmlModule() {}
        bool OnInit() { wxXmlDocument::InitStandardHandlers(); return TRUE; };
        void OnExit() { wxXmlDocument::CleanUpHandlers(); };
};

IMPLEMENT_DYNAMIC_CLASS(wxXmlModule, wxModule)




// When wxXml is loaded dynamically after the application is already running
// then the built-in module system won't pick this one up.  Add it manually.
void wxXmlInitXmlModule()
{
    wxModule* module = new wxXmlModule;
    module->Init();
    wxModule::RegisterModule(module);
}

