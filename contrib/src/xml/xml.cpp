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

#include "wx/xml/xml.h"
#include "wx/xml/xmlio.h"



wxXmlNode::wxXmlNode(wxXmlNode *parent,wxXmlNodeType type, 
                     const wxString& name, const wxString& content,
                     wxXmlProperty *props, wxXmlNode *next)
    : m_Type(type), m_Name(name), m_Content(content),
      m_Properties(props), m_Parent(parent), 
      m_Children(NULL), m_Next(next)
{
    if (m_Parent)
    { 
        if (m_Parent->m_Children)
        {
            m_Next = m_Parent->m_Children;
            m_Parent->m_Children = this;
        }
        else
            m_Parent->m_Children = this;
    }
}



wxXmlNode::wxXmlNode(wxXmlNodeType type, const wxString& name, 
                     const wxString& content)
    : m_Type(type), m_Name(name), m_Content(content),
      m_Properties(NULL), m_Parent(NULL),
      m_Children(NULL), m_Next(NULL)
{}



wxXmlNode::wxXmlNode(const wxXmlNode& node)
{
    m_Next = NULL;
    m_Parent = NULL;
    DoCopy(node);
}



wxXmlNode& wxXmlNode::operator=(const wxXmlNode& node)
{
    delete m_Properties;
    delete m_Children;
    DoCopy(node);
    return *this;
}



void wxXmlNode::DoCopy(const wxXmlNode& node)
{
    m_Type = node.m_Type;
    m_Name = node.m_Name;
    m_Content = node.m_Content;
    m_Children = NULL;

    wxXmlNode *n = node.m_Children;
    while (n)
    {
        AddChild(new wxXmlNode(*n));
        n = n->GetNext();
    }
    
    m_Properties = NULL;
    wxXmlProperty *p = node.m_Properties;
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
    if (m_Children == NULL)
        m_Children = child;
    else
    {
        wxXmlNode *ch = m_Children;
        while (ch->m_Next) ch = ch->m_Next;
        ch->m_Next = child;
    }
    child->m_Next = NULL;
    child->m_Parent = this;
}



void wxXmlNode::InsertChild(wxXmlNode *child, wxXmlNode *before_node)
{
    wxASSERT_MSG(before_node->GetParent() == this, wxT("wxXmlNode::InsertChild - the node has incorrect parent"));

    if (m_Children == before_node)
       m_Children = child;
    else
    {
        wxXmlNode *ch = m_Children;
        while (ch->m_Next != before_node) ch = ch->m_Next;
        ch->m_Next = child;
    }

    child->m_Parent = this;
    child->m_Next = before_node;
}



bool wxXmlNode::RemoveChild(wxXmlNode *child)
{
    if (m_Children == NULL)
        return FALSE;
    else if (m_Children == child)
    {
        m_Children = child->m_Next;
        child->m_Parent = NULL;
        child->m_Next = NULL;
        return TRUE;
    }
    else
    {
        wxXmlNode *ch = m_Children;
        while (ch->m_Next)
        {
            if (ch->m_Next == child)
            {
                ch->m_Next = child->m_Next;
                child->m_Parent = NULL;
                child->m_Next = NULL;
                return TRUE;
            }
            ch = ch->m_Next;
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
    if (m_Properties == NULL)
        m_Properties = prop;
    else
    {
        wxXmlProperty *p = m_Properties;
        while (p->GetNext()) p = p->GetNext();
        p->SetNext(prop);
    }
}



bool wxXmlNode::DeleteProperty(const wxString& name)
{
    if (m_Properties == NULL)
        return FALSE;

    else if (m_Properties->GetName() == name)
    {
        wxXmlProperty *prop = m_Properties;
        m_Properties = prop->GetNext();
        prop->SetNext(NULL);
        delete prop;
        return TRUE;
    }

    else
    {
        wxXmlProperty *p = m_Properties;
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










wxXmlDocument::wxXmlDocument(const wxString& filename, wxXmlIOType io_type)
                          : wxObject(), m_Root(NULL)
{
    if (!Load(filename, io_type)) 
    {
        delete m_Root;
        m_Root = NULL;
    }
}



wxXmlDocument::wxXmlDocument(wxInputStream& stream, wxXmlIOType io_type)
                          : wxObject(), m_Root(NULL)
{
    if (!Load(stream, io_type)) 
    {
        delete m_Root;
        m_Root = NULL;
    }
}



wxXmlDocument::wxXmlDocument(const wxXmlDocument& doc)
{
    DoCopy(doc);
}



wxXmlDocument& wxXmlDocument::operator=(const wxXmlDocument& doc)
{
    delete m_Root;
    DoCopy(doc);
    return *this;
}



void wxXmlDocument::DoCopy(const wxXmlDocument& doc)
{
    m_Version = doc.m_Version;
    m_Encoding = doc.m_Encoding;
    m_Root = new wxXmlNode(*doc.m_Root);
}



bool wxXmlDocument::Load(const wxString& filename, wxXmlIOType io_type)
{
    wxFileInputStream stream(filename);
    return Load(stream, io_type);
}



bool wxXmlDocument::Load(wxInputStream& stream, wxXmlIOType io_type)
{
    wxNode *n = sm_Handlers->GetFirst();
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
    wxNode *n = sm_Handlers->GetFirst();
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






wxList *wxXmlDocument::sm_Handlers = NULL;

void wxXmlDocument::AddHandler(wxXmlIOHandler *handler)
{
    if (sm_Handlers == NULL)
    {
        sm_Handlers = new wxList;
        sm_Handlers->DeleteContents(TRUE);
    }
    sm_Handlers->Append(handler);
}


void wxXmlDocument::CleanUpHandlers()
{
    delete sm_Handlers;
    sm_Handlers = NULL;
}


void wxXmlDocument::InitStandardHandlers()
{
    AddHandler(new wxXmlIOHandlerBin);
    AddHandler(new wxXmlIOHandlerBinZ);
    AddHandler(new wxXmlIOHandlerLibxml);
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
