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

#ifndef _HTMLPARSER_H_
#define _HTMLPARSER_H_

//#include "wx/module.h"
#include "wx/stream.h"

/*
 * wxSimpleHtmlAttribute
 * Representation of an attribute
 */

class wxSimpleHtmlAttribute
{
    friend class wxSimpleHtmlTag;
public:
    wxSimpleHtmlAttribute(const wxString& name, const wxString& value)
    {
        m_name = name; m_value = value; m_next = NULL;
    }
//// Operations

    // Write this attribute
    void Write(wxOutputStream& stream);

//// Accessors
    const wxString& GetName() const { return m_name; }
    const wxString& GetValue() const { return m_value; }
    void SetName(const wxString& name)  { m_name = name; }
    void SetValue(const wxString& value)  { m_value = value; }

    wxSimpleHtmlAttribute* GetNextAttribute() { return m_next; }
    void SetNextAttribute(wxSimpleHtmlAttribute* attr) { m_next = attr; }

    bool HasName(const wxString& name) const { return (0 == m_name.CmpNoCase(name)); }
    bool HasValue(const wxString& val) const { return (0 == m_value.CmpNoCase(val)); }

private:
    wxString                m_name;
    wxString                m_value;
    wxSimpleHtmlAttribute*  m_next;
};


/*
 * wxSimpleHtmlTag
 * Representation of a tag or chunk of text
 */

enum { wxSimpleHtmlTag_Text, wxSimpleHtmlTag_TopLevel, wxSimpleHtmlTag_Open, wxSimpleHtmlTag_Close, wxSimpleHtmlTag_Directive, wxSimpleHtmlTag_XMLDeclaration  };

class wxSimpleHtmlTag
{
public:
    wxSimpleHtmlTag(const wxString& tagName, int tagType);
    ~wxSimpleHtmlTag();

//// Operations
    void ClearAttributes();
    wxSimpleHtmlAttribute* FindAttribute(const wxString& name) const ;
    void AppendAttribute(const wxString& name, const wxString& value);
    void ClearChildren();
    // Remove 1 tag from the child list.
    void RemoveChild(wxSimpleHtmlTag *remove);
    // Appaned tag to the end of the child list.
    void AppendTag(wxSimpleHtmlTag* tag); 
    // Insert tag after ourself in the parents child list.
    void AppendTagAfterUs(wxSimpleHtmlTag* tag);
    // Write this tag
    void Write(wxOutputStream& stream);

    // Gets the text from this tag and its descendants
    wxString GetTagText();

//// Accessors
    const wxString& GetName() const { return m_name; }
    void SetName(const wxString& name) { m_name = name; }

    int GetType() const { return m_type; }
    void SetType(int t) { m_type = t; }

    // If type is wxSimpleHtmlTag_Text, m_text will contain some text.
    const wxString& GetText() const { return m_text; }
    void SetText(const wxString& text) { m_text = text; }

    wxSimpleHtmlAttribute* GetFirstAttribute() { return m_attributes; }
    void SetFirstAttribute(wxSimpleHtmlAttribute* attr) { m_attributes = attr; }

    int GetAttributeCount() const ;
    wxSimpleHtmlAttribute* GetAttribute(int i) const ;

    wxSimpleHtmlTag* GetChildren() const { return m_children; }
    void SetChildren(wxSimpleHtmlTag* children) { m_children = children; }

    wxSimpleHtmlTag* GetParent() const { return m_parent; }
    void SetParent(wxSimpleHtmlTag* parent) { m_parent = parent; }
    int GetChildCount() const;
    wxSimpleHtmlTag*    GetChild(int i) const;
    wxSimpleHtmlTag*    GetNext() const { return m_next; }

//// Convenience accessors & search functions
    bool NameIs(const wxString& name) { return (m_name.CmpNoCase(name) == 0); }
    bool HasAttribute(const wxString& name, const wxString& value) const;
    bool HasAttribute(const wxString& name) const;
    bool GetAttributeValue(wxString& value, const wxString& attrName);

    // Search forward from this tag until we find a tag with this name & optionally attribute 
    wxSimpleHtmlTag* FindTag(const wxString& tagName, const wxString& attrName = wxEmptyString);

    // Gather the text until we hit the given close tag
    bool FindTextUntilTagClose(wxString& text, const wxString& tagName);

private:
    wxString                m_name;
    int                     m_type;
    wxString                m_text;
    wxSimpleHtmlAttribute*  m_attributes;

    // List of children
    wxSimpleHtmlTag*        m_children;
    wxSimpleHtmlTag*        m_next; // Next sibling
    wxSimpleHtmlTag*        m_parent;
};

/*
 * wxSimpleHtmlParser
 * Simple HTML parser, for such tasks as scanning HTML for keywords, contents, etc.
 */

class wxSimpleHtmlParser : public wxObject
{
    
public:
    wxSimpleHtmlParser();
    virtual ~wxSimpleHtmlParser();

//// Operations
    bool ParseFile(const wxString& filename);
    bool ParseString(const wxString& str);
    void Clear();
    // Write this file
    void Write(wxOutputStream& stream);
    bool WriteFile(wxString& filename);

//// Helpers

    // Main recursive parsing function
    bool ParseHtml(wxSimpleHtmlTag* parent);

    wxSimpleHtmlTag* ParseTagHeader();
    wxSimpleHtmlTag* ParseTagClose();
    bool ParseAttributes(wxSimpleHtmlTag* tag);
    wxSimpleHtmlTag* ParseDirective(); // e.g. <!DOCTYPE ....>    
    wxSimpleHtmlTag* ParseXMLDeclaration(); // e.g. <?xml .... ?>
    bool ParseComment(); // Throw away comments
    // Plain text, up until an angled bracket
    bool ParseText(wxString& text);

    bool EatWhitespace(); // Throw away whitespace
    bool EatWhitespace(int& pos); // Throw away whitespace: using 'pos'
    bool ReadString(wxString& str, bool eatIt = false);
    bool ReadWord(wxString& str, bool eatIt = false);
    bool ReadNumber(wxString& str, bool eatIt = false);
    // Could be number, string, whatever, but read up until whitespace.
    bool ReadLiteral(wxString& str, bool eatIt = false);

    bool IsComment();
    bool IsDirective();
    bool IsXMLDeclaration();    
    bool IsString();
    bool IsWord();
    bool IsTagClose();
    bool IsTagStartBracket(int ch);
    bool IsTagEndBracket(int ch);
    bool IsWhitespace(int ch);
    bool IsAlpha(int ch);
    bool IsWordChar(int ch);
    bool IsNumeric(int ch);
    // Check if a specific tag needs a close tag. If not this function should return false.
    // If no close tag is needed the result will be that the tag will be insert in a none 
    // hierarchical way. i.e. if the function would return false all the time we would get
    // a flat list of all tags (like it used to be previously).
    virtual bool IsCloseTagNeeded(const wxString &name);
    
    // Encode/Decode Special Characters like:
    // >    Begins a tag.      &gt;
    // <    Ends a tag.        &lt;
    // "    Quotation mark.    &quot;
    // '    Apostrophe.        &apos;
    // &    Ampersand.         &amp;
    static void DecodeSpecialChars(wxString &value);
    static wxString EncodeSpecialChars(const wxString &value);

    // Matches this string (case insensitive)
    bool Matches(const wxString& tok, bool eatIt = false) ;
    bool Eof() const { return (m_pos >= m_length); }
    bool Eof(int pos) const { return (pos >= m_length); }

    void SetPosition(int pos) { m_pos = pos; }


//// Accessors
    wxSimpleHtmlTag* GetTopLevelTag() const { return m_topLevel; }

    // Safe way of getting a character
    int GetChar(size_t i) const;
    
private:

    wxSimpleHtmlTag*    m_topLevel;
    int                 m_pos;    // Position in string
    int                 m_length; // Length of string
    wxString            m_text;   // The actual text

};

/*
 * wxSimpleHtmlTagSpec
 * Describes a tag, and what type it is.
 * wxSimpleHtmlModule will initialise/cleanup a list of these, one per tag type
 */

#if 0
class wxSimpleHtmlTagSpec : public wxObject
{
    
public:
    wxSimpleHtmlTagSpec(const wxString& name, int type);

//// Operations
    static void AddTagSpec(wxSimpleHtmlTagSpec* spec);
    static void Clear();

//// Accessors
    const wxString& GetName() const { return m_name; }
    int GetType() const { return m_type; }

private:

    wxString    m_name;
    int         m_type;

    static wxList* sm_tagSpecs;
};

/*
 * wxSimpleHtmlModule
 * Responsible for init/cleanup of appropriate data structures
 */

class wxSimpleHtmlModule : public wxModule
{
DECLARE_DYNAMIC_CLASS(wxSimpleHtmlModule)

public:
    wxSimpleHtmlModule() {};

    bool OnInit() ;
    void OnExit() ;
};
#endif

#endif
