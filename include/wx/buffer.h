///////////////////////////////////////////////////////////////////////////////
// Name:        buffer.h
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

#include <string.h> // strdup
#include <wchar.h>  // wchar_t

// ----------------------------------------------------------------------------
// Special classes for (wide) character strings: they use malloc/free instead
// of new/delete
// ----------------------------------------------------------------------------

class wxCharBuffer
{
public:
    wxCharBuffer(const char *str)
    {
        wxASSERT_MSG( str, _T("NULL string in wxCharBuffer") );

        m_str = str ? strdup(str) : (char *)NULL;
    }
    wxCharBuffer(size_t len)
    {
        m_str = (char *)malloc(len+1);
	m_str[len] = '\0';
    }
    // no need to check for NULL, free() does it
   ~wxCharBuffer() { free(m_str); }

   wxCharBuffer(const wxCharBuffer& src)
   {
     m_str = src.m_str;
     // no reference count yet...
     (wxCharBuffer)src.m_str = (char *)NULL;
   }
   wxCharBuffer& operator=(const wxCharBuffer& src)
   {
     m_str = src.m_str;
     // no reference count yet...
     (wxCharBuffer)src.m_str = (char *)NULL;
     return *this;
   }

   operator const char *() const { return m_str; }

private:
   char *m_str;
};

class wxWCharBuffer
{
public:
    wxWCharBuffer(const wchar_t *wcs)
    {
        wxASSERT_MSG( wcs, _T("NULL string in wxWCharBuffer") );

        m_wcs = wcs ? (wchar_t *)malloc((wcslen(wcs)+1)*sizeof(wchar_t))
	            : (wchar_t *)NULL;
	if (m_wcs) wcscpy(m_wcs, wcs);
    }
    wxWCharBuffer(size_t len)
    {
        m_wcs = (wchar_t *)malloc((len+1)*sizeof(wchar_t));
	m_wcs[len] = L'\0';
    }

    // no need to check for NULL, free() does it
   ~wxWCharBuffer() { free(m_wcs); }

   wxWCharBuffer(const wxWCharBuffer& src)
   {
     m_wcs = src.m_wcs;
     // no reference count yet...
     (wxWCharBuffer)src.m_wcs = (wchar_t *)NULL;
   }
   wxWCharBuffer& operator=(const wxWCharBuffer& src)
   {
     m_wcs = src.m_wcs;
     // no reference count yet...
     (wxWCharBuffer)src.m_wcs = (wchar_t *)NULL;
     return *this;
   }

   operator const wchar_t *() const { return m_wcs; }

private:
   wchar_t *m_wcs;
};

#if wxUSE_UNICODE
#define wxMB2WXbuf wxWCharBuffer
#define wxWX2MBbuf wxCharBuffer
#define wxWC2WXbuf wxChar*
#define wxWX2WCbuf wxChar*
#else
#define wxMB2WXbuf wxChar*
#define wxWX2MBbuf wxChar*
#define wxWC2WXbuf wxCharBuffer
#define wxWX2WCbuf wxWCharBuffer
#endif

// ----------------------------------------------------------------------------
// template class for any kind of data
// ----------------------------------------------------------------------------

// TODO

#endif // _WX_BUFFER_H
