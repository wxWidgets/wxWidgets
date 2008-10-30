///////////////////////////////////////////////////////////////////////////////
// Name:        wx/buffer.h
// Purpose:     auto buffer classes: buffers which automatically free memory
// Author:      Vadim Zeitlin
// Modified by:
// Created:     12.04.99
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BUFFER_H
#define _WX_BUFFER_H

#include "wx/chartype.h"
#include "wx/wxcrtbase.h"

#ifndef __WXPALMOS5__
#include <stdlib.h>             // malloc() and free()
#endif // ! __WXPALMOS5__

class WXDLLIMPEXP_FWD_BASE wxCStrData;

// ----------------------------------------------------------------------------
// Special classes for (wide) character strings: they use malloc/free instead
// of new/delete
// ----------------------------------------------------------------------------

template <typename T>
class wxCharTypeBuffer
{
public:
    typedef T CharType;

    wxCharTypeBuffer(const CharType *str = NULL)
    {
        if ( str )
            m_data = new Data(wxStrdup(str));
        else
            m_data = GetNullData();
    }

    wxCharTypeBuffer(size_t len)
    {
        m_data = new Data((CharType *)malloc((len + 1)*sizeof(CharType)));
        m_data->m_str[len] = (CharType)0;
    }

    static const wxCharTypeBuffer CreateNonOwned(const CharType *str)
    {
        wxCharTypeBuffer buf;
        if ( str )
            buf.m_data = new Data(wx_const_cast(CharType*, str), Data::NonOwned);
        return buf;
    }

    ~wxCharTypeBuffer()
    {
        DecRef();
    }

    // NB: this method is only const for backward compatibility. It used to
    //     be needed for auto_ptr-like semantics of the copy ctor, but now
    //     that ref-counting is used, it's not really needed.
    CharType *release() const
    {
        if ( m_data == GetNullData() )
            return NULL;

        wxASSERT_MSG( m_data->m_owned, _T("can't release non-owned buffer") );
        wxASSERT_MSG( m_data->m_ref == 1, _T("can't release shared buffer") );

        CharType *p = m_data->m_str;

        wxCharTypeBuffer *self = wx_const_cast(wxCharTypeBuffer*, this);
        self->m_data->m_str = NULL;
        self->DecRef();

        return p;
    }

    void reset()
    {
        DecRef();
    }

    wxCharTypeBuffer(const wxCharTypeBuffer& src)
    {
        m_data = src.m_data;
        IncRef();
    }

    wxCharTypeBuffer& operator=(const CharType *str)
    {
        DecRef();

        if ( str )
            m_data = new Data(wxStrdup(str));
        return *this;
    }

    wxCharTypeBuffer& operator=(const wxCharTypeBuffer& src)
    {
        if ( &src == this )
            return *this;

        DecRef();
        m_data = src.m_data;
        IncRef();

        return *this;
    }

    bool extend(size_t len)
    {
        wxASSERT_MSG( m_data->m_owned, _T("cannot extend non-owned buffer") );
        wxASSERT_MSG( m_data->m_ref == 1, _T("can't extend shared buffer") );

        CharType *str =
            (CharType *)realloc(data(), (len + 1) * sizeof(CharType));
        if ( !str )
            return false;

        if ( m_data == GetNullData() )
        {
            m_data = new Data(str);
        }
        else
        {
            m_data->m_str = str;
            m_data->m_owned = true;
        }

        return true;
    }

    CharType *data() { return m_data->m_str; }
    const CharType *data() const { return  m_data->m_str; }
    operator const CharType *() const { return data(); }
    CharType operator[](size_t n) const { return data()[n]; }

private:
    // reference-counted data
    struct Data
    {
        enum Kind
        {
            Owned,
            NonOwned
        };

        Data(CharType *str, Kind kind = Owned)
            : m_str(str), m_ref(1), m_owned(kind == Owned) {}

        ~Data()
        {
            if ( m_owned )
                free(m_str);
        }

        CharType *m_str;

        // "short" to have sizeof(Data)=8 on 32bit archs
        unsigned short m_ref;

        bool m_owned;
    };

    // placeholder for NULL string, to simplify this code
    static Data *GetNullData()
    {
        static Data s_nullData(NULL);

        return &s_nullData;
    }

    void IncRef()
    {
        if ( m_data == GetNullData() ) // exception, not ref-counted
            return;
        m_data->m_ref++;
    }

    void DecRef()
    {
        if ( m_data == GetNullData() ) // exception, not ref-counted
            return;
        if ( --m_data->m_ref == 0 )
            delete m_data;
        m_data = GetNullData();
    }

private:
    Data *m_data;
};

WXDLLIMPEXP_TEMPLATE_INSTANCE_BASE( wxCharTypeBuffer<char> )

class wxCharBuffer : public wxCharTypeBuffer<char>
{
public:
    typedef wxCharTypeBuffer<char> wxCharTypeBufferBase;

    wxCharBuffer(const wxCharTypeBufferBase& buf)
        : wxCharTypeBufferBase(buf) {}

    wxCharBuffer(const CharType *str = NULL) : wxCharTypeBufferBase(str) {}
    wxCharBuffer(size_t len) : wxCharTypeBufferBase(len) {}

    wxCharBuffer(const wxCStrData& cstr);
};

#if wxUSE_WCHAR_T
WXDLLIMPEXP_TEMPLATE_INSTANCE_BASE( wxCharTypeBuffer<wchar_t> )

class wxWCharBuffer : public wxCharTypeBuffer<wchar_t>
{
public:
    typedef wxCharTypeBuffer<wchar_t> wxCharTypeBufferBase;

    wxWCharBuffer(const wxCharTypeBufferBase& buf)
        : wxCharTypeBufferBase(buf) {}

    wxWCharBuffer(const CharType *str = NULL) : wxCharTypeBufferBase(str) {}
    wxWCharBuffer(size_t len) : wxCharTypeBufferBase(len) {}

    wxWCharBuffer(const wxCStrData& cstr);
};
#endif // wxUSE_WCHAR_T

// wxCharTypeBuffer<T> implicitly convertible to T*
template <typename T>
class wxWritableCharTypeBuffer : public wxCharTypeBuffer<T>
{
public:
    typedef typename wxCharTypeBuffer<T>::CharType CharType;

    wxWritableCharTypeBuffer(const wxCharTypeBuffer<T>& src)
        : wxCharTypeBuffer<T>(src) {}
    // FIXME-UTF8: this won't be needed after converting mb_str()/wc_str() to
    //             always return a buffer
    wxWritableCharTypeBuffer(const CharType *str = NULL)
        : wxCharTypeBuffer<T>(str) {}

    operator CharType*() { return this->data(); }
};

typedef wxWritableCharTypeBuffer<char> wxWritableCharBuffer;
typedef wxWritableCharTypeBuffer<wchar_t> wxWritableWCharBuffer;


#if wxUSE_UNICODE
    #define wxWxCharBuffer wxWCharBuffer

    #define wxMB2WXbuf wxWCharBuffer
    #define wxWX2MBbuf wxCharBuffer
    #if wxUSE_UNICODE_WCHAR
        #define wxWC2WXbuf wxChar*
        #define wxWX2WCbuf wxChar*
    #elif wxUSE_UNICODE_UTF8
        #define wxWC2WXbuf wxWCharBuffer
        #define wxWX2WCbuf wxWCharBuffer
    #endif
#else // ANSI
    #define wxWxCharBuffer wxCharBuffer

    #define wxMB2WXbuf wxChar*
    #define wxWX2MBbuf wxChar*
    #define wxWC2WXbuf wxCharBuffer
    #define wxWX2WCbuf wxWCharBuffer
#endif // Unicode/ANSI

// type of the value returned by wxString::utf8_str()
#if wxUSE_UNICODE_UTF8
    #define wxUTF8Buf char *
#else
    #define wxUTF8Buf wxCharBuffer
#endif

// ----------------------------------------------------------------------------
// A class for holding growable data buffers (not necessarily strings)
// ----------------------------------------------------------------------------

// This class manages the actual data buffer pointer and is ref-counted.
class wxMemoryBufferData
{
public:
    // the initial size and also the size added by ResizeIfNeeded()
    enum { DefBufSize = 1024 };

    friend class wxMemoryBuffer;

    // everyting is private as it can only be used by wxMemoryBuffer
private:
    wxMemoryBufferData(size_t size = wxMemoryBufferData::DefBufSize)
        : m_data(size ? malloc(size) : NULL), m_size(size), m_len(0), m_ref(0)
    {
    }
    ~wxMemoryBufferData() { free(m_data); }


    void ResizeIfNeeded(size_t newSize)
    {
        if (newSize > m_size)
        {
            void *dataOld = m_data;
            m_data = realloc(m_data, newSize + wxMemoryBufferData::DefBufSize);
            if ( !m_data )
            {
                free(dataOld);
            }

            m_size = newSize + wxMemoryBufferData::DefBufSize;
        }
    }

    void IncRef() { m_ref += 1; }
    void DecRef()
    {
        m_ref -= 1;
        if (m_ref == 0)  // are there no more references?
            delete this;
    }


    // the buffer containing the data
    void  *m_data;

    // the size of the buffer
    size_t m_size;

    // the amount of data currently in the buffer
    size_t m_len;

    // the reference count
    size_t m_ref;

    DECLARE_NO_COPY_CLASS(wxMemoryBufferData)
};


class wxMemoryBuffer
{
public:
    // ctor and dtor
    wxMemoryBuffer(size_t size = wxMemoryBufferData::DefBufSize)
    {
        m_bufdata = new wxMemoryBufferData(size);
        m_bufdata->IncRef();
    }

    ~wxMemoryBuffer() { m_bufdata->DecRef(); }


    // copy and assignment
    wxMemoryBuffer(const wxMemoryBuffer& src)
        : m_bufdata(src.m_bufdata)
    {
        m_bufdata->IncRef();
    }

    wxMemoryBuffer& operator=(const wxMemoryBuffer& src)
    {
        if (&src != this)
        {
            m_bufdata->DecRef();
            m_bufdata = src.m_bufdata;
            m_bufdata->IncRef();
        }
        return *this;
    }


    // Accessors
    void  *GetData() const    { return m_bufdata->m_data; }
    size_t GetBufSize() const { return m_bufdata->m_size; }
    size_t GetDataLen() const { return m_bufdata->m_len; }

    void   SetBufSize(size_t size) { m_bufdata->ResizeIfNeeded(size); }
    void   SetDataLen(size_t len)
    {
        wxASSERT(len <= m_bufdata->m_size);
        m_bufdata->m_len = len;
    }

    // Ensure the buffer is big enough and return a pointer to it
    void *GetWriteBuf(size_t sizeNeeded)
    {
        m_bufdata->ResizeIfNeeded(sizeNeeded);
        return m_bufdata->m_data;
    }

    // Update the length after the write
    void  UngetWriteBuf(size_t sizeUsed) { SetDataLen(sizeUsed); }

    // Like the above, but appends to the buffer
    void *GetAppendBuf(size_t sizeNeeded)
    {
        m_bufdata->ResizeIfNeeded(m_bufdata->m_len + sizeNeeded);
        return (char*)m_bufdata->m_data + m_bufdata->m_len;
    }

    // Update the length after the append
    void  UngetAppendBuf(size_t sizeUsed)
    {
        SetDataLen(m_bufdata->m_len + sizeUsed);
    }

    // Other ways to append to the buffer
    void  AppendByte(char data)
    {
        wxCHECK_RET( m_bufdata->m_data, _T("invalid wxMemoryBuffer") );

        m_bufdata->ResizeIfNeeded(m_bufdata->m_len + 1);
        *(((char*)m_bufdata->m_data) + m_bufdata->m_len) = data;
        m_bufdata->m_len += 1;
    }

    void  AppendData(const void *data, size_t len)
    {
        memcpy(GetAppendBuf(len), data, len);
        UngetAppendBuf(len);
    }

    operator const char *() const { return (const char*)GetData(); }

private:
    wxMemoryBufferData*  m_bufdata;
};

// ----------------------------------------------------------------------------
// template class for any kind of data
// ----------------------------------------------------------------------------

// TODO

#endif // _WX_BUFFER_H
