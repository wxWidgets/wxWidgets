/////////////////////////////////////////////////////////////////////////////
// Name:        htmlparser.cpp
// Purpose:     Simple HTML parser
// Author:      Julian Smart
// Modified by:
// Created:     2002-09-25
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "htmlparser.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/wfstream.h"
#include "wx/textfile.h"
#include "wx/txtstrm.h"
#include "htmlparser.h"

/// Useful insertion operators for wxOutputStream.
static wxOutputStream& operator <<(wxOutputStream& stream, const wxString& s)
{
    wxTextOutputStream txt(stream); // This is to make sure the line-ending is native!

    txt.WriteString(s);
    return stream;
}

#if 0 // Gives warning because not used...
static wxOutputStream& operator <<(wxOutputStream& stream, long l)
{
    wxString str;
    str.Printf("%ld", l);
    return stream << str;
}

static wxOutputStream& operator <<(wxOutputStream& stream, const char c)
{
    wxString str;
    str.Printf("%c", c);
    return stream << str;
}
#endif // 0

/*
 * wxSimpleHtmlAttribute
 * Representation of an attribute
 */

wxSimpleHtmlParser::wxSimpleHtmlParser()
{
    m_topLevel = NULL;
    m_pos = 0;
}


wxSimpleHtmlParser::~wxSimpleHtmlParser()
{
    Clear();
}

bool wxSimpleHtmlParser::ParseFile(const wxString& filename)
{
    wxTextFile textFile;

    if (textFile.Open(filename))
    {
        wxString text;
        wxString line;
        int i;
        int count = textFile.GetLineCount();
        for (i = 0; i < count; i++)
        {
            if (i == 0)
                line = textFile.GetFirstLine();
            else
                line = textFile.GetNextLine();

            text += line;
            if (i != (count - 1))
                text += wxT("\n");
        }

#if 0
        for ( line = textFile.GetFirstLine(); !textFile.Eof(); line = textFile.GetNextLine() )
        {
            text += line;
            if (!textFile.Eof())
                text += wxT("\n");
        }
#endif

        return ParseString(text);
    }
    else
        return false;
}

bool wxSimpleHtmlParser::ParseString(const wxString& str)
{
    Clear();

    m_pos = 0;
    m_text = str;
    m_length = str.Length();

    m_topLevel = new wxSimpleHtmlTag(wxT("TOPLEVEL"), wxSimpleHtmlTag_TopLevel);

    bool bResult = ParseHtml(m_topLevel);

    wxASSERT(bResult); // Failed to parse the TAGs.
                       // Hint: Check if every open tag has a close tag!

    return bResult;
}

// Main recursive parsing function
bool wxSimpleHtmlParser::ParseHtml(wxSimpleHtmlTag* parent)
{
    if (!parent)
        return false;

    while (!Eof())
    {
        EatWhitespace();
        if (IsComment())
        {
            ParseComment();
        }
        else if (IsDirective())
        {
            wxSimpleHtmlTag* tag = ParseDirective();
            if (tag)
                parent->AppendTag(tag);
        }
        else if (IsXMLDeclaration())
        {
            wxSimpleHtmlTag* tag = ParseXMLDeclaration();
            if (tag)
                parent->AppendTag(tag);
        }
        else if (IsTagClose())
        {
            wxSimpleHtmlTag* tag = ParseTagClose();
            if (tag)
            {
                if (IsCloseTagNeeded(tag->GetName()))
                {
                    if (!parent->GetParent())
                        return false;
                    parent->GetParent()->AppendTag(tag);
                    return true;
                }
                else
                    parent->AppendTag(tag);
            }
        }
        else if (IsTagStartBracket(GetChar(m_pos)))
        {
            wxSimpleHtmlTag* tag = ParseTagHeader();
            if (tag)
                parent->AppendTag(tag);

            if (IsCloseTagNeeded(tag->GetName()))
            {
                if (!ParseHtml(tag))
                    return false;   // Something didn't go ok, so don't continue.
            }
        }
        else
        {
            // Just a text string
            wxString text;
            ParseText(text);

            wxSimpleHtmlTag* tag = new wxSimpleHtmlTag(wxT("TEXT"), wxSimpleHtmlTag_Text);
            tag->SetText(text);
            if(parent->GetParent())
                parent->GetParent()->AppendTag(tag);
            else
                parent->AppendTag(tag); // When this occurs it is probably the
                                        // empty lines at the end of the file...
        }
    }
    return true;
}

// Plain text, up until an angled bracket
bool wxSimpleHtmlParser::ParseText(wxString& text)
{
    while (!Eof() && GetChar(m_pos) != wxT('<'))
    {
        text += (wxChar)GetChar(m_pos);
        m_pos ++;
    }
    DecodeSpecialChars(text);
    return true;
}

wxSimpleHtmlTag* wxSimpleHtmlParser::ParseTagHeader()
{
    if (IsTagStartBracket(GetChar(m_pos)))
    {
        m_pos ++;
        EatWhitespace();

        wxString word;
        ReadWord(word, true);

        EatWhitespace();

        wxSimpleHtmlTag* tag = new wxSimpleHtmlTag(word, wxSimpleHtmlTag_Open);

        ParseAttributes(tag);

        EatWhitespace();

        if (IsTagEndBracket(GetChar(m_pos)))
            m_pos ++;

        return tag;
    }
    else
        return NULL;
}

wxSimpleHtmlTag* wxSimpleHtmlParser::ParseTagClose()
{
    Matches(wxT("</"), true);

    EatWhitespace();

    wxString word;
    ReadWord(word, true);

    EatWhitespace();
    m_pos ++;

    wxSimpleHtmlTag* tag = new wxSimpleHtmlTag(word, wxSimpleHtmlTag_Close);
    return tag;
}

bool wxSimpleHtmlParser::ParseAttributes(wxSimpleHtmlTag* tag)
{
    // Parse attributes of a tag header until we reach >
    while (!IsTagEndBracket(GetChar(m_pos)) && !Eof())
    {
        EatWhitespace();

        wxString attrName, attrValue;

        if (IsString())
        {
            ReadString(attrName, true);
            tag->AppendAttribute(attrName, wxEmptyString);
        }
        else if (IsNumeric(GetChar(m_pos)))
        {
            ReadNumber(attrName, true);
            tag->AppendAttribute(attrName, wxEmptyString);
        }
        else
        {
            // Try to read an attribute name/value pair, or at least a name
            // without the value
            ReadLiteral(attrName, true);
            EatWhitespace();

            if (GetChar(m_pos) == wxT('='))
            {
                m_pos ++;
                EatWhitespace();

                if (IsString())
                    ReadString(attrValue, true);
                else if (!Eof() && !IsTagEndBracket(GetChar(m_pos)))
                    ReadLiteral(attrValue, true);
            }
            if (!attrName.IsEmpty())
                tag->AppendAttribute(attrName, attrValue);
        }
    }
    return true;
}

// e.g. <!DOCTYPE ....>
wxSimpleHtmlTag* wxSimpleHtmlParser::ParseDirective()
{
    Matches(wxT("<!"), true);

    EatWhitespace();

    wxString word;
    ReadWord(word, true);

    EatWhitespace();

    wxSimpleHtmlTag* tag = new wxSimpleHtmlTag(word, wxSimpleHtmlTag_Directive);

    ParseAttributes(tag);

    EatWhitespace();

    if (IsTagEndBracket(GetChar(m_pos)))
        m_pos ++;

    return tag;
}

// e.g. <?xml .... ?>
wxSimpleHtmlTag* wxSimpleHtmlParser::ParseXMLDeclaration()
{
    Matches(wxT("<?"), true);

    EatWhitespace();

    wxString word;
    ReadWord(word, true);

    EatWhitespace();

    wxSimpleHtmlTag* tag = new wxSimpleHtmlTag(word, wxSimpleHtmlTag_XMLDeclaration);

    ParseAttributes(tag);

    EatWhitespace();

    if (IsTagEndBracket(GetChar(m_pos)))
        m_pos ++;

    return tag;
}

bool wxSimpleHtmlParser::ParseComment()
{
    // Eat the comment tag start
    Matches(wxT("<!--"), true);

    while (!Eof() && !Matches(wxT("-->"), true))
    {
        m_pos ++;
    }

    return true;
}

bool wxSimpleHtmlParser::EatWhitespace()
{
    while (!Eof() && IsWhitespace(GetChar(m_pos)))
        m_pos ++;
    return true;
}

bool wxSimpleHtmlParser::EatWhitespace(int& pos)
{
    while (!Eof(pos) && IsWhitespace(GetChar(pos)))
        pos ++;
    return true;
}

bool wxSimpleHtmlParser::ReadString(wxString& str, bool eatIt)
{
    int pos = m_pos;
    if (GetChar(pos) == (int) '"')
    {
        pos ++;
        while (!Eof(pos) && GetChar(pos) != (int) '"')
        {
            // TODO: how are quotes escaped in HTML?
            str += (wxChar) GetChar(pos);
            pos ++;
        }
        if (GetChar(pos) == (int) '"')
            pos ++;
        if (eatIt)
            m_pos = pos;
        DecodeSpecialChars(str);
        return true;
    }
    else
        return false;
}

bool wxSimpleHtmlParser::ReadWord(wxString& str, bool eatIt)
{
    int pos = m_pos;

    if (!IsAlpha(GetChar(pos)))
        return false;

    str += (wxChar) GetChar(pos) ;
    pos ++;

    while (!Eof(pos) && IsWordChar(GetChar(pos)))
    {
        str += (wxChar) GetChar(pos);
        pos ++;
    }
    if (eatIt)
        m_pos = pos;
    DecodeSpecialChars(str);
    return true;
}

bool wxSimpleHtmlParser::ReadNumber(wxString& str, bool eatIt)
{
    int pos = m_pos;

    if (!IsNumeric(GetChar(pos)))
        return false;

    str += (wxChar) GetChar(pos) ;
    pos ++;

    while (!Eof(pos) && IsNumeric(GetChar(pos)))
    {
        str += (wxChar) GetChar(pos);
        pos ++;
    }
    if (eatIt)
        m_pos = pos;
    DecodeSpecialChars(str);
    return true;
}

// Could be number, string, whatever, but read up until whitespace or end of tag (but not a quoted string)
bool wxSimpleHtmlParser::ReadLiteral(wxString& str, bool eatIt)
{
    int pos = m_pos;

    while (!Eof(pos) && !IsWhitespace(GetChar(pos)) && !IsTagEndBracket(GetChar(pos)) && GetChar(pos) != wxT('='))
    {
        str += (wxChar)GetChar(pos);
        pos ++;
    }
    if (eatIt)
        m_pos = pos;
    DecodeSpecialChars(str);
    return true;
}

bool wxSimpleHtmlParser::IsComment()
{
    return Matches(wxT("<!--"));
}

bool wxSimpleHtmlParser::IsDirective()
{
    return Matches(wxT("<!"));
}

bool wxSimpleHtmlParser::IsXMLDeclaration()
{
    return Matches(wxT("<?xml"));
}

bool wxSimpleHtmlParser::IsString()
{
    return (GetChar(m_pos) == (int) '"') ;
}

bool wxSimpleHtmlParser::IsWord()
{
    return (IsAlpha(GetChar(m_pos)));
}

bool wxSimpleHtmlParser::IsTagClose()
{
    return Matches(wxT("</"));
}

bool wxSimpleHtmlParser::IsTagStartBracket(int ch)
{
    return (ch == wxT('<'));
}

bool wxSimpleHtmlParser::IsTagEndBracket(int ch)
{
    return (ch == wxT('>'));
}

bool wxSimpleHtmlParser::IsWhitespace(int ch)
{
    return ((ch == 13) || (ch == 10) || (ch == 32) || (ch == (int) '\t')) ;
}

bool wxSimpleHtmlParser::IsAlpha(int ch)
{
    return (wxIsalpha((wxChar) ch) != 0);
}

bool wxSimpleHtmlParser::IsWordChar(int ch)
{
    return (wxIsalpha((wxChar) ch) != 0 || ch == wxT('-') || ch == wxT('_') || IsNumeric(ch));
}

bool wxSimpleHtmlParser::IsNumeric(int ch)
{
    return (wxIsdigit((wxChar) ch) != 0 || ch == wxT('-') || ch == wxT('.')) ;
}

bool wxSimpleHtmlParser::IsCloseTagNeeded(const wxString &name)
{
    if (name.IsSameAs(wxT("P"), false)) // e.g <P>
        return false;

    // ToDo add more items here.

    return true;
}

// Encode/Decode Special Characters.
// See here for the used table: http://msdn.microsoft.com/library/default.asp?url=/library/en-us/xmlsql/ac_xml1_1nqk.asp
/* static */ void wxSimpleHtmlParser::DecodeSpecialChars(wxString &value)
{
    // XML translation
    value.Replace(wxT("&gt;"),   wxT(">"),  true);
    value.Replace(wxT("&lt;"),   wxT("<"),  true);
    value.Replace(wxT("&quot;"), wxT("\""), true);
    value.Replace(wxT("&apos;"), wxT("'"),  true);
    value.Replace(wxT("&amp;"),  wxT("&"),  true);    // Note: do this as last to prevent replace problems.
}

/* static */ wxString wxSimpleHtmlParser::EncodeSpecialChars(const wxString &value)
{
    wxString newvalue = value;

    // XML translation
    newvalue.Replace(wxT("&"), wxT("&amp;"),  true);    // Note: do this as first to prevent replace problems.
    newvalue.Replace(wxT(">"), wxT("&gt;"),   true);
    newvalue.Replace(wxT("<"), wxT("&lt;"),   true);
    newvalue.Replace(wxT("\""),wxT("&quot;"), true);
    newvalue.Replace(wxT("'"), wxT("&apos;"), true);

    return newvalue;
}

// Matches this string (case insensitive)
bool wxSimpleHtmlParser::Matches(const wxString& tok, bool eatIt)
{
    wxString text(m_text.Mid(m_pos, tok.Length()));
    bool success = (text.CmpNoCase(tok) == 0) ;
    if (success && eatIt)
    {
        m_pos += tok.Length();
    }
    return success;
}

// Safe way of getting a character
int wxSimpleHtmlParser::GetChar(size_t i) const
{
    if (i >= (size_t) m_length)
        return -1;
    return m_text[i];
}

void wxSimpleHtmlParser::Clear()
{
    if (m_topLevel)
        delete m_topLevel;
    m_topLevel = NULL;
    m_text = wxEmptyString;
    m_pos = 0;
    m_length = 0;
}

// Write this file
void wxSimpleHtmlParser::Write(wxOutputStream& stream)
{
    if (m_topLevel)
        m_topLevel->Write(stream);
}

bool wxSimpleHtmlParser::WriteFile(wxString& filename)
{
    wxFileOutputStream fstream(filename);
    if (fstream.Ok())
    {
        Write(fstream);
        return true;
    }
    else
        return false;
}

/*
 * wxSimpleHtmlTag
 * Representation of a tag or chunk of text
 */

wxSimpleHtmlTag::wxSimpleHtmlTag(const wxString& tagName, int tagType)
{
    m_name = tagName;
    m_type = tagType;
    m_attributes = NULL;
    m_children = NULL;
    m_parent = NULL;
    m_next = NULL;
}

wxSimpleHtmlTag::~wxSimpleHtmlTag()
{
    ClearAttributes();
    ClearChildren();
}

//// Operations
void wxSimpleHtmlTag::ClearAttributes()
{
    if (m_attributes)
    {
        wxSimpleHtmlAttribute* attr = m_attributes;
        while (attr)
        {
            wxSimpleHtmlAttribute* next = attr->m_next;

            attr->m_next = NULL;
            delete attr;
            attr = next;
        }
        m_attributes = NULL;
    }
}

wxSimpleHtmlAttribute* wxSimpleHtmlTag::FindAttribute(const wxString& name) const
{
    wxSimpleHtmlAttribute* attr = m_attributes;
    while (attr)
    {
        if (attr->GetName().CmpNoCase(name) == 0)
        {
            return attr;
        }
        attr = attr->m_next;
    }
    return NULL;
}

void wxSimpleHtmlTag::AppendAttribute(const wxString& name, const wxString& value)
{
    wxSimpleHtmlAttribute* attr = new wxSimpleHtmlAttribute(name, value);
    if (m_attributes)
    {
        // Find tail
        wxSimpleHtmlAttribute* last = m_attributes;
        while (last->m_next)
            last = last->m_next;

        last->m_next = attr;
    }
    else
        m_attributes = attr;
}

void wxSimpleHtmlTag::ClearChildren()
{
    if (m_children)
    {
        wxSimpleHtmlTag* child = m_children;
        while (child)
        {
            wxSimpleHtmlTag* next = child->m_next;

            child->m_next = NULL;
            delete child;
            child = next;
        }
        m_children = NULL;
    }
}

void wxSimpleHtmlTag::RemoveChild(wxSimpleHtmlTag *remove)
{
    if (m_children)
    {
        wxSimpleHtmlTag* child = m_children;
        wxSimpleHtmlTag* prev = NULL;
        while (child)
        {
            wxSimpleHtmlTag* next = child->m_next;

            if (child == remove)
            {
                child->m_next = NULL;
                delete child;

                if (prev != NULL)
                    prev->m_next = next;
                else
                    m_children = next;

                return;
            }
            prev = child;
            child = next;
        }
    }
}

void wxSimpleHtmlTag::AppendTag(wxSimpleHtmlTag* tag)
{
    if (!tag)
        return;

    if (m_children)
    {
        // Find tail
        wxSimpleHtmlTag* last = m_children;
        while (last->m_next)
            last = last->m_next;

        last->m_next = tag;
    }
    else
    {
        m_children = tag;
    }

    tag->m_parent = this;
}

void wxSimpleHtmlTag::AppendTagAfterUs(wxSimpleHtmlTag* tag)
{
    if (!tag)
        return;

    tag->m_parent = m_parent;
    tag->m_next = m_next;
    m_next = tag;
}

// Gets the text from this tag and its descendants
wxString wxSimpleHtmlTag::GetTagText()
{
    wxString text;
    if (m_children)
    {
        wxSimpleHtmlTag* tag = m_children;
        while (tag)
        {
            text += tag->GetTagText();
            tag = tag->m_next;
        }
        return text;
    }
    else if (GetType() == wxSimpleHtmlTag_Text)
        return GetText();
    else
        return wxEmptyString;
}

int wxSimpleHtmlTag::GetAttributeCount() const
{
    int count = 0;
    wxSimpleHtmlAttribute* attr = m_attributes;
    while (attr)
    {
        count ++;
        attr = attr->m_next;
    }
    return count;
}

wxSimpleHtmlAttribute* wxSimpleHtmlTag::GetAttribute(int i) const
{
    int count = 0;
    wxSimpleHtmlAttribute* attr = m_attributes;
    while (attr)
    {
        if (count == i)
            return attr;
        count ++;
        attr = attr->m_next;
    }
    return NULL;
}

int wxSimpleHtmlTag::GetChildCount() const
{
    int count = 0;
    wxSimpleHtmlTag* tag = m_children;
    while (tag)
    {
        count ++;
        tag = tag->m_next;
    }
    return count;
}

bool wxSimpleHtmlTag::HasAttribute(const wxString& name, const wxString& value) const
{
    wxSimpleHtmlAttribute* attr = FindAttribute(name);

    return (attr && (attr->GetValue().CmpNoCase(value) == 0)) ;
}

bool wxSimpleHtmlTag::HasAttribute(const wxString& name) const
{
    return FindAttribute(name) != NULL ;
}

bool wxSimpleHtmlTag::GetAttributeValue(wxString& value, const wxString& attrName)
{
    wxSimpleHtmlAttribute* attr = FindAttribute(attrName);
    if (attr)
    {
        value = attr->GetValue();
        return true;
    }
    else
        return false;
}

// Search forward from this tag until we find a tag with this name & attribute
wxSimpleHtmlTag* wxSimpleHtmlTag::FindTag(const wxString& tagName, const wxString& attrName)
{
    wxSimpleHtmlTag* tag = m_next;
    while (tag)
    {
        if (tag->NameIs(tagName) && (attrName.IsEmpty() || tag->FindAttribute(attrName)))
            return tag;

        tag = tag->m_next;
    }
    return NULL;
}

bool wxSimpleHtmlTag::FindTextUntilTagClose(wxString& text, const wxString& tagName)
{
    wxSimpleHtmlTag* tag = this;
    while (tag)
    {
        if (tag->GetType() == wxSimpleHtmlTag_Close && tag->NameIs(tagName))
            return true;

        if (tag->GetType() == wxSimpleHtmlTag_Text)
            text += tag->GetText();

        tag = tag->m_next;
    }
    return true;
}


wxSimpleHtmlTag* wxSimpleHtmlTag::GetChild(int i) const
{
    int count = 0;
    wxSimpleHtmlTag* tag = m_children;
    while (tag)
    {
        if (count == i)
            return tag;

        count ++;
        tag = tag->m_next;
    }
    return NULL;
}

void wxSimpleHtmlTag::Write(wxOutputStream& stream)
{
    // Some helpers to layout the open and close tags.
    static bool sbUseTab = true;
    static size_t snTabLevel = 0;

#if 0 // Enable if no tabs should be used to align the tags.
    snTabLevel = 0;
#endif

    // Handle the different types of tags we can write.
    switch (GetType())
    {
    case wxSimpleHtmlTag_Text:
        {
            stream << wxSimpleHtmlParser::EncodeSpecialChars(m_text);
            break;
        }
    case wxSimpleHtmlTag_Open:
        {
            size_t tab;
            for(tab = 0; tab < snTabLevel; tab++)
                stream << wxT("\t");
            stream << wxT("<") << wxSimpleHtmlParser::EncodeSpecialChars(m_name);
            if (GetAttributeCount() > 0)
                stream << wxT(" ");
            int i;
            for (i = 0; i < GetAttributeCount(); i++)
            {
                wxSimpleHtmlAttribute* attr = GetAttribute(i);
                attr->Write(stream);
                if (i < GetAttributeCount() - 1)
                    stream << wxT(" ");
            }
            if(!m_children)
            {
                sbUseTab = false;   // We're putting the open a close tag on the same line,
                                    // so we don't wan't any tabs
                stream << wxT(">");
            }
            else
            {
                // sbUseTab = true;
                stream << wxT(">\n");
            }
            snTabLevel++;
            break;
        }
    case wxSimpleHtmlTag_Directive:
        {
            stream << wxT("<!") << wxSimpleHtmlParser::EncodeSpecialChars(m_name) << wxT(" ");
            int i;
            for (i = 0; i < GetAttributeCount(); i++)
            {
                wxSimpleHtmlAttribute* attr = GetAttribute(i);
                attr->Write(stream);
                if (i < GetAttributeCount() - 1)
                    stream << wxT(" ");
            }
            stream << wxT(">\n");
            break;
        }
    case wxSimpleHtmlTag_XMLDeclaration:
        {
            stream << wxT("<?") << wxSimpleHtmlParser::EncodeSpecialChars(m_name) << wxT(" ");
            int i;
            for (i = 0; i < GetAttributeCount(); i++)
            {
                wxSimpleHtmlAttribute* attr = GetAttribute(i);
                attr->Write(stream);
                if (i < GetAttributeCount() - 1)
                    stream << wxT(" ");
            }
            stream << wxT(">\n\n");
            break;
        }
    case wxSimpleHtmlTag_Close:
        {
            if (snTabLevel)     // Safety to prevent going around...
                snTabLevel--;   // Reduce the tab level
            if (sbUseTab)   // Do we write the open tag and close tag on a other line?
            {
                size_t tab;
                for(tab = 0; tab < snTabLevel; tab++)
                    stream << wxT("\t");
            }
            stream << wxT("</") << wxSimpleHtmlParser::EncodeSpecialChars(m_name) << wxT(">\n");
            sbUseTab = true;
            break;
        }
    default:
        {
            break;
        }
    }
    wxSimpleHtmlTag* tag = m_children;
    while (tag)
    {
        tag->Write(stream);
        tag = tag->m_next;
    }

}

void wxSimpleHtmlAttribute::Write(wxOutputStream& stream)
{
    if (m_value.IsEmpty())
        stream << wxSimpleHtmlParser::EncodeSpecialChars(m_name);
    else
    {
        stream << wxSimpleHtmlParser::EncodeSpecialChars(m_name);
        stream << wxT("=\"");
        stream << wxSimpleHtmlParser::EncodeSpecialChars(m_value);
        stream << wxT("\"");
    }
}
