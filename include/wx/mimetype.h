/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mimetype.h
// Purpose:     classes and functions to manage MIME types
// Author:      Vadim Zeitlin
// Modified by:
// Created:     23.09.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license (part of wxExtra library)
/////////////////////////////////////////////////////////////////////////////

#ifndef _MIMETYPE_H
#define _MIMETYPE_H

// fwd decls
class wxIcon;
class wxFileTypeImpl;
class wxMimeTypesManagerImpl;

#include "wx/defs.h"

#if wxUSE_FILE

// the things we really need
#include "wx/string.h"

// This class holds information about a given "file type". File type is the
// same as MIME type under Unix, but under Windows it corresponds more to an
// extension than to MIME type (in fact, several extensions may correspond to a
// file type). This object may be created in many different ways and depending
// on how it was created some fields may be unknown so the return value of all
// the accessors *must* be checked!
class WXDLLEXPORT wxFileType
{
friend wxMimeTypesManagerImpl;  // it has access to m_impl

public:
    // An object of this class must be passed to Get{Open|Print}Command. The
    // default implementation is trivial and doesn't know anything at all about
    // parameters, only filename and MIME type are used (so it's probably ok for
    // Windows where %{param} is not used anyhow)
    class MessageParameters
    {
    public:
        // ctors
        MessageParameters() { }
        MessageParameters(const wxString& filename, const wxString& mimetype)
            : m_filename(filename), m_mimetype(mimetype) { }

        // accessors (called by GetOpenCommand)
            // filename
        const wxString& GetFileName() const { return m_filename; }
            // mime type
        const wxString& GetMimeType() const { return m_mimetype; }

        // override this function in derived class
        virtual wxString GetParamValue(const wxString& WXUNUSED(paramName)) const
            { return ""; }

        // virtual dtor as in any base class
        virtual ~MessageParameters() { }

    protected:
        wxString m_filename, m_mimetype;
    };

    // accessors: all of them return true if the corresponding information
    // could be retrieved/found, false otherwise (and in this case all [out]
    // parameters are unchanged)
        // return the MIME type for this file type
    bool GetMimeType(wxString *mimeType) const;
        // fill passed in array with all extensions associated with this file
        // type
    bool GetExtensions(wxArrayString& extensions);
        // get the icon corresponding to this file type
    bool GetIcon(wxIcon *icon) const;
        // get a brief file type description ("*.txt" => "text document")
    bool GetDescription(wxString *desc) const;

    // get the command to be used to open/print the given file.
        // get the command to execute the file of given type
    bool GetOpenCommand(wxString *openCmd,
                        const MessageParameters& params) const;
        // get the command to print the file of given type
    bool GetPrintCommand(wxString *printCmd,
                         const MessageParameters& params) const;

    // operations
        // expand a string in the format of GetOpenCommand (which may contain
        // '%s' and '%t' format specificators for the file name and mime type
        // and %{param} constructions).
    static wxString ExpandCommand(const wxString& command,
                                  const MessageParameters& params);

    // dtor (not virtual, shouldn't be derived from)
    ~wxFileType();

private:
    // default ctor is private because the user code never creates us
    wxFileType();

    // no copy ctor/assignment operator
    wxFileType(const wxFileType&);
    wxFileType& operator=(const wxFileType&);

    wxFileTypeImpl *m_impl;
};

// This class is only used wuth wxMimeTypesManager::AddFallbacks() and is meant
// just as the container for the wxFileType data.
class WXDLLEXPORT wxFileTypeInfo
{
public:
    // ctors
        // a normal item
    wxFileTypeInfo(const char *mimeType,
                   const char *openCmd,
                   const char *printCmd,
                   const char *desc,
                   // the other parameters form a NULL terminated list of
                   // extensions
                   ...);
                   
        // invalid item - use this to terminate the array passed to
        // wxMimeTypesManager::AddFallbacks
    wxFileTypeInfo() { }

    bool IsValid() const { return !m_mimeType.IsEmpty(); }

private:
    wxString m_mimeType,    // the MIME type in "type/subtype" form
             m_openCmd,     // command to use for opening the file (%s allowed)
             m_printCmd,    // command to use for printing the file (%s allowed)
             m_desc;        // a free form description of this file type

    wxArrayString m_exts;   // the extensions which are mapped on this filetype
};

// This class accesses the information about all known MIME types and allows
// the application to retrieve information (including how to handle data of
// given type) about them.
//
// NB: currently it doesn't support modifying MIME database (read-only access).
class WXDLLEXPORT wxMimeTypesManager
{
public:
    // static helper functions
    // -----------------------

        // check if the given MIME type is the same as the other one: the second
        // argument may contain wildcards ('*'), but not the first. If the
        // types are equal or if the mimeType matches wildcard the function
        // returns TRUE, otherwise it returns FALSE
    static bool IsOfType(const wxString& mimeType, const wxString& wildcard);

    // ctor
    wxMimeTypesManager();

    // Database lookup: all functions return a pointer to wxFileType object
    // whose methods may be used to query it for the information you're
    // interested in. If the return value is !NULL, caller is responsible for
    // deleting it.
        // get file type from file extension
    wxFileType *GetFileTypeFromExtension(const wxString& ext);
        // get file type from MIME type (in format <category>/<format>)
    wxFileType *GetFileTypeFromMimeType(const wxString& mimeType);

    // other operations: return TRUE if there were no errors or FALSE if there
    // were some unreckognized entries (the good entries are always read anyhow)
        // read in additional file (the standard ones are read automatically)
        // in mailcap format (see mimetype.cpp for description)
        //
        // 'fallback' parameter may be set to TRUE to avoid overriding the
        // settings from other, previously parsed, files by this one: normally,
        // the files read most recently would override the older files, but with
        // fallback == TRUE this won't happen
    bool ReadMailcap(const wxString& filename, bool fallback = FALSE);
        // read in additional file in mime.types format
    bool ReadMimeTypes(const wxString& filename);

    // these functions can be used to provide default values for some of the
    // MIME types inside the program itself (you may also use
    // ReadMailcap(filenameWithDefaultTypes, TRUE /* use as fallback */) to
    // achieve the same goal, but this requires having this info in a file).
    //
    // It isn't possible (currently) to provide fallback icons using this
    // function.
    //
    // The filetypes array should be terminated by a NULL entry
    bool AddFallbacks(const wxFileTypeInfo *filetypes);

    // dtor (not virtual, shouldn't be derived from)
    ~wxMimeTypesManager();

private:
    // no copy ctor/assignment operator
    wxMimeTypesManager(const wxMimeTypesManager&);
    wxMimeTypesManager& operator=(const wxMimeTypesManager&);

    wxMimeTypesManagerImpl *m_impl;
};

#endif
  // wxUSE_FILE

#endif
  //_MIMETYPE_H

/* vi: set cin tw=80 ts=4 sw=4: */
