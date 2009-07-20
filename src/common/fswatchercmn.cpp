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
}

bool wxFileSystemWatcherBase::Add(const wxFileName& path, int events)
{
    if (!path.FileExists() && !path.DirExists())
        return false;

    wxFileName path2 = path;
    if ( !path2.Normalize() )
    {
        wxFAIL_MSG(wxString::Format("Unable to normalize path '%s'",
                                                path2.GetFullPath()));
        return false;
    }

    wxString canonical = path2.GetFullPath();
    wxCHECK_MSG(m_watches.find(canonical) == m_watches.end(), false,
                    wxString::Format("path %s is already watched", canonical));

    // XXX now that I see it, it should be different structure
    // than wxFSWatchEntry and then subclasses would delete wxFSWatchEntry
    // and this class would own these other structures
    wxFSWatchEntry* watch = CreateWatch(path2, events);
    if (!DoAdd(*watch)) {
        delete watch;
        return false;
    }

    // always succeeds, checked above
    wxFSWatchEntries::value_type val(canonical, watch);
    return m_watches.insert(val).second;
}

bool wxFileSystemWatcherBase::Remove(const wxFileName& path)
{
    // normalize
    wxFileName path2 = path;
    if ( !path2.Normalize() )
    {
    	wxFAIL_MSG(wxString::Format("Unable to normalize path '%s'",
    													path2.GetFullPath()));
    	return false;
    }

    // get watch entry
    wxString canonical = path2.GetFullPath();
    wxFSWatchEntries::iterator it = m_watches.find(canonical);
    wxCHECK_MSG(it != m_watches.end(), false,
    				wxString::Format("path %s is not watched", canonical));

    // remove
    wxFSWatchEntry* watch = it->second;
    m_watches.erase(it);
    return DoRemove(*watch);
}

bool wxFileSystemWatcherBase::RemoveTree(const wxFileName& path)
{
    if (!path.DirExists())
    	return false;

    // OPT could be optimised if we stored information about relationships
    // between paths
    class RemoveTraverser : public wxDirTraverser
    {
    public:
    	RemoveTraverser(wxFileSystemWatcherBase* watcher) :
    		m_watcher(watcher)
    	{
    	}

    	virtual wxDirTraverseResult OnFile(const wxString& filename)
    	{
    		m_watcher->Remove(wxFileName(filename));
    		return wxDIR_CONTINUE;
    	}

    	virtual wxDirTraverseResult OnDir(const wxString& dirname)
    	{
    		m_watcher->RemoveTree(wxFileName(dirname));
    		return wxDIR_CONTINUE;
    	}

    private:
    	wxFileSystemWatcherBase* m_watcher;
    };

    wxDir dir(path.GetFullPath());
    RemoveTraverser traverser(this);
    dir.Traverse(traverser);

    return true;
}

bool wxFileSystemWatcherBase::RemoveAll()
{
    wxFSWatchEntries::iterator it = m_watches.begin();
    for ( ; it != m_watches.end(); ++it)
    {
    	// ignore return, we want to remove as much as possible
    	(void) DoRemove(*(it->second));
    }
    m_watches.clear();

    return true;
}

int wxFileSystemWatcherBase::GetWatchedPathsCount() const
{
    return m_watches.size();
}

int wxFileSystemWatcherBase::GetWatchedPaths(wxArrayString* paths) const
{
    wxCHECK_MSG( paths != NULL, -1, "Null array passed to retrieve paths");

    wxFSWatchEntries::const_iterator it = m_watches.begin();
    for ( ; it != m_watches.end(); ++it)
    {
        paths->push_back(it->first);
    }

    return m_watches.size();
}

#endif // wxUSE_FSWATCHER
