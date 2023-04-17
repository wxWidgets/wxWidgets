///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/persist.cpp
// Purpose:     common persistence support classes
// Author:      Vadim Zeitlin
// Created:     2009-01-20
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


#if wxUSE_CONFIG

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/persist.h"

namespace
{

wxPersistenceManager* gs_manager = nullptr;

} // anonymous namespace

// ============================================================================
// wxPersistenceManager implementation
// ============================================================================

/* static */
void wxPersistenceManager::Set(wxPersistenceManager& manager)
{
    gs_manager = &manager;
}

/* static */
wxPersistenceManager& wxPersistenceManager::Get()
{
    if ( !gs_manager )
    {
        static wxPersistenceManager s_manager;

        gs_manager = &s_manager;
    }

    return *gs_manager;
}

wxPersistenceManager::~wxPersistenceManager()
{
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
    const auto it = m_persistentObjects.find(obj);
    return it == m_persistentObjects.end() ? nullptr : it->second.get();
}

wxPersistentObject *
wxPersistenceManager::Register(void *obj, wxPersistentObject *po)
{
    // Avoid memory leaks in any case by ensuring this object gets deleted.
    wxPersistentObjectPtr ptr{po};

    if ( wxPersistentObject *old = Find(obj) )
    {
        wxFAIL_MSG( "object is already registered" );

        return old;
    }

    m_persistentObjects[obj] = std::move(ptr);

    return po;
}

void wxPersistenceManager::Unregister(void *obj)
{
    if ( !m_persistentObjects.erase(obj) )
    {
        wxFAIL_MSG( "unregistering object which is not registered" );
    }
}

void wxPersistenceManager::Save(void *obj)
{
    if ( !m_doSave )
        return;

    const auto it = m_persistentObjects.find(obj);
    wxCHECK_RET( it != m_persistentObjects.end(), "not registered" );

    it->second->Save();
}

bool wxPersistenceManager::Restore(void *obj)
{
    if ( !m_doRestore )
        return false;

    const auto it = m_persistentObjects.find(obj);
    wxCHECK_MSG( it != m_persistentObjects.end(), false, "not registered" );

    return it->second->Restore();
}

namespace
{

template <typename T>
inline bool
DoSaveValue(wxConfigBase *conf, const wxString& key, T value)
{
    return conf && conf->Write(key, value);
}

template <typename T>
bool
DoRestoreValue(wxConfigBase *conf, const wxString& key, T *value)
{
    return conf && conf->Read(key, value);
}

} // anonymous namespace

#define wxPERSIST_DEFINE_SAVE_RESTORE_FOR(Type)                               \
    bool wxPersistenceManager::SaveValue(const wxPersistentObject& who,       \
                                         const wxString& name,                \
                                         Type value)                          \
    {                                                                         \
        return DoSaveValue(GetConfig(), GetKey(who, name), value);            \
    }                                                                         \
                                                                              \
    bool wxPersistenceManager::RestoreValue(const wxPersistentObject& who,    \
                                            const wxString& name,             \
                                            Type *value)                      \
    {                                                                         \
        return DoRestoreValue(GetConfig(), GetKey(who, name), value);         \
    }

wxPERSIST_DEFINE_SAVE_RESTORE_FOR(bool)
wxPERSIST_DEFINE_SAVE_RESTORE_FOR(int)
wxPERSIST_DEFINE_SAVE_RESTORE_FOR(long)
wxPERSIST_DEFINE_SAVE_RESTORE_FOR(wxString)

#undef wxPERSIST_DEFINE_SAVE_RESTORE_FOR

#endif // wxUSE_CONFIG
