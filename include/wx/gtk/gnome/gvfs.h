/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/gnome/gvfs.h
// Author:      Robert Roebling
// Purpose:     GNOME VFS support
// Created:     17/03/06
// RCS-ID:      $Id$
// Copyright:   Robert Roebling
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __gvfs_H__
#define __gvfs_H__

// Include wxWindows' headers

#include "wx/defs.h"

#if wxUSE_MIMETYPE && wxUSE_LIBGNOMEVFS

#include "wx/string.h"
#include "wx/unix/mimetype.h"

//----------------------------------------------------------------------------
// wxGnomeVFSMimeTypesManagerImpl
//----------------------------------------------------------------------------

class wxGnomeVFSMimeTypesManagerImpl: public wxMimeTypesManagerImpl
{
public:
    wxGnomeVFSMimeTypesManagerImpl() { }
    
    virtual bool DoAssociation(const wxString& strType,
                       const wxString& strIcon,
                       wxMimeTypeCommands *entry,
                       const wxArrayString& strExtensions,
                       const wxString& strDesc);
};

//----------------------------------------------------------------------------
// wxGnomeVFSMimeTypesManagerFactory
//----------------------------------------------------------------------------

class wxGnomeVFSMimeTypesManagerFactory: public wxMimeTypesManagerFactory
{
public:
    wxGnomeVFSMimeTypesManagerFactory() {}

    virtual wxMimeTypesManagerImpl *CreateMimeTypesManagerImpl();
};

#endif
  // wxUSE_MIMETYPE 

#endif
