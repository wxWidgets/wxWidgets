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

class wxMimeArrayString;

WX_DEFINE_ARRAY(wxMimeArrayString *, wxArrayTypeEntries);

// this is the real wxMimeTypesManager for Unix
class WXDLLEXPORT wxMimeTypesManagerImpl
{
public:
    // ctor and dtor
    wxMimeTypesManagerImpl();
    ~wxMimeTypesManagerImpl();

    // load all data into memory - done when it is needed for the first time
    void Initialize(int mailcapStyles = wxMAILCAP_STANDARD,
                    const wxString& extraDir = wxEmptyString);

    // and delete the data here
    void ClearData();

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

    // add a new record to the user .mailcap/.mime.types files
    wxFileType *Associate(const wxFileTypeInfo& ftInfo);
    // remove association
    bool Unassociate(wxFileType *ft);

    // accessors
        // get the string containing space separated extensions for the given
        // file type
    wxString GetExtension(size_t index) { return m_aExtensions[index]; }

private:
    void InitIfNeeded()
    {
        if ( !m_initialized ) {
            // set the flag first to prevent recursion
            m_initialized = TRUE;
            Initialize();
        }
    }

    wxArrayString m_aTypes,         // MIME types
                  m_aDescriptions,  // descriptions (just some text)
                  m_aExtensions,    // space separated list of extensions
                  m_aIcons;         // Icon filenames

    // verb=command pairs for this file type
    wxArrayTypeEntries m_aEntries;

    // are we initialized?
    bool m_initialized;

    // keep track of the files we had already loaded (this is a bitwise OR of
    // wxMailcapStyle values)
    int m_mailcapStylesInited;

    wxString GetCommand(const wxString &verb, size_t nIndex) const;

    // read Gnome files
    void LoadGnomeDataFromKeyFile(const wxString& filename);
    void LoadGnomeMimeTypesFromMimeFile(const wxString& filename);
    void LoadGnomeMimeFilesFromDir(const wxString& dirbase);
    void GetGnomeMimeInfo(const wxString& sExtraDir);

    // write gnome files
    bool CheckGnomeDirsExist();
    bool WriteGnomeKeyFile(int index, bool delete_index);
    bool WriteGnomeMimeFile(int index, bool delete_index);

    // read KDE
    void LoadKDELinksForMimeSubtype(const wxString& dirbase,
                                    const wxString& subdir,
                                    const wxString& filename,
                                    const wxArrayString& icondirs);
    void LoadKDELinksForMimeType(const wxString& dirbase,
                                 const wxString& subdir,
                                 const wxArrayString& icondirs);
    void LoadKDELinkFilesFromDir(const wxString& dirbase,
                                 const wxArrayString& icondirs);
    void GetKDEMimeInfo(const wxString& sExtraDir);

    // write KDE
    bool WriteKDEMimeFile(int index, bool delete_index);
    bool CheckKDEDirsExist(const wxString & sOK, const wxString& sTest);

    //read write Netscape and MetaMail formats
    void GetMimeInfo (const wxString& sExtraDir);
    bool WriteToMailCap (int index, bool delete_index);
    bool WriteToMimeTypes (int index, bool delete_index);
    bool WriteToNSMimeTypes (int index, bool delete_index);

    // functions used to do associations

    int AddToMimeData(const wxString& strType,
                      const wxString& strIcon,
                      wxMimeArrayString *entry,
                      const wxArrayString& strExtensions,
                      const wxString& strDesc,
                      bool replaceExisting = TRUE);

    bool DoAssociation(const wxString& strType,
                       const wxString& strIcon,
                       wxMimeArrayString *entry,
                       const wxArrayString& strExtensions,
                       const wxString& strDesc);

    bool WriteMimeInfo(int nIndex, bool delete_mime );

    // give it access to m_aXXX variables
    friend class WXDLLEXPORT wxFileTypeImpl;
};



class WXDLLEXPORT wxFileTypeImpl
{
public:
    // initialization functions
    // this is used to construct a list of mimetypes which match;
    // if built with GetFileTypeFromMimetype index 0 has the exact match and
    // index 1 the type / * match
    // if built with GetFileTypeFromExtension, index 0 has the mimetype for
    // the first extension found, index 1 for the second and so on

    void Init(wxMimeTypesManagerImpl *manager, size_t index)
        { m_manager = manager; m_index.Add(index); }

    // accessors
    bool GetExtensions(wxArrayString& extensions);
    bool GetMimeType(wxString *mimeType) const
        { *mimeType = m_manager->m_aTypes[m_index[0]]; return TRUE; }
    bool GetMimeTypes(wxArrayString& mimeTypes) const;
    bool GetIcon(wxIcon *icon, wxString *iconFile = NULL,
                 int *iconIndex = NULL) const;

    bool GetDescription(wxString *desc) const
        { *desc = m_manager->m_aDescriptions[m_index[0]]; return TRUE; }

    bool GetOpenCommand(wxString *openCmd,
                        const wxFileType::MessageParameters& params) const
    {
        *openCmd = GetExpandedCommand(wxT("open"), params);
        return (! openCmd -> IsEmpty() );
    }

    bool GetPrintCommand(wxString *printCmd,
                         const wxFileType::MessageParameters& params) const
    {
        *printCmd = GetExpandedCommand(wxT("print"), params);
        return (! printCmd -> IsEmpty() );
    }

        // return the number of commands defined for this file type, 0 if none
    size_t GetAllCommands(wxArrayString *verbs, wxArrayString *commands,
                          const wxFileType::MessageParameters& params) const;


    // remove the record for this file type
    // probably a mistake to come here, use wxMimeTypesManager.Unassociate (ft) instead
    bool Unassociate(wxFileType *ft)
    {
        return m_manager->Unassociate(ft);
    }

    // set an arbitrary command, ask confirmation if it already exists and
    // overwriteprompt is TRUE
    bool SetCommand(const wxString& cmd, const wxString& verb, bool overwriteprompt = TRUE);
    bool SetDefaultIcon(const wxString& strIcon = wxEmptyString, int index = 0);

private:
    wxString
    GetExpandedCommand(const wxString & verb,
                       const wxFileType::MessageParameters& params) const;

    wxMimeTypesManagerImpl *m_manager;
    wxArrayInt              m_index; // in the wxMimeTypesManagerImpl arrays
};

#endif
  // wxUSE_FILE

#endif
  //_MIMETYPE_IMPL_H

