///////////////////////////////////////////////////////////////////////////////
// Name:        msw/snglinst.cpp
// Purpose:     implements wxSingleInstanceChecker class for Win32 using
//              named mutexes
// Author:      Vadim Zeitlin
// Modified by:
// Created:     08.06.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// License:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "snglinst.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_SNGLINST_CHECKER && defined(__WIN32__)

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/log.h"
#endif //WX_PRECOMP

#include "wx/snglinst.h"

#include "wx/msw/private.h"

//variables held in common by the callback and wxSingleInstanceCheckerImpl
static HWND FirsthWnd;
static wxString s_Title;

// callback to look for windows whose titles include the search string
// BCC (at least) does not like the callback to be part of the class :-((
bool CALLBACK EnumWindowsProc ( HWND hwnd, LPARAM lParam )
{
    // Get the title of this window
    int iTitleLen = ::GetWindowTextLength(hwnd);
    // possible UNICODE/ANSI bug here, see SDK documentation,
    // so allow extra space
    char * cTitle = new char [iTitleLen*2+10] ;
    ::GetWindowText(hwnd, cTitle, iTitleLen*2+10);
    
    bool bSuccess = wxString(cTitle).Contains(s_Title) ;
    delete [] cTitle ;
    
    if (bSuccess)
    {
        FirsthWnd = hwnd ;
        return FALSE ;
    }
    else
    {
        //not this window
        return TRUE;
    }
    
}
    
// ----------------------------------------------------------------------------
// wxSingleInstanceCheckerImpl: the real implementation class
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxSingleInstanceCheckerImpl
{
public:
    wxSingleInstanceCheckerImpl()
    {
        // we don't care about m_wasOpened, it can't be accessed before being
        // initialized
        m_hMutex = NULL;
    }

    bool Create(const wxString& name)
    {
        m_hMutex = ::CreateMutex(NULL, FALSE, name);
        if ( !m_hMutex )
        {
            wxLogLastError(_T("CreateMutex"));

            return FALSE;
        }

        // mutex was either created or opened - see what really happened
        m_wasOpened = ::GetLastError() == ERROR_ALREADY_EXISTS;

        return TRUE;
    }

    bool WasOpened() const
    {
        wxCHECK_MSG( m_hMutex, FALSE,
                     _T("can't be called if mutex creation failed") );

        return m_wasOpened;
    };
    


    // Activates Previous Instance if a window matching Title is found
    bool ActivatePrevInstance(const wxString & sSearch)
    {
        //store search text and window handle for use by callback
         s_Title = sSearch ;
         FirsthWnd = 0;
         
         EnumWindows (WNDENUMPROC(&EnumWindowsProc), 0L);
         if (FirsthWnd == 0)
         {
            //no matching window found
            return FALSE;
         }
         
         if (::IsIconic(FirsthWnd))
         {
             ::ShowWindow(FirsthWnd, SW_SHOWDEFAULT);
         }
         ::SetForegroundWindow(FirsthWnd);
         
         // now try to deal with any active children
         // Handles to child of previous instance 
         HWND FirstChildhWnd;
         
         FirstChildhWnd = ::GetLastActivePopup(FirsthWnd);
         if (FirsthWnd != FirstChildhWnd)
         {
            // A pop-up is active so bring it to the top too.
            ::BringWindowToTop(FirstChildhWnd); 
         }
        return TRUE;
    }
    
    // Activates Previous Instance and passes CommandLine to wxCommandLineEvent
    // if a window matching Title is found
    bool PassCommandLineToPrevInstance(const wxString & sTitle, const wxString & sCmdLine)
    {
        // this stores a string of up to 255 bytes
        //ATOM myAtom = GlobalAddAtom ( sCmdLine );

        // this would create a call to wxWindow::OnCommandLine(wxCommandLineEvent & event)
        // which should retrieve the commandline, and then delete the atom, GlobalDeleteAtom( myAtom );
        //::SendMessage (FirsthWnd, wxCOMMANDLINE_MESSAGE, 0, myAtom) ;
        return FALSE;
    }

    ~wxSingleInstanceCheckerImpl()
    {
        if ( m_hMutex )
        {
            if ( !::CloseHandle(m_hMutex) )
            {
                wxLogLastError(_T("CloseHandle(mutex)"));
            }
        }
    }

private:
    // the result of the CreateMutex() call
    bool m_wasOpened;

    // the mutex handle, may be NULL
    HANDLE m_hMutex;
    
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
    wxCHECK_MSG( m_impl, FALSE, _T("must call Create() first") );

    // if the mutex had been opened, another instance is running - otherwise we
    // would have created it
    return m_impl->WasOpened();
}

    // Activates Previous Instance if a window whose Title contains the search string is found
bool wxSingleInstanceChecker::ActivatePrevInstance(const wxString & sSearch)
{
    if (!IsAnotherRunning())
    {
        return FALSE;
    }
    return m_impl->ActivatePrevInstance(sSearch) ;
}
    
    // Activates Previous Instance and passes CommandLine to wxCommandLineEvent
    // if a window matching Title is found
bool wxSingleInstanceChecker::PassCommandLineToPrevInstance(const wxString & sSearch, const wxString & sCmdLine)
{
    if (!ActivatePrevInstance(sSearch))
    {
        return FALSE;
    }
    return m_impl->PassCommandLineToPrevInstance(sSearch, sCmdLine);
}

wxSingleInstanceChecker::~wxSingleInstanceChecker()
{
    delete m_impl;
}

#endif // wxUSE_SNGLINST_CHECKER
