///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/threadinfo.cpp
// Purpose:     declaration of wxThreadSpecificInfo: thread-specific information
// Author:      Vaclav Slavik
// Created:     2013-09-14
// Copyright:   (c) 2013 Vaclav Slavik <vslavik@fastmail.fm>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
    #pragma hdrstop
#endif

#include "wx/private/threadinfo.h"

#if wxUSE_THREADS

#include "wx/tls.h"
#include "wx/thread.h"
#include "wx/sharedptr.h"
#include "wx/vector.h"

namespace
{

// All thread info objects are stored in a global list so that they are
// freed when global objects are destroyed and no memory leaks are reported.
//
// TODO: This could be made more efficient by freeing g_thisThreadInfo when
//       wxThread terminates.
wxCriticalSection g_csAllThreadInfos;
wxVector< wxSharedPtr<wxThreadSpecificInfo> > g_allThreadInfos;

// Pointer to currenct thread's instance
wxTLS_TYPE(wxThreadSpecificInfo*) g_thisThreadInfo;

} // anonymous namespace


wxThreadSpecificInfo& wxThreadSpecificInfo::Get()
{
    if ( !wxTLS_VALUE(g_thisThreadInfo) )
    {
        wxTLS_VALUE(g_thisThreadInfo) = new wxThreadSpecificInfo;
        wxCriticalSectionLocker lock(g_csAllThreadInfos);
        g_allThreadInfos.push_back(
                wxSharedPtr<wxThreadSpecificInfo>(wxTLS_VALUE(g_thisThreadInfo)));
    }
    return *wxTLS_VALUE(g_thisThreadInfo);
}

#else // !wxUSE_THREADS

wxThreadSpecificInfo& wxThreadSpecificInfo::Get()
{
    static wxThreadSpecificInfo s_instance;
    return s_instance;
}

#endif // wxUSE_THREADS/wxUSE_THREADS
