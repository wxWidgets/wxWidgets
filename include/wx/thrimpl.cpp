/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/thrimpl.cpp
// Purpose:     common part of wxThread Implementations
// Author:      Vadim Zeitlin
// Modified by:
// Created:     04.06.02 (extracted from src/*/thread.cpp files)
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin (2002)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// this file is supposed to be included only by the various thread.cpp

// ----------------------------------------------------------------------------
// wxMutex
// ----------------------------------------------------------------------------

wxMutex::wxMutex(wxMutexType mutexType)
{
    m_internal = new wxMutexInternal(mutexType);

    if ( !m_internal->IsOk() )
    {
        delete m_internal;
        m_internal = NULL;
    }
}

wxMutex::~wxMutex()
{
    delete m_internal;
}

bool wxMutex::IsOk() const
{
    return m_internal != NULL;
}

wxMutexError wxMutex::Lock()
{
    wxCHECK_MSG( m_internal, wxMUTEX_INVALID,
                 _T("wxMutex::Lock(): not initialized") );

    return m_internal->Lock();
}

wxMutexError wxMutex::TryLock()
{
    wxCHECK_MSG( m_internal, wxMUTEX_INVALID,
                 _T("wxMutex::TryLock(): not initialized") );

    return m_internal->TryLock();
}

wxMutexError wxMutex::Unlock()
{
    wxCHECK_MSG( m_internal, wxMUTEX_INVALID,
                 _T("wxMutex::Unlock(): not initialized") );

    return m_internal->Unlock();
}

// ----------------------------------------------------------------------------
// wxCondition
// ----------------------------------------------------------------------------

wxCondition::wxCondition(wxMutex& mutex)
{
    m_internal = new wxConditionInternal(mutex);

    if ( !m_internal->IsOk() )
    {
        delete m_internal;
        m_internal = NULL;
    }
}

wxCondition::~wxCondition()
{
    delete m_internal;
}

bool wxCondition::IsOk() const
{
    return m_internal != NULL;
}

wxCondError wxCondition::Wait()
{
    wxCHECK_MSG( m_internal, wxCOND_INVALID,
                 _T("wxCondition::Wait(): not initialized") );

    return m_internal->Wait();
}

wxCondError wxCondition::WaitTimeout(unsigned long milliseconds)
{
    wxCHECK_MSG( m_internal, wxCOND_INVALID,
                 _T("wxCondition::Wait(): not initialized") );

    return m_internal->WaitTimeout(milliseconds);
}

wxCondError wxCondition::Signal()
{
    wxCHECK_MSG( m_internal, wxCOND_INVALID,
                 _T("wxCondition::Signal(): not initialized") );

    return m_internal->Signal();
}

wxCondError wxCondition::Broadcast()
{
    wxCHECK_MSG( m_internal, wxCOND_INVALID,
                 _T("wxCondition::Broadcast(): not initialized") );

    return m_internal->Broadcast();
}

// --------------------------------------------------------------------------
// wxSemaphore
// --------------------------------------------------------------------------

wxSemaphore::wxSemaphore(int initialcount, int maxcount)
{
    m_internal = new wxSemaphoreInternal( initialcount, maxcount );
    if ( !m_internal->IsOk() )
    {
        delete m_internal;
        m_internal = NULL;
    }
}

wxSemaphore::~wxSemaphore()
{
    delete m_internal;
}

bool wxSemaphore::IsOk() const
{
    return m_internal != NULL;
}

wxSemaError wxSemaphore::Wait()
{
    wxCHECK_MSG( m_internal, wxSEMA_INVALID,
                 _T("wxSemaphore::Wait(): not initialized") );

    return m_internal->Wait();
}

wxSemaError wxSemaphore::TryWait()
{
    wxCHECK_MSG( m_internal, wxSEMA_INVALID,
                 _T("wxSemaphore::TryWait(): not initialized") );

    return m_internal->TryWait();
}

wxSemaError wxSemaphore::WaitTimeout(unsigned long milliseconds)
{
    wxCHECK_MSG( m_internal, wxSEMA_INVALID,
                 _T("wxSemaphore::WaitTimeout(): not initialized") );

    return m_internal->WaitTimeout(milliseconds);
}

wxSemaError wxSemaphore::Post()
{
    wxCHECK_MSG( m_internal, wxSEMA_INVALID,
                 _T("wxSemaphore::Post(): not initialized") );

    return m_internal->Post();
}

