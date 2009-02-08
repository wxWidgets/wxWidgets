///////////////////////////////////////////////////////////////////////////////
// Name:        wx/unix/tls.h
// Purpose:     Win32 implementation of wxTlsValue<>
// Author:      Vadim Zeitlin
// Created:     2008-08-08
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_TLS_H_
#define _WX_MSW_TLS_H_

#include "wx/msw/wrapwin.h"

// ----------------------------------------------------------------------------
// wxTlsKey is a helper class encapsulating a TLS slot
// ----------------------------------------------------------------------------

class wxTlsKey
{
public:
    // ctor allocates a new key
    wxTlsKey()
    {
        m_slot = ::TlsAlloc();
    }

    // return true if the key was successfully allocated
    bool IsOk() const { return m_slot != TLS_OUT_OF_INDEXES; }

    // get the key value, there is no error return
    void *Get() const
    {
        return ::TlsGetValue(m_slot);
    }

    // change the key value, return true if ok
    bool Set(void *value)
    {
        return ::TlsSetValue(m_slot, value) != 0;
    }

    // free the key
    ~wxTlsKey()
    {
        if ( IsOk() )
            ::TlsFree(m_slot);
    }

private:
    DWORD m_slot;

    wxDECLARE_NO_COPY_CLASS(wxTlsKey);
};

#endif // _WX_MSW_TLS_H_

