/////////////////////////////////////////////////////////////////////////////
// Name:        xmlwrite.cpp
// Purpose:     wxXmlDocument - XML text writer
// Author:      Vaclav Slavik
// Created:     2001/04/30
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
// nothing - already in xml.cpp
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/wfstream.h"
#include "wx/intl.h"
#include "wx/log.h"
#include "wx/strconv.h"
#include "wx/xrc/xml.h"
#include "wx/xrc/xmlio.h"

// write string to output:
inline static void OutputString(wxOutputStream& stream, const wxString& str)
{
    if (str.IsEmpty()) return;
#if wxUSE_UNICODE
    char *buf = str.mb_str(wxMBConvUTF8);
    stream.Write(buf, strlen(buf));
#else
    stream.Write(str.mb_str(), str.Len());        
#endif
}

// Same as above, but create entities first. 
// Translates '<' to "&lt;", '>' to "&gt;" and '&' to "&amp;"
static void OutputStringEnt(wxOutputStream& stream, const wxString& str)
{
    wxString buf;
    size_t i, last, len;
    char c;
    
    len = str.Len();
    last = 0;
    for (i = 0; i < len; i++)
    {
        c = str.GetChar(i);
        if (c == '<' || c == '>' || 
            (c == '&' && str.Mid(i+1, 4) != wxT("amp;")))
        {
            OutputString(stream, str.Mid(last, i - last));
            switch (c)
            {
                case '<': OutputString(stream, wxT("&lt;")); break;
                case '>': OutputString(stream, wxT("&gt;")); break;
                case '&': OutputString(stream, wxT("&amp;")); break;
                default: break;
            }
            last = i + 1;
        }
    }
    OutputString(stream, str.Mid(last, i - last));
}

inline static void OutputIndentation(wxOutputStream& stream, int indent)
{
    wxString str = wxT("\n");
    for (int i = 0; i < indent; i++)
        str << wxT(' ') << wxT(' ');
    OutputString(stream, str);
}

static void OutputNode(wxOutputStream& stream, wxXmlNode *node, int indent)
{
    wxXmlNode *n, *prev;
    wxXmlProperty *prop;

    switch (node->GetType())
    {
        case wxXML_TEXT_NODE:
            OutputStringEnt(stream, node->GetContent());
            break;
            
        case wxXML_ELEMENT_NODE:
            OutputString(stream, wxT("<"));
            OutputString(stream, node->GetName());
            
            prop = node->GetProperties();
            while (prop)
            {
                OutputString(stream, wxT(" ") + prop->GetName() +
                             wxT("=\"") + prop->GetValue() + wxT("\""));
                // FIXME - what if prop contains '"'?
                prop = prop->GetNext();
            }
            
            if (node->GetChildren())
            {
                OutputString(stream, wxT(">"));
                prev = NULL;
                n = node->GetChildren();
                while (n)
                {
                    if (n && n->GetType() != wxXML_TEXT_NODE)
                        OutputIndentation(stream, indent + 1);
                    OutputNode(stream, n, indent + 1);
                    prev = n;
                    n = n->GetNext();
                }
                if (prev && prev->GetType() != wxXML_TEXT_NODE)
                    OutputIndentation(stream, indent);
                OutputString(stream, wxT("</"));
                OutputString(stream, node->GetName());
                OutputString(stream, wxT(">"));
            }
            else
                OutputString(stream, wxT("/>"));
            break;
             
        case wxXML_COMMENT_NODE:
            OutputString(stream, wxT("<!--"));
            OutputString(stream, node->GetContent());
            OutputString(stream, wxT("-->"));
            break;
            
        default:
            wxFAIL_MSG(wxT("unsupported node type"));
    }
}

bool wxXmlIOHandlerWriter::Save(wxOutputStream& stream, const wxXmlDocument& doc)
{
    if (!doc.IsOk())
        return FALSE;
        
    wxString s;
    
    s = wxT("<?xml version=\"") + doc.GetVersion() + 
        wxT("\" encoding=\"utf-8\"?>\n");
    OutputString(stream, s);  
    
    OutputNode(stream, doc.GetRoot(), 0);
    OutputString(stream, wxT("\n"));
        
    return TRUE;
}
