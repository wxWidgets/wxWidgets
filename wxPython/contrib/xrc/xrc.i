/////////////////////////////////////////////////////////////////////////////
// Name:        xrc.i
// Purpose:     Wrappers for the XML based Resource system
//
// Author:      Robin Dunn
//
// Created:     4-June-2001
// RCS-ID:      $Id$
// Copyright:   (c) 2001 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module xrc


%{
#include "wxPython.h"
#include "pyistream.h"
#include "wx/xrc/xml.h"
#include "wx/xrc/xmlres.h"
#include <wx/filesys.h>
#include <wx/fs_mem.h>
%}

//---------------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

%extern wx.i
%extern windows.i
%extern _defs.i
%extern events.i
%extern controls.i

%extern streams.i


//---------------------------------------------------------------------------
%{
    // Put some wx default wxChar* values into wxStrings.
    static const wxString wxPyEmptyString(wxT(""));
    static const wxString wxPyUTF8String(wxT("UTF-8"));
    static const wxString wxPyStyleString(wxT("style"));
    static const wxString wxPySizeString(wxT("size"));
    static const wxString wxPyPosString(wxT("pos"));
    static const wxString wxPyBitmapString(wxT("bitmap"));
    static const wxString wxPyIconString(wxT("icon"));
    static const wxString wxPyFontString(wxT("font"));
%}

class wxPyXmlSubclassFactory;

//---------------------------------------------------------------------------

enum wxXmlResourceFlags
{
    wxXRC_USE_LOCALE     = 1,
    wxXRC_NO_SUBCLASSING = 2
};


// This class holds XML resources from one or more .xml files
// (or derived forms, either binary or zipped -- see manual for
// details).

class wxXmlResource : public wxObject
{
public:
    // Ctors.
    // Flags: wxXRC_USE_LOCALE
    //              translatable strings will be translated via _()
    //        wxXRC_NO_SUBCLASSING
    //              subclass property of object nodes will be ignored
    //              (useful for previews in XRC editors)
    wxXmlResource(const wxString& filemask, int flags = wxXRC_USE_LOCALE);
    %name(wxEmptyXmlResource) wxXmlResource(int flags = wxXRC_USE_LOCALE);
    %pragma(python) addtomethod = "__init__:self.InitAllHandlers()"
    %pragma(python) addtomethod = "wxEmptyXmlResource:val.InitAllHandlers()"

    ~wxXmlResource();


    // Loads resources from XML files that match given filemask.
    // This method understands VFS (see filesys.h).
    bool Load(const wxString& filemask);

    %addmethods {
        bool LoadFromString(const wxString& data) {
            static int s_memFileIdx = 0;

            // Check for memory FS. If not present, load the handler:
            wxMemoryFSHandler::AddFile(wxT("XRC_resource/dummy_file"),
                                       wxT("dummy data"));
            wxFileSystem fsys;
            wxFSFile *f = fsys.OpenFile(wxT("memory:XRC_resource/dummy_file"));
            wxMemoryFSHandler::RemoveFile(wxT("XRC_resource/dummy_file"));
            if (f)
                delete f;
            else
                wxFileSystem::AddHandler(new wxMemoryFSHandler);

            // Now put the resource data into the memory FS
            wxString filename(wxT("XRC_resource/data_string_"));
            filename << s_memFileIdx;
            s_memFileIdx += 1;
            wxMemoryFSHandler::AddFile(filename, data);

            // Load the "file" into the resource object
            bool retval = self->Load(wxT("memory:") + filename );

            return retval;
        }
    }

    // Initialize handlers for all supported controls/windows. This will
    // make the executable quite big because it forces linking against
    // most of wxWin library
    void InitAllHandlers();

    // Initialize only specific handler (or custom handler). Convention says
    // that handler name is equal to control's name plus 'XmlHandler', e.g.
    // wxTextCtrlXmlHandler, wxHtmlWindowXmlHandler. XML resource compiler
    // (xmlres) can create include file that contains initialization code for
    // all controls used within the resource.
    void AddHandler(wxPyXmlResourceHandler *handler);

    // Add a new handler at the begining of the handler list
    void InsertHandler(wxPyXmlResourceHandler *handler);

    // Removes all handlers
    void ClearHandlers();

    // Registers subclasses factory for use in XRC. This function is not meant
    // for public use, please see the comment above wxXmlSubclassFactory
    // definition.
    static void AddSubclassFactory(wxPyXmlSubclassFactory *factory);


    // Loads menu from resource. Returns NULL on failure.
    wxMenu *LoadMenu(const wxString& name);

    // Loads menubar from resource. Returns NULL on failure.
    wxMenuBar *LoadMenuBar(const wxString& name);
    %name(LoadMenuBarOnFrame) wxMenuBar *LoadMenuBar(wxWindow *parent, const wxString& name);


    // Loads toolbar
    wxToolBar *LoadToolBar(wxWindow *parent, const wxString& name);

    // Loads dialog. dlg points to parent window (if any). Second form
    // is used to finish creation of already existing instance (main reason
    // for this is that you may want to use derived class with new event table)
    // Example (typical usage):
    //      MyDialog dlg;
    //      wxTheXmlResource->LoadDialog(&dlg, mainFrame, "my_dialog");
    //      dlg->ShowModal();
    wxDialog *LoadDialog(wxWindow *parent, const wxString& name);
    %name(LoadOnDialog)bool LoadDialog(wxDialog *dlg, wxWindow *parent, const wxString& name);

    // Loads panel. panel points to parent window (if any). Second form
    // is used to finish creation of already existing instance.
    wxPanel *LoadPanel(wxWindow *parent, const wxString& name);
    %name(LoadOnPanel)bool LoadPanel(wxPanel *panel, wxWindow *parent, const wxString& name);

    // Load a frame's contents from a resource
    wxFrame *LoadFrame(wxWindow* parent, const wxString& name);
    %name(LoadOnFrame)bool LoadFrame(wxFrame* frame, wxWindow *parent, const wxString& name);

    // Load an object from the resource specifying both the resource name and
    // the classname.  This lets you load nonstandard container windows.
    wxObject *LoadObject(wxWindow *parent, const wxString& name,
                         const wxString& classname);

    // Load an object from the resource specifying both the resource name and
    // the classname.  This form lets you finish the creation of an existing
    // instance.
    %name(LoadOnObject)bool LoadObject(wxObject *instance, wxWindow *parent, const wxString& name,
                                       const wxString& classname);

        // Loads bitmap or icon resource from file:
    wxBitmap LoadBitmap(const wxString& name);
    wxIcon LoadIcon(const wxString& name);

    // Attaches unknown control into given panel/window/dialog:
    // (unknown controls are used in conjunction with <object class="unknown">)
    bool AttachUnknownControl(const wxString& name, wxWindow *control,
                              wxWindow *parent = NULL);

    // Returns numeric ID that is equivalent to string id used in XML
    // resource. To be used in event tables
    // Macro XMLID is provided for convenience
    static int GetXRCID(const wxString& str_id);

    // Returns version info (a.b.c.d = d+ 256*c + 256^2*b + 256^3*a)
    long GetVersion() const;

    // Compares resources version to argument. Returns -1 if resources version
    // is less than the argument, +1 if greater and 0 if they equal.
    int CompareVersion(int major, int minor, int release, int revision) const;


    // Gets global resources object or create one if none exists
    static wxXmlResource *Get();
    // Sets global resources object and returns pointer to previous one (may be NULL).
    static wxXmlResource *Set(wxXmlResource *res);

    // Returns flags, which may be a bitlist of wxXRC_USE_LOCALE and wxXRC_NO_SUBCLASSING.
    int GetFlags();

    // Set flags after construction.
    void SetFlags(int flags) { m_flags = flags; }

};

//----------------------------------------------------------------------

%pragma(python) code = "
def XRCID(str_id):
    return wxXmlResource_GetXRCID(str_id)

def XRCCTRL(window, str_id, *ignoreargs):
    return window.FindWindowById(XRCID(str_id))

XMLID = XRCID
XMLCTRL = XRCCTRL
"

//----------------------------------------------------------------------
// wxXmlSubclassFactory


%{
class wxPyXmlSubclassFactory : public wxXmlSubclassFactory
{
public:
    wxPyXmlSubclassFactory() {}
    DEC_PYCALLBACK_OBJECT_STRING_pure(Create);
    PYPRIVATE;
};

IMP_PYCALLBACK_OBJECT_STRING_pure(wxPyXmlSubclassFactory, wxXmlSubclassFactory, Create);
%}


%name(wxXmlSubclassFactory)class wxPyXmlSubclassFactory {
public:
    wxPyXmlSubclassFactory();

    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxXmlSubclassFactory)"
};


//----------------------------------------------------------------------
// In order to provide wrappers for wxXmlResourceHandler we need to also
// provide the classes for representing and parsing XML.


// Represents XML node type.
enum wxXmlNodeType
{
    // note: values are synchronized with xmlElementType from libxml
    wxXML_ELEMENT_NODE,
    wxXML_ATTRIBUTE_NODE,
    wxXML_TEXT_NODE,
    wxXML_CDATA_SECTION_NODE,
    wxXML_ENTITY_REF_NODE,
    wxXML_ENTITY_NODE,
    wxXML_PI_NODE,
    wxXML_COMMENT_NODE,
    wxXML_DOCUMENT_NODE,
    wxXML_DOCUMENT_TYPE_NODE,
    wxXML_DOCUMENT_FRAG_NODE,
    wxXML_NOTATION_NODE,
    wxXML_HTML_DOCUMENT_NODE
};



// Represents node property(ies).
// Example: in <img src="hello.gif" id="3"/> "src" is property with value
//          "hello.gif" and "id" is property with value "3".
class wxXmlProperty
{
public:
    wxXmlProperty(const wxString& name = wxPyEmptyString,
                  const wxString& value = wxPyEmptyString,
                  wxXmlProperty *next = NULL);

    wxString GetName() const;
    wxString GetValue() const;
    wxXmlProperty *GetNext() const;

    void SetName(const wxString& name);
    void SetValue(const wxString& value);
    void SetNext(wxXmlProperty *next);
};




// Represents node in XML document. Node has name and may have content
// and properties. Most common node types are wxXML_TEXT_NODE (name and props
// are irrelevant) and wxXML_ELEMENT_NODE (e.g. in <title>hi</title> there is
// element with name="title", irrelevant content and one child (wxXML_TEXT_NODE
// with content="hi").
//
// If wxUSE_UNICODE is 0, all strings are encoded in the encoding given to Load
// (default is UTF-8).
class wxXmlNode
{
public:
    wxXmlNode(wxXmlNode *parent = NULL,
              wxXmlNodeType type = 0,
              const wxString& name = wxPyEmptyString,
              const wxString& content = wxPyEmptyString,
              wxXmlProperty *props = NULL,
              wxXmlNode *next = NULL);
    ~wxXmlNode();


    // user-friendly creation:
    %name(wxXmlNodeEasy)wxXmlNode(wxXmlNodeType type, const wxString& name,
                                  const wxString& content = wxPyEmptyString);

    void AddChild(wxXmlNode *child);
    void InsertChild(wxXmlNode *child, wxXmlNode *before_node);
    bool RemoveChild(wxXmlNode *child);
    void AddProperty(wxXmlProperty *prop);
    %name(AddPropertyName)void AddProperty(const wxString& name, const wxString& value);
    bool DeleteProperty(const wxString& name);

    // access methods:
    wxXmlNodeType GetType() const;
    wxString GetName() const;
    wxString GetContent() const;

    wxXmlNode *GetParent() const;
    wxXmlNode *GetNext() const;
    wxXmlNode *GetChildren() const;

    wxXmlProperty *GetProperties() const;
    wxString GetPropVal(const wxString& propName,
                        const wxString& defaultVal) const;
    bool HasProp(const wxString& propName) const;

    void SetType(wxXmlNodeType type);
    void SetName(const wxString& name);
    void SetContent(const wxString& con);

    void SetParent(wxXmlNode *parent);
    void SetNext(wxXmlNode *next);
    void SetChildren(wxXmlNode *child);

    void SetProperties(wxXmlProperty *prop);
};



// This class holds XML data/document as parsed by XML parser.
class wxXmlDocument : public wxObject
{
public:
    wxXmlDocument(const wxString& filename,
                  const wxString& encoding = wxPyUTF8String);
    %name(wxXmlDocumentFromStream)wxXmlDocument(wxInputStream& stream,
                                                const wxString& encoding = wxPyUTF8String);
    %name(wxEmptyXmlDocument)wxXmlDocument();

    ~wxXmlDocument();


    // Parses .xml file and loads data. Returns TRUE on success, FALSE
    // otherwise.
    bool Load(const wxString& filename,
              const wxString& encoding = wxPyUTF8String);
    %name(LoadFromStream)bool Load(wxInputStream& stream,
                                   const wxString& encoding = wxPyUTF8String);

    // Saves document as .xml file.
    bool Save(const wxString& filename) const;
    %name(SaveToStream)bool Save(wxOutputStream& stream) const;

    bool IsOk() const;

    // Returns root node of the document.
    wxXmlNode *GetRoot() const;

    // Returns version of document (may be empty).
    wxString GetVersion() const;

    // Returns encoding of document (may be empty).
    // Note: this is the encoding original file was saved in, *not* the
    // encoding of in-memory representation!
    wxString GetFileEncoding() const;

    // Write-access methods:
    void SetRoot(wxXmlNode *node);
    void SetVersion(const wxString& version);
    void SetFileEncoding(const wxString& encoding);

    %addmethods {
        // Returns encoding of in-memory representation of the document (same
        // as passed to Load or ctor, defaults to UTF-8).  NB: this is
        // meaningless in Unicode build where data are stored as wchar_t*
        wxString GetEncoding() {
        #if wxUSE_UNICODE
            return wxPyEmptyString;
        #else
            return self->GetEncoding();
        #endif
        }
        void SetEncoding(const wxString& enc) {
        #if wxUSE_UNICODE
            // do nothing
        #else
            self->SetEncoding(enc);
        #endif
        }
    }
};


//----------------------------------------------------------------------
// And now for wxXmlResourceHandler...



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
    wxWindow* GetInstanceAsWindow()     { return m_instanceAsWindow; }


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

    wxString GetText(const wxString& param, bool translate = TRUE)
        { return wxXmlResourceHandler::GetText(param, translate); }

    int GetID()
        { return wxXmlResourceHandler::GetID(); }

    wxString GetName()
        { return wxXmlResourceHandler::GetName(); }

    bool GetBool(const wxString& param, bool defaultv = FALSE)
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

    void CreateChildren(wxObject *parent, bool this_hnd_only = FALSE)
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


//----------------------------------------------------------------------
// Now the version that will be SWIGged.


%name(wxXmlResourceHandler) class wxPyXmlResourceHandler : public wxObject {
public:
    wxPyXmlResourceHandler() : wxXmlResourceHandler() {}
    //~wxPyXmlResourceHandler();

    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxXmlResourceHandler)"



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
    wxWindow* GetInstanceAsWindow()     { return m_instanceAsWindow; }


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
    wxString GetText(const wxString& param, bool translate = TRUE);

    // Returns the XRCID.
    int GetID();

    // Returns the resource name.
    wxString GetName();

    // Gets a bool flag (1, t, yes, on, true are TRUE, everything else is FALSE).
    bool GetBool(const wxString& param, bool defaultv = FALSE);

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
    void CreateChildren(wxObject *parent, bool this_hnd_only = FALSE);

    // Helper function.
    void CreateChildrenPrivately(wxObject *parent, wxXmlNode *rootnode = NULL);

    // Creates a resource from a node.
    wxObject *CreateResFromNode(wxXmlNode *node,
                                wxObject *parent, wxObject *instance = NULL);

    // helper
    wxFileSystem& GetCurFileSystem();
};


//----------------------------------------------------------------------
//----------------------------------------------------------------------

%init %{

    wxClassInfo::CleanUpClasses();
    wxClassInfo::InitializeClasses();

    wxXmlInitResourceModule();
    wxXmlResource::Get()->InitAllHandlers();

%}

//----------------------------------------------------------------------
// This file gets appended to the shadow class file.
//----------------------------------------------------------------------

%pragma(python) include="_xrcextras.py";


