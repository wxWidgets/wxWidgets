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
        PyObject* ret = PyTuple_New(3);
        if (ret) {
            PyTuple_SET_ITEM(ret, 0, PyInt_FromLong(flag));
	    PyTuple_SET_ITEM(ret, 1, wx2PyString(str));
            PyTuple_SET_ITEM(ret, 2, PyInt_FromLong(index));
        }
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



// abstract base class wxConfigBase which defines the interface for derived
// classes
//
// wxConfig organizes the items in a tree-like structure (modeled after the
// Unix/Dos filesystem). There are groups (directories) and keys (files).
// There is always one current group given by the current path.
//
// Keys are pairs "key_name = value" where value may be of string or integer
// (long) type (TODO doubles and other types such as wxDate coming soon).
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


    // sets the config object, returns the previous pointer
    static wxConfigBase *Set(wxConfigBase *pConfig);

    // get the config object, creates it on demand unless DontCreateOnDemand
    // was called
    static wxConfigBase *Get(bool createOnDemand = TRUE);

    // create a new config object: this function will create the "best"
    // implementation of wxConfig available for the current platform, see
    // comments near definition wxUSE_CONFIG_NATIVE for details. It returns
    // the created object and also sets it as ms_pConfig.
    static wxConfigBase *Create();

    // should Get() try to create a new log object if the current one is NULL?
    static void DontCreateOnDemand();



    // set current path: if the first character is '/', it's the absolute path,
    // otherwise it's a relative path. '..' is supported. If the strPath
    // doesn't exist it is created.
    virtual void SetPath(const wxString& strPath);

    // retrieve the current path (always as absolute path)
    virtual const wxString& GetPath() const;




    // Each of these enumeration methods return a 3-tuple consisting of
    // the continue flag, the value string, and the index for the next call.
    %extend {
        // enumerate subgroups
        PyObject* GetFirstGroup() {
            bool     cont;
            long     index = 0;
            wxString value;

            cont = self->GetFirstGroup(value, index);
            return __EnumerationHelper(cont, value, index);
        }
        PyObject* GetNextGroup(long index) {
            bool     cont;
            wxString value;

            cont = self->GetNextGroup(value, index);
            return __EnumerationHelper(cont, value, index);
        }

        // enumerate entries
        PyObject* GetFirstEntry() {
            bool     cont;
            long     index = 0;
            wxString value;

            cont = self->GetFirstEntry(value, index);
            return __EnumerationHelper(cont, value, index);
        }
        PyObject* GetNextEntry(long index) {
            bool     cont;
            wxString value;

            cont = self->GetNextEntry(value, index);
            return __EnumerationHelper(cont, value, index);
        }
    }



    // get number of entries/subgroups in the current group, with or without
    // it's subgroups
    virtual size_t GetNumberOfEntries(bool bRecursive = FALSE) const;
    virtual size_t GetNumberOfGroups(bool bRecursive = FALSE) const;

    // returns TRUE if the group by this name exists
    virtual bool HasGroup(const wxString& strName) const;

    // same as above, but for an entry
    virtual bool HasEntry(const wxString& strName) const;

    // returns TRUE if either a group or an entry with a given name exist
    bool Exists(const wxString& strName) const;

    // get the entry type
    virtual EntryType GetEntryType(const wxString& name) const;


    // Key access.  Returns the value of key if it exists, defaultVal otherwise
    wxString Read(const wxString& key, const wxString& defaultVal = wxPyEmptyString);

    %extend {
        long ReadInt(const wxString& key, long defaultVal = 0) {
            long rv;
            self->Read(key, &rv, defaultVal);
            return rv;
        }
        double ReadFloat(const wxString& key, double defaultVal = 0.0) {
            double rv;
            self->Read(key, &rv, defaultVal);
            return rv;
        }
        bool ReadBool(const wxString& key, bool defaultVal = FALSE) {
            bool rv;
            self->Read(key, &rv, defaultVal);
            return rv;
        }
    }


    // write the value (return true on success)
    bool Write(const wxString& key, const wxString& value);
    %name(WriteInt)bool Write(const wxString& key, long value);
    %name(WriteFloat)bool Write(const wxString& key, double value);
    %name(WriteBool)bool Write(const wxString& key, bool value);


    // permanently writes all changes
    virtual bool Flush(bool bCurrentOnly = FALSE);

    // renaming, all functions return FALSE on failure (probably because the new
    // name is already taken by an existing entry)
    // rename an entry
    virtual bool RenameEntry(const wxString& oldName,
                             const wxString& newName);
    // rename a group
    virtual bool RenameGroup(const wxString& oldName,
                             const wxString& newName);

    // deletes the specified entry and the group it belongs to if
    // it was the last key in it and the second parameter is true
    virtual bool DeleteEntry(const wxString& key,
                             bool bDeleteGroupIfEmpty = TRUE);

    // delete the group (with all subgroups)
    virtual bool DeleteGroup(const wxString& key);

    // delete the whole underlying object (disk file, registry key, ...)
    // primarly for use by desinstallation routine.
    virtual bool DeleteAll();


    // we can automatically expand environment variables in the config entries
    // (this option is on by default, you can turn it on/off at any time)
    bool IsExpandingEnvVars() const;
    void SetExpandEnvVars(bool bDoIt = TRUE);

    // recording of default values
    void SetRecordDefaults(bool bDoIt = TRUE);
    bool IsRecordingDefaults() const;

    // does expansion only if needed
    wxString ExpandEnvVars(const wxString& str) const;

    // misc accessors
    wxString GetAppName() const;
    wxString GetVendorName() const;

    // Used wxIniConfig to set members in constructor
    void SetAppName(const wxString& appName);
    void SetVendorName(const wxString& vendorName);

    void SetStyle(long style);
    long GetStyle() const;
};


//---------------------------------------------------------------------------

// a handy little class which changes current path to the path of given entry
// and restores it in dtor: so if you declare a local variable of this type,
// you work in the entry directory and the path is automatically restored
// when the function returns
// Taken out of wxConfig since not all compilers can cope with nested classes.
class wxConfigPathChanger
{
public:
  // ctor/dtor do path changing/restorin
  wxConfigPathChanger(const wxConfigBase *pContainer, const wxString& strEntry);
 ~wxConfigPathChanger();

  // get the key name
  const wxString& Name() const { return m_strName; }
};


//---------------------------------------------------------------------------

// This will be a wxRegConfig on Win32 and wxFileConfig otherwise.
class wxConfig : public wxConfigBase {
public:
    wxConfig(const wxString& appName = wxPyEmptyString,
             const wxString& vendorName = wxPyEmptyString,
             const wxString& localFilename = wxPyEmptyString,
             const wxString& globalFilename = wxPyEmptyString,
             long style = 0);
    ~wxConfig();
};


// Sometimes it's nice to explicitly have a wxFileConfig too.
class wxFileConfig : public wxConfigBase {
public:
    wxFileConfig(const wxString& appName = wxPyEmptyString,
                 const wxString& vendorName = wxPyEmptyString,
                 const wxString& localFilename = wxPyEmptyString,
                 const wxString& globalFilename = wxPyEmptyString,
                 long style = 0);
    ~wxFileConfig();
};


//---------------------------------------------------------------------------


//  Replace environment variables ($SOMETHING) with their values. The format is
//  $VARNAME or ${VARNAME} where VARNAME contains alphanumeric characters and
//  '_' only. '$' must be escaped ('\$') in order to be taken literally.
wxString wxExpandEnvVars(const wxString &sz);


//---------------------------------------------------------------------------
