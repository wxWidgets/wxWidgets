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
#include "wx/strconv.h"

#include "wx/xrc/xml.h"

#include "xmlparse.h" // from Expat

//-----------------------------------------------------------------------------
//  wxXmlNode
//-----------------------------------------------------------------------------

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
    wxDELETE(m_properties);
    wxDELETE(m_children);
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
    wxXmlProperty *prop;

    if (m_properties == NULL)
        return FALSE;

    else if (m_properties->GetName() == name)
    {
        prop = m_properties;
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
                prop = p->GetNext();
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



//-----------------------------------------------------------------------------
//  wxXmlDocument
//-----------------------------------------------------------------------------

wxXmlDocument::wxXmlDocument(const wxString& filename, const wxString& encoding)
                          : wxObject(), m_root(NULL)
{
    if ( !Load(filename, encoding) )
    {
        wxDELETE(m_root);
    }
}

wxXmlDocument::wxXmlDocument(wxInputStream& stream, const wxString& encoding)
                          : wxObject(), m_root(NULL)
{
    if ( !Load(stream, encoding) )
    {
        wxDELETE(m_root);
    }
}

wxXmlDocument::wxXmlDocument(const wxXmlDocument& doc)
{
    DoCopy(doc);
}

wxXmlDocument& wxXmlDocument::operator=(const wxXmlDocument& doc)
{
    wxDELETE(m_root);
    DoCopy(doc);
    return *this;
}

void wxXmlDocument::DoCopy(const wxXmlDocument& doc)
{
    m_version = doc.m_version;
#if !wxUSE_UNICODE
    m_encoding = doc.m_encoding;
#endif
    m_fileEncoding = doc.m_fileEncoding;
    m_root = new wxXmlNode(*doc.m_root);
}

bool wxXmlDocument::Load(const wxString& filename, const wxString& encoding)
{
    wxFileInputStream stream(filename);
    return Load(stream, encoding);
}

bool wxXmlDocument::Save(const wxString& filename) const
{
    wxFileOutputStream stream(filename);
    return Save(stream);
}



//-----------------------------------------------------------------------------
//  wxXmlDocument loading routines
//-----------------------------------------------------------------------------

/*
    FIXME:
       - process all elements, including CDATA
 */

// converts Expat-produced string in UTF-8 into wxString.
inline static wxString CharToString(wxMBConv *conv,
                                    const char *s, size_t len = wxSTRING_MAXLEN)
{
#if wxUSE_UNICODE
    (void)conv;
    return wxString(s, wxConvUTF8, len);
#else
    if ( conv )
    {
        size_t nLen = (len != wxSTRING_MAXLEN) ? len :
                          nLen = wxConvUTF8.MB2WC((wchar_t*) NULL, s, 0);

        wchar_t *buf = new wchar_t[nLen+1];
        wxConvUTF8.MB2WC(buf, s, nLen);
        buf[nLen] = 0;
        wxString s(buf, *conv, len);
        delete[] buf;
        return s;
    }
    else
        return wxString(s, len);
#endif
}

struct wxXmlParsingContext
{
    wxMBConv  *conv;
    wxXmlNode *root;
    wxXmlNode *node;
    wxXmlNode *lastAsText;
    wxString   encoding;
    wxString   version;
};

static void StartElementHnd(void *userData, const char *name, const char **atts)
{
    wxXmlParsingContext *ctx = (wxXmlParsingContext*)userData;
    wxXmlNode *node = new wxXmlNode(wxXML_ELEMENT_NODE, CharToString(ctx->conv, name));
    const char **a = atts;
    while (*a)
    {
        node->AddProperty(CharToString(ctx->conv, a[0]), CharToString(ctx->conv, a[1]));
        a += 2;
    }
    if (ctx->root == NULL)
        ctx->root = node;
    else
        ctx->node->AddChild(node);
    ctx->node = node;
    ctx->lastAsText = NULL;
}

static void EndElementHnd(void *userData, const char* WXUNUSED(name))
{
    wxXmlParsingContext *ctx = (wxXmlParsingContext*)userData;

    ctx->node = ctx->node->GetParent();
    ctx->lastAsText = NULL;
}

static void TextHnd(void *userData, const char *s, int len)
{
    wxXmlParsingContext *ctx = (wxXmlParsingContext*)userData;
    char *buf = new char[len + 1];

    buf[len] = '\0';
    memcpy(buf, s, (size_t)len);

    if (ctx->lastAsText)
    {
        ctx->lastAsText->SetContent(ctx->lastAsText->GetContent() +
                                    CharToString(ctx->conv, buf));
    }
    else
    {
        bool whiteOnly = TRUE;
        for (char *c = buf; *c != '\0'; c++)
            if (*c != ' ' && *c != '\t' && *c != '\n' && *c != '\r')
            {
                whiteOnly = FALSE;
                break;
            }
        if (!whiteOnly)
        {
            ctx->lastAsText = new wxXmlNode(wxXML_TEXT_NODE, wxT("text"),
                                            CharToString(ctx->conv, buf));
            ctx->node->AddChild(ctx->lastAsText);
        }
    }

    delete[] buf;
}

static void CommentHnd(void *userData, const char *data)
{
    wxXmlParsingContext *ctx = (wxXmlParsingContext*)userData;

    if (ctx->node)
    {
        // VS: ctx->node == NULL happens if there is a comment before
        //     the root element (e.g. wxDesigner's output). We ignore such
        //     comments, no big deal...
        ctx->node->AddChild(new wxXmlNode(wxXML_COMMENT_NODE,
                            wxT("comment"), CharToString(ctx->conv, data)));
    }
    ctx->lastAsText = NULL;
}

static void DefaultHnd(void *userData, const char *s, int len)
{
    // XML header:
    if (len > 6 && memcmp(s, "<?xml ", 6) == 0)
    {
        wxXmlParsingContext *ctx = (wxXmlParsingContext*)userData;

        wxString buf = CharToString(ctx->conv, s, (size_t)len);
        int pos;
        pos = buf.Find(wxT("encoding="));
        if (pos != wxNOT_FOUND)
            ctx->encoding = buf.Mid(pos + 10).BeforeFirst(buf[(size_t)pos+9]);
        pos = buf.Find(wxT("version="));
        if (pos != wxNOT_FOUND)
            ctx->version = buf.Mid(pos + 9).BeforeFirst(buf[(size_t)pos+8]);
    }
}

static int UnknownEncodingHnd(void * WXUNUSED(encodingHandlerData),
                               const XML_Char *name, XML_Encoding *info)
{
    // We must build conversion table for expat. The easiest way to do so
    // is to let wxCSConv convert as string containing all characters to
    // wide character representation:
    wxCSConv conv(wxString(name, wxConvLibc));
    char mbBuf[255];
    wchar_t wcBuf[255];
    size_t i;

    for (i = 0; i < 255; i++)
        mbBuf[i] = (char) (i+1);
    mbBuf[255] = 0;
    conv.MB2WC(wcBuf, mbBuf, 255);
    wcBuf[255] = 0;

    info->map[0] = 0;
    for (i = 0; i < 255; i++)
        info->map[i+1] = (int)wcBuf[i];

    info->data = NULL;
    info->convert = NULL;
    info->release = NULL;

    return 1;
}

bool wxXmlDocument::Load(wxInputStream& stream, const wxString& encoding)
{
#if wxUSE_UNICODE
    (void)encoding;
#else
    m_encoding = encoding;
#endif

    const size_t BUFSIZE = 1024;
    char buf[BUFSIZE];
    wxXmlParsingContext ctx;
    bool done;
    XML_Parser parser = XML_ParserCreate(NULL);

    ctx.root = ctx.node = NULL;
    ctx.encoding = wxT("UTF-8"); // default in absence of encoding=""
    ctx.conv = NULL;
#if !wxUSE_UNICODE
    if ( encoding != wxT("UTF-8") && encoding != wxT("utf-8") )
        ctx.conv = new wxCSConv(encoding);
#endif

    XML_SetUserData(parser, (void*)&ctx);
    XML_SetElementHandler(parser, StartElementHnd, EndElementHnd);
    XML_SetCharacterDataHandler(parser, TextHnd);
    XML_SetCommentHandler(parser, CommentHnd);
    XML_SetDefaultHandler(parser, DefaultHnd);
    XML_SetUnknownEncodingHandler(parser, UnknownEncodingHnd, NULL);

    do
    {
        size_t len = stream.Read(buf, BUFSIZE).LastRead();
        done = (len < BUFSIZE);
        if (!XML_Parse(parser, buf, len, done))
        {
            wxLogError(_("XML parsing error: '%s' at line %d"),
                       XML_ErrorString(XML_GetErrorCode(parser)),
                       XML_GetCurrentLineNumber(parser));
          return FALSE;
        }
    } while (!done);

    SetVersion(ctx.version);
    SetFileEncoding(ctx.encoding);
    SetRoot(ctx.root);

    XML_ParserFree(parser);
#if !wxUSE_UNICODE
    if ( ctx.conv )
        delete ctx.conv;
#endif

    return TRUE;

}



//-----------------------------------------------------------------------------
//  wxXmlDocument saving routines
//-----------------------------------------------------------------------------

// write string to output:
inline static void OutputString(wxOutputStream& stream, const wxString& str,
                                wxMBConv *convMem, wxMBConv *convFile)
{
    if (str.IsEmpty()) return;
#if wxUSE_UNICODE
    const wxWX2MBbuf buf(str.mb_str(convFile ? *convFile : wxConvUTF8));
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

bool wxXmlDocument::Save(wxOutputStream& stream) const
{
    if ( !IsOk() )
        return FALSE;

    wxString s;

    wxMBConv *convMem = NULL, *convFile = NULL;
#if wxUSE_UNICODE
    convFile = new wxCSConv(GetFileEncoding());
#else
    if ( GetFileEncoding() != GetEncoding() )
    {
        convFile = new wxCSConv(GetFileEncoding());
        convMem = new wxCSConv(GetEncoding());
    }
#endif

    s.Printf(wxT("<?xml version=\"%s\" encoding=\"%s\"?>\n"),
             GetVersion().c_str(), GetFileEncoding().c_str());
    OutputString(stream, s, NULL, NULL);

    OutputNode(stream, GetRoot(), 0, convMem, convFile);
    OutputString(stream, wxT("\n"), NULL, NULL);

    if ( convFile )
        delete convFile;
    if ( convMem )
        delete convMem;

    return TRUE;
}
