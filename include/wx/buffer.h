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

#include <string.h> // strdup()
#include <wchar.h>  // wcsdup()

// ----------------------------------------------------------------------------
// Special classes for (wide) character strings: they use malloc/free instead
// of new/delete
// ----------------------------------------------------------------------------

class wxCharBuffer
{
public:
    wxCharBuffer(const char *str)
    {
        wxASSERT_MSG( str, "NULL string in wxCharBuffer" );

        m_str = str ? strdup(str) : (char *)NULL;
    }

    // no need to check for NULL, free() does it
   ~wxCharBuffer() { free(m_str); }

   operator const char *() const { return m_str; }

private:
   char *m_str;
};

class wxWCharBuffer
{
public:
    wxWCharBuffer(const wchar_t *wcs)
    {
        wxASSERT_MSG( wcs, "NULL string in wxWCharBuffer" );

        m_wcs = wcs ? wcsdup(wcs) : (wchar_t *)NULL;
    }

    // no need to check for NULL, free() does it
   ~wxWCharBuffer() { free(m_wcs); }

   operator const wchar_t *() const { return m_wcs; }

private:
   wchar_t *m_wcs;
};

// ----------------------------------------------------------------------------
// template class for any kind of data
// ----------------------------------------------------------------------------

// TODO

#endif // _WX_BUFFER_H
