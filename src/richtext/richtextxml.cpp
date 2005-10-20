/////////////////////////////////////////////////////////////////////////////
// Name:        richtext/richtextxml.cpp
// Purpose:     XML and HTML I/O for wxRichTextCtrl
// Author:      Julian Smart
// Modified by:
// Created:     2005-09-30
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include "wx/image.h"

#if wxUSE_RICHTEXT

#include "wx/filename.h"
#include "wx/clipbrd.h"
#include "wx/wfstream.h"
#include "wx/sstream.h"
#include "wx/module.h"
#include "wx/txtstrm.h"
#include "wx/xml/xml.h"

#include "wx/richtext/richtextxml.h"

IMPLEMENT_DYNAMIC_CLASS(wxRichTextXMLHandler, wxRichTextFileHandler)

#if wxUSE_STREAMS
bool wxRichTextXMLHandler::DoLoadFile(wxRichTextBuffer *buffer, wxInputStream& stream)
{
    if (!stream.IsOk())
        return false;

    buffer->Clear();

    wxXmlDocument* xmlDoc = new wxXmlDocument;
    bool success = true;

    if (!xmlDoc->Load(stream, wxT("ISO-8859-1")))
    {
        success = false;
    }
    else
    {
        if (xmlDoc->GetRoot() && xmlDoc->GetRoot()->GetType() == wxXML_ELEMENT_NODE && xmlDoc->GetRoot()->GetName() == wxT("richtext"))
        {
            wxXmlNode* child = xmlDoc->GetRoot()->GetChildren();
            while (child)
            {
                if (child->GetType() == wxXML_ELEMENT_NODE)
                {
                    wxString name = child->GetName();
                    if (name == wxT("richtext-version"))
                    {
                    }
                    else
                        ImportXML(buffer, child);
                }

                child = child->GetNext();
            }
        }
        else
        {
            success = false;
        }
    }

    delete xmlDoc;

    buffer->UpdateRanges();

    return success;
}

/// Recursively import an object
bool wxRichTextXMLHandler::ImportXML(wxRichTextBuffer* buffer, wxXmlNode* node)
{
    wxString name = node->GetName();

    bool doneChildren = false;

    if (name == wxT("paragraphlayout"))
    {
    }
    else if (name == wxT("paragraph"))
    {
        wxRichTextParagraph* para = new wxRichTextParagraph(buffer);
        buffer->AppendChild(para);

        GetStyle(para->GetAttributes(), node, true);

        wxXmlNode* child = node->GetChildren();
        while (child)
        {
            wxString childName = child->GetName();
            if (childName == wxT("text"))
            {
                wxString text;
                wxXmlNode* textChild = child->GetChildren();
                while (textChild)
                {
                    if (textChild->GetType() == wxXML_TEXT_NODE ||
                        textChild->GetType() == wxXML_CDATA_SECTION_NODE)
                    {
                        wxString text2 = textChild->GetContent();

                        // Strip whitespace from end
                        if (text2.Length() > 0 && text2[text2.Length()-1] == wxT('\n'))
                            text2 = text2.Mid(0, text2.Length()-1);

                        if (text2.Length() > 0 && text2[0] == wxT('"'))
                            text2 = text2.Mid(1);
                        if (text2.Length() > 0 && text2[text2.Length()-1] == wxT('"'))
                            text2 = text2.Mid(0, text2.Length() - 1);

                        // TODO: further entity translation
                        text2.Replace(wxT("&lt;"), wxT("<"));
                        text2.Replace(wxT("&gt;"), wxT(">"));
                        text2.Replace(wxT("&amp;"), wxT("&"));
                        text2.Replace(wxT("&quot;"), wxT("\""));

                        text += text2;
                    }
                    textChild = textChild->GetNext();
                }

                wxRichTextPlainText* textObject = new wxRichTextPlainText(text, para);
                GetStyle(textObject->GetAttributes(), child, false);

                para->AppendChild(textObject);
            }
            else if (childName == wxT("image"))
            {
                int imageType = wxBITMAP_TYPE_PNG;
                wxString value = node->GetPropVal(wxT("imagetype"), wxEmptyString);
                if (!value.empty())
                    imageType = wxAtoi(value);

                wxString data;

                wxXmlNode* imageChild = child->GetChildren();
                while (imageChild)
                {
                    wxString childName = imageChild->GetName();
                    if (childName == wxT("data"))
                    {
                        wxXmlNode* dataChild = imageChild->GetChildren();
                        while (dataChild)
                        {
                            data = dataChild->GetContent();
                            // wxLogDebug(data);
                            dataChild = dataChild->GetNext();
                        }

                    }
                    imageChild = imageChild->GetNext();
                }

                if (!data.empty())
                {
                    wxRichTextImage* imageObj = new wxRichTextImage(para);
                    para->AppendChild(imageObj);

                    wxStringInputStream strStream(data);

                    imageObj->GetImageBlock().ReadHex(strStream, data.Length(), imageType);
                }
            }
            child = child->GetNext();
        }

        doneChildren = true;
    }

    if (!doneChildren)
    {
        wxXmlNode* child = node->GetChildren();
        while (child)
        {
            ImportXML(buffer, child);
            child = child->GetNext();
        }
    }

    return true;
}


//-----------------------------------------------------------------------------
//  xml support routines
//-----------------------------------------------------------------------------

bool wxRichTextXMLHandler::HasParam(wxXmlNode* node, const wxString& param)
{
    return (GetParamNode(node, param) != NULL);
}

wxXmlNode *wxRichTextXMLHandler::GetParamNode(wxXmlNode* node, const wxString& param)
{
    wxCHECK_MSG(node, NULL, wxT("You can't access node data before it was initialized!"));

    wxXmlNode *n = node->GetChildren();

    while (n)
    {
        if (n->GetType() == wxXML_ELEMENT_NODE && n->GetName() == param)
            return n;
        n = n->GetNext();
    }
    return NULL;
}


wxString wxRichTextXMLHandler::GetNodeContent(wxXmlNode *node)
{
    wxXmlNode *n = node;
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


wxString wxRichTextXMLHandler::GetParamValue(wxXmlNode *node, const wxString& param)
{
    if (param.empty())
        return GetNodeContent(node);
    else
        return GetNodeContent(GetParamNode(node, param));
}

wxString wxRichTextXMLHandler::GetText(wxXmlNode *node, const wxString& param, bool WXUNUSED(translate))
{
    wxXmlNode *parNode = GetParamNode(node, param);
    if (!parNode)
        parNode = node;
    wxString str1(GetNodeContent(parNode));
    return str1;
}

// For use with earlier versions of wxWidgets
#ifndef WXUNUSED_IN_UNICODE
#if wxUSE_UNICODE
#define WXUNUSED_IN_UNICODE(x) WXUNUSED(x)
#else
#define WXUNUSED_IN_UNICODE(x) x
#endif
#endif

// write string to output:
inline static void OutputString(wxOutputStream& stream, const wxString& str,
                                wxMBConv *WXUNUSED_IN_UNICODE(convMem) = NULL, wxMBConv *convFile = NULL)
{
    if (str.empty()) return;
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
                            wxMBConv *convMem = NULL, wxMBConv *convFile = NULL)
{
    wxString buf;
    size_t i, last, len;
    wxChar c;

    len = str.Len();
    last = 0;
    for (i = 0; i < len; i++)
    {
        c = str.GetChar(i);
        if (c == wxT('<') || c == wxT('>') || c == wxT('"') ||
            (c == wxT('&') && (str.Mid(i+1, 4) != wxT("amp;"))))
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
            case wxT('"'):
                OutputString(stream, wxT("&quot;"), NULL, NULL);
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

static wxOutputStream& operator <<(wxOutputStream& stream, const wxString& s)
{
    stream.Write(s, s.Length());
    return stream;
}

static wxOutputStream& operator <<(wxOutputStream& stream, long l)
{
    wxString str;
    str.Printf(wxT("%ld"), l);
    return stream << str;
}

static wxOutputStream& operator <<(wxOutputStream& stream, const char c)
{
    wxString str;
    str.Printf(wxT("%c"), c);
    return stream << str;
}

// Convert a colour to a 6-digit hex string
static wxString ColourToHexString(const wxColour& col)
{
    wxString hex;

    hex += wxDecToHex(col.Red());
    hex += wxDecToHex(col.Green());
    hex += wxDecToHex(col.Blue());

    return hex;
}

// Convert 6-digit hex string to a colour
wxColour HexStringToColour(const wxString& hex)
{
    unsigned char r = (unsigned char)wxHexToDec(hex.Mid(0, 2));
    unsigned char g = (unsigned char)wxHexToDec(hex.Mid(2, 2));
    unsigned char b = (unsigned char)wxHexToDec(hex.Mid(4, 2));

    return wxColour(r, g, b);
}

bool wxRichTextXMLHandler::DoSaveFile(wxRichTextBuffer *buffer, wxOutputStream& stream)
{
    if (!stream.IsOk())
        return false;

    wxString version(wxT("1.0") ) ;
#if wxUSE_UNICODE
    wxString fileencoding(wxT("UTF-8")) ;
    wxString memencoding(wxT("UTF-8")) ;
#else
    wxString fileencoding(wxT("ISO-8859-1")) ;
    wxString memencoding(wxT("ISO-8859-1")) ;
#endif
    wxString s ;

    wxMBConv *convMem = NULL, *convFile = NULL;
#if wxUSE_UNICODE
    convFile = new wxCSConv(fileencoding);
#else
    if ( fileencoding != memencoding )
    {
        convFile = new wxCSConv(fileencoding);
        convMem = new wxCSConv(memencoding);
    }
#endif

    s.Printf(wxT("<?xml version=\"%s\" encoding=\"%s\"?>\n"),
        (const wxChar*) version, (const wxChar*) fileencoding );
    OutputString(stream, s, NULL, NULL);
    OutputString(stream, wxT("<richtext version=\"1.0.0.0\" xmlns=\"http://www.wxwidgets.org\">") , NULL, NULL);

    int level = 1;
    ExportXML(stream, convMem, convFile, *buffer, level);

    OutputString(stream, wxT("\n</richtext>") , NULL, NULL);
    OutputString(stream, wxT("\n"), NULL, NULL);

    delete convFile;
    delete convMem;

    return true;
}

/// Recursively export an object
bool wxRichTextXMLHandler::ExportXML(wxOutputStream& stream, wxMBConv* convMem, wxMBConv* convFile, wxRichTextObject& obj, int indent)
{
    wxString objectName;
    if (obj.IsKindOf(CLASSINFO(wxRichTextParagraphLayoutBox)))
        objectName = wxT("paragraphlayout");
    else if (obj.IsKindOf(CLASSINFO(wxRichTextParagraph)))
        objectName = wxT("paragraph");
    else if (obj.IsKindOf(CLASSINFO(wxRichTextPlainText)))
        objectName = wxT("text");
    else if (obj.IsKindOf(CLASSINFO(wxRichTextImage)))
        objectName = wxT("image");
    else
        objectName = wxT("object");

    if (obj.IsKindOf(CLASSINFO(wxRichTextPlainText)))
    {
        wxRichTextPlainText& text = (wxRichTextPlainText&) obj;

        OutputIndentation(stream, indent);
        stream << wxT("<") << objectName;

        wxString style = CreateStyle(obj.GetAttributes(), false);

        stream << style << wxT(">");

        wxString str = text.GetText();
        if (str.Length() > 0 && (str[0] == wxT(' ') || str[str.Length()-1] == wxT(' ')))
        {
            stream << wxT("\"");
            OutputStringEnt(stream, str, convMem, convFile);
            stream << wxT("\"");
        }
        else
            OutputStringEnt(stream, str, convMem, convFile);
    }
    else if (obj.IsKindOf(CLASSINFO(wxRichTextImage)))
    {
        wxRichTextImage& imageObj = (wxRichTextImage&) obj;

        if (imageObj.GetImage().Ok() && !imageObj.GetImageBlock().Ok())
            imageObj.MakeBlock();

        OutputIndentation(stream, indent);
        stream << wxT("<") << objectName;
        if (!imageObj.GetImageBlock().Ok())
        {
            // No data
            stream << wxT(">");
        }
        else
        {
            stream << wxString::Format(wxT(" imagetype=\"%d\""), (int) imageObj.GetImageBlock().GetImageType()) << wxT(">");
        }

        OutputIndentation(stream, indent+1);
        stream << wxT("<data>");

        imageObj.GetImageBlock().WriteHex(stream);

        stream << wxT("</data>");
    }
    else if (obj.IsKindOf(CLASSINFO(wxRichTextCompositeObject)))
    {
        OutputIndentation(stream, indent);
        stream << wxT("<") << objectName;

        bool isPara = false;
        if (objectName == wxT("paragraph") || objectName == wxT("paragraphlayout"))
            isPara = true;

        wxString style = CreateStyle(obj.GetAttributes(), isPara);

        stream << style << wxT(">");

        wxRichTextCompositeObject& composite = (wxRichTextCompositeObject&) obj;
        size_t i;
        for (i = 0; i < composite.GetChildCount(); i++)
        {
            wxRichTextObject* child = composite.GetChild(i);
            ExportXML(stream, convMem, convFile, *child, indent+1);
        }
    }

    if (objectName != wxT("text"))
        OutputIndentation(stream, indent);

    stream << wxT("</") << objectName << wxT(">");

    return true;
}

/// Create style parameters
wxString wxRichTextXMLHandler::CreateStyle(const wxTextAttrEx& attr, bool isPara)
{
    wxString str;
    if (attr.GetTextColour().Ok())
    {
        str << wxT(" textcolor=\"#") << ColourToHexString(attr.GetTextColour()) << wxT("\"");
    }
    if (attr.GetBackgroundColour().Ok())
    {
        str << wxT(" bgcolor=\"#") << ColourToHexString(attr.GetBackgroundColour()) << wxT("\"");
    }

    if (attr.GetFont().Ok())
    {
        str << wxT(" fontsize=\"") << attr.GetFont().GetPointSize() << wxT("\"");
        str << wxT(" fontfamily=\"") << attr.GetFont().GetFamily() << wxT("\"");
        str << wxT(" fontstyle=\"") << attr.GetFont().GetStyle() << wxT("\"");
        str << wxT(" fontweight=\"") << attr.GetFont().GetWeight() << wxT("\"");
        str << wxT(" fontunderlined=\"") << (int) attr.GetFont().GetUnderlined() << wxT("\"");
        str << wxT(" fontface=\"") << attr.GetFont().GetFaceName() << wxT("\"");
    }

    if (!attr.GetCharacterStyleName().empty())
        str << wxT(" charactertyle=\"") << wxString(attr.GetCharacterStyleName()) << wxT("\"");

    if (isPara)
    {
        str << wxT(" alignment=\"") << (int) attr.GetAlignment() << wxT("\"");
        str << wxT(" leftindent=\"") << (int) attr.GetLeftIndent() << wxT("\"");
        str << wxT(" leftsubindent=\"") << (int) attr.GetLeftSubIndent() << wxT("\"");
        str << wxT(" rightindent=\"") << (int) attr.GetRightIndent() << wxT("\"");
        str << wxT(" parspacingafter=\"") << (int) attr.GetParagraphSpacingAfter() << wxT("\"");
        str << wxT(" parspacingbefore=\"") << (int) attr.GetParagraphSpacingBefore() << wxT("\"");
        str << wxT(" linespacing=\"") << (int) attr.GetLineSpacing() << wxT("\"");
        str << wxT(" bulletstyle=\"") << (int) attr.GetBulletStyle() << wxT("\"");
        str << wxT(" bulletnumber=\"") << (int) attr.GetBulletNumber() << wxT("\"");
        str << wxT(" bulletsymbol=\"") << wxString(attr.GetBulletSymbol()) << wxT("\"");

        if (!attr.GetParagraphStyleName().empty())
            str << wxT(" parstyle=\"") << wxString(attr.GetParagraphStyleName()) << wxT("\"");
    }

    return str;
}

/// Get style parameters
bool wxRichTextXMLHandler::GetStyle(wxTextAttrEx& attr, wxXmlNode* node, bool isPara)
{
    wxString fontFacename;
    int fontSize = 12;
    int fontFamily = wxDEFAULT;
    int fontWeight = wxNORMAL;
    int fontStyle = wxNORMAL;
    bool fontUnderlined = false;

    fontFacename = node->GetPropVal(wxT("fontface"), wxEmptyString);

    wxString value = node->GetPropVal(wxT("fontfamily"), wxEmptyString);
    if (!value.empty())
        fontFamily = wxAtoi(value);

    value = node->GetPropVal(wxT("fontstyle"), wxEmptyString);
    if (!value.empty())
        fontStyle = wxAtoi(value);

    value = node->GetPropVal(wxT("fontsize"), wxEmptyString);
    if (!value.empty())
        fontSize = wxAtoi(value);

    value = node->GetPropVal(wxT("fontweight"), wxEmptyString);
    if (!value.empty())
        fontWeight = wxAtoi(value);

    value = node->GetPropVal(wxT("fontunderlined"), wxEmptyString);
    if (!value.empty())
        fontUnderlined = wxAtoi(value) != 0;

    attr.SetFont(* wxTheFontList->FindOrCreateFont(fontSize, fontFamily, fontStyle, fontWeight, fontUnderlined, fontFacename));

    value = node->GetPropVal(wxT("textcolor"), wxEmptyString);
    if (!value.empty())
    {
        if (value[0] == wxT('#'))
            attr.SetTextColour(HexStringToColour(value.Mid(1)));
        else
            attr.SetTextColour(value);
    }

    value = node->GetPropVal(wxT("backgroundcolor"), wxEmptyString);
    if (!value.empty())
    {
        if (value[0] == wxT('#'))
            attr.SetBackgroundColour(HexStringToColour(value.Mid(1)));
        else
            attr.SetBackgroundColour(value);
    }

    value = node->GetPropVal(wxT("characterstyle"), wxEmptyString);
    if (!value.empty())
        attr.SetCharacterStyleName(value);

    // Set paragraph attributes
    if (isPara)
    {
        value = node->GetPropVal(wxT("alignment"), wxEmptyString);
        if (!value.empty())
            attr.SetAlignment((wxTextAttrAlignment) wxAtoi(value));

        int leftSubIndent = 0;
        int leftIndent = 0;
        value = node->GetPropVal(wxT("leftindent"), wxEmptyString);
        if (!value.empty())
            leftIndent = wxAtoi(value);
        value = node->GetPropVal(wxT("leftsubindent"), wxEmptyString);
        if (!value.empty())
            leftSubIndent = wxAtoi(value);
        attr.SetLeftIndent(leftIndent, leftSubIndent);

        value = node->GetPropVal(wxT("rightindent"), wxEmptyString);
        if (!value.empty())
            attr.SetRightIndent(wxAtoi(value));

        value = node->GetPropVal(wxT("parspacingbefore"), wxEmptyString);
        if (!value.empty())
            attr.SetParagraphSpacingBefore(wxAtoi(value));

        value = node->GetPropVal(wxT("parspacingafter"), wxEmptyString);
        if (!value.empty())
            attr.SetParagraphSpacingAfter(wxAtoi(value));

        value = node->GetPropVal(wxT("linespacing"), wxEmptyString);
        if (!value.empty())
            attr.SetLineSpacing(wxAtoi(value));

        value = node->GetPropVal(wxT("bulletstyle"), wxEmptyString);
        if (!value.empty())
            attr.SetBulletStyle(wxAtoi(value));

        value = node->GetPropVal(wxT("bulletnumber"), wxEmptyString);
        if (!value.empty())
            attr.SetBulletNumber(wxAtoi(value));

        value = node->GetPropVal(wxT("bulletsymbol"), wxEmptyString);
        if (!value.empty())
            attr.SetBulletSymbol(value[0]);

        value = node->GetPropVal(wxT("parstyle"), wxEmptyString);
        if (!value.empty())
            attr.SetParagraphStyleName(value);
    }

    return true;
}

#endif

IMPLEMENT_DYNAMIC_CLASS(wxRichTextHTMLHandler, wxRichTextFileHandler)

/// Can we handle this filename (if using files)? By default, checks the extension.
bool wxRichTextHTMLHandler::CanHandle(const wxString& filename) const
{
    wxString path, file, ext;
    wxSplitPath(filename, & path, & file, & ext);

    return (ext.Lower() == wxT("html") || ext.Lower() == wxT("htm"));
}


#if wxUSE_STREAMS
bool wxRichTextHTMLHandler::DoLoadFile(wxRichTextBuffer *WXUNUSED(buffer), wxInputStream& WXUNUSED(stream))
{
    return false;
}

/*
 * We need to output only _changes_ in character formatting.
 */

bool wxRichTextHTMLHandler::DoSaveFile(wxRichTextBuffer *buffer, wxOutputStream& stream)
{
    buffer->Defragment();

    wxTextOutputStream str(stream);

    wxTextAttrEx currentParaStyle = buffer->GetAttributes();
    wxTextAttrEx currentCharStyle = buffer->GetAttributes();

    str << wxT("<html><head></head><body>\n");

    wxRichTextObjectList::compatibility_iterator node = buffer->GetChildren().GetFirst();
    while (node)
    {
        wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxASSERT (para != NULL);

        if (para)
        {
            OutputParagraphFormatting(currentParaStyle, para->GetAttributes(), stream, true);

            wxRichTextObjectList::compatibility_iterator node2 = para->GetChildren().GetFirst();
            while (node2)
            {
                wxRichTextObject* obj = node2->GetData();
                wxRichTextPlainText* textObj = wxDynamicCast(obj, wxRichTextPlainText);
                if (textObj && !textObj->IsEmpty())
                {
                    OutputCharacterFormatting(currentCharStyle, obj->GetAttributes(), stream, true);

                    str << textObj->GetText();

                    OutputCharacterFormatting(currentCharStyle, obj->GetAttributes(), stream, false);
                }

                node2 = node2->GetNext();
            }

            OutputParagraphFormatting(currentParaStyle, para->GetAttributes(), stream, false);

            str << wxT("<P>\n");
        }

        node = node->GetNext();
    }

    str << wxT("</body></html>\n");

    return true;
}

/// Output character formatting
void wxRichTextHTMLHandler::OutputCharacterFormatting(const wxTextAttrEx& WXUNUSED(currentStyle), const wxTextAttrEx& thisStyle, wxOutputStream& stream, bool start)
{
    wxTextOutputStream str(stream);

    bool isBold = false;
    bool isItalic = false;
    bool isUnderline = false;
    wxString faceName;

    if (thisStyle.GetFont().Ok())
    {
        if (thisStyle.GetFont().GetWeight() == wxBOLD)
            isBold = true;
        if (thisStyle.GetFont().GetStyle() == wxITALIC)
            isItalic = true;
        if (thisStyle.GetFont().GetUnderlined())
            isUnderline = true;

        faceName = thisStyle.GetFont().GetFaceName();
    }

    if (start)
    {
        if (isBold)
            str << wxT("<b>");
        if (isItalic)
            str << wxT("<i>");
        if (isUnderline)
            str << wxT("<u>");
    }
    else
    {
        if (isUnderline)
            str << wxT("</u>");
        if (isItalic)
            str << wxT("</i>");
        if (isBold)
            str << wxT("</b>");
    }
}

/// Output paragraph formatting
void wxRichTextHTMLHandler::OutputParagraphFormatting(const wxTextAttrEx& WXUNUSED(currentStyle), const wxTextAttrEx& thisStyle, wxOutputStream& stream, bool start)
{
    // TODO: lists, indentation (using tables), fonts, right-align, ...

    wxTextOutputStream str(stream);
    bool isCentered = false;

    if (thisStyle.GetAlignment() == wxTEXT_ALIGNMENT_CENTRE)
    {
        isCentered = true;
    }

    if (start)
    {
        if (isCentered)
            str << wxT("<center>");
    }
    else
    {
        if (isCentered)
            str << wxT("</center>");
    }
}

#endif

#endif
    // wxUSE_RICHTEXT
