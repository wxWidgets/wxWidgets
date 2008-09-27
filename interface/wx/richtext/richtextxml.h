/////////////////////////////////////////////////////////////////////////////
// Name:        richtext/richtextxml.h
// Purpose:     interface of wxRichTextXMLHandler
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxRichTextXMLHandler

    A handler for loading and saving content in an XML format specific
    to wxRichTextBuffer. You can either add the handler to the buffer
    and load and save through the buffer or control API, or you can
    create an instance of the handler on the stack and call its
    functions directly.

    @library{wxrichtext}
    @category{richtext}
*/
class wxRichTextXMLHandler : public wxRichTextFileHandler
{
public:
    /**
        ,  wxString&@e ext = wxT("xml"), @b int@e type = wxRICHTEXT_TYPE_XML)
        Constructor.
    */
    wxRichTextXMLHandler() const;

    /**
        Returns @true.
    */
    virtual bool CanLoad() const;

    /**
        Returns @true.
    */
    virtual bool CanSave() const;

    /**
        Creates XML code for a given character or paragraph style.
    */
    wxString CreateStyle(const wxTextAttr& attr, bool isPara = false);

    /**
        Loads buffer context from the given stream.
    */
    bool DoLoadFile(wxRichTextBuffer* buffer, wxInputStream& stream);

    /**
        Saves buffer context to the given stream.
    */
    bool DoSaveFile(wxRichTextBuffer* buffer, wxOutputStream& stream);

    /**
        Recursively exports an object to the stream.
    */
    bool ExportXML(wxOutputStream& stream, wxMBConv* convMem,
                   wxMBConv* convFile,
                   wxRichTextObject& obj,
                   int level);

    /**
        Helper function: gets node context.
    */
    wxString GetNodeContent(wxXmlNode* node);

    /**
        Helper function: gets a named parameter from the XML node.
    */
    wxXmlNode* GetParamNode(wxXmlNode* node, const wxString& param);

    /**
        Helper function: gets a named parameter from the XML node.
    */
    wxString GetParamValue(wxXmlNode* node, const wxString& param);

    /**
        Helper function: gets style parameters from the given XML node.
    */
    bool GetStyle(wxTextAttr& attr, wxXmlNode* node,
                  bool isPara = false);

    /**
        Helper function: gets text from the node.
    */
    wxString GetText(wxXmlNode* node,
                     const wxString& param = wxEmptyString,
                     bool translate = false);

    /**
        Helper function: returns @true if the node has the given parameter.
    */
    bool HasParam(wxXmlNode* node, const wxString& param);

    /**
        Recursively imports an object.
    */
    bool ImportXML(wxRichTextBuffer* buffer, wxXmlNode* node);
};

