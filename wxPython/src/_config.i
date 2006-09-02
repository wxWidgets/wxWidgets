/////////////////////////////////////////////////////////////////////////////
// Name:        _config.i
// Purpose:     SWIG interface for wxConfig, wxFileConfig, etc.
//
// Author:      Robin Dunn
//
// Created:     25-Nov-1998
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
%newgroup

%{
%}

//---------------------------------------------------------------------------


%{
    static PyObject* __EnumerationHelper(bool flag, wxString& str, long index) {
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        PyObject* ret = PyTuple_New(3);
        if (ret) {
            PyTuple_SET_ITEM(ret, 0, PyInt_FromLong(flag));
	    PyTuple_SET_ITEM(ret, 1, wx2PyString(str));
            PyTuple_SET_ITEM(ret, 2, PyInt_FromLong(index));
        }
        wxPyEndBlockThreads(blocked);
        return ret;
    }
%}


enum
{
    wxCONFIG_USE_LOCAL_FILE,
    wxCONFIG_USE_GLOBAL_FILE,
    wxCONFIG_USE_RELATIVE_PATH,
    wxCONFIG_USE_NO_ESCAPE_CHARACTERS
};



DocStr(wxConfigBase,
"wx.ConfigBase class defines the basic interface of all config
classes. It can not be used by itself (it is an abstract base class)
and you will always use one of its derivations: wx.Config or
wx.FileConfig.

wx.ConfigBase organizes the items in a tree-like structure, modeled
after the Unix/Dos filesystem. There are groups that act like
directories and entries, key/value pairs that act like files.  There
is always one current group given by the current path.  As in the file
system case, to specify a key in the config class you must use a path
to it.  Config classes also support the notion of the current group,
which makes it possible to use relative paths.

Keys are pairs \"key_name = value\" where value may be of string,
integer floating point or boolean, you can not store binary data
without first encoding it as a string.  For performance reasons items
should be kept small, no more than a couple kilobytes.
", "");


class wxConfigBase {
public:
//      wxConfigBase(const wxString& appName = wxPyEmptyString,       **** An ABC
//                   const wxString& vendorName = wxPyEmptyString,
//                   const wxString& localFilename = wxPyEmptyString,
//                   const wxString& globalFilename = wxPyEmptyString,
//                   long style = 0);
    ~wxConfigBase();

    enum EntryType
    {
        Type_Unknown,
        Type_String,
        Type_Boolean,
        Type_Integer,    // use Read(long *)
        Type_Float       // use Read(double *)
    };


    %disownarg( wxConfigBase *config );
    DocDeclStr(
        static wxConfigBase *, Set(wxConfigBase *config),
        "Sets the global config object (the one returned by Get) and returns a
reference to the previous global config object.", "");
    %cleardisown( wxConfigBase *config );

    DocDeclStr(
        static wxConfigBase *, Get(bool createOnDemand = true),
        "Returns the current global config object, creating one if neccessary.", "");
    

    DocDeclStr(
        static wxConfigBase *, Create(),
        "Create and return a new global config object.  This function will
create the \"best\" implementation of wx.Config available for the
current platform.", "");
    

    
    DocDeclStr(
        static void , DontCreateOnDemand(),
        "Should Get() try to create a new log object if there isn't a current
one?", "");
    



    DocDeclStr(
        virtual void , SetPath(const wxString& path),
        "Set current path: if the first character is '/', it's the absolute
path, otherwise it's a relative path. '..' is supported. If the
strPath doesn't exist it is created.", "");
    

    DocDeclStr(
        virtual const wxString& , GetPath() const,
        "Retrieve the current path (always as absolute path)", "");
    


    %extend {
        DocAStr(GetFirstGroup,
                "GetFirstGroup() -> (more, value, index)",                
                "Allows enumerating the subgroups in a config object.  Returns a tuple
containing a flag indicating there are more items, the name of the
current item, and an index to pass to GetNextGroup to fetch the next
item.", "");
        PyObject* GetFirstGroup() {
            bool     cont;
            long     index = 0;
            wxString value;

            cont = self->GetFirstGroup(value, index);
            return __EnumerationHelper(cont, value, index);
        }


        
        DocAStr(GetNextGroup,
                "GetNextGroup(long index) -> (more, value, index)",                
                "Allows enumerating the subgroups in a config object.  Returns a tuple
containing a flag indicating there are more items, the name of the
current item, and an index to pass to GetNextGroup to fetch the next
item.", "");
        PyObject* GetNextGroup(long index) {
            bool     cont;
            wxString value;

            cont = self->GetNextGroup(value, index);
            return __EnumerationHelper(cont, value, index);
        }

        
        DocAStr(GetFirstEntry,
                "GetFirstEntry() -> (more, value, index)",
                "Allows enumerating the entries in the current group in a config
object.  Returns a tuple containing a flag indicating there are more
items, the name of the current item, and an index to pass to
GetNextGroup to fetch the next item.", "");        
        PyObject* GetFirstEntry() {
            bool     cont;
            long     index = 0;
            wxString value;

            cont = self->GetFirstEntry(value, index);
            return __EnumerationHelper(cont, value, index);
        }


        DocAStr(GetNextEntry,
                "GetNextEntry(long index) -> (more, value, index)",
                "Allows enumerating the entries in the current group in a config
object.  Returns a tuple containing a flag indicating there are more
items, the name of the current item, and an index to pass to
GetNextGroup to fetch the next item.", "");        
        PyObject* GetNextEntry(long index) {
            bool     cont;
            wxString value;

            cont = self->GetNextEntry(value, index);
            return __EnumerationHelper(cont, value, index);
        }
    }



    DocDeclStr(
        virtual size_t , GetNumberOfEntries(bool recursive = false) const,
        "Get the number of entries in the current group, with or without its
subgroups.", "");
    
    DocDeclStr(
        virtual size_t , GetNumberOfGroups(bool recursive = false) const,
        "Get the number of subgroups in the current group, with or without its
subgroups.", "");
    

    
    DocDeclStr(
        virtual bool , HasGroup(const wxString& name) const,
        "Returns True if the group by this name exists", "");
    

    DocDeclStr(
        virtual bool , HasEntry(const wxString& name) const,
        "Returns True if the entry by this name exists", "");
    

    DocDeclStr(
        bool , Exists(const wxString& name) const,
        "Returns True if either a group or an entry with a given name exists", "");
    

    // get the entry type
    DocDeclStr(
        virtual EntryType , GetEntryType(const wxString& name) const,
        "Get the type of the entry.  Returns one of the wx.Config.Type_XXX values.", "");
    


    DocDeclStr(
        wxString , Read(const wxString& key, const wxString& defaultVal = wxPyEmptyString),
        "Returns the value of key if it exists, defaultVal otherwise.", "");
    

    %extend {
        DocStr(ReadInt,
               "Returns the value of key if it exists, defaultVal otherwise.", "");
        long ReadInt(const wxString& key, long defaultVal = 0) {
            long rv;
            self->Read(key, &rv, defaultVal);
            return rv;
        }

        DocStr(ReadFloat,
               "Returns the value of key if it exists, defaultVal otherwise.", "");
        double ReadFloat(const wxString& key, double defaultVal = 0.0) {
            double rv;
            self->Read(key, &rv, defaultVal);
            return rv;
        }
        
        DocStr(ReadBool,
               "Returns the value of key if it exists, defaultVal otherwise.", "");
        bool ReadBool(const wxString& key, bool defaultVal = false) {
            bool rv;
            self->Read(key, &rv, defaultVal);
            return rv;
        }
    }


    // write the value (return True on success)
    DocDeclStr(
        bool , Write(const wxString& key, const wxString& value),
        "write the value (return True on success)", "");

    DocDeclStrName(
        bool, Write(const wxString& key, long value),
        "write the value (return True on success)", "",
        WriteInt);

    DocDeclStrName(
        bool, Write(const wxString& key, double value),
        "write the value (return True on success)", "",
        WriteFloat);

    DocDeclStrName(
        bool, Write(const wxString& key, bool value),
        "write the value (return True on success)", "",
        WriteBool);


    DocDeclStr(
        virtual bool , Flush(bool currentOnly = false),
        "permanently writes all changes", "");
    

    DocDeclStr(
        virtual bool , RenameEntry(const wxString& oldName,
                                   const wxString& newName),
        "Rename an entry.  Returns False on failure (probably because the new
name is already taken by an existing entry)", "");
    
    DocDeclStr(
        virtual bool , RenameGroup(const wxString& oldName,
                                   const wxString& newName),
        "Rename a group.  Returns False on failure (probably because the new
name is already taken by an existing entry)", "");
    

    // deletes the specified entry and the group it belongs to if
    // it was the last key in it and the second parameter is True
    DocDeclStr(
        virtual bool , DeleteEntry(const wxString& key,
                                   bool deleteGroupIfEmpty = true),
        "Deletes the specified entry and the group it belongs to if it was the
last key in it and the second parameter is True", "");
    

    DocDeclStr(
        virtual bool , DeleteGroup(const wxString& key),
        "Delete the group (with all subgroups)", "");
    

    DocDeclStr(
        virtual bool , DeleteAll(),
        "Delete the whole underlying object (disk file, registry key, ...)
primarly intended for use by deinstallation routine.", "");
    


    DocDeclStr(
        void , SetExpandEnvVars(bool doIt = true),
        "We can automatically expand environment variables in the config
entries this option is on by default, you can turn it on/off at any
time)", "");
    
    DocDeclStr(
        bool , IsExpandingEnvVars() const,
        "Are we currently expanding environment variables?", "");
    

    DocDeclStr(
        void , SetRecordDefaults(bool doIt = true),
        "Set whether the config objec should record default values.", "");
    
    DocDeclStr(
        bool , IsRecordingDefaults() const,
        "Are we currently recording default values?", "");
    

    DocDeclStr(
        wxString , ExpandEnvVars(const wxString& str) const,
        "Expand any environment variables in str and return the result", "");
    

    DocDeclStr(
        wxString , GetAppName() const,
        "", "");
    
    DocDeclStr(
        wxString , GetVendorName() const,
        "", "");
    

    DocDeclStr(
        void , SetAppName(const wxString& appName),
        "", "");
    
    DocDeclStr(
        void , SetVendorName(const wxString& vendorName),
        "", "");
    

    DocDeclStr(
        void , SetStyle(long style),
        "", "");
    
    DocDeclStr(
        long , GetStyle() const,
        "", "");
    
    %property(AppName, GetAppName, SetAppName, doc="See `GetAppName` and `SetAppName`");
    %property(EntryType, GetEntryType, doc="See `GetEntryType`");
    %property(FirstEntry, GetFirstEntry, doc="See `GetFirstEntry`");
    %property(FirstGroup, GetFirstGroup, doc="See `GetFirstGroup`");
    %property(NextEntry, GetNextEntry, doc="See `GetNextEntry`");
    %property(NextGroup, GetNextGroup, doc="See `GetNextGroup`");
    %property(NumberOfEntries, GetNumberOfEntries, doc="See `GetNumberOfEntries`");
    %property(NumberOfGroups, GetNumberOfGroups, doc="See `GetNumberOfGroups`");
    %property(Path, GetPath, SetPath, doc="See `GetPath` and `SetPath`");
    %property(Style, GetStyle, SetStyle, doc="See `GetStyle` and `SetStyle`");
    %property(VendorName, GetVendorName, SetVendorName, doc="See `GetVendorName` and `SetVendorName`");
};


//---------------------------------------------------------------------------

DocStr(wxConfig,
"This ConfigBase-derived class will use the registry on Windows,
and will be a wx.FileConfig on other platforms.", "");

class wxConfig : public wxConfigBase {
public:
    DocCtorStr(
        wxConfig(const wxString& appName = wxPyEmptyString,
                 const wxString& vendorName = wxPyEmptyString,
                 const wxString& localFilename = wxPyEmptyString,
                 const wxString& globalFilename = wxPyEmptyString,
                 long style = wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_GLOBAL_FILE),
        "", "");
    
    ~wxConfig();
};




DocStr(wxFileConfig,
       "This config class will use a file for storage on all platforms.", "");

class wxFileConfig : public wxConfigBase {
public:
    DocCtorStr(
        wxFileConfig(const wxString& appName = wxPyEmptyString,
                     const wxString& vendorName = wxPyEmptyString,
                     const wxString& localFilename = wxPyEmptyString,
                     const wxString& globalFilename = wxPyEmptyString,
                     long style = wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_GLOBAL_FILE),
        "", "");
    
    ~wxFileConfig();
};


//---------------------------------------------------------------------------

DocStr(wxConfigPathChanger,
"A handy little class which changes current path to the path of given
entry and restores it in the destructoir: so if you declare a local
variable of this type, you work in the entry directory and the path is
automatically restored when the function returns.", "");

class wxConfigPathChanger
{
public:
    DocCtorStr(
        wxConfigPathChanger(const wxConfigBase *config, const wxString& entry),
        "", "");
    
    ~wxConfigPathChanger();

    DocDeclStr(
        const wxString& , Name() const,
        "Get the key name", "");   
};


//---------------------------------------------------------------------------



DocDeclStr(
    wxString , wxExpandEnvVars(const wxString &sz),
    "Replace environment variables ($SOMETHING) with their values. The
format is $VARNAME or ${VARNAME} where VARNAME contains alphanumeric
characters and '_' only. '$' must be escaped ('\$') in order to be
taken literally.", "");



//---------------------------------------------------------------------------
