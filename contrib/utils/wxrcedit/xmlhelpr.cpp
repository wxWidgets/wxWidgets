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
#include "xmlhelpr.h"



wxXmlNode *XmlFindNode(wxXmlNode *parent, const wxString& param)
{
    wxXmlNode *n = parent->GetChildren();
    
    while (n)
    {
        if (n->GetType() == wxXML_ELEMENT_NODE && n->GetName() == param)
            return n;
        n = n->GetNext();
    }
    return NULL;
}


void XmlWriteValue(wxXmlNode *parent, const wxString& name, const wxString& value)
{
    wxXmlNode *n = XmlFindNode(parent, name);
    if (n == NULL) 
    {
        n = new wxXmlNode(wxXML_ELEMENT_NODE, name);
        parent->AddChild(n);
        n->AddChild(new wxXmlNode(wxXML_TEXT_NODE, ""));
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





