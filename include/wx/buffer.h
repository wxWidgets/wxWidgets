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
        wxASSERT_MSG( str, wxT("NULL string in wxCharBuffer") );
    }

    wxCharBuffer(size_t len)
        : m_str((char *)malloc((len + 1)*sizeof(char)))
    {
        m_str[len] = '\0';
    }

    // no need to check for NULL, free() does it
    ~wxCharBuffer() { free(m_str); }

    wxCharBuffer(const wxCharBuffer& src)
        : m_str(src.m_str)
    {
        // no reference count yet...
        ((wxCharBuffer*)&src)->m_str = (char *)NULL;
    }
    wxCharBuffer& operator=(const wxCharBuffer& src)
    {
        m_str = src.m_str;
        // no reference count yet...
        ((wxCharBuffer*)&src)->m_str = (char *)NULL;
        return *this;
    }

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
        wxASSERT_MSG( wcs, wxT("NULL string in wxWCharBuffer") );

        if (wcs) {
#if ( defined(__BORLANDC__) && (__BORLANDC__ > 0x530) ) \
    || ( defined(__MWERKS__) && defined(__WXMSW__) )
          size_t siz = (std::wcslen(wcs)+1)*sizeof(wchar_t);
#else
          size_t siz = (::wcslen(wcs)+1)*sizeof(wchar_t);
#endif
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

    wxWCharBuffer(const wxWCharBuffer& src)
        : m_wcs(src.m_wcs)
    {
       // no reference count yet...
       ((wxWCharBuffer*)&src)->m_wcs = (wchar_t *)NULL;
    }
    wxWCharBuffer& operator=(const wxWCharBuffer& src)
    {
        m_wcs = src.m_wcs;
        // no reference count yet...
        ((wxWCharBuffer*)&src)->m_wcs = (wchar_t *)NULL;
        return *this;
    }

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

class wxMemoryBuffer
{
public:
    enum { BLOCK_SIZE = 1024 };
    wxMemoryBuffer(size_t size = wxMemoryBuffer::BLOCK_SIZE)
        : m_data(malloc(size)), m_size(size), m_len(0)
    {
    }

    ~wxMemoryBuffer() { free(m_data); }

    // Accessors
    void*  GetData() const    { return m_data; }
    size_t GetBufSize() const { return m_size; }
    size_t GetDataLen() const { return m_len; }

    void   SetBufSize(size_t size) { ResizeIfNeeded(size); }
    void   SetDataLen(size_t len)
    {
        wxASSERT(len <= m_size);
        m_len = len;
    }

    // Ensure the buffer is big enough and return a pointer to it
    void* GetWriteBuf(size_t sizeNeeded)
    {
        ResizeIfNeeded(sizeNeeded);
        return m_data;
    }
    // Update the length after the write
    void  UngetWriteBuf(size_t sizeUsed) { SetDataLen(sizeUsed); }

    // Like the above, but appends to the buffer
    void* GetAppendBuf(size_t sizeNeeded)
    {
        ResizeIfNeeded(m_len + sizeNeeded);
        return (char*)m_data + m_len;
    }
    void  UngetAppendBuf(size_t sizeUsed) { SetDataLen(m_len + sizeUsed); }

    // Other ways to append to the buffer
    void  AppendByte(char data) {
        ResizeIfNeeded(m_len + 1);
        *(((char*)m_data)+m_len) = data;
        m_len += 1;
    }
    void  AppendData(void* data, size_t len)
    {
        memcpy(GetAppendBuf(len), data, len);
        UngetAppendBuf(len);
    }

    operator const char *() const { return (const char*)m_data; }


    // Copy and assignment
    wxMemoryBuffer(const wxMemoryBuffer& src)
        : m_data(src.m_data), m_size(src.m_size), m_len(src.m_len)
    {
        // no reference count yet...
        ((wxMemoryBuffer*)&src)->m_data = NULL;
        ((wxMemoryBuffer*)&src)->m_size = 0;
        ((wxMemoryBuffer*)&src)->m_len  = 0;
    }

    wxMemoryBuffer& operator=(const wxMemoryBuffer& src)
    {
        m_data = src.m_data;
        m_size = src.m_size;
        m_len  = src.m_len;

        // no reference count yet...
        ((wxMemoryBuffer*)&src)->m_data = NULL;
        ((wxMemoryBuffer*)&src)->m_size = 0;
        ((wxMemoryBuffer*)&src)->m_len  = 0;

        return *this;
   }


protected:
    void ResizeIfNeeded(size_t newSize)
    {
        if (newSize > m_size)
        {
            m_data = realloc(m_data, newSize + wxMemoryBuffer::BLOCK_SIZE);
            wxASSERT(m_data != NULL);
            m_size = newSize + wxMemoryBuffer::BLOCK_SIZE;
        }
    }

private:
    void*       m_data;
    size_t      m_size;
    size_t      m_len;
};

// ----------------------------------------------------------------------------
// template class for any kind of data
// ----------------------------------------------------------------------------

// TODO

#endif // _WX_BUFFER_H
