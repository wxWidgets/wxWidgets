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

#include "wx/wxchar.h"

#include <stdlib.h>             // malloc() and free()

inline char *wxStrDup(const char *s) { return wxStrdupA(s); }
#if wxUSE_WCHAR_T
    inline wchar_t *wxStrDup(const wchar_t *ws) { return wxStrdupW(ws); }
#endif

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
        : m_str(str ? wxStrDup(str) : NULL)
    {
    }

    wxCharTypeBuffer(size_t len)
        : m_str((CharType *)malloc((len + 1)*sizeof(CharType)))
    {
        m_str[len] = (CharType)0;
    }

    /* no need to check for NULL, free() does it */
    ~wxCharTypeBuffer() { free(m_str); }

    /*
        WARNING:

        the copy ctor and assignment operators change the passed in object
        even although it is declared as "const", so:

        a) it shouldn't be really const
        b) you shouldn't use it afterwards (or know that it was reset)

        This is very ugly but is unfortunately needed to make the normal use\
        of wxCharTypeBuffer buffer objects possible and is very similar to what
        std::auto_ptr<> does (as if it were an excuse...)
    */

    /*
       because of the remark above, release() is declared const even if it
       isn't really const
     */
    CharType *release() const
    {
        CharType *p = m_str;
        ((wxCharTypeBuffer *)this)->m_str = NULL;
        return p;
    }

    void reset()
    {
        free(m_str);
        m_str = NULL;
    }

    wxCharTypeBuffer(const wxCharTypeBuffer& src)
        : m_str(src.release())
    {
    }

    wxCharTypeBuffer& operator=(const CharType *str)
    {
        free(m_str);
        m_str = str ? wxStrDup(str) : NULL;
        return *this;
    }

    wxCharTypeBuffer& operator=(const wxCharTypeBuffer& src)
    {
        free(m_str);
        m_str = src.release();

        return *this;
    }

    bool extend(size_t len)
    {
        CharType *
            str = (CharType *)realloc(m_str, (len + 1)*sizeof(CharType));
        if ( !str )
            return false;

        m_str = str;

        return true;
    }

    CharType *data() { return m_str; }
    const CharType *data() const { return m_str; }
    operator const CharType *() const { return m_str; }
    CharType operator[](size_t n) const { return m_str[n]; }

private:
    CharType *m_str;
};

class WXDLLIMPEXP_BASE wxCharBuffer : public wxCharTypeBuffer<char>
{
public:
    typedef wxCharTypeBuffer<char> wxCharTypeBufferBase;

    wxCharBuffer(const CharType *str = NULL) : wxCharTypeBufferBase(str) {}
    wxCharBuffer(size_t len) : wxCharTypeBufferBase(len) {}

#if !wxUSE_UNICODE
    wxCharBuffer(const wxCStrData& cstr);
#endif
};

#if wxUSE_WCHAR_T
class WXDLLIMPEXP_BASE wxWCharBuffer : public wxCharTypeBuffer<wchar_t>
{
public:
    typedef wxCharTypeBuffer<wchar_t> wxCharTypeBufferBase;

    wxWCharBuffer(const CharType *str = NULL) : wxCharTypeBufferBase(str) {}
    wxWCharBuffer(size_t len) : wxCharTypeBufferBase(len) {}

#if wxUSE_UNICODE
    wxWCharBuffer(const wxCStrData& cstr);
#endif
};
#endif // wxUSE_WCHAR_T

// wxCharTypeBuffer<T> implicitly convertible to T*
template <typename T>
class wxWritableCharTypeBuffer : wxCharTypeBuffer<T>
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
    #define wxWC2WXbuf wxChar*
    #define wxWX2WCbuf wxChar*
#else // ANSI
    #define wxWxCharBuffer wxCharBuffer

    #define wxMB2WXbuf wxChar*
    #define wxWX2MBbuf wxChar*
    #define wxWC2WXbuf wxCharBuffer
    #define wxWX2WCbuf wxWCharBuffer
#endif // Unicode/ANSI

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
        m_bufdata->DecRef();
        m_bufdata = src.m_bufdata;
        m_bufdata->IncRef();
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
