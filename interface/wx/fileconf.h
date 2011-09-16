/////////////////////////////////////////////////////////////////////////////
// Name:        fileconf.h
// Purpose:     interface of wxFileConfig
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxFileConfig

    wxFileConfig implements wxConfigBase interface for
    storing and retrieving configuration information using plain text files. The
    files have a simple format reminiscent of Windows INI files with lines of the
    form @c "key = value" defining the keys and lines of special form
    @c "[group]" indicating the start of each group.

    This class is used by default for wxConfig on Unix platforms but may also be
    used explicitly if you want to use files and not the registry even under
    Windows.

    @library{wxbase}
    @category{cfg}

    @see wxFileConfig::Save
*/
class wxFileConfig : public wxConfigBase
{
public:

    // New constructor: one size fits all. Specify wxCONFIG_USE_LOCAL_FILE or
    // wxCONFIG_USE_GLOBAL_FILE to say which files should be used.
    wxFileConfig(const wxString& appName = wxEmptyString,
               const wxString& vendorName = wxEmptyString,
               const wxString& localFilename = wxEmptyString,
               const wxString& globalFilename = wxEmptyString,
               long style = wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_GLOBAL_FILE,
               const wxMBConv& conv = wxConvAuto());

    /**
        Read the config data from the specified stream instead of the associated file,
        as usual.

        @see Save()
    */
    wxFileConfig(wxInputStream& is, const wxMBConv& conv = wxConvAuto());

    /**
        Return the full path to the file which would be used by wxFileConfig as global,
        system-wide, file if it were constructed with @a basename as "global filename"
        parameter in the constructor.

        Notice that this function cannot be used if @a basename is already a full path name.
    */
    static wxFileName GetGlobalFile(const wxString& basename);

    /**
        Return the full path to the file which would be used by wxFileConfig as local,
        user-specific, file if it were constructed with @a basename as "local filename"
        parameter in the constructor.

        @a style has the same meaning as in @ref wxConfigBase::wxConfigBase "wxConfig constructor"
        and can contain any combination of styles but only wxCONFIG_USE_SUBDIR bit is
        examined by this function.

        Notice that this function cannot be used if @a basename is already a full path name.
    */
    static wxFileName GetLocalFile(const wxString& basename, int style = 0);

    static wxString GetGlobalFileName(const wxString& szFile);
    static wxString GetLocalFileName(const wxString& szFile, int style = 0);

    /**
        Saves all config data to the given stream, returns @true if data was saved
        successfully or @false on error.

        Note the interaction of this function with the internal "dirty flag": the
        data is saved unconditionally, i.e. even if the object is not dirty. However
        after saving it successfully, the dirty flag is reset so no changes will be
        written back to the file this object is associated with until you change its
        contents again.

        @see wxConfigBase::Flush
    */
    virtual bool Save(wxOutputStream& os, const wxMBConv& conv = wxConvAuto());

    /**
        Allows to set the mode to be used for the config file creation. For example, to
        create a config file which is not readable by other users (useful if it stores
        some sensitive information, such as passwords), you could use @c SetUmask(0077).

        This function doesn't do anything on non-Unix platforms.

        @see wxCHANGE_UMASK()
    */
    void SetUmask(int mode);
    
  // implement inherited pure virtual functions
  virtual void SetPath(const wxString& strPath);
  virtual const wxString& GetPath() const;

  virtual bool GetFirstGroup(wxString& str, long& lIndex) const;
  virtual bool GetNextGroup (wxString& str, long& lIndex) const;
  virtual bool GetFirstEntry(wxString& str, long& lIndex) const;
  virtual bool GetNextEntry (wxString& str, long& lIndex) const;

  virtual size_t GetNumberOfEntries(bool bRecursive = false) const;
  virtual size_t GetNumberOfGroups(bool bRecursive = false) const;

  virtual bool HasGroup(const wxString& strName) const;
  virtual bool HasEntry(const wxString& strName) const;

  virtual bool Flush(bool bCurrentOnly = false);

  virtual bool RenameEntry(const wxString& oldName, const wxString& newName);
  virtual bool RenameGroup(const wxString& oldName, const wxString& newName);

  virtual bool DeleteEntry(const wxString& key, bool bGroupIfEmptyAlso = true);
  virtual bool DeleteGroup(const wxString& szKey);
  virtual bool DeleteAll();
};

