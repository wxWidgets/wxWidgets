/////////////////////////////////////////////////////////////////////////////
// Name:        msw/registry.h
// Purpose:     interface of wxRegKey
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxRegKey
    @headerfile registry.h wx/msw/registry.h

    wxRegKey is a class representing the Windows registry (it is only available
    under Windows). One can create, query and delete registry keys using this
    class.

    The Windows registry is easy to understand. There are five registry keys,
    namely:

     HKEY_CLASSES_ROOT (HKCR)
     HKEY_CURRENT_USER (HKCU)
     HKEY_LOCAL_MACHINE (HKLM)
     HKEY_CURRENT_CONFIG (HKCC)
     HKEY_USERS (HKU)

    After creating a key, it can hold a value. The values can be:

     String Value
     Binary Value
     DWORD Value
     Multi String Value
     Expandable String Value


    @library{wxbase}
    @category{FIXME}
*/
class wxRegKey
{
public:
    //@{
    /**
        The constructor to set the full name of the key under a previously created
        parent.
    */
    wxRegKey();
    wxRegKey(const wxString& strKey);
    wxRegKey(const wxRegKey& keyParent, const wxString& strKey);
    //@}

    /**
        Closes the key.
    */
    void Close();

    /**
        Creates the key. Will fail if the key already exists and @a bOkIfExists is
        @false.
    */
    bool Create(bool bOkIfExists = true);

    /**
        Deletes the subkey with all of its subkeys/values recursively.
    */
    void DeleteKey(const wxChar* szKey);

    /**
        Deletes this key and all of its subkeys and values recursively.
    */
    void DeleteSelf();

    /**
        Deletes the named value.
    */
    void DeleteValue(const wxChar* szKey);

    /**
        Returns @true if the key exists.
    */
    static bool Exists() const;

    /**
        Gets the first key.
    */
    bool GetFirstKey(wxString& strKeyName, long& lIndex);

    /**
        Gets the first value of this key.
    */
    bool GetFirstValue(wxString& strValueName, long& lIndex);

    /**
        Gets information about the key.
        
        @param pnSubKeys
            The number of subkeys.
        @param pnMaxKeyLen
            The maximum length of the subkey name.
        @param pnValues
            The number of values.
        @param pnMaxValueLen
            The maximum length of a value.
    */
    bool GetKeyInfo(size_t* pnSubKeys, size_t* pnValues,
                    size_t* pnMaxValueLen) const;

    /**
        Gets the name of the registry key.
    */
    wxString GetName(bool bShortPrefix = true) const;

    /**
        Gets the next key.
    */
    bool GetNextKey(wxString& strKeyName, long& lIndex) const;

    /**
        Gets the next key value for this key.
    */
    bool GetNextValue(wxString& strValueName, long& lIndex) const;

    /**
        Returns @true if given subkey exists.
    */
    bool HasSubKey(const wxChar* szKey) const;

    /**
        Returns @true if any subkeys exist.
    */
    bool HasSubKeys() const;

    /**
        Returns @true if the value exists.
    */
    bool HasValue(const wxChar* szValue) const;

    /**
        Returns @true if any values exist.
    */
    bool HasValues() const;

    /**
        Returns @true if this key is empty, nothing under this key.
    */
    bool IsEmpty() const;

    /**
        Returns @true if the key is opened.
    */
    bool IsOpened() const;

    /**
        Explicitly opens the key. This method also allows the key to be opened in
        read-only mode by passing @c Read() instead of default
        @c Write() parameter.
    */
    bool Open(AccessMode mode = Write);

    //@{
    /**
        Retrieves the numeric value.
    */
    bool QueryValue(const wxChar* szValue, wxString& strValue) const;
    const bool QueryValue(const wxChar* szValue, long* plValue) const;
    //@}

    /**
        Renames the key.
    */
    bool Rename(const wxChar* szNewName);

    /**
        Renames a value.
    */
    bool RenameValue(const wxChar* szValueOld,
                     const wxChar* szValueNew);

    //@{
    /**
        Sets the given @a szValue which must be numeric, string or binary depending
        on the overload used. If the value doesn't exist, it is created.
    */
    bool SetValue(const wxChar* szValue, long lValue);
    bool SetValue(const wxChar* szValue,
                  const wxString& strValue);
    bool SetValue(const wxChar* szValue,
                  const wxMemoryBuffer& buf);
    //@}
};

