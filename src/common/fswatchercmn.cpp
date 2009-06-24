/////////////////////////////////////////////////////////////////////////////
// Name:        common/fswatchercmn.cpp
// Purpose:     wxMswFileSystemWatcher
// Author:      Bartosz Bekier
// Created:     2009-05-26
// RCS-ID:      $Id$
// Copyright:   (c) 2009 Bartosz Bekier <bartosz.bekier@gmail.com>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_FSWATCHER

#include "wx/fswatcher.h"

wxDEFINE_EVENT(wxEVT_FSWATCHER, wxFileSystemWatcherEvent);

static wxString GetFSWEventChangeTypeName(int type)
{
    switch (type)
    {
    case wxFSW_EVENT_CREATE:
        return "CREATE";
    case wxFSW_EVENT_DELETE:
        return "DELETE";
    case wxFSW_EVENT_RENAME:
        return "RENAME";
    case wxFSW_EVENT_MODIFY:
        return "MODIFY";
    case wxFSW_EVENT_ACCESS:
        return "ACCESS";
    }

    // should never be reached!
    wxFAIL_MSG("Unknown change type");
    return "INVALID_TYPE";
}

wxString wxFileSystemWatcherEvent::ToString() const
{
    return wxString::Format("FSW_EVT type=%d (%s) path='%s'", m_changeType,
            GetFSWEventChangeTypeName(m_changeType), GetPath().GetFullPath());
}

wxFileSystemWatcherBase::wxFileSystemWatcherBase() :
    m_service(0), m_owner(this)
{
}

wxFileSystemWatcherBase::~wxFileSystemWatcherBase()
{
    wxWatchMap::iterator it = m_watches.begin();
    for ( ; it != m_watches.end(); ++it )
    {
       wxASSERT(it->second);
       delete it->second;
    }
}

bool wxFileSystemWatcherBase::Add(const wxFileName& path, int events)
{
    if (!path.FileExists() && !path.DirExists())
        return false;

    wxFileName path2 = path;
    if ( !path2.Normalize() )
    {
        wxCHECK_MSG(0, false, wxString::Format("Unable to normalize path '%s'",
                                                        path2.GetFullPath()));
    }

    wxString canonical = path2.GetFullPath();
    wxCHECK_MSG(m_watches.find(canonical) == m_watches.end(), false,
                    wxString::Format("path %s is already watched", canonical));

    wxWatch* watch = CreateWatch(path2, events);
    if (!DoAdd(*watch)) {
        delete watch;
        return false;
    }

    // CHECK are we sure we got identity here (IOW canonical path) ?
    wxWatchMap::value_type val(canonical, watch);

    // always succeeds, checked above
    return m_watches.insert(val).second;
}

bool wxFileSystemWatcherBase::AddTree(const wxFileName& path, int events,
                                                        const wxString& filter)
{
    // TODO impl
#if 0
    if (!path.DirExists())
        return false;

    // TODO maybe optimise in future to use wxDirTraverser
    wxArrayString files;
    int flags = wxDIR_DIRS | wxDIR_FILES | wxDIR_HIDDEN;
    if ((size_t)-1 == wxDir::GetAllFiles(path.GetFullPath(),
                                         &files, filter, flags))
    {
        return false;
    }

    wxArrayString::iterator it = files.begin();
#endif
    return false;
}

#endif // wxUSE_FSWATCHER
