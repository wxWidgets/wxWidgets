/////////////////////////////////////////////////////////////////////////////
// Name:        xmlres.h
// Purpose:     XML resources
// Author:      Vaclav Slavik
// Created:     2000/03/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XMLRES_H_
#define _WX_XMLRES_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "xmlres.h"
#endif

#include "wx/defs.h"
#include "wx/string.h"
#include "wx/dynarray.h"
#include "wx/datetime.h"
#include "wx/list.h"
#include "wx/gdicmn.h"
#include "wx/filesys.h"
#include "wx/bitmap.h"
#include "wx/icon.h"
#include "wx/artprov.h"

#include "wx/xrc/xml.h"

class WXDLLEXPORT wxMenu;
class WXDLLEXPORT wxMenuBar;
class WXDLLEXPORT wxDialog;
class WXDLLEXPORT wxPanel;
class WXDLLEXPORT wxWindow;
class WXDLLEXPORT wxFrame;
class WXDLLEXPORT wxToolBar;

class WXXMLDLLEXPORT wxXmlResourceHandler;
class WXXMLDLLEXPORT wxXmlSubclassFactory;
class WXXMLDLLEXPORT wxXmlSubclassFactoriesList;
class wxXmlResourceModule;


// These macros indicate current version of XML resources (this information is
// encoded in root node of XRC file as "version" property).
//
// Rules for increasing version number:
//   - change it only if you made incompatible change to the format. Addition of new
//     attribute to control handler is _not_ incompatible change, because older
//     versions of the library may ignore it.
//   - if you change version number, follow these steps:
//       - set major, minor and release numbers to respective version numbers of
//         the wxWindows library (see wx/version.h)
//       - reset revision to 0 unless the first three are same as before, in which
//         case you should increase revision by one
#define WX_XMLRES_CURRENT_VERSION_MAJOR            2
#define WX_XMLRES_CURRENT_VERSION_MINOR            3
#define WX_XMLRES_CURRENT_VERSION_RELEASE          0
#define WX_XMLRES_CURRENT_VERSION_REVISION         1
#define WX_XMLRES_CURRENT_VERSION_STRING    "2.3.0.1"

#define WX_XMLRES_CURRENT_VERSION \
                (WX_XMLRES_CURRENT_VERSION_MAJOR * 256*256*256 + \
                 WX_XMLRES_CURRENT_VERSION_MINOR * 256*256 + \
                 WX_XMLRES_CURRENT_VERSION_RELEASE * 256 + \
                 WX_XMLRES_CURRENT_VERSION_REVISION)

class WXXMLDLLEXPORT wxXmlResourceDataRecord
{
public:
    wxXmlResourceDataRecord() : Doc(NULL), Time(wxDateTime::Now()) {}
    ~wxXmlResourceDataRecord() {delete Doc;}

    wxString File;
    wxXmlDocument *Doc;
    wxDateTime Time;
};


#ifdef WXXMLISDLL
WX_DECLARE_EXPORTED_OBJARRAY(wxXmlResourceDataRecord, wxXmlResourceDataRecords);
#else
WX_DECLARE_OBJARRAY(wxXmlResourceDataRecord, wxXmlResourceDataRecords);
#endif

enum wxXmlResourceFlags
{
    wxXRC_USE_LOCALE     = 1,
    wxXRC_NO_SUBCLASSING = 2
};

// This class holds XML resources from one or more .xml files
// (or derived forms, either binary or zipped -- see manual for
// details).
class WXXMLDLLEXPORT wxXmlResource : public wxObject
{
public:
    // Constructor.
    // Flags: wxXRC_USE_LOCALE
    //              translatable strings will be translated via _()
    //        wxXRC_NO_SUBCLASSING
    //              subclass property of object nodes will be ignored
    //              (useful for previews in XRC editors)
    wxXmlResource(int flags = wxXRC_USE_LOCALE);

    // Constructor.
    // Flags: wxXRC_USE_LOCALE
    //              translatable strings will be translated via _()
    //        wxXRC_NO_SUBCLASSING
    //              subclass property of object nodes will be ignored
    //              (useful for previews in XRC editors)
    wxXmlResource(const wxString& filemask, int flags = wxXRC_USE_LOCALE);

    // Destructor.
    ~wxXmlResource();

    // Loads resources from XML files that match given filemask.
    // This method understands VFS (see filesys.h).
    bool Load(const wxString& filemask);

    // Initialize handlers for all supported controls/windows. This will
    // make the executable quite big because it forces linking against
    // most of the wxWindows library.
    void InitAllHandlers();

    // Initialize only a specific handler (or custom handler). Convention says
    // that handler name is equal to the control's name plus 'XmlHandler', for example
    // wxTextCtrlXmlHandler, wxHtmlWindowXmlHandler. The XML resource compiler
    // (xmlres) can create include file that contains initialization code for
    // all controls used within the resource.
    void AddHandler(wxXmlResourceHandler *handler);

    // Add a new handler at the begining of the handler list
    void InsertHandler(wxXmlResourceHandler *handler);

    // Removes all handlers
    void ClearHandlers();
    
    // Registers subclasses factory for use in XRC. This function is not meant
    // for public use, please see the comment above wxXmlSubclassFactory
    // definition.
    static void AddSubclassFactory(wxXmlSubclassFactory *factory);

    // Loads menu from resource. Returns NULL on failure.
    wxMenu *LoadMenu(const wxString& name);

    // Loads menubar from resource. Returns NULL on failure.
    wxMenuBar *LoadMenuBar(wxWindow *parent, const wxString& name);

    // Loads menubar from resource. Returns NULL on failure.
    wxMenuBar *LoadMenuBar(const wxString& name) { return LoadMenuBar(NULL, name); }

#if wxUSE_TOOLBAR
    // Loads a toolbar.
    wxToolBar *LoadToolBar(wxWindow *parent, const wxString& name);
#endif

    // Loads a dialog. dlg points to parent window (if any).
    wxDialog *LoadDialog(wxWindow *parent, const wxString& name);

    // Loads a dialog. dlg points to parent window (if any). This form
    // is used to finish creation of already existing instance (main reason
    // for this is that you may want to use derived class with new event table)
    // Example (typical usage):
    //      MyDialog dlg;
    //      wxTheXmlResource->LoadDialog(&dlg, mainFrame, "my_dialog");
    //      dlg->ShowModal();
    bool LoadDialog(wxDialog *dlg, wxWindow *parent, const wxString& name);

    // Loads a panel. panel points to parent window (if any).
    wxPanel *LoadPanel(wxWindow *parent, const wxString& name);

    // Loads a panel. panel points to parent window (if any). This form
    // is used to finish creation of already existing instance.
    bool LoadPanel(wxPanel *panel, wxWindow *parent, const wxString& name);

    // Loads a frame.
    wxFrame *LoadFrame(wxWindow* parent, const wxString& name);
    bool LoadFrame(wxFrame* frame, wxWindow *parent, const wxString& name);

    // Load an object from the resource specifying both the resource name and
    // the classname.  This lets you load nonstandard container windows.
    wxObject *LoadObject(wxWindow *parent, const wxString& name,
                         const wxString& classname);

    // Load an object from the resource specifying both the resource name and
    // the classname.  This form lets you finish the creation of an existing
    // instance.
    bool LoadObject(wxObject *instance, wxWindow *parent, const wxString& name,
                    const wxString& classname);

    // Loads a bitmap resource from a file.
    wxBitmap LoadBitmap(const wxString& name);

    // Loads an icon resource from a file.
    wxIcon LoadIcon(const wxString& name);

    // Attaches an unknown control to the given panel/window/dialog.
    // Unknown controls are used in conjunction with <object class="unknown">.
    bool AttachUnknownControl(const wxString& name, wxWindow *control,
                              wxWindow *parent = NULL);

    // Returns a numeric ID that is equivalent to the string id used in an XML
    // resource. To be used in event tables.
    // Macro XRCID is provided for convenience
    static int GetXRCID(const wxChar *str_id);

    // Returns version information (a.b.c.d = d+ 256*c + 256^2*b + 256^3*a).
    long GetVersion() const { return m_version; }

    // Compares resources version to argument. Returns -1 if resources version
    // is less than the argument, +1 if greater and 0 if they equal.
    int CompareVersion(int major, int minor, int release, int revision) const
        { return GetVersion() -
                 (major*256*256*256 + minor*256*256 + release*256 + revision); }

//// Singleton accessors.

    // Gets the global resources object or creates one if none exists.
    static wxXmlResource *Get();

    // Sets the global resources object and returns a pointer to the previous one (may be NULL).
    static wxXmlResource *Set(wxXmlResource *res);

    // Returns flags, which may be a bitlist of wxXRC_USE_LOCALE and wxXRC_NO_SUBCLASSING.
    int GetFlags() const { return m_flags; }
    // Set flags after construction.
    void SetFlags(int flags) { m_flags = flags; }

protected:
    // Scans the resources list for unloaded files and loads them. Also reloads
    // files that have been modified since last loading.
    void UpdateResources();

    // Finds a resource (calls UpdateResources) and returns a node containing it.
    wxXmlNode *FindResource(const wxString& name, const wxString& classname, bool recursive = FALSE);

    // Helper function: finds a resource (calls UpdateResources) and returns a node containing it.
    wxXmlNode *DoFindResource(wxXmlNode *parent, const wxString& name, const wxString& classname, bool recursive);

    // Creates a resource from information in the given node.
    wxObject *CreateResFromNode(wxXmlNode *node, wxObject *parent, wxObject *instance = NULL);

private:
    long m_version;

    int m_flags;
    wxList m_handlers;
    wxXmlResourceDataRecords m_data;
#if wxUSE_FILESYSTEM
    wxFileSystem m_curFileSystem;
    wxFileSystem& GetCurFileSystem() { return m_curFileSystem; }
#endif

    friend class wxXmlResourceHandler;
    friend class wxXmlResourceModule;

    static wxXmlSubclassFactoriesList *ms_subclassFactories;

    // singleton instance:
    static wxXmlResource *ms_instance;
};


// This macro translates string identifier (as used in XML resource,
// e.g. <menuitem id="my_menu">...</menuitem>) to integer id that is needed by
// wxWindows event tables.
// Example:
//    BEGIN_EVENT_TABLE(MyFrame, wxFrame)
//       EVT_MENU(XRCID("quit"), MyFrame::OnQuit)
//       EVT_MENU(XRCID("about"), MyFrame::OnAbout)
//       EVT_MENU(XRCID("new"), MyFrame::OnNew)
//       EVT_MENU(XRCID("open"), MyFrame::OnOpen)
//    END_EVENT_TABLE()

#define XRCID(str_id) \
    wxXmlResource::GetXRCID(wxT(str_id))


// This macro returns pointer to particular control in dialog
// created using XML resources. You can use it to set/get values from
// controls.
// Example:
//    wxDialog dlg;
//    wxXmlResource::Get()->LoadDialog(&dlg, mainFrame, "my_dialog");
//    XRCCTRL(dlg, "my_textctrl", wxTextCtrl)->SetValue(wxT("default value"));

#ifdef __WXDEBUG__
#define XRCCTRL(window, id, type) \
    (wxDynamicCast((window).FindWindow(XRCID(id)), type))
#else
#define XRCCTRL(window, id, type) \
    ((type*)((window).FindWindow(XRCID(id))))
#endif

// wxXmlResourceHandler is an abstract base class for resource handlers
// capable of creating a control from an XML node.

class WXXMLDLLEXPORT wxXmlResourceHandler : public wxObject
{
public:
    // Constructor.
    wxXmlResourceHandler();

    // Destructor.
    virtual ~wxXmlResourceHandler() {}

    // Creates an object (menu, dialog, control, ...) from an XML node.
    // Should check for validity.
    // parent is a higher-level object (usually window, dialog or panel)
    // that is often neccessary to create the resource.
    // If instance is non-NULL it should not create a new instance via 'new' but
    // should rather use this one, and call its Create method.
    wxObject *CreateResource(wxXmlNode *node, wxObject *parent,
                             wxObject *instance);

    // This one is called from CreateResource after variables
    // were filled.
    virtual wxObject *DoCreateResource() = 0;

    // Returns TRUE if it understands this node and can create
    // a resource from it, FALSE otherwise.
    virtual bool CanHandle(wxXmlNode *node) = 0;

    // Sets the parent resource.
    void SetParentResource(wxXmlResource *res) { m_resource = res; }

protected:
    wxXmlResource *m_resource;
    wxArrayString m_styleNames;
    wxArrayInt m_styleValues;

    // Variables (filled by CreateResource)
    wxXmlNode *m_node;
    wxString m_class;
    wxObject *m_parent, *m_instance;
    wxWindow *m_parentAsWindow, *m_instanceAsWindow;

    // --- Handy methods:

    // Returns true if the node has a property class equal to classname,
    // e.g. <object class="wxDialog">.
    bool IsOfClass(wxXmlNode *node, const wxString& classname)
        { return node->GetPropVal(wxT("class"), wxEmptyString) == classname; }

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
    int GetStyle(const wxString& param = wxT("style"), int defaults = 0);

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
    wxSize GetSize(const wxString& param = wxT("size"));

    // Gets the position (may be in dialog units).
    wxPoint GetPosition(const wxString& param = wxT("pos"));

    // Gets a dimension (may be in dialog units).
    wxCoord GetDimension(const wxString& param, wxCoord defaultv = 0);

    // Gets a bitmap.
    wxBitmap GetBitmap(const wxString& param = wxT("bitmap"),
                       const wxArtClient& defaultArtClient = wxART_OTHER,
                       wxSize size = wxDefaultSize);

    // Gets an icon.
    wxIcon GetIcon(const wxString& param = wxT("icon"),
                   const wxArtClient& defaultArtClient = wxART_OTHER,
                   wxSize size = wxDefaultSize);

    // Gets a font.
    wxFont GetFont(const wxString& param = wxT("font"));

    // Sets common window options.
    void SetupWindow(wxWindow *wnd);

    // Creates children.
    void CreateChildren(wxObject *parent, bool this_hnd_only = FALSE);

    // Helper function.
    void CreateChildrenPrivately(wxObject *parent, wxXmlNode *rootnode = NULL);

    // Creates a resource from a node.
    wxObject *CreateResFromNode(wxXmlNode *node,
                                wxObject *parent, wxObject *instance = NULL)
        { return m_resource->CreateResFromNode(node, parent, instance); }

    // helper
#if wxUSE_FILESYSTEM
    wxFileSystem& GetCurFileSystem() { return m_resource->GetCurFileSystem(); }
#endif
};


// Programmer-friendly macros for writing XRC handlers:

#define XRC_ADD_STYLE(style) AddStyle(wxT(#style), style)

#define XRC_MAKE_INSTANCE(variable, classname) \
   classname *variable = NULL; \
   if (m_instance) \
       variable = wxStaticCast(m_instance, classname); \
   if (!variable) \
       variable = new classname;


// FIXME -- remove this $%^#$%#$@# as soon as Ron checks his changes in!!
void wxXmlInitResourceModule();


// This class is used to create instances of XRC "object" nodes with "subclass"
// property. It is _not_ supposed to be used by XRC users, you should instead
// register your subclasses via wxWindows' RTTI mechanism. This class is useful
// only for language bindings developer who need a way to implement subclassing
// in wxWindows ports that don't support wxRTTI (e.g. wxPython).
class WXXMLDLLEXPORT wxXmlSubclassFactory
{
public:
    // Try to create instance of given class and return it, return NULL on failure:
    virtual wxObject *Create(const wxString& className) = 0;
    virtual ~wxXmlSubclassFactory() {}
};


/* -------------------------------------------------------------------------
   Backward compatibility macros. Do *NOT* use, they may disappear in future
   versions of the XRC library!
   ------------------------------------------------------------------------- */
#define ADD_STYLE         XRC_ADD_STYLE
#define wxTheXmlResource  wxXmlResource::Get()
#define XMLID             XRCID
#define XMLCTRL           XRCCTRL
#define GetXMLID          GetXRCID


#endif // _WX_XMLRES_H_
