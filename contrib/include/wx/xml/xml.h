/////////////////////////////////////////////////////////////////////////////
// Name:        xml.h
// Purpose:     wxXmlDocument - XML parser & data holder class
// Author:      Vaclav Slavik
// Created:     2000/03/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XML_H_
#define _WX_XML_H_

#ifdef __GNUG__
#pragma interface "xml.h"
#endif

#include "wx/defs.h"
#include "wx/string.h"
#include "wx/object.h"
#include "wx/list.h"


class WXDLLEXPORT wxXmlNode;
class WXDLLEXPORT wxXmlProperty;
class WXDLLEXPORT wxXmlDocument;
class WXDLLEXPORT wxXmlIOHandler;
class WXDLLEXPORT wxInputStream;
class WXDLLEXPORT wxOutputStream;


// Represents XML node type.
enum wxXmlNodeType
{
    // note: values are synchronized with xmlElementType from libxml
    wxXML_ELEMENT_NODE       =  1,
    wxXML_ATTRIBUTE_NODE     =  2,
    wxXML_TEXT_NODE          =  3,
    wxXML_CDATA_SECTION_NODE =  4,
    wxXML_ENTITY_REF_NODE    =  5,
    wxXML_ENTITY_NODE        =  6,
    wxXML_PI_NODE            =  7,
    wxXML_COMMENT_NODE       =  8,
    wxXML_DOCUMENT_NODE      =  9,
    wxXML_DOCUMENT_TYPE_NODE = 10,
    wxXML_DOCUMENT_FRAG_NODE = 11,
    wxXML_NOTATION_NODE      = 12,
    wxXML_HTML_DOCUMENT_NODE = 13
};


// Types of XML files:

enum wxXmlIOType
{
    wxXML_IO_AUTO = 0,    // detect it automatically
    wxXML_IO_EXPAT,       // use Expat to load from text/xml document
    wxXML_IO_TEXT_OUTPUT, // generic saver into text/xml
    wxXML_IO_BIN,         // save in binary uncompressed proprietary format
    wxXML_IO_BINZ         // svae in binary zlib-compressed proprietary format
};


// Represents node property(ies).
// Example: in <img src="hello.gif" id="3"/> "src" is property with value
//          "hello.gif" and "id" is prop. with value "3".

class WXDLLEXPORT wxXmlProperty
{
public:
    wxXmlProperty() : m_next(NULL) {}
    wxXmlProperty(const wxString& name, const wxString& value, 
                  wxXmlProperty *next)
            : m_name(name), m_value(value), m_next(next) {}
    ~wxXmlProperty() { delete m_next; }

    wxString GetName() const { return m_name; }
    wxString GetValue() const { return m_value; }
    wxXmlProperty *GetNext() const { return m_next; }

    void SetName(const wxString& name) { m_name = name; }
    void SetValue(const wxString& value) { m_value = value; }
    void SetNext(wxXmlProperty *next) { m_next = next; }

private:
    wxString m_name;
    wxString m_value;
    wxXmlProperty *m_next;
};



// Represents node in XML document. Node has name and may have content
// and properties. Most common node types are wxXML_TEXT_NODE (name and props
// are irrelevant) and wxXML_ELEMENT_NODE (e.g. in <title>hi</title> there is
// element with name="title", irrelevant content and one child (wxXML_TEXT_NODE
// with content="hi").
//
// If wxUSE_UNICODE is 0, all strings are encoded in UTF-8 encoding (same as
// ASCII for characters 0-127). You can use wxMBConvUTF8 to convert then to 
// desired encoding:
//
//     wxCSConv myConv("iso8859-2");
//     wxString s(cMB2WC(node->GetContent().c_str()), myConv);

class WXDLLEXPORT wxXmlNode
{
public:
    wxXmlNode() : m_properties(NULL), m_parent(NULL), 
                  m_children(NULL), m_next(NULL) {}
    wxXmlNode(wxXmlNode *parent,wxXmlNodeType type, 
              const wxString& name, const wxString& content,
              wxXmlProperty *props, wxXmlNode *next);
    ~wxXmlNode() { delete m_properties; delete m_next; delete m_children; }

    // copy ctor & operator=. Note that this does NOT copy syblings
    // and parent pointer, i.e. m_parent and m_next will be NULL
    // after using copy ctor and are never unmodified by operator=.
    // On the other hand, it DOES copy children and properties.
    wxXmlNode(const wxXmlNode& node);
    wxXmlNode& operator=(const wxXmlNode& node);

    // user-friendly creation:
    wxXmlNode(wxXmlNodeType type, const wxString& name, 
              const wxString& content = wxEmptyString);
    void AddChild(wxXmlNode *child);
    void InsertChild(wxXmlNode *child, wxXmlNode *before_node);
    bool RemoveChild(wxXmlNode *child);
    void AddProperty(const wxString& name, const wxString& value);
    bool DeleteProperty(const wxString& name);

    // access methods:
    wxXmlNodeType GetType() const { return m_type; }
    wxString GetName() const { return m_name; }
    wxString GetContent() const { return m_content; }

    wxXmlNode *GetParent() const { return m_parent; }
    wxXmlNode *GetNext() const { return m_next; }
    wxXmlNode *GetChildren() const { return m_children; }

    wxXmlProperty *GetProperties() const { return m_properties; }
    bool GetPropVal(const wxString& propName, wxString *value) const;
    wxString GetPropVal(const wxString& propName, 
                        const wxString& defaultVal) const;
    bool HasProp(const wxString& propName) const;

    void SetType(wxXmlNodeType type) { m_type = type; }
    void SetName(const wxString& name) { m_name = name; }
    void SetContent(const wxString& con) { m_content = con; }

    void SetParent(wxXmlNode *parent) { m_parent = parent; }
    void SetNext(wxXmlNode *next) { m_next = next; }
    void SetChildren(wxXmlNode *child) { m_children = child; }

    void SetProperties(wxXmlProperty *prop) { m_properties = prop; }
    void AddProperty(wxXmlProperty *prop);

private:
    wxXmlNodeType m_type;
    wxString m_name;
    wxString m_content;
    wxXmlProperty *m_properties;
    wxXmlNode *m_parent, *m_children, *m_next;

    void DoCopy(const wxXmlNode& node);
};







// This class holds XML data/document as parsed by libxml. Note that 
// internal representation is independant on libxml and you can use 
// it without libxml (see Load/SaveBinary).

class WXDLLEXPORT wxXmlDocument : public wxObject
{
public:
    wxXmlDocument() : wxObject(), m_version(wxT("1.0")), m_root(NULL)  {}
    wxXmlDocument(const wxString& filename, 
                  wxXmlIOType io_type = wxXML_IO_AUTO);
    wxXmlDocument(wxInputStream& stream, 
                  wxXmlIOType io_type = wxXML_IO_AUTO);
    ~wxXmlDocument() { delete m_root; }

    wxXmlDocument(const wxXmlDocument& doc);
    wxXmlDocument& operator=(const wxXmlDocument& doc);

    // Parses .xml file and loads data. Returns TRUE on success, FALSE
    // otherwise. 
    // NOTE: Any call to this method will result into linking against libxml
    //       and app's binary size will grow by ca. 250kB
    bool Load(const wxString& filename, wxXmlIOType io_type = wxXML_IO_AUTO);
    bool Load(wxInputStream& stream, wxXmlIOType io_type = wxXML_IO_AUTO);

    // Saves document as .xml file.
    bool Save(const wxString& filename, 
              wxXmlIOType io_type = wxXML_IO_TEXT_OUTPUT) const;
    bool Save(wxOutputStream& stream, 
              wxXmlIOType io_type = wxXML_IO_TEXT_OUTPUT) const;

    bool IsOk() const { return m_root != NULL; }

    // Returns root node of the document.
    wxXmlNode *GetRoot() const { return m_root; }

    // Returns version of document (may be empty).
    wxString GetVersion() const { return m_version; }
    // Returns encoding of document (may be empty).
    // Note: this is the encoding original fail was saved in, *not* the
    // encoding of in-memory representation! Data in wxXmlNode are always
    // stored in wchar_t (in Unicode build) or UTF-8 encoded 
    // (if wxUSE_UNICODE is 0).
    wxString GetEncoding() const { return m_encoding; }

    // Write-access methods:
    void SetRoot(wxXmlNode *node) { delete m_root ; m_root = node; }
    void SetVersion(const wxString& version) { m_version = version; }
    void SetEncoding(const wxString& encoding) { m_encoding = encoding; }

    static void AddHandler(wxXmlIOHandler *handler);
    static void CleanUpHandlers();
    static void InitStandardHandlers();

protected:
    static wxList *sm_handlers;

private:
    wxString m_version, m_encoding;
    wxXmlNode *m_root;

    void DoCopy(const wxXmlDocument& doc);
};



// wxXmlIOHandler takes care of loading and/or saving XML data.
// see xmlio.h for available handlers

class WXDLLEXPORT wxXmlIOHandler : public wxObject
{
    public:
        wxXmlIOHandler() {}
        
        virtual wxXmlIOType GetType() = 0;
        virtual bool CanLoad(wxInputStream& stream) = 0;
        virtual bool CanSave() = 0;
        
        virtual bool Load(wxInputStream& stream, wxXmlDocument& doc) = 0;
        virtual bool Save(wxOutputStream& stream, const wxXmlDocument& doc) = 0;
};





#endif // _WX_XML_H_
