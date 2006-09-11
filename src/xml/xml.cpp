/////////////////////////////////////////////////////////////////////////////
// Name:        src/xml/xml.cpp
// Purpose:     wxXmlDocument - XML parser & data holder class
// Author:      Vaclav Slavik
// Created:     2000/03/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_XML

#include "wx/xml/xml.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/app.h"
#endif

#include "wx/wfstream.h"
#include "wx/datstrm.h"
#include "wx/zstream.h"
#include "wx/strconv.h"

#include "expat.h" // from Expat

// DLL options compatibility check:
WX_CHECK_BUILD_OPTIONS("wxXML")


IMPLEMENT_CLASS(wxXmlDocument, wxObject)


// a private utility used by wxXML
static bool wxIsWhiteOnly(const wxChar *buf);


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
        if (prop->GetName() == propName) return true;
        prop = prop->GetNext();
    }

    return false;
}

bool wxXmlNode::GetPropVal(const wxString& propName, wxString *value) const
{
    wxXmlProperty *prop = GetProperties();

    while (prop)
    {
        if (prop->GetName() == propName)
        {
            *value = prop->GetValue();
            return true;
        }
        prop = prop->GetNext();
    }

    return false;
}

wxString wxXmlNode::GetPropVal(const wxString& propName, const wxString& defaultVal) const
{
    wxString tmp;
    if (GetPropVal(propName, &tmp))
        return tmp;

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

bool wxXmlNode::InsertChild(wxXmlNode *child, wxXmlNode *before_node)
{
    wxCHECK_MSG(before_node == NULL || before_node->GetParent() == this, false,
                 wxT("wxXmlNode::InsertChild - the node has incorrect parent"));
    wxCHECK_MSG(child, false, wxT("Cannot insert a NULL pointer!"));

    if (m_children == before_node)
       m_children = child;
    else if (m_children == NULL)
    {
        if (before_node != NULL)
            return false;       // we have no children so we don't need to search
        m_children = child;
    }
    else if (before_node == NULL)
    {
        // prepend child
        child->m_parent = this;
        child->m_next = m_children;
        m_children = child;
        return true;
    }
    else
    {
        wxXmlNode *ch = m_children;
        while (ch && ch->m_next != before_node) ch = ch->m_next;
        if (!ch)
            return false;       // before_node not found
        ch->m_next = child;
    }

    child->m_parent = this;
    child->m_next = before_node;
    return true;
}

bool wxXmlNode::RemoveChild(wxXmlNode *child)
{
    if (m_children == NULL)
        return false;
    else if (m_children == child)
    {
        m_children = child->m_next;
        child->m_parent = NULL;
        child->m_next = NULL;
        return true;
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
                return true;
            }
            ch = ch->m_next;
        }
        return false;
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
        return false;

    else if (m_properties->GetName() == name)
    {
        prop = m_properties;
        m_properties = prop->GetNext();
        prop->SetNext(NULL);
        delete prop;
        return true;
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
                return true;
            }
            p = p->GetNext();
        }
        return false;
    }
}

wxString wxXmlNode::GetNodeContent() const
{
    wxXmlNode *n = GetChildren();

    while (n)
    {
        if (n->GetType() == wxXML_TEXT_NODE ||
            n->GetType() == wxXML_CDATA_SECTION_NODE)
            return n->GetContent();
        n = n->GetNext();
    }
    return wxEmptyString;
}

int wxXmlNode::GetDepth(wxXmlNode *grandparent) const
{
    const wxXmlNode *n = this;
    int ret = -1;

    do
    {
        ret++;
        n = n->GetParent();
        if (n == grandparent)
            return ret;

    } while (n);

    return wxNOT_FOUND;
}

bool wxXmlNode::IsWhitespaceOnly() const
{
    return wxIsWhiteOnly(m_content);
}



//-----------------------------------------------------------------------------
//  wxXmlDocument
//-----------------------------------------------------------------------------

wxXmlDocument::wxXmlDocument()
    : m_version(wxT("1.0")), m_fileEncoding(wxT("utf-8")), m_root(NULL)
{
#if !wxUSE_UNICODE
    m_encoding = wxT("UTF-8");
#endif
}

wxXmlDocument::wxXmlDocument(const wxString& filename, const wxString& encoding)
              :wxObject(), m_root(NULL)
{
    if ( !Load(filename, encoding) )
    {
        wxDELETE(m_root);
    }
}

wxXmlDocument::wxXmlDocument(wxInputStream& stream, const wxString& encoding)
              :wxObject(), m_root(NULL)
{
    if ( !Load(stream, encoding) )
    {
        wxDELETE(m_root);
    }
}

wxXmlDocument::wxXmlDocument(const wxXmlDocument& doc)
              :wxObject()
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

bool wxXmlDocument::Load(const wxString& filename, const wxString& encoding, int flags)
{
    wxFileInputStream stream(filename);
    if (!stream.Ok())
        return false;
    return Load(stream, encoding, flags);
}

bool wxXmlDocument::Save(const wxString& filename, int indentstep) const
{
    wxFileOutputStream stream(filename);
    if (!stream.Ok())
        return false;
    return Save(stream, indentstep);
}



//-----------------------------------------------------------------------------
//  wxXmlDocument loading routines
//-----------------------------------------------------------------------------

// converts Expat-produced string in UTF-8 into wxString using the specified
// conv or keep in UTF-8 if conv is NULL
static wxString CharToString(wxMBConv *conv,
                                    const char *s, size_t len = wxSTRING_MAXLEN)
{
#if wxUSE_UNICODE
    wxUnusedVar(conv);

    return wxString(s, wxConvUTF8, len);
#else // !wxUSE_UNICODE
    if ( conv )
    {
        // there can be no embedded NULs in this string so we don't need the
        // output length, it will be NUL-terminated
        const wxWCharBuffer wbuf(
            wxConvUTF8.cMB2WC(s, len == wxSTRING_MAXLEN ? wxNO_LEN : len, NULL));

        return wxString(wbuf, *conv);
    }
    else // already in UTF-8, no conversion needed
    {
        return wxString(s, len != wxSTRING_MAXLEN ? len : strlen(s));
    }
#endif // wxUSE_UNICODE/!wxUSE_UNICODE
}

// returns true if the given string contains only whitespaces
bool wxIsWhiteOnly(const wxChar *buf)
{
    for (const wxChar *c = buf; *c != wxT('\0'); c++)
        if (*c != wxT(' ') && *c != wxT('\t') && *c != wxT('\n') && *c != wxT('\r'))
            return false;
    return true;
}


struct wxXmlParsingContext
{
    wxMBConv  *conv;
    wxXmlNode *root;
    wxXmlNode *node;
    wxXmlNode *lastAsText;
    wxString   encoding;
    wxString   version;
	bool	bLastCdata;
    bool       removeWhiteOnlyNodes;
};

extern "C" {
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
}

extern "C" {
static void EndElementHnd(void *userData, const char* WXUNUSED(name))
{
    wxXmlParsingContext *ctx = (wxXmlParsingContext*)userData;

    ctx->node = ctx->node->GetParent();
    ctx->lastAsText = NULL;
}
}

extern "C" {
static void TextHnd(void *userData, const char *s, int len)
{
    wxXmlParsingContext *ctx = (wxXmlParsingContext*)userData;
    wxString str = CharToString(ctx->conv, s, len);

    if (ctx->lastAsText)
    {
		if ( ctx->bLastCdata )
		{
			ctx->lastAsText->SetContent(ctx->lastAsText->GetContent() +
                                        CharToString(NULL, s, len));
		}
		else
		{
            ctx->lastAsText->SetContent(ctx->lastAsText->GetContent() + str);
		}
    }
    else
    {
        bool whiteOnly = false;
        if (ctx->removeWhiteOnlyNodes)
            whiteOnly = wxIsWhiteOnly(str);

        if (!whiteOnly)
        {
            ctx->lastAsText = new wxXmlNode(wxXML_TEXT_NODE, wxT("text"), str);
            ctx->node->AddChild(ctx->lastAsText);
        }
    }
}
}

extern "C" {
static void StartCdataHnd(void *userData)
{
    wxXmlParsingContext *ctx = (wxXmlParsingContext*)userData;

	ctx->bLastCdata = true;
   
	ctx->lastAsText = new wxXmlNode(wxXML_CDATA_SECTION_NODE, wxT("cdata"),wxT(""));
    ctx->node->AddChild(ctx->lastAsText);         
}
}

extern "C" {
static void EndCdataHnd(void *userData)
{
    wxXmlParsingContext *ctx = (wxXmlParsingContext*)userData;

	ctx->bLastCdata = false;      
}
}

extern "C" {
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
}

extern "C" {
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
}

extern "C" {
static int UnknownEncodingHnd(void * WXUNUSED(encodingHandlerData),
                              const XML_Char *name, XML_Encoding *info)
{
    // We must build conversion table for expat. The easiest way to do so
    // is to let wxCSConv convert as string containing all characters to
    // wide character representation:
    wxString str(name, wxConvLibc);
    wxCSConv conv(str);
    char mbBuf[2];
    wchar_t wcBuf[10];
    size_t i;

    mbBuf[1] = 0;
    info->map[0] = 0;
    for (i = 0; i < 255; i++)
    {
        mbBuf[0] = (char)(i+1);
        if (conv.MB2WC(wcBuf, mbBuf, 2) == (size_t)-1)
        {
            // invalid/undefined byte in the encoding:
            info->map[i+1] = -1;
        }
        info->map[i+1] = (int)wcBuf[0];
    }

    info->data = NULL;
    info->convert = NULL;
    info->release = NULL;

    return 1;
}
}

bool wxXmlDocument::Load(wxInputStream& stream, const wxString& encoding, int flags)
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
    ctx.removeWhiteOnlyNodes = (flags & wxXMLDOC_KEEP_WHITESPACE_NODES) == 0;
	ctx.bLastCdata = false;

    XML_SetUserData(parser, (void*)&ctx);
    XML_SetElementHandler(parser, StartElementHnd, EndElementHnd);
    XML_SetCharacterDataHandler(parser, TextHnd);
	XML_SetCdataSectionHandler(parser, StartCdataHnd, EndCdataHnd );
    XML_SetCommentHandler(parser, CommentHnd);
    XML_SetDefaultHandler(parser, DefaultHnd);
    XML_SetUnknownEncodingHandler(parser, UnknownEncodingHnd, NULL);

    bool ok = true;
    do
    {
        size_t len = stream.Read(buf, BUFSIZE).LastRead();
        done = (len < BUFSIZE);
        if (!XML_Parse(parser, buf, len, done))
        {
            wxString error(XML_ErrorString(XML_GetErrorCode(parser)),
                           *wxConvCurrent);
            wxLogError(_("XML parsing error: '%s' at line %d"),
                       error.c_str(),
                       XML_GetCurrentLineNumber(parser));
            ok = false;
            break;
        }
    } while (!done);

    if (ok)
    {
        if (!ctx.version.empty())
            SetVersion(ctx.version);
        if (!ctx.encoding.empty())
            SetFileEncoding(ctx.encoding);
        SetRoot(ctx.root);
    }
    else
    {
        delete ctx.root;
    }

    XML_ParserFree(parser);
#if !wxUSE_UNICODE
    if ( ctx.conv )
        delete ctx.conv;
#endif

    return ok;

}



//-----------------------------------------------------------------------------
//  wxXmlDocument saving routines
//-----------------------------------------------------------------------------

// write string to output:
inline static void OutputString(wxOutputStream& stream, const wxString& str,
                                wxMBConv *convMem = NULL,
                                wxMBConv *convFile = NULL)
{
    if (str.empty())
        return;

#if wxUSE_UNICODE
    wxUnusedVar(convMem);

    const wxWX2MBbuf buf(str.mb_str(*(convFile ? convFile : &wxConvUTF8)));
    stream.Write((const char*)buf, strlen((const char*)buf));
#else // !wxUSE_UNICODE
    if ( convFile && convMem )
    {
        wxString str2(str.wc_str(*convMem), *convFile);
        stream.Write(str2.mb_str(), str2.Len());
    }
    else // no conversions to do
    {
        stream.Write(str.mb_str(), str.Len());
    }
#endif // wxUSE_UNICODE/!wxUSE_UNICODE
}

// flags for OutputStringEnt()
enum
{
    XML_ESCAPE_QUOTES = 1
};

// Same as above, but create entities first.
// Translates '<' to "&lt;", '>' to "&gt;" and '&' to "&amp;"
static void OutputStringEnt(wxOutputStream& stream, const wxString& str,
                            wxMBConv *convMem = NULL,
                            wxMBConv *convFile = NULL,
                            int flags = 0)
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
            (c == wxT('&') && str.Mid(i+1, 4) != wxT("amp;")) ||
            ((flags & XML_ESCAPE_QUOTES) && c == wxT('"')))
        {
            OutputString(stream, str.Mid(last, i - last), convMem, convFile);
            switch (c)
            {
                case wxT('<'):
                    OutputString(stream, wxT("&lt;"));
                    break;
                case wxT('>'):
                    OutputString(stream, wxT("&gt;"));
                    break;
                case wxT('&'):
                    OutputString(stream, wxT("&amp;"));
                    break;
                case wxT('"'):
                    OutputString(stream, wxT("&quot;"));
                    break;
                default:
                    break;
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
    OutputString(stream, str);
}

static void OutputNode(wxOutputStream& stream, wxXmlNode *node, int indent,
                       wxMBConv *convMem, wxMBConv *convFile, int indentstep)
{
    wxXmlNode *n, *prev;
    wxXmlProperty *prop;

    switch (node->GetType())
    {
		case wxXML_CDATA_SECTION_NODE:
			OutputString( stream, wxT("<![CDATA["));
			OutputString( stream, node->GetContent() );
			OutputString( stream, wxT("]]>") );
			break;

        case wxXML_TEXT_NODE:
            OutputStringEnt(stream, node->GetContent(), convMem, convFile);
            break;

        case wxXML_ELEMENT_NODE:
            OutputString(stream, wxT("<"));
            OutputString(stream, node->GetName());

            prop = node->GetProperties();
            while (prop)
            {
                OutputString(stream, wxT(" ") + prop->GetName() +  wxT("=\""));
                OutputStringEnt(stream, prop->GetValue(), convMem, convFile,
                                XML_ESCAPE_QUOTES);
                OutputString(stream, wxT("\""));
                prop = prop->GetNext();
            }

            if (node->GetChildren())
            {
                OutputString(stream, wxT(">"));
                prev = NULL;
                n = node->GetChildren();
                while (n)
                {
                    if (indentstep >= 0 && n && n->GetType() != wxXML_TEXT_NODE)
                        OutputIndentation(stream, indent + indentstep);
                    OutputNode(stream, n, indent + indentstep, convMem, convFile, indentstep);
                    prev = n;
                    n = n->GetNext();
                }
                if (indentstep >= 0 && prev && prev->GetType() != wxXML_TEXT_NODE)
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
            OutputString(stream, node->GetContent(), convMem, convFile);
            OutputString(stream, wxT("-->"));
            break;

        default:
            wxFAIL_MSG(wxT("unsupported node type"));
    }
}

bool wxXmlDocument::Save(wxOutputStream& stream, int indentstep) const
{
    if ( !IsOk() )
        return false;

    wxString s;

    wxMBConv *convMem = NULL;

#if wxUSE_UNICODE
    wxMBConv *convFile = new wxCSConv(GetFileEncoding());
#else
    wxMBConv *convFile = NULL;
    if ( GetFileEncoding() != GetEncoding() )
    {
        convFile = new wxCSConv(GetFileEncoding());
        convMem = new wxCSConv(GetEncoding());
    }
#endif

    s.Printf(wxT("<?xml version=\"%s\" encoding=\"%s\"?>\n"),
             GetVersion().c_str(), GetFileEncoding().c_str());
    OutputString(stream, s);

    OutputNode(stream, GetRoot(), 0, convMem, convFile, indentstep);
    OutputString(stream, wxT("\n"));

    if ( convFile )
        delete convFile;
    if ( convMem )
        delete convMem;

    return true;
}

#endif // wxUSE_XML
