/////////////////////////////////////////////////////////////////////////////
// Name:        archive.cpp
// Purpose:     Streams for archive formats
// Author:      Mike Wetherell
// RCS-ID:      $Id$
// Copyright:   (c) Mike Wetherell
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
  #pragma implementation "archive.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include "wx/defs.h"
#endif

#if wxUSE_ZLIB && wxUSE_STREAMS && wxUSE_ZIPSTREAM

#include "wx/archive.h"
#include "wx/html/forcelnk.h"

IMPLEMENT_ABSTRACT_CLASS(wxArchiveEntry, wxObject)
IMPLEMENT_ABSTRACT_CLASS(wxArchiveClassFactory, wxObject)

FORCE_LINK(zipstrm)


/////////////////////////////////////////////////////////////////////////////
// wxArchiveInputStream

wxArchiveInputStream::wxArchiveInputStream(wxInputStream& stream,
                                           wxMBConv& conv)
  : wxFilterInputStream(stream),
    m_conv(conv)
{
}


/////////////////////////////////////////////////////////////////////////////
// wxArchiveOutputStream

wxArchiveOutputStream::wxArchiveOutputStream(wxOutputStream& stream,
                                             wxMBConv& conv)
  : wxFilterOutputStream(stream),
    m_conv(conv)
{
}


/////////////////////////////////////////////////////////////////////////////
// wxArchiveEntry

void wxArchiveEntry::SetNotifier(wxArchiveNotifier& notifier)
{
    UnsetNotifier();
    m_notifier = &notifier;
    m_notifier->OnEntryUpdated(*this);
}

wxArchiveEntry& wxArchiveEntry::operator=(const wxArchiveEntry& entry)
{
    m_notifier = entry.m_notifier;
    return *this;
}

#endif
