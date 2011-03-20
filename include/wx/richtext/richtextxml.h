/////////////////////////////////////////////////////////////////////////////
// Name:        wx/richtext/richtextxml.h
// Purpose:     XML and HTML I/O for wxRichTextCtrl
// Author:      Julian Smart
// Modified by:
// Created:     2005-09-30
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RICHTEXTXML_H_
#define _WX_RICHTEXTXML_H_

/*!
 * Includes
 */

#include "wx/richtext/richtextbuffer.h"
#include "wx/richtext/richtextstyles.h"

#if wxUSE_RICHTEXT && wxUSE_XML

/*!
 * wxRichTextXMLHandler
 */

class WXDLLIMPEXP_FWD_XML wxXmlNode;
class WXDLLIMPEXP_FWD_XML wxXmlDocument;

class WXDLLIMPEXP_RICHTEXT wxRichTextXMLHandler: public wxRichTextFileHandler
{
    DECLARE_CLASS(wxRichTextXMLHandler)
public:
    wxRichTextXMLHandler(const wxString& name = wxT("XML"), const wxString& ext = wxT("xml"), int type = wxRICHTEXT_TYPE_XML)
        : wxRichTextFileHandler(name, ext, type)
        { Init(); }
        
    void Init();

#if wxUSE_STREAMS

#if wxRICHTEXT_HAVE_DIRECT_OUTPUT
    /// Recursively export an object
    bool ExportXML(wxOutputStream& stream, wxRichTextObject& obj, int level);
    bool ExportStyleDefinition(wxOutputStream& stream, wxRichTextStyleDefinition* def, int level);
    wxString AddAttributes(const wxRichTextAttr& attr, bool isPara = false);
    bool WriteProperties(wxOutputStream& stream, const wxRichTextProperties& properties, int level);
    void OutputString(wxOutputStream& stream, const wxString& str);
    void OutputStringEnt(wxOutputStream& stream, const wxString& str);
    void OutputIndentation(wxOutputStream& stream, int indent);
    static wxString AttributeToXML(const wxString& str);
#endif

#if wxRICHTEXT_HAVE_XMLDOCUMENT_OUTPUT
    bool ExportXML(wxXmlNode* parent, wxRichTextObject& obj);
    bool ExportStyleDefinition(wxXmlNode* parent, wxRichTextStyleDefinition* def);
    bool AddAttributes(wxXmlNode* node, wxRichTextAttr& attr, bool isPara = false);
    bool WriteProperties(wxXmlNode* node, const wxRichTextProperties& properties);
#endif

    /// Make a string from the given property. This can be overridden for custom variants.
    virtual wxString MakeStringFromProperty(const wxVariant& var);

    /// Create a proprty from the string read from the XML file.
    virtual wxVariant MakePropertyFromString(const wxString& name, const wxString& value, const wxString& type);

    /// Recursively import an object
    bool ImportXML(wxRichTextBuffer* buffer, wxRichTextObject* obj, wxXmlNode* node);
    bool ImportStyleDefinition(wxRichTextStyleSheet* sheet, wxXmlNode* node);
    bool ImportProperties(wxRichTextObject* obj, wxXmlNode* node);

    /// Import style parameters
    bool ImportStyle(wxRichTextAttr& attr, wxXmlNode* node, bool isPara = false);
#endif

    /// Creates an object given an XML element name
    virtual wxRichTextObject* CreateObjectForXMLName(wxRichTextObject* parent, const wxString& name) const;
    
    /// Can we save using this handler?
    virtual bool CanSave() const { return true; }

    /// Can we load using this handler?
    virtual bool CanLoad() const { return true; }

    // Used during saving
    wxMBConv* GetConvMem() const { return m_convMem; }
    wxMBConv* GetConvFile() const { return m_convFile; }

// Implementation

    bool HasParam(wxXmlNode* node, const wxString& param);
    wxXmlNode *GetParamNode(wxXmlNode* node, const wxString& param);
    wxString GetNodeContent(wxXmlNode *node);
    wxString GetParamValue(wxXmlNode *node, const wxString& param);
    wxString GetText(wxXmlNode *node, const wxString& param = wxEmptyString, bool translate = false);
    static wxXmlNode* FindNode(wxXmlNode* node, const wxString& name);

protected:
#if wxUSE_STREAMS
    virtual bool DoLoadFile(wxRichTextBuffer *buffer, wxInputStream& stream);
    virtual bool DoSaveFile(wxRichTextBuffer *buffer, wxOutputStream& stream);
#endif

#if wxRICHTEXT_HAVE_DIRECT_OUTPUT
    // Used during saving
    wxMBConv* m_convMem;
    wxMBConv* m_convFile;
#endif
};

#endif
    // wxUSE_RICHTEXT && wxUSE_XML

#endif
    // _WX_RICHTEXTXML_H_
