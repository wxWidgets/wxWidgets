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
inline static void OutputString(wxOutputStream& stream, const wxString& str,
                                wxMBConv *convMem, wxMBConv *convFile)
{
    if (str.IsEmpty()) return;
#if wxUSE_UNICODE
    const wxW2MBbuf *buf = str.mb_str(convFile ? *convFile : wxConvUTF8);
    stream.Write((const char*)buf, strlen((const char*)buf));
#else
    if ( convFile == NULL )
        stream.Write(str.mb_str(), str.Len());
    else
    {
        wxString str2(str.wc_str(*convMem), *convFile);
        stream.Write(str2.mb_str(), str2.Len());
    }
#endif
}

// Same as above, but create entities first. 
// Translates '<' to "&lt;", '>' to "&gt;" and '&' to "&amp;"
static void OutputStringEnt(wxOutputStream& stream, const wxString& str,
                            wxMBConv *convMem, wxMBConv *convFile)
{
    wxString buf;
    size_t i, last, len;
    wxChar c;
    
    len = str.Len();
    last = 0;
    for (i = 0; i < len; i++)
    {
        c = str.GetChar(i);
        if (c == wxT('<') || c == wxT('>') || 
            (c == wxT('&') && str.Mid(i+1, 4) != wxT("amp;")))
        {
            OutputString(stream, str.Mid(last, i - last), convMem, convFile);
            switch (c)
            {
                case wxT('<'): 
                    OutputString(stream, wxT("&lt;"), NULL, NULL);
                    break;
                case wxT('>'): 
                    OutputString(stream, wxT("&gt;"), NULL, NULL);
                    break;
                case wxT('&'): 
                    OutputString(stream, wxT("&amp;"), NULL, NULL);
                    break;
                default: break;
            }
            last = i + 1;
        }
    }
    OutputString(stream, str.Mid(last, i - last), convMem, convFile);
}

inline static void OutputIndentation(wxOutputStream& stream, int indent)
{
    wxString str = wxT("\n");
    for (int i = 0; i < indent; i++)
        str << wxT(' ') << wxT(' ');
    OutputString(stream, str, NULL, NULL);
}

static void OutputNode(wxOutputStream& stream, wxXmlNode *node, int indent,
                       wxMBConv *convMem, wxMBConv *convFile)
{
    wxXmlNode *n, *prev;
    wxXmlProperty *prop;

    switch (node->GetType())
    {
        case wxXML_TEXT_NODE:
            OutputStringEnt(stream, node->GetContent(), convMem, convFile);
            break;
            
        case wxXML_ELEMENT_NODE:
            OutputString(stream, wxT("<"), NULL, NULL);
            OutputString(stream, node->GetName(), NULL, NULL);
            
            prop = node->GetProperties();
            while (prop)
            {
                OutputString(stream, wxT(" ") + prop->GetName() +
                             wxT("=\"") + prop->GetValue() + wxT("\""),
                             NULL, NULL);
                // FIXME - what if prop contains '"'?
                prop = prop->GetNext();
            }
            
            if (node->GetChildren())
            {
                OutputString(stream, wxT(">"), NULL, NULL);
                prev = NULL;
                n = node->GetChildren();
                while (n)
                {
                    if (n && n->GetType() != wxXML_TEXT_NODE)
                        OutputIndentation(stream, indent + 1);
                    OutputNode(stream, n, indent + 1, convMem, convFile);
                    prev = n;
                    n = n->GetNext();
                }
                if (prev && prev->GetType() != wxXML_TEXT_NODE)
                    OutputIndentation(stream, indent);
                OutputString(stream, wxT("</"), NULL, NULL);
                OutputString(stream, node->GetName(), NULL, NULL);
                OutputString(stream, wxT(">"), NULL, NULL);
            }
            else
                OutputString(stream, wxT("/>"), NULL, NULL);
            break;
             
        case wxXML_COMMENT_NODE:
            OutputString(stream, wxT("<!--"), NULL, NULL);
            OutputString(stream, node->GetContent(), convMem, convFile);
            OutputString(stream, wxT("-->"), NULL, NULL);
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
    
    wxMBConv *convMem = NULL, *convFile = NULL;
#if wxUSE_UNICODE
    convFile = new wxCSConv(doc.GetFileEncoding());
#else
    if ( doc.GetFileEncoding() != doc.GetEncoding() )
    {
        convFile = new wxCSConv(doc.GetFileEncoding());
        convMem = new wxCSConv(doc.GetEncoding());
    }
#endif
    
    s.Printf(wxT("<?xml version=\"%s\" encoding=\"%s\"?>\n"),
             doc.GetVersion().c_str(), doc.GetFileEncoding().c_str());
    OutputString(stream, s, NULL, NULL);
    
    OutputNode(stream, doc.GetRoot(), 0, convMem, convFile);
    OutputString(stream, wxT("\n"), NULL, NULL);
    
    if ( convFile )
        delete convFile;
    if ( convMem )
        delete convMem;
        
    return TRUE;
}
