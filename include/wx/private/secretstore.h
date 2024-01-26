///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/secretstore.h
// Purpose:     Classes used in wxSecretStore implementation only.
// Author:      Vadim Zeitlin
// Created:     2016-05-27
// Copyright:   (c) 2016 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_SECRETSTORE_H_
#define _WX_PRIVATE_SECRETSTORE_H_

#include "wx/object.h"          // wxRefCounter

// Both of the implementation classes here are ref-counted so that the
// corresponding public objects could be copied cheaply and, in the case of
// wxSecretValue, also to avoid having the secret in more than one place in
// memory at a time.

// ----------------------------------------------------------------------------
// Class holding wxSecretValue data
// ----------------------------------------------------------------------------

// This is a common base class for our own and libsecret-based implementations.
class wxSecretValueImpl : public wxRefCounter
{
public:
    wxSecretValueImpl()
    {
    }

    virtual size_t GetSize() const = 0;
    virtual const void *GetData() const = 0;
};

// Trivial common implementation of wxSecretValueImpl used under MSW and OS X.
#if defined(__WINDOWS__) || defined(__DARWIN__)

class wxSecretValueGenericImpl : public wxSecretValueImpl
{
public:
    wxSecretValueGenericImpl(size_t size, const void *data)
        : m_size(size),
          m_data(new char[size])
    {
        memcpy(m_data, data, size);
    }

    virtual ~wxSecretValueGenericImpl()
    {
        if ( m_data )
        {
            wxSecretValue::Wipe(m_size, m_data);
            delete [] m_data;
        }
    }

    virtual size_t GetSize() const override { return m_size; }
    virtual const void *GetData() const override { return m_data; }

private:
    const size_t m_size;
    char* const m_data;
};

#endif // MSW or OSX

// ----------------------------------------------------------------------------
// Base class for wxSecretStore implementations
// ----------------------------------------------------------------------------

// All its methods are similar to the methods of the public class except that
// they work with the implementation rather than public objects and they all
// take an extra "wxString&" output parameter which is filled with the low
// level error message in case of an error. This message will be logged by
// wxSecretStore itself, wxSecretStoreImpl methods shouldn't do any logging on
// their own.
class wxSecretStoreImpl : public wxRefCounter
{
public:
    virtual bool IsOk(wxString* WXUNUSED(errmsg)) const { return true; }

    virtual bool Save(const wxString& service,
                      const wxString& username,
                      const wxSecretValueImpl& password,
                      wxString& errmsg) = 0;
    virtual bool Load(const wxString& service,
                      wxString* username,
                      wxSecretValueImpl** password,
                      wxString& errmsg) const = 0;
    virtual bool Delete(const wxString& service,
                        wxString& errmsg) = 0;
};

#endif // _WX_PRIVATE_SECRETSTORE_H_
