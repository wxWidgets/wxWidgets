/////////////////////////////////////////////////////////////////////////////
// Name:        _xmlres.i
// Purpose:     SWIG interface for wxXmlResource
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



class wxPyXmlSubclassFactory;

enum {
    WX_XMLRES_CURRENT_VERSION_MAJOR,
    WX_XMLRES_CURRENT_VERSION_MINOR,
    WX_XMLRES_CURRENT_VERSION_RELEASE,
    WX_XMLRES_CURRENT_VERSION_REVISION,
};


enum wxXmlResourceFlags
{
    wxXRC_USE_LOCALE,
    wxXRC_NO_SUBCLASSING,
    wxXRC_NO_RELOADING
};



// This class holds XML resources from one or more .xml files
// (or derived forms, either binary or zipped -- see manual for
// details).

class wxXmlResource : public wxObject
{
public:

    %pythonAppend wxXmlResource(const wxString& filemask, int flags,
                  const wxString& domain=wxEmptyString)   "self.InitAllHandlers()"
    %pythonAppend wxXmlResource(int flags,
                  const wxString& domain=wxEmptyString)   "val.InitAllHandlers()"

    // Ctors.
    // Flags: wxXRC_USE_LOCALE
    //              translatable strings will be translated via _()
    //        wxXRC_NO_SUBCLASSING
    //              subclass property of object nodes will be ignored
    //              (useful for previews in XRC editors)
    wxXmlResource(const wxString& filemask, int flags = wxXRC_USE_LOCALE,
                  const wxString& domain=wxEmptyString);
    %RenameCtor(EmptyXmlResource,  wxXmlResource(int flags = wxXRC_USE_LOCALE,
                  const wxString& domain=wxEmptyString));
    
    ~wxXmlResource();


    // Loads resources from XML files that match given filemask.
    // This method understands VFS (see filesys.h).
    bool Load(const wxString& filemask);

    %extend {
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

    // Unload resource from the given XML file (wildcards not allowed)
    bool Unload(const wxString& filename);
    
    // Initialize handlers for all supported controls/windows. 
    void InitAllHandlers();


    %disownarg( wxPyXmlResourceHandler *handler );
    
    // Initialize only specific handler (or custom handler). Convention says
    // that handler name is equal to control's name plus 'XmlHandler', e.g.
    // wxTextCtrlXmlHandler, wxHtmlWindowXmlHandler. XML resource compiler
    // (xmlres) can create include file that contains initialization code for
    // all controls used within the resource.
    void AddHandler(wxPyXmlResourceHandler *handler);

    // Add a new handler at the begining of the handler list
    void InsertHandler(wxPyXmlResourceHandler *handler);

    %cleardisown( wxPyXmlResourceHandler *handler );

    
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
    %Rename(LoadMenuBarOnFrame,  wxMenuBar* , LoadMenuBar(wxWindow *parent, const wxString& name));


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
    %Rename(LoadOnDialog, bool,  LoadDialog(wxDialog *dlg, wxWindow *parent, const wxString& name));

    // Loads panel. panel points to parent window (if any). Second form
    // is used to finish creation of already existing instance.
    wxPanel *LoadPanel(wxWindow *parent, const wxString& name);
    %Rename(LoadOnPanel, bool,  LoadPanel(wxPanel *panel, wxWindow *parent, const wxString& name));

    // Load a frame's contents from a resource
    wxFrame *LoadFrame(wxWindow* parent, const wxString& name);
    %Rename(LoadOnFrame, bool,  LoadFrame(wxFrame* frame, wxWindow *parent, const wxString& name));

    // Load an object from the resource specifying both the resource name and
    // the classname.  This lets you load nonstandard container windows.
    wxObject *LoadObject(wxWindow *parent, const wxString& name,
                         const wxString& classname);

    // Load an object from the resource specifying both the resource name and
    // the classname.  This form lets you finish the creation of an existing
    // instance.
    %Rename(LoadOnObject, bool,  LoadObject(wxObject *instance, wxWindow *parent, const wxString& name,
                                       const wxString& classname));

    // Loads a bitmap resource from a file.
    wxBitmap LoadBitmap(const wxString& name);

    // Loads an icon resource from a file.
    wxIcon LoadIcon(const wxString& name);

    // Attaches unknown control into given panel/window/dialog:
    // (unknown controls are used in conjunction with <object class="unknown">)
    bool AttachUnknownControl(const wxString& name, wxWindow *control,
                              wxWindow *parent = NULL);

    // Returns a numeric ID that is equivalent to the string ID used in an XML
    // resource. If an unknown str_id is requested (i.e. other than wxID_XXX
    // or integer), a new record is created which associates the given string
    // with a number. If value_if_not_found == wxID_NONE, the number is obtained via
    // wxNewId(). Otherwise value_if_not_found is used.
    // Macro XRCID(name) is provided for convenient use in event tables.
    static int GetXRCID(const wxString& str_id, int value_if_not_found = wxID_NONE);

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

    // Get/Set the domain to be passed to the translation functions, defaults to NULL.
    wxString GetDomain() const;
    void SetDomain(const wxString& domain);
};

//----------------------------------------------------------------------

%pythoncode {
def XRCID(str_id, value_if_not_found = wx.ID_NONE):
    return XmlResource_GetXRCID(str_id, value_if_not_found)

def XRCCTRL(window, str_id, *ignoreargs):
    return window.FindWindowById(XRCID(str_id))
};

//---------------------------------------------------------------------------
