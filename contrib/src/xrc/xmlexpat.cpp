/////////////////////////////////////////////////////////////////////////////
// Name:        xmlexpat.cpp
// Purpose:     wxXmlDocument - XML reader via Expat
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
#include "wx/xrc/xmlio.h"

#include "xmlparse.h"

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
        return wxString(buf, *conv, len);
        delete[] buf;
    }
    else
        return wxString(s, len);
#endif
}

bool wxXmlIOHandlerExpat::CanLoad(wxInputStream& stream)
{
    char cheader[7];
    cheader[6] = 0;
    stream.Read(cheader, 6);
    stream.SeekI(-6, wxFromCurrent);
    return (strcmp(cheader, "<?xml ") == 0);
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
    wxCSConv conv(name);
    char mbBuf[255];
    wchar_t wcBuf[255];
    size_t i;
    
    for (i = 0; i < 255; i++)
        mbBuf[i] = i+1;
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

bool wxXmlIOHandlerExpat::Load(wxInputStream& stream, wxXmlDocument& doc,
                               const wxString& encoding)
{
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

    doc.SetVersion(ctx.version);
    doc.SetFileEncoding(ctx.encoding);
    doc.SetRoot(ctx.root);

    XML_ParserFree(parser);
#if !wxUSE_UNICODE
    if ( ctx.conv )
        delete ctx.conv;
#endif
    
    return TRUE;
}
