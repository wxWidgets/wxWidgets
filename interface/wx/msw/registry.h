/////////////////////////////////////////////////////////////////////////////
// Name:        msw/registry.h
// Purpose:     interface of wxRegKey
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxRegKey

    wxRegKey is a class representing the Windows registry (it is only available
    under Windows). One can create, query and delete registry keys using this
    class.

    The Windows registry is easy to understand. There are five registry keys,
    namely:

    @li @c HKEY_CLASSES_ROOT (HKCR)
    @li @c HKEY_CURRENT_USER (HKCU)
    @li @c HKEY_LOCAL_MACHINE (HKLM)
    @li @c HKEY_CURRENT_CONFIG (HKCC)
    @li @c HKEY_USERS (HKU)

    After creating a key, it can hold a value. The values can be:

    @li String Value
    @li Binary Value
    @li DWORD Value
    @li Multi String Value
    @li Expandable String Value

    @onlyfor{wxmsw}

    @library{wxbase}
    @category{misc}

    @b Example:

    @code
    wxRegKey *key = new wxRegKey("HKEY_LOCAL_MACHINE\\Software\\MyKey");

    // Create the key if it does not exist.
    if( !key->Exists() )
        key->Create();

    // Create a new value "MYVALUE" and set it to 12.
    key->SetValue("MYVALUE", 12);

    // Read the value back.
    long value;
    key->QueryValue("MYVALUE", &value);
    wxMessageBox(wxString::Format("%d", value), "Registry Value", wxOK);

    // Get the number of subkeys and enumerate them.
    size_t subkeys;
    key->GetKeyInfo(&subkeys, NULL, NULL, NULL);

    wxString key_name;
    key->GetFirstKey(key_name, 1);
    for(int i = 0; i < subkeys; i++)
    {
        wxMessageBox(key_name, "Subkey Name", wxOK);
        key->GetNextKey(key_name, 1);
    }
    @endcode
*/
class wxRegKey
{
public:
    /**
        Default constructor, initializes to @c HKEY_CLASSES_ROOT.
    */
    wxRegKey();
    /**
        The constructor to set the full name of the key.
    */
    wxRegKey(const wxString& strKey);
    /**
        The constructor to set the full name of the key under a previously created
        parent.
    */
    wxRegKey(const wxRegKey& keyParent, const wxString& strKey);

    /**
        Access modes for wxRegKey.
    */
    enum AccessMode
    {
        Read,   ///< Read-only
        Write   ///< Read and Write
    };

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
    void DeleteKey(const wxString& szKey);

    /**
        Deletes this key and all of its subkeys and values recursively.
    */
    void DeleteSelf();

    /**
        Deletes the named value.
    */
    void DeleteValue(const wxString& szKey);

    /**
        Returns @true if the key exists.
    */
    bool Exists() const;

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
    bool GetKeyInfo(size_t* pnSubKeys, size_t* pnMaxKeyLen,
                    size_t* pnValues, size_t* pnMaxValueLen) const;

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
    bool HasSubKey(const wxString& szKey) const;

    /**
        Returns @true if any subkeys exist.
    */
    bool HasSubKeys() const;

    /**
        Returns @true if the value exists.
    */
    bool HasValue(const wxString& szValue) const;

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
        read-only mode by passing wxRegKey::Read instead of default
        wxRegKey::Write parameter.
    */
    bool Open(AccessMode mode = Write);

    /**
        Retrieves the string value.
    */
    bool QueryValue(const wxString& szValue, wxString& strValue) const;

    /**
        Retrieves the numeric value.
    */
    const bool QueryValue(const wxString& szValue, long* plValue) const;

    /**
        Renames the key.
    */
    bool Rename(const wxString& szNewName);

    /**
        Renames a value.
    */
    bool RenameValue(const wxString& szValueOld,
                     const wxString& szValueNew);

    /**
        Sets the given @a szValue which must be numeric.
        If the value doesn't exist, it is created.
    */
    bool SetValue(const wxString& szValue, long lValue);
    /**
        Sets the given @a szValue which must be string.
        If the value doesn't exist, it is created.
    */
    bool SetValue(const wxString& szValue, const wxString& strValue);
    /**
        Sets the given @a szValue which must be binary.
        If the value doesn't exist, it is created.
    */
    bool SetValue(const wxString& szValue, const wxMemoryBuffer& buf);
};
