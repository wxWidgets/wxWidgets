/////////////////////////////////////////////////////////////////////////////
// Name:        unix/mimetype.h
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


#ifdef __GNUG__
#pragma interface "mimetype.h"
#endif

#include "wx/mimetype.h"

#if (wxUSE_FILE && wxUSE_TEXTFILE)

class MailCapEntry;
class wxMimeTypeIconHandler;

WX_DEFINE_ARRAY(wxMimeTypeIconHandler *, ArrayIconHandlers);
WX_DEFINE_ARRAY(MailCapEntry *, ArrayTypeEntries);

// this is the real wxMimeTypesManager for Unix
class WXDLLEXPORT wxMimeTypesManagerImpl
{
friend class WXDLLEXPORT wxFileTypeImpl; // give it access to m_aXXX variables

public:
    // ctor loads all info into memory for quicker access later on
    // TODO it would be nice to load them all, but parse on demand only...
    wxMimeTypesManagerImpl();

    // implement containing class functions
    wxFileType *GetFileTypeFromExtension(const wxString& ext);
    wxFileType *GetFileTypeFromMimeType(const wxString& mimeType);

    size_t EnumAllFileTypes(wxArrayString& mimetypes);

    bool ReadMailcap(const wxString& filename, bool fallback = FALSE);
    bool ReadMimeTypes(const wxString& filename);

    void AddFallback(const wxFileTypeInfo& filetype);

    // add information about the given mimetype
    void AddMimeTypeInfo(const wxString& mimetype,
                         const wxString& extensions,
                         const wxString& description);
    void AddMailcapInfo(const wxString& strType,
                        const wxString& strOpenCmd,
                        const wxString& strPrintCmd,
                        const wxString& strTest,
                        const wxString& strDesc);

    // accessors
        // get the string containing space separated extensions for the given
        // file type
    wxString GetExtension(size_t index) { return m_aExtensions[index]; }

        // get the array of icon handlers
    static ArrayIconHandlers& GetIconHandlers();

private:
    wxArrayString m_aTypes,         // MIME types
                  m_aDescriptions,  // descriptions (just some text)
                  m_aExtensions;    // space separated list of extensions
    ArrayTypeEntries m_aEntries;    // commands and tests for this file type

    // head of the linked list of the icon handlers
    static ArrayIconHandlers ms_iconHandlers;
};



class WXDLLEXPORT wxFileTypeImpl
{
public:
    // initialization functions
    void Init(wxMimeTypesManagerImpl *manager, size_t index)
        { m_manager = manager; m_index.Add(index); }

    // accessors
    bool GetExtensions(wxArrayString& extensions);
    bool GetMimeType(wxString *mimeType) const
        { *mimeType = m_manager->m_aTypes[m_index[0]]; return TRUE; }
    bool GetMimeTypes(wxArrayString& mimeTypes) const;
    bool GetIcon(wxIcon *icon) const;
    bool GetDescription(wxString *desc) const
        { *desc = m_manager->m_aDescriptions[m_index[0]]; return TRUE; }

    bool GetOpenCommand(wxString *openCmd,
                        const wxFileType::MessageParameters& params) const
    {
        return GetExpandedCommand(openCmd, params, TRUE);
    }

    bool GetPrintCommand(wxString *printCmd,
                         const wxFileType::MessageParameters& params) const
    {
        return GetExpandedCommand(printCmd, params, FALSE);
    }

private:
    // get the entry which passes the test (may return NULL)
    MailCapEntry *GetEntry(const wxFileType::MessageParameters& params) const;

    // choose the correct entry to use and expand the command
    bool GetExpandedCommand(wxString *expandedCmd,
                            const wxFileType::MessageParameters& params,
                            bool open) const;

    wxMimeTypesManagerImpl *m_manager;
    wxArrayInt              m_index; // in the wxMimeTypesManagerImpl arrays
};



#endif
  // wxUSE_FILE

#endif
  //_MIMETYPE_IMPL_H

