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
    wxXML_IO_AUTO = 0, // detect it automatically
    wxXML_IO_LIBXML,   // use libxml2 to parse/save XML document
    wxXML_IO_BIN,      // save in binary uncompressed proprietary format
    wxXML_IO_BINZ      // svae in binary zlib-compressed proprietary format
};


// Represents node property(ies).
// Example: in <img src="hello.gif" id="3"/> "src" is property with value
//          "hello.gif" and "id" is prop. with value "3".

class WXDLLEXPORT wxXmlProperty
{
    public:
        wxXmlProperty() : m_Next(NULL) {}
        wxXmlProperty(const wxString& name, const wxString& value, wxXmlProperty *next)
            : m_Name(name), m_Value(value), m_Next(next) {}
        ~wxXmlProperty() { delete m_Next; }
    
        wxString GetName() const { return m_Name; }
        wxString GetValue() const { return m_Value; }
        wxXmlProperty *GetNext() const { return m_Next; }
        
        void SetName(const wxString& name) { m_Name = name; }
        void SetValue(const wxString& value) { m_Value = value; }
        void SetNext(wxXmlProperty *next) { m_Next = next; }

    private:
        wxString m_Name;
        wxString m_Value;
        wxXmlProperty *m_Next;
};



// Represents node in XML document. Node has name and may have content
// and properties. Most common node types are wxXML_TEXT_NODE (name and props
// are irrelevant) and wxXML_ELEMENT_NODE (e.g. in <title>hi</title> there is
// element with name="title", irrelevant content and one child (wxXML_TEXT_NODE
// with content="hi").

class WXDLLEXPORT wxXmlNode
{
    public:
        wxXmlNode() : m_Properties(NULL), m_Parent(NULL), 
                      m_Children(NULL), m_Next(NULL) {}
        wxXmlNode(wxXmlNode *parent,wxXmlNodeType type, 
                  const wxString& name, const wxString& content,
                  wxXmlProperty *props, wxXmlNode *next);
        ~wxXmlNode() { delete m_Properties; delete m_Next; delete m_Children; }

        // copy ctor & operator=. Note that this does NOT copy syblings
        // and parent pointer, i.e. m_Parent and m_Next will be NULL
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
        wxXmlNodeType GetType() const { return m_Type; }
        wxString GetName() const { return m_Name; }
        wxString GetContent() const { return m_Content; }
        
        wxXmlNode *GetParent() const { return m_Parent; }
        wxXmlNode *GetNext() const { return m_Next; }
        wxXmlNode *GetChildren() const { return m_Children; }

        wxXmlProperty *GetProperties() const { return m_Properties; }
        bool GetPropVal(const wxString& propName, wxString *value) const;
        wxString GetPropVal(const wxString& propName, const wxString& defaultVal) const;
        bool HasProp(const wxString& propName) const;
        
        void SetType(wxXmlNodeType type) { m_Type = type; }
        void SetName(const wxString& name) { m_Name = name; }
        void SetContent(const wxString& con) { m_Content = con; }
        
        void SetParent(wxXmlNode *parent) { m_Parent = parent; }
        void SetNext(wxXmlNode *next) { m_Next = next; }
        void SetChildren(wxXmlNode *child) { m_Children = child; }
        
        void SetProperties(wxXmlProperty *prop) { m_Properties = prop; }
        void AddProperty(wxXmlProperty *prop);

    private:
        wxXmlNodeType m_Type;
        wxString m_Name;
        wxString m_Content;
        wxXmlProperty *m_Properties;
        wxXmlNode *m_Parent, *m_Children, *m_Next;

        void DoCopy(const wxXmlNode& node);
};







// This class holds XML data/document as parsed by libxml. Note that 
// internal representation is independant on libxml and you can use 
// it without libxml (see Load/SaveBinary).

class WXDLLEXPORT wxXmlDocument : public wxObject
{
    public:
        wxXmlDocument() : wxObject(), m_Version(wxT("1.0")), m_Root(NULL)  {}
        wxXmlDocument(const wxString& filename, wxXmlIOType io_type = wxXML_IO_AUTO);
        wxXmlDocument(wxInputStream& stream, wxXmlIOType io_type = wxXML_IO_AUTO);
        ~wxXmlDocument() { delete m_Root; }
        
        wxXmlDocument(const wxXmlDocument& doc);
        wxXmlDocument& operator=(const wxXmlDocument& doc);
    
        // Parses .xml file and loads data. Returns TRUE on success, FALSE
        // otherwise. 
        // NOTE: Any call to this method will result into linking against libxml
        //       and app's binary size will grow by ca. 250kB
        bool Load(const wxString& filename, wxXmlIOType io_type = wxXML_IO_AUTO);
        bool Load(wxInputStream& stream, wxXmlIOType io_type = wxXML_IO_AUTO);
        
        // Saves document as .xml file.
        bool Save(const wxString& filename, wxXmlIOType io_type) const;
        bool Save(wxOutputStream& stream, wxXmlIOType io_type) const;
        
        // Returns root node of the document.
        wxXmlNode *GetRoot() const { return m_Root; }
        
        // Returns version of document (may be empty).
        wxString GetVersion() const { return m_Version; }
        // Returns encoding of document (may be empty).
        wxString GetEncoding() const { return m_Encoding; }
        
        // Write-access methods:
        void SetRoot(wxXmlNode *node) { delete m_Root ; m_Root = node; }
        void SetVersion(const wxString& version) { m_Version = version; }
        void SetEncoding(const wxString& encoding) { m_Encoding = encoding; }

        static void AddHandler(wxXmlIOHandler *handler);
        static void CleanUpHandlers();
        static void InitStandardHandlers();

    protected:
        static wxList *sm_Handlers;

    private:
        wxString m_Version, m_Encoding;
        wxXmlNode *m_Root;
        
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
