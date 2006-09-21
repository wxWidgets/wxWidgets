/////////////////////////////////////////////////////////////////////////////
// Name:        _xml.i
// Purpose:     SWIG interface for other wxXml classes
//
// Author:      Robin Dunn
//
// Created:     4-June-2001
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
%newgroup



%{ // C++ version of Python aware wxXmlResourceHandler, for the pure virtual
   // callbacks, as well as to make some protected things public so they can
   // be wrapped.
class wxPyXmlResourceHandler : public wxXmlResourceHandler {
public:
    wxPyXmlResourceHandler() : wxXmlResourceHandler() {}
    //~wxPyXmlResourceHandler();

    // Base class virtuals

    DEC_PYCALLBACK_OBJECT__pure(DoCreateResource);
    DEC_PYCALLBACK_BOOL_NODE_pure(CanHandle);


    // accessors for protected members

    wxXmlResource* GetResource()        { return m_resource; }
    wxXmlNode* GetNode()                { return m_node; }
    wxString GetClass()                 { return m_class; }
    wxObject* GetParent()               { return m_parent; }
    wxObject* GetInstance()             { return m_instance; }
    wxWindow* GetParentAsWindow()       { return m_parentAsWindow; }


    // turn some protected methods into public via delegation

    bool IsOfClass(wxXmlNode *node, const wxString& classname)
        { return wxXmlResourceHandler::IsOfClass(node, classname); }

    wxString GetNodeContent(wxXmlNode *node)
        { return wxXmlResourceHandler::GetNodeContent(node); }

    bool HasParam(const wxString& param)
        { return wxXmlResourceHandler::HasParam(param); }

    wxXmlNode *GetParamNode(const wxString& param)
        { return wxXmlResourceHandler::GetParamNode(param); }

    wxString GetParamValue(const wxString& param)
        { return wxXmlResourceHandler::GetParamValue(param); }

    void AddStyle(const wxString& name, int value)
        { wxXmlResourceHandler::AddStyle(name, value); }

    void AddWindowStyles()
        { wxXmlResourceHandler::AddWindowStyles(); }

    int GetStyle(const wxString& param = wxT("style"), int defaults = 0)
        { return wxXmlResourceHandler::GetStyle(param, defaults); }

    wxString GetText(const wxString& param, bool translate = true)
        { return wxXmlResourceHandler::GetText(param, translate); }

    int GetID()
        { return wxXmlResourceHandler::GetID(); }

    wxString GetName()
        { return wxXmlResourceHandler::GetName(); }

    bool GetBool(const wxString& param, bool defaultv = false)
        { return wxXmlResourceHandler::GetBool(param, defaultv); }

    long GetLong( const wxString& param, long defaultv = 0 )
        { return wxXmlResourceHandler::GetLong(param, defaultv); }

    wxColour GetColour(const wxString& param)
        { return wxXmlResourceHandler::GetColour(param); }

    wxSize GetSize(const wxString& param = wxT("size"))
        { return wxXmlResourceHandler::GetSize(param); }

    wxPoint GetPosition(const wxString& param = wxT("pos"))
        { return wxXmlResourceHandler::GetPosition(param); }

    wxCoord GetDimension(const wxString& param, wxCoord defaultv = 0)
        { return wxXmlResourceHandler::GetDimension(param, defaultv); }

    wxBitmap GetBitmap(const wxString& param = wxT("bitmap"),
                       const wxArtClient& defaultArtClient = wxART_OTHER,
                       wxSize size = wxDefaultSize)
        { return wxXmlResourceHandler::GetBitmap(param, defaultArtClient, size); }

    wxIcon GetIcon(const wxString& param = wxT("icon"),
                   const wxArtClient& defaultArtClient = wxART_OTHER,
                   wxSize size = wxDefaultSize)
        { return wxXmlResourceHandler::GetIcon(param, defaultArtClient, size); }

    wxFont GetFont(const wxString& param = wxT("font"))
        { return wxXmlResourceHandler::GetFont(param); }

    void SetupWindow(wxWindow *wnd)
        { wxXmlResourceHandler::SetupWindow(wnd); }

    void CreateChildren(wxObject *parent, bool this_hnd_only = false)
        { wxXmlResourceHandler::CreateChildren(parent, this_hnd_only); }

    void CreateChildrenPrivately(wxObject *parent, wxXmlNode *rootnode = NULL)
        { wxXmlResourceHandler::CreateChildrenPrivately(parent, rootnode); }

    wxObject *CreateResFromNode(wxXmlNode *node,
                                wxObject *parent, wxObject *instance = NULL)
        { return wxXmlResourceHandler::CreateResFromNode(node, parent, instance); }

    wxFileSystem& GetCurFileSystem()
        { return wxXmlResourceHandler::GetCurFileSystem(); }


    PYPRIVATE;
};

IMP_PYCALLBACK_OBJECT__pure(wxPyXmlResourceHandler, wxXmlResourceHandler, DoCreateResource);
IMP_PYCALLBACK_BOOL_NODE_pure(wxPyXmlResourceHandler, wxXmlResourceHandler, CanHandle);

%}


// Now the version that will be SWIGged.


%rename(XmlResourceHandler) wxPyXmlResourceHandler;
class wxPyXmlResourceHandler : public wxObject {
public:
    %pythonAppend wxPyXmlResourceHandler "self._setCallbackInfo(self, XmlResourceHandler)"
    wxPyXmlResourceHandler() : wxXmlResourceHandler() {}
    ~wxPyXmlResourceHandler();

    void _setCallbackInfo(PyObject* self, PyObject* _class);


    // Creates an object (menu, dialog, control, ...) from an XML node.
    // Should check for validity.
    // parent is a higher-level object (usually window, dialog or panel)
    // that is often neccessary to create the resource.
    // If instance is non-NULL it should not create a new instance via 'new' but
    // should rather use this one, and call its Create method.
    wxObject *CreateResource(wxXmlNode *node, wxObject *parent,
                             wxObject *instance);

    // Sets the parent resource.
    void SetParentResource(wxXmlResource *res);


    wxXmlResource* GetResource()        { return m_resource; }
    wxXmlNode* GetNode()                { return m_node; }
    wxString GetClass()                 { return m_class; }
    wxObject* GetParent()               { return m_parent; }
    wxObject* GetInstance()             { return m_instance; }
    wxWindow* GetParentAsWindow()       { return m_parentAsWindow; }
//     wxWindow* GetInstanceAsWindow()     { return m_instanceAsWindow; }


    // Returns true if the node has a property class equal to classname,
    // e.g. <object class="wxDialog">.
    bool IsOfClass(wxXmlNode *node, const wxString& classname);

    // Gets node content from wxXML_ENTITY_NODE
    // The problem is, <tag>content<tag> is represented as
    // wxXML_ENTITY_NODE name="tag", content=""
    //    |-- wxXML_TEXT_NODE or
    //        wxXML_CDATA_SECTION_NODE name="" content="content"
    wxString GetNodeContent(wxXmlNode *node);

    // Check to see if a parameter exists.
    bool HasParam(const wxString& param);

    // Finds the node or returns NULL.
    wxXmlNode *GetParamNode(const wxString& param);

    // Finds the parameter value or returns the empty string.
    wxString GetParamValue(const wxString& param);

    // Add a style flag (e.g. wxMB_DOCKABLE) to the list of flags
    // understood by this handler.
    void AddStyle(const wxString& name, int value);

    // Add styles common to all wxWindow-derived classes.
    void AddWindowStyles();

    // Gets style flags from text in form "flag | flag2| flag3 |..."
    // Only understads flags added with AddStyle
    int GetStyle(const wxString& param = wxPyStyleString, int defaults = 0);

    // Gets text from param and does some conversions:
    // - replaces \n, \r, \t by respective chars (according to C syntax)
    // - replaces _ by & and __ by _ (needed for _File => &File because of XML)
    // - calls wxGetTranslations (unless disabled in wxXmlResource)
    wxString GetText(const wxString& param, bool translate = true);

    // Returns the XRCID.
    int GetID();

    // Returns the resource name.
    wxString GetName();

    // Gets a bool flag (1, t, yes, on, true are True, everything else is false).
    bool GetBool(const wxString& param, bool defaultv = false);

    // Gets the integer value from the parameter.
    long GetLong( const wxString& param, long defaultv = 0 );

    // Gets colour in HTML syntax (#RRGGBB).
    wxColour GetColour(const wxString& param);

    // Gets the size (may be in dialog units).
    wxSize GetSize(const wxString& param = wxPySizeString);

    // Gets the position (may be in dialog units).
    wxPoint GetPosition(const wxString& param = wxPyPosString);

    // Gets a dimension (may be in dialog units).
    wxCoord GetDimension(const wxString& param, wxCoord defaultv = 0);

    // Gets a bitmap.
    wxBitmap GetBitmap(const wxString& param = wxPyBitmapString,
                       const wxArtClient& defaultArtClient = wxART_OTHER,
                       wxSize size = wxDefaultSize);

    // Gets an icon.
    wxIcon GetIcon(const wxString& param = wxPyIconString,
                   const wxArtClient& defaultArtClient = wxART_OTHER,
                   wxSize size = wxDefaultSize);

    // Gets a font.
    wxFont GetFont(const wxString& param = wxPyFontString);

    // Sets common window options.
    void SetupWindow(wxWindow *wnd);

    // Creates children.
    void CreateChildren(wxObject *parent, bool this_hnd_only = false);

    // Helper function.
    void CreateChildrenPrivately(wxObject *parent, wxXmlNode *rootnode = NULL);

    // Creates a resource from a node.
    wxObject *CreateResFromNode(wxXmlNode *node,
                                wxObject *parent, wxObject *instance = NULL);

    // helper
    wxFileSystem& GetCurFileSystem();
};



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
