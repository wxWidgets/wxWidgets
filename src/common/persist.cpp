///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/persist.cpp
// Purpose:     common persistence support classes
// Author:      Vadim Zeitlin
// Created:     2009-01-20
// RCS-ID:      $Id$
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/persist.h"

// ============================================================================
// wxPersistenceManager implementation
// ============================================================================

/* static */
wxPersistenceManager& wxPersistenceManager::Get()
{
    static wxPersistenceManager s_manager;

    return s_manager;
}

wxString
wxPersistenceManager::GetKey(const wxPersistentObject& who,
                             const wxString& name) const
{
    wxString key("Persistent_Options"); // TODO: make this configurable
    key << wxCONFIG_PATH_SEPARATOR << who.GetKind()
        << wxCONFIG_PATH_SEPARATOR << who.GetName()
        << wxCONFIG_PATH_SEPARATOR << name;

    return key;
}

wxPersistentObject *wxPersistenceManager::Find(void *obj) const
{
    const wxPersistentObjectsMap::const_iterator
        it = m_persistentObjects.find(obj);
    return it == m_persistentObjects.end() ? NULL : it->second;
}

wxPersistentObject *
wxPersistenceManager::Register(void *obj, wxPersistentObject *po)
{
    if ( wxPersistentObject *old = Find(obj) )
    {
        wxFAIL_MSG( "object is already registered" );

        delete po; // still avoid the memory leaks
        return old;
    }

    m_persistentObjects[obj] = po;

    return po;
}

void wxPersistenceManager::Unregister(void *obj)
{
    wxPersistentObjectsMap::iterator it = m_persistentObjects.find(obj);
    wxCHECK_RET( it != m_persistentObjects.end(), "not registered" );

    wxPersistentObject * const po = it->second;
    m_persistentObjects.erase(it);
    delete po;
}

void wxPersistenceManager::Save(void *obj)
{
    if ( !m_doSave )
        return;

    wxPersistentObjectsMap::iterator it = m_persistentObjects.find(obj);
    wxCHECK_RET( it != m_persistentObjects.end(), "not registered" );

    it->second->Save();
}

bool wxPersistenceManager::Restore(void *obj)
{
    if ( !m_doRestore )
        return false;

    wxPersistentObjectsMap::iterator it = m_persistentObjects.find(obj);
    wxCHECK_MSG( it != m_persistentObjects.end(), false, "not registered" );

    return it->second->Restore();
}

