///////////////////////////////////////////////////////////////////////////////
// Name:        wx/buffer.h
// Purpose:     auto buffer classes: buffers which automatically free memory
// Author:      Vadim Zeitlin
// Modified by:
// Created:     12.04.99
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// these classes are for private use only for now, they're not documented

#ifndef _WX_BUFFER_H
#define _WX_BUFFER_H

#include "wx/wxchar.h"

#include <string.h> // strdup

// ----------------------------------------------------------------------------
// Special classes for (wide) character strings: they use malloc/free instead
// of new/delete
// ----------------------------------------------------------------------------

class wxCharBuffer
{
public:
    wxCharBuffer(const char *str)
        : m_str(str ? strdup(str) : NULL)
    {
    }

    wxCharBuffer(size_t len)
        : m_str((char *)malloc((len + 1)*sizeof(char)))
    {
        m_str[len] = '\0';
    }

    // no need to check for NULL, free() does it
    ~wxCharBuffer() { free(m_str); }

    wxCharBuffer(/* non const! */ wxCharBuffer& src)
        : m_str(src.m_str)
    {
        // no reference count yet...
        src.m_str = (char *)NULL;
    }
    wxCharBuffer& operator=(/* non const! */ wxCharBuffer& src)
    {
        m_str = src.m_str;

        // no reference count yet...
        src.m_str = (char *)NULL;
        return *this;
    }

    char *data() { return m_str; }
    const char *data() const { return m_str; }
    operator const char *() const { return m_str; }
    char operator[](size_t n) const { return m_str[n]; }

private:
    char *m_str;
};

#if wxUSE_WCHAR_T

class wxWCharBuffer
{
public:
    wxWCharBuffer(const wchar_t *wcs)
        : m_wcs((wchar_t *)NULL)
    {
        if (wcs) {
          size_t siz = (wxWcslen(wcs) + 1)*sizeof(wchar_t);
          m_wcs = (wchar_t *)malloc(siz);
          memcpy(m_wcs, wcs, siz);
        }
    }
    wxWCharBuffer(size_t len)
        : m_wcs((wchar_t *)malloc((len + 1)*sizeof(wchar_t)))
    {
        m_wcs[len] = L'\0';
    }

    // no need to check for NULL, free() does it
    ~wxWCharBuffer() { free(m_wcs); }

    wxWCharBuffer(/* non const! */ wxWCharBuffer& src)
        : m_wcs(src.m_wcs)
    {
       // no reference count yet...
       src.m_wcs = (wchar_t *)NULL;
    }
    wxWCharBuffer& operator=(/* non const! */ wxWCharBuffer& src)
    {
        m_wcs = src.m_wcs;
        // no reference count yet...
        src.m_wcs = (wchar_t *)NULL;
        return *this;
    }

    wchar_t *data() { return m_wcs; }
    const wchar_t *data() const { return m_wcs; }
    operator const wchar_t *() const { return m_wcs; }
    wchar_t operator[](size_t n) const { return m_wcs[n]; }

private:
    wchar_t *m_wcs;
};

#endif // wxUSE_WCHAR_T

#if wxUSE_UNICODE
    #define wxMB2WXbuf wxWCharBuffer
    #define wxWX2MBbuf wxCharBuffer
    #define wxWC2WXbuf wxChar*
    #define wxWX2WCbuf wxChar*
#else // ANSI
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
    enum { BLOCK_SIZE = 1024 };

    friend class wxMemoryBuffer;

    // everyting is private as it can only be used by wxMemoryBuffer
private:
    wxMemoryBufferData(size_t size = wxMemoryBufferData::BLOCK_SIZE)
        : m_data(size ? malloc(size) : NULL), m_size(size), m_len(0), m_ref(0)
    {
    }
    ~wxMemoryBufferData() { free(m_data); }


    void ResizeIfNeeded(size_t newSize)
    {
        if (newSize > m_size)
        {
            void *dataOld = m_data;
            m_data = realloc(m_data, newSize + wxMemoryBufferData::BLOCK_SIZE);
            if ( !m_data )
            {
                free(dataOld);
            }

            m_size = newSize + wxMemoryBufferData::BLOCK_SIZE;
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
};


class wxMemoryBuffer
{
public:
    // ctor and dtor
    wxMemoryBuffer(size_t size = wxMemoryBufferData::BLOCK_SIZE)
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

    void  AppendData(void* data, size_t len)
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
