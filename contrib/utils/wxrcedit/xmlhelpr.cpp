/////////////////////////////////////////////////////////////////////////////
// Author:      Vaclav Slavik
// Created:     2000/05/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "xmlhelpr.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xml/xml.h"
#include "wx/wx.h"
#include "wx/tokenzr.h"
#include "xmlhelpr.h"



wxXmlNode *XmlFindNodeSimple(wxXmlNode *parent, const wxString& param)
{
    if (param.IsEmpty()) return parent;

    wxXmlNode *n = parent->GetChildren();
    
    while (n)
    {
        if (n->GetType() == wxXML_ELEMENT_NODE && n->GetName() == param)
            return n;
        n = n->GetNext();
    }
    return NULL;
}



wxXmlNode *XmlFindNode(wxXmlNode *parent, const wxString& path)
{
    wxXmlNode *n = parent;
    wxStringTokenizer tkn(path, _T("/"));
    while (tkn.HasMoreTokens())
    {
        n = XmlFindNodeSimple(n, tkn.GetNextToken());
        if (n == NULL) break;
    }
    return n;
}



void XmlWriteValue(wxXmlNode *parent, const wxString& name, const wxString& value)
{
    wxXmlNode *n = XmlFindNode(parent, name);
    if (n == NULL) 
    {
        wxString pname = name.BeforeLast(_T('/'));
        if (pname.IsEmpty()) pname = name;
        wxXmlNode *p = XmlFindNode(parent, pname);
        if (p == NULL) p = parent;
        n = new wxXmlNode(wxXML_ELEMENT_NODE, name.AfterLast(_T('/')));
        p->AddChild(n);
        n->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString));
    }
    
    n = n->GetChildren();
    
    while (n)
    {
        if (n->GetType() == wxXML_TEXT_NODE || 
            n->GetType() == wxXML_CDATA_SECTION_NODE)
        {
            n->SetContent(value);
            break;
        }
        n = n->GetNext();
    }
}



wxString XmlReadValue(wxXmlNode *parent, const wxString& name)
{
    wxXmlNode *n = XmlFindNode(parent, name);
    if (n == NULL) return wxEmptyString;
    n = n->GetChildren();
    
    while (n)
    {
        if (n->GetType() == wxXML_TEXT_NODE || 
            n->GetType() == wxXML_CDATA_SECTION_NODE)
            return n->GetContent();
        n = n->GetNext();
    }
    return wxEmptyString;
}



wxString XmlGetClass(wxXmlNode *parent)
{
    return parent->GetPropVal(_T("class"), wxEmptyString);
}



void XmlSetClass(wxXmlNode *parent, const wxString& classname)
{
    parent->DeleteProperty(_T("class"));
    parent->AddProperty(_T("class"), classname);
}





