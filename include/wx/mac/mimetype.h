/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/mimetype.h
// Purpose:     classes and functions to manage MIME types
// Author:      Vadim Zeitlin
// Modified by:
// Created:     23.09.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license (part of wxExtra library)
/////////////////////////////////////////////////////////////////////////////

#ifndef _MIMETYPE_IMPL_H
#define _MIMETYPE_IMPL_H


#include "wx/defs.h"
#include "wx/mimetype.h"



class wxMimeTypesManagerImpl
{
public :
    wxMimeTypesManagerImpl() { }

    // implement containing class functions
    wxFileType *GetFileTypeFromExtension(const wxString& ext);
    wxFileType *GetFileTypeFromMimeType(const wxString& mimeType);

    size_t EnumAllFileTypes(wxArrayString& mimetypes);

    // this are NOPs under MacOS
    bool ReadMailcap(const wxString& filename, bool fallback = TRUE) { return TRUE; }
    bool ReadMimeTypes(const wxString& filename) { return TRUE; }

    void AddFallback(const wxFileTypeInfo& ft) { m_fallbacks.Add(ft); }

private:
    wxArrayFileTypeInfo m_fallbacks;
};

class wxFileTypeImpl
{
public:
    // initialize us with our file type name
    void SetFileType(const wxString& strFileType)
        { m_strFileType = strFileType; }
    void SetExt(const wxString& ext)
        { m_ext = ext; }

    // implement accessor functions
    bool GetExtensions(wxArrayString& extensions);
    bool GetMimeType(wxString *mimeType) const;
    bool GetMimeTypes(wxArrayString& mimeTypes) const;
    bool GetIcon(wxIcon *icon) const;
    bool GetDescription(wxString *desc) const;
    bool GetOpenCommand(wxString *openCmd,
                        const wxFileType::MessageParameters&) const
        { return GetCommand(openCmd, "open"); }
    bool GetPrintCommand(wxString *printCmd,
                         const wxFileType::MessageParameters&) const
        { return GetCommand(printCmd, "print"); }

private:
    // helper function
    bool GetCommand(wxString *command, const char *verb) const;

    wxString m_strFileType, m_ext;
};



#endif
  //_MIMETYPE_H

/* vi: set cin tw=80 ts=4 sw=4: */
