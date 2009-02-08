///////////////////////////////////////////////////////////////////////////////
// Name:        wx/os2/tls.h
// Purpose:     OS/2 implementation of wxTlsValue<>
// Author:      Stefan Neis
// Created:     2008-08-30
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Stefan Neis
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OS2_TLS_H_
#define _WX_OS2_TLS_H_

#include "wx/os2/private.h"

// ----------------------------------------------------------------------------
// wxTlsKey is a helper class encapsulating a TLS slot
// ----------------------------------------------------------------------------

class wxTlsKey
{
public:
    // ctor allocates a new key
    wxTlsKey()
    {
        APIRET rc = ::DosAllocThreadLocalMemory(1, &m_slot);
        if (rc != NO_ERROR)
            m_slot = NULL;
    }

    // return true if the key was successfully allocated
    bool IsOk() const { return m_slot != NULL; }

    // get the key value, there is no error return
    void *Get() const
    {
        return (void *)m_slot;
    }

    // change the key value, return true if ok
    bool Set(void *value)
    {
        m_slot = (ULONG*)value;
        return true;
    }

    // free the key
    ~wxTlsKey()
    {
        if ( IsOk() )
            ::DosFreeThreadLocalMemory(m_slot);
    }

private:
    ULONG* m_slot;

    wxDECLARE_NO_COPY_CLASS(wxTlsKey);
};

#endif // _WX_OS2_TLS_H_

