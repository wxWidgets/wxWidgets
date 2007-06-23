/////////////////////////////////////////////////////////////////////////////
// Name:        _richtextxml
// Purpose:     wxRichTextXMLHandler
//
// Author:      Robin Dunn
//
// Created:     18-May-2007
// RCS-ID:      $Id$
// Copyright:   (c) 2007 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

%{
#include <wx/richtext/richtextxml.h>
%}

//---------------------------------------------------------------------------
%newgroup

MAKE_CONST_WXSTRING2(XmlName, wxT("XML"));
MAKE_CONST_WXSTRING2(XmlExt,  wxT("xml"));


class wxRichTextXMLHandler: public wxRichTextFileHandler
{
public:
    wxRichTextXMLHandler(const wxString& name = wxPyXmlName,
                         const wxString& ext = wxPyXmlExt,
                         int type = wxRICHTEXT_TYPE_XML);

// #if wxUSE_STREAMS
//     /// Recursively export an object
//     bool ExportXML(wxOutputStream& stream, wxMBConv* convMem, wxMBConv* convFile, wxRichTextObject& obj, int level);
//     bool ExportStyleDefinition(wxOutputStream& stream, wxMBConv* convMem, wxMBConv* convFile, wxRichTextStyleDefinition* def, int level);

//     /// Recursively import an object
//     bool ImportXML(wxRichTextBuffer* buffer, wxXmlNode* node);
//     bool ImportStyleDefinition(wxRichTextStyleSheet* sheet, wxXmlNode* node);

//     /// Create style parameters
//     wxString CreateStyle(const wxTextAttrEx& attr, bool isPara = false);

//     /// Get style parameters
//     bool GetStyle(wxTextAttrEx& attr, wxXmlNode* node, bool isPara = false);
// #endif

    /// Can we save using this handler?
    virtual bool CanSave() const;

    /// Can we load using this handler?
    virtual bool CanLoad() const;


//     bool HasParam(wxXmlNode* node, const wxString& param);
//     wxXmlNode *GetParamNode(wxXmlNode* node, const wxString& param);
//     wxString GetNodeContent(wxXmlNode *node);
//     wxString GetParamValue(wxXmlNode *node, const wxString& param);
//     wxString GetText(wxXmlNode *node, const wxString& param = wxEmptyString, bool translate = false);

};

//---------------------------------------------------------------------------
