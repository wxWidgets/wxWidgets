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

     - handle unknown encodings
     - process all elements, including CDATA
     - XML resources should automatically select desired encoding besed on
       runtime environment (?) (would need BIN and BINZ formats modification,
       too)

 */


// converts Expat-produced string in UTF-8 into wxString.
inline static wxString CharToString(const char *s, size_t len = wxSTRING_MAXLEN)
{
#if wxUSE_UNICODE
    return wxString(s, wxMBConvUTF8, len);
#else
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
    wxXmlNode *root;
    wxXmlNode *node;
    wxXmlNode *lastAsText;
    wxString encoding;
    wxString version;
};

static void StartElementHnd(void *userData, const char *name, const char **atts)
{
    wxXmlParsingContext *ctx = (wxXmlParsingContext*)userData;
    wxXmlNode *node = new wxXmlNode(wxXML_ELEMENT_NODE, CharToString(name));
    const char **a = atts;
    while (*a)
    {
        node->AddProperty(CharToString(a[0]), CharToString(a[1]));
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
                                    CharToString(buf));
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
                                            CharToString(buf));
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
                            wxT("comment"), CharToString(data)));
    }
    ctx->lastAsText = NULL;
}

static void DefaultHnd(void *userData, const char *s, int len)
{
    // XML header:
    if (len > 6 && memcmp(s, "<?xml ", 6) == 0)
    {
        wxXmlParsingContext *ctx = (wxXmlParsingContext*)userData;

        wxString buf = CharToString(s, (size_t)len);
        int pos;
        pos = buf.Find(wxT("encoding="));
        if (pos != wxNOT_FOUND)
            ctx->encoding = buf.Mid(pos + 10).BeforeFirst(buf[(size_t)pos+9]);
        pos = buf.Find(wxT("version="));
        if (pos != wxNOT_FOUND)
            ctx->version = buf.Mid(pos + 9).BeforeFirst(buf[(size_t)pos+8]);
    }
}

bool wxXmlIOHandlerExpat::Load(wxInputStream& stream, wxXmlDocument& doc)
{
    const size_t BUFSIZE = 1024;
    char buf[BUFSIZE];
    wxXmlParsingContext ctx;
    bool done;
    XML_Parser parser = XML_ParserCreate(NULL);

    ctx.root = ctx.node = NULL;
    XML_SetUserData(parser, (void*)&ctx);
    XML_SetElementHandler(parser, StartElementHnd, EndElementHnd);
    XML_SetCharacterDataHandler(parser, TextHnd);
    XML_SetCommentHandler(parser, CommentHnd);
    XML_SetDefaultHandler(parser, DefaultHnd);

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
    doc.SetEncoding(ctx.encoding);
    doc.SetRoot(ctx.root);

    XML_ParserFree(parser);
    return TRUE;
}
