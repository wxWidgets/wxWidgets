/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/gnome/gvfs.cpp
// Author:      Robert Roebling
// Purpose:     Implement GNOME VFS support
// Created:     03/17/06
// RCS-ID:      $Id$
// Copyright:   Robert Roebling
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_MIMETYPE && wxUSE_LIBGNOMEVFS

#include "wx/gtk/gnome/gvfs.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/module.h"
#endif

#include "wx/mimetype.h"
#include "wx/dynlib.h"

#include <libgnomevfs/gnome-vfs-mime-handlers.h>

#include "wx/link.h"
wxFORCE_LINK_THIS_MODULE(gnome_vfs)

//----------------------------------------------------------------------------
// wxGnomeVFSLibrary
//----------------------------------------------------------------------------

#define wxDL_METHOD_DEFINE( rettype, name, args, shortargs, defret ) \
    typedef rettype (* name ## Type) args ; \
    name ## Type pfn_ ## name; \
    rettype name args \
    { if (m_ok) return pfn_ ## name shortargs ; return defret; }

#define wxDL_METHOD_LOAD( lib, name, success ) \
    pfn_ ## name = (name ## Type) lib->GetSymbol( wxT(#name), &success ); \
    if (!success) return;

class wxGnomeVFSLibrary
{
public:
    wxGnomeVFSLibrary();
    ~wxGnomeVFSLibrary();

    bool IsOk();
    void InitializeMethods();

private:
    bool              m_ok;
    wxDynamicLibrary *m_gnome_vfs_lib;

public:
    wxDL_METHOD_DEFINE( gboolean, gnome_vfs_init,
        (), (), FALSE )
    wxDL_METHOD_DEFINE( void, gnome_vfs_shutdown,
        (), (), /**/ )

    wxDL_METHOD_DEFINE( GnomeVFSResult, gnome_vfs_mime_set_icon,
        (const char *mime_type, const char *filename), (mime_type, filename), GNOME_VFS_OK )
};

wxGnomeVFSLibrary::wxGnomeVFSLibrary()
{
    m_gnome_vfs_lib = NULL;

    wxLogNull log;

    m_gnome_vfs_lib = new wxDynamicLibrary( wxT("libgnomevfs-2.so.0") );
    m_ok = m_gnome_vfs_lib->IsLoaded();
    if (!m_ok) return;

    InitializeMethods();
}

wxGnomeVFSLibrary::~wxGnomeVFSLibrary()
{
    if (m_gnome_vfs_lib)
    {
        // we crash on exit later (i.e. after main() finishes) if we unload
        // this library, apparently it inserts some hooks in other libraries to
        // which we link implicitly (GTK+ itself?) which are not uninstalled
        // when it's unloaded resulting in this crash, so just leave it in
        // memory -- it's a lesser evil
        m_gnome_vfs_lib->Detach();
        delete m_gnome_vfs_lib;
    }
}

bool wxGnomeVFSLibrary::IsOk()
{
    return m_ok;
}

void wxGnomeVFSLibrary::InitializeMethods()
{
    m_ok = false;
    bool success;

    wxDL_METHOD_LOAD( m_gnome_vfs_lib, gnome_vfs_init, success )
    wxDL_METHOD_LOAD( m_gnome_vfs_lib, gnome_vfs_shutdown, success )

    m_ok = true;
}

static wxGnomeVFSLibrary* gs_lgvfs = NULL;

//----------------------------------------------------------------------------
// wxGnomeVFSMimeTypesManagerFactory
//----------------------------------------------------------------------------

wxMimeTypesManagerImpl *wxGnomeVFSMimeTypesManagerFactory::CreateMimeTypesManagerImpl()
{
    return new wxGnomeVFSMimeTypesManagerImpl;
}


//----------------------------------------------------------------------------
// wxGnomeVFSMimeTypesManagerImpl
//----------------------------------------------------------------------------

bool wxGnomeVFSMimeTypesManagerImpl::DoAssociation(const wxString& strType,
                       const wxString& strIcon,
                       wxMimeTypeCommands *entry,
                       const wxArrayString& strExtensions,
                       const wxString& strDesc)
{
    int nIndex = AddToMimeData(strType, strIcon, entry, strExtensions, strDesc, true);

    if ( nIndex == wxNOT_FOUND )
        return false;

    if (m_mailcapStylesInited & wxMAILCAP_GNOME)
    {
        // User modificationt to the MIME database
        // are not supported :-)
    }

    return false;
}

//----------------------------------------------------------------------------
// wxGnomeVFSModule
//----------------------------------------------------------------------------

class wxGnomeVFSModule: public wxModule
{
public:
    wxGnomeVFSModule() {}
    bool OnInit();
    void OnExit();

private:
    DECLARE_DYNAMIC_CLASS(wxGnomeVFSModule)
};

bool wxGnomeVFSModule::OnInit()
{
    gs_lgvfs = new wxGnomeVFSLibrary;
    if (gs_lgvfs->IsOk())
    {
        if (gs_lgvfs->gnome_vfs_init())
            wxMimeTypesManagerFactory::Set( new wxGnomeVFSMimeTypesManagerFactory );
    }
    return true;
}

void wxGnomeVFSModule::OnExit()
{
    if (gs_lgvfs->IsOk())
        gs_lgvfs->gnome_vfs_shutdown();

    delete gs_lgvfs;
}

IMPLEMENT_DYNAMIC_CLASS(wxGnomeVFSModule, wxModule)

#endif
    // wxUSE_LIBGNOMEVS
    // wxUSE_MIMETYPE
