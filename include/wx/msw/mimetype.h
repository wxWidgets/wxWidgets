/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/mimetype.h
// Purpose:     classes and functions to manage MIME types
// Author:      Vadim Zeitlin
// Modified by:
// Created:     23.09.98
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWidgets licence (part of base library)
/////////////////////////////////////////////////////////////////////////////

#ifndef _MIMETYPE_IMPL_H
#define _MIMETYPE_IMPL_H

#include "wx/defs.h"

#if wxUSE_MIMETYPE

#include "wx/mimetype.h"

// ----------------------------------------------------------------------------
// wxFileTypeImpl is the MSW version of wxFileType, this is a private class
// and is never used directly by the application
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxFileTypeImpl
{
public:
    // ctor
    wxFileTypeImpl() { }

    // one of these Init() function must be called (ctor can't take any
    // arguments because it's common)

        // initialize us with our file type name and extension - in this case
        // we will read all other data from the registry
    void Init(const wxString& strFileType, const wxString& ext);

    // implement accessor functions
    bool GetExtensions(wxArrayString& extensions);
    bool GetMimeType(wxString *mimeType) const;
    bool GetMimeTypes(wxArrayString& mimeTypes) const;
    bool GetIcon(wxIconLocation *iconLoc) const;
    bool GetDescription(wxString *desc) const;
    bool GetOpenCommand(wxString *openCmd,
                        const wxFileType::MessageParameters& params) const
    {
        *openCmd = GetExpandedCommand(wxS("open"), params);
        return !openCmd->empty();
    }

    bool GetPrintCommand(wxString *printCmd,
                         const wxFileType::MessageParameters& params) const
    {
        *printCmd = GetExpandedCommand(wxS("print"), params);
        return !printCmd->empty();
    }

    size_t GetAllCommands(wxArrayString * verbs, wxArrayString * commands,
                          const wxFileType::MessageParameters& params) const;

    bool Unassociate();

    // set an arbitrary command, ask confirmation if it already exists and
    // overwriteprompt is true
    bool SetCommand(const wxString& cmd,
                    const wxString& verb,
                    bool overwriteprompt = true);

    bool SetDefaultIcon(const wxString& cmd = wxEmptyString, int index = 0);

    // this is called  by Associate
    bool SetDescription (const wxString& desc);

    // This is called by all our own methods modifying the registry to let the
    // Windows Shell know about the changes.
    //
    // It is also called from Associate() and Unassociate() which suppress the
    // internally generated notifications using the method below, which is why
    // it has to be public.
    void MSWNotifyShell();

    // Call before/after performing several registry changes in a row to
    // temporarily suppress multiple notifications that would be generated for
    // them and generate a single one at the end using MSWNotifyShell()
    // explicitly.
    void MSWSuppressNotifications(bool suppress);

    wxString
    GetExpandedCommand(const wxString& verb,
                       const wxFileType::MessageParameters& params) const;
private:
    // helper function: reads the command corresponding to the specified verb
    // from the registry (returns an empty string if not found)
    wxString GetCommand(const wxString& verb) const;

    // get the registry path for the given verb
    wxString GetVerbPath(const wxString& verb) const;

    // check that the registry key for our extension exists, create it if it
    // doesn't, return false if this failed
    bool EnsureExtKeyExists();

    wxString m_strFileType,         // may be empty
             m_ext;
    bool m_suppressNotify;

    // these methods are not publicly accessible (as wxMimeTypesManager
    // doesn't know about them), and should only be called by Unassociate

    bool RemoveOpenCommand();
    bool RemoveCommand(const wxString& verb);
    bool RemoveMimeType();
    bool RemoveDefaultIcon();
    bool RemoveDescription();
};

class WXDLLIMPEXP_BASE wxMimeTypesManagerImpl
{
public:
    // nothing to do here, we don't load any data but just go and fetch it from
    // the registry when asked for
    wxMimeTypesManagerImpl() { }

    // implement containing class functions
    wxFileType *GetFileTypeFromExtension(const wxString& ext);
    wxFileType *GetOrAllocateFileTypeFromExtension(const wxString& ext);
    wxFileType *GetFileTypeFromMimeType(const wxString& mimeType);

    size_t EnumAllFileTypes(wxArrayString& mimetypes);

    // create a new filetype association
    wxFileType *Associate(const wxFileTypeInfo& ftInfo);

    // create a new filetype with the given name and extension
    wxFileType *CreateFileType(const wxString& filetype, const wxString& ext);
};

#endif // wxUSE_MIMETYPE

#endif
  //_MIMETYPE_IMPL_H

