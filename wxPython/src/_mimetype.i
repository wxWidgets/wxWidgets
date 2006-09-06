/////////////////////////////////////////////////////////////////////////////
// Name:        _joystick.i
// Purpose:     SWIG interface stuff for wxJoystick
//
// Author:      Robin Dunn
//
// Created:     18-June-1999
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
%newgroup

%{
#include <wx/mimetype.h>
%}    

//---------------------------------------------------------------------------

enum wxMailcapStyle
{
    wxMAILCAP_STANDARD = 1,
    wxMAILCAP_NETSCAPE = 2,
    wxMAILCAP_KDE = 4,
    wxMAILCAP_GNOME = 8,

    wxMAILCAP_ALL = 15
};




// wxFileTypeInfo: static container of information accessed via wxFileType.
class wxFileTypeInfo
{
public:
    // a normal item
    wxFileTypeInfo(const wxString& mimeType,
                   const wxString& openCmd,
                   const wxString& printCmd,
                   const wxString& desc);


    // the array elements correspond to the parameters of the ctor above in
    // the same order
    %Rename(FileTypeInfoSequence,, wxFileTypeInfo(const wxArrayString& sArray));

    // invalid item - use this to terminate the array passed to
    // wxMimeTypesManager::AddFallbacks
    %Rename(NullFileTypeInfo,, wxFileTypeInfo());


    // test if this object can be used
    bool IsValid() const;

    // set the icon info
    void SetIcon(const wxString& iconFile, int iconIndex = 0);

    // set the short desc
    void SetShortDesc(const wxString& shortDesc);

    // get the MIME type
    const wxString& GetMimeType() const;

    // get the open command
    const wxString& GetOpenCommand() const;

    // get the print command
    const wxString& GetPrintCommand() const;

    // get the short description (only used under Win32 so far)
    const wxString& GetShortDesc() const;

    // get the long, user visible description
    const wxString& GetDescription() const;

    // get the array of all extensions
    const wxArrayString& GetExtensions() const;
    size_t GetExtensionsCount() const;

    // get the icon info
    const wxString& GetIconFile() const;
    int GetIconIndex() const;
    
    %property(Description, GetDescription, doc="See `GetDescription`");
    %property(Extensions, GetExtensions, doc="See `GetExtensions`");
    %property(ExtensionsCount, GetExtensionsCount, doc="See `GetExtensionsCount`");
    %property(IconFile, GetIconFile, doc="See `GetIconFile`");
    %property(IconIndex, GetIconIndex, doc="See `GetIconIndex`");
    %property(MimeType, GetMimeType, doc="See `GetMimeType`");
    %property(OpenCommand, GetOpenCommand, doc="See `GetOpenCommand`");
    %property(PrintCommand, GetPrintCommand, doc="See `GetPrintCommand`");
    %property(ShortDesc, GetShortDesc, SetShortDesc, doc="See `GetShortDesc` and `SetShortDesc`");
};



//---------------------------------------------------------------------------

// wxFileType: gives access to all information about the files of given type.
//
// This class holds information about a given "file type". File type is the
// same as MIME type under Unix, but under Windows it corresponds more to an
// extension than to MIME type (in fact, several extensions may correspond to a
// file type). This object may be created in many different ways and depending
// on how it was created some fields may be unknown so the return value of all
// the accessors *must* be checked!
class wxFileType
{
public:

//     // TODO: Make a wxPyMessageParameters with virtual GetParamValue...

//     // An object of this class must be passed to Get{Open|Print}Command. The
//     // default implementation is trivial and doesn't know anything at all about
//     // parameters, only filename and MIME type are used (so it's probably ok for
//     // Windows where %{param} is not used anyhow)
//     class MessageParameters
//     {
//     public:
//         // ctors
//         MessageParameters(const wxString& filename=wxPyEmptyString,
//                           const wxString& mimetype=wxPyEmptyString);

//         // accessors (called by GetOpenCommand)
//             // filename
//         const wxString& GetFileName() const;
//             // mime type
//         const wxString& GetMimeType() const;;

//         // override this function in derived class
//         virtual wxString GetParamValue(const wxString& name) const;

//         // virtual dtor as in any base class
//         virtual ~MessageParameters();
//     };


    // ctor from static data
    wxFileType(const wxFileTypeInfo& ftInfo);
    ~wxFileType();

    
    // return the MIME type for this file type
    %extend {
        PyObject* GetMimeType() {
            wxString str;
            if (self->GetMimeType(&str)) 
                return wx2PyString(str);
            else
                RETURN_NONE();
        }
        
        PyObject* GetMimeTypes() {
            wxArrayString arr;
            if (self->GetMimeTypes(arr))
                return wxArrayString2PyList_helper(arr);
            else
                RETURN_NONE();
        }
    }


    // Get all extensions associated with this file type
    %extend {
        PyObject* GetExtensions() {
            wxArrayString arr;
            if (self->GetExtensions(arr))
                return wxArrayString2PyList_helper(arr);
            else
                RETURN_NONE();
        }
    }


    %extend {
        // Get the icon corresponding to this file type
        %newobject GetIcon;
        wxIcon* GetIcon() {
            wxIconLocation loc;
            if (self->GetIcon(&loc))
                return new wxIcon(loc);
            else
                return NULL;
        }

        // Get the icon corresponding to this file type, the name of the file
        // where this icon resides, and its index in this file if applicable.
        PyObject* GetIconInfo() {
            wxIconLocation loc;
            if (self->GetIcon(&loc)) {
                wxString iconFile = loc.GetFileName();
                int iconIndex     = -1;
#ifdef __WXMSW__
                iconIndex = loc.GetIndex();
#endif
                // Make a tuple and put the values in it
                wxPyBlock_t blocked = wxPyBeginBlockThreads();
                PyObject* tuple = PyTuple_New(3);
                PyTuple_SetItem(tuple, 0, wxPyConstructObject(new wxIcon(loc),
                                                              wxT("wxIcon"), true));
                PyTuple_SetItem(tuple, 1, wx2PyString(iconFile));
                PyTuple_SetItem(tuple, 2, PyInt_FromLong(iconIndex));
                wxPyEndBlockThreads(blocked);
                return tuple;
            }
            else
                RETURN_NONE();
        }
    }

    %extend {
        // get a brief file type description ("*.txt" => "text document")
        PyObject* GetDescription() {
            wxString str;
            if (self->GetDescription(&str)) 
                return wx2PyString(str);
            else
                RETURN_NONE();
        }
    }


    // get the command to open/execute the file of given type
    %extend {
        PyObject* GetOpenCommand(const wxString& filename,
                                 const wxString& mimetype=wxPyEmptyString) {
            wxString str;
            if (self->GetOpenCommand(&str, wxFileType::MessageParameters(filename, mimetype))) 
                return wx2PyString(str);
            else
                RETURN_NONE();
        }
    }


    // get the command to print the file of given type
    %extend {
        PyObject* GetPrintCommand(const wxString& filename,
                                  const wxString& mimetype=wxPyEmptyString) {
            wxString str;
            if (self->GetPrintCommand(&str, wxFileType::MessageParameters(filename, mimetype))) 
                return wx2PyString(str);
             else
                RETURN_NONE();
        }
    }


    // Get all commands defined for this file type
    %extend {
        PyObject* GetAllCommands(const wxString& filename,
                                 const wxString& mimetype=wxPyEmptyString) {
            wxArrayString verbs;
            wxArrayString commands;
            if (self->GetAllCommands(&verbs, &commands,
                                     wxFileType::MessageParameters(filename, mimetype))) {
                wxPyBlock_t blocked = wxPyBeginBlockThreads();
                PyObject* tuple = PyTuple_New(2);
                PyTuple_SetItem(tuple, 0, wxArrayString2PyList_helper(verbs));
                PyTuple_SetItem(tuple, 1, wxArrayString2PyList_helper(commands));
                wxPyEndBlockThreads(blocked);
                return tuple;
            }
            else
                RETURN_NONE();
        }
    }


    // set an arbitrary command, ask confirmation if it already exists and
    // overwriteprompt is True
    bool SetCommand(const wxString& cmd, const wxString& verb,
                    bool overwriteprompt = true);

    bool SetDefaultIcon(const wxString& cmd = wxPyEmptyString, int index = 0);


    // remove the association for this filetype from the system MIME database:
    // notice that it will only work if the association is defined in the user
    // file/registry part, we will never modify the system-wide settings
    bool Unassociate();

    // operations
        // expand a string in the format of GetOpenCommand (which may contain
        // '%s' and '%t' format specificators for the file name and mime type
        // and %{param} constructions).
    %extend {
        static wxString ExpandCommand(const wxString& command,
                                      const wxString& filename,
                                      const wxString& mimetype=wxPyEmptyString) {
            return wxFileType::ExpandCommand(command,
                                             wxFileType::MessageParameters(filename, mimetype));
        }
    }

    %property(AllCommands, GetAllCommands, doc="See `GetAllCommands`");
    %property(Description, GetDescription, doc="See `GetDescription`");
    %property(Extensions, GetExtensions, doc="See `GetExtensions`");
    %property(Icon, GetIcon, doc="See `GetIcon`");
    %property(IconInfo, GetIconInfo, doc="See `GetIconInfo`");
    %property(MimeType, GetMimeType, doc="See `GetMimeType`");
    %property(MimeTypes, GetMimeTypes, doc="See `GetMimeTypes`");
    %property(OpenCommand, GetOpenCommand, doc="See `GetOpenCommand`");
    %property(PrintCommand, GetPrintCommand, doc="See `GetPrintCommand`");
};



//---------------------------------------------------------------------------

wxMimeTypesManager* const wxTheMimeTypesManager;


// wxMimeTypesManager: interface to system MIME database.
//
// This class accesses the information about all known MIME types and allows
// the application to retrieve information (including how to handle data of
// given type) about them.
class wxMimeTypesManager
{
public:
    // static helper functions
    // -----------------------

        // check if the given MIME type is the same as the other one: the
        // second argument may contain wildcards ('*'), but not the first. If
        // the types are equal or if the mimeType matches wildcard the function
        // returns True, otherwise it returns False
    static bool IsOfType(const wxString& mimeType, const wxString& wildcard);

    // ctor
    wxMimeTypesManager();

    // loads data from standard files according to the mailcap styles
    // specified: this is a bitwise OR of wxMailcapStyle values
    //
    // use the extraDir parameter if you want to look for files in another
    // directory
    void Initialize(int mailcapStyle = wxMAILCAP_ALL,
                    const wxString& extraDir = wxPyEmptyString);

    // and this function clears all the data from the manager
    void ClearData();

    // Database lookup: all functions return a pointer to wxFileType object
    // whose methods may be used to query it for the information you're
    // interested in. If the return value is !NULL, caller is responsible for
    // deleting it.
    // get file type from file extension
    %newobject GetFileTypeFromExtension;
    wxFileType *GetFileTypeFromExtension(const wxString& ext);

    // get file type from MIME type (in format <category>/<format>)
    %newobject GetFileTypeFromMimeType;
    wxFileType *GetFileTypeFromMimeType(const wxString& mimeType);

    // other operations: return True if there were no errors or False if there
    // were some unrecognized entries (the good entries are always read anyhow)
    //

    // read in additional file (the standard ones are read automatically)
    // in mailcap format (see mimetype.cpp for description)
    //
    // 'fallback' parameter may be set to True to avoid overriding the
    // settings from other, previously parsed, files by this one: normally,
    // the files read most recently would override the older files, but with
    // fallback == True this won't happen
    bool ReadMailcap(const wxString& filename, bool fallback = false);

    // read in additional file in mime.types format
    bool ReadMimeTypes(const wxString& filename);

    // enumerate all known MIME types
    %extend {
        PyObject* EnumAllFileTypes() {
            wxArrayString arr;
            self->EnumAllFileTypes(arr);
            return wxArrayString2PyList_helper(arr);
        }
    }

    // these functions can be used to provide default values for some of the
    // MIME types inside the program itself (you may also use
    // ReadMailcap(filenameWithDefaultTypes, True /* use as fallback */) to
    // achieve the same goal, but this requires having this info in a file).
    //
    void AddFallback(const wxFileTypeInfo& ft);


    // create or remove associations

    // create a new association using the fields of wxFileTypeInfo (at least
    // the MIME type and the extension should be set)
    // if the other fields are empty, the existing values should be left alone
    %newobject Associate;
    wxFileType *Associate(const wxFileTypeInfo& ftInfo);

        // undo Associate()
    bool Unassociate(wxFileType *ft) ;

    ~wxMimeTypesManager();
};

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
