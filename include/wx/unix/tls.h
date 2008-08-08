///////////////////////////////////////////////////////////////////////////////
// Name:        wx/unix/tls.h
// Purpose:     Pthreads implementation of wxTlsValue<>
// Author:      Vadim Zeitlin
// Created:     2008-08-08
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIX_TLS_H_
#define _WX_UNIX_TLS_H_

#include "wx/intl.h"
#include "wx/log.h"

#include <pthread.h>

// ----------------------------------------------------------------------------
// wxTlsKey is a helper class encapsulating the TLS value index
// ----------------------------------------------------------------------------

class wxTlsKey
{
public:
    // ctor allocates a new key
    wxTlsKey()
    {
        int rc = pthread_key_create(&m_key, NULL);
        if ( rc )
            wxLogSysError(_("Creating TLS key failed"), rc);
    }

    // return true if the key was successfully allocated
    bool IsOk() const { return m_key != 0; }

    // get the key value, there is no error return
    void *Get() const
    {
        return pthread_getspecific(m_key);
    }

    // change the key value, return true if ok
    bool Set(void *value)
    {
        int rc = pthread_setspecific(m_key, value);
        if ( rc )
        {
            wxLogSysError(_("Failed to set TLS value"));
            return false;
        }

        return true;
    }

    // free the key
    ~wxTlsKey()
    {
        if ( IsOk() )
            pthread_key_delete(m_key);
    }

private:
    pthread_key_t m_key;

    DECLARE_NO_COPY_CLASS(wxTlsKey)
};

#endif // _WX_UNIX_TLS_H_

