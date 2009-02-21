///////////////////////////////////////////////////////////////////////////////
// Name:        os2/snglinst.cpp
// Purpose:     implements wxSingleInstanceChecker class for OS/2 using
//              named mutexes
// Author:      Vadim Zeitlin
// Modified by: Lauri Nurmi (modified for OS/2)
// Created:     08.02.2009
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_SNGLINST_CHECKER

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/log.h"
#endif //WX_PRECOMP

#include "wx/snglinst.h"

#define INCL_DOSSEMAPHORES
#define INCL_DOSERRORS
#include <os2.h>

#include "wx/os2/private.h"

// ----------------------------------------------------------------------------
// wxSingleInstanceCheckerImpl: the real implementation class
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxSingleInstanceCheckerImpl
{
public:
    wxSingleInstanceCheckerImpl()
    {
        m_hMutex = NULL;
        m_anotherRunning = false;
    }

    bool Create(const wxString& name)
    {
        wxString semName;
        semName.Printf(wxT("\\SEM32\\%s"), name.c_str());
        int rc = DosCreateMutexSem(semName.c_str(), &m_hMutex, DC_SEM_SHARED, 1);

        if ( rc == NO_ERROR ) {
            m_anotherRunning = false;
            return true;
        } else if ( rc == ERROR_DUPLICATE_NAME ) {
            m_anotherRunning = true;
            return true;
        } else {
            m_anotherRunning = false;  // we don't know for sure in this case
            wxLogLastError(_T("DosCreateMutexSem"));
            return false;
        }
    }

    bool IsAnotherRunning() const
    {
        return m_anotherRunning;
    }

    ~wxSingleInstanceCheckerImpl()
    {
        if ( m_hMutex )
        {
            if ( !::DosCloseMutexSem(m_hMutex) )
            {
                wxLogLastError(_T("DosCloseMutexSem"));
            }
        }
    }

private:
    // if true, creating the mutex either succeeded
    // or we know it failed because another app is running.
    bool m_anotherRunning;

    // the mutex handle, may be NULL
    HMTX m_hMutex;

    DECLARE_NO_COPY_CLASS(wxSingleInstanceCheckerImpl)
};

// ============================================================================
// wxSingleInstanceChecker implementation
// ============================================================================

bool wxSingleInstanceChecker::Create(const wxString& name,
                                     const wxString& WXUNUSED(path))
{
    wxASSERT_MSG( !m_impl,
                  _T("calling wxSingleInstanceChecker::Create() twice?") );

    // creating unnamed mutex doesn't have the same semantics!
    wxASSERT_MSG( !name.empty(), _T("mutex name can't be empty") );

    m_impl = new wxSingleInstanceCheckerImpl;

    return m_impl->Create(name);
}

bool wxSingleInstanceChecker::IsAnotherRunning() const
{
    wxCHECK_MSG( m_impl, false, _T("must call Create() first") );

    return m_impl->IsAnotherRunning();
}

wxSingleInstanceChecker::~wxSingleInstanceChecker()
{
    delete m_impl;
}

#endif // wxUSE_SNGLINST_CHECKER
