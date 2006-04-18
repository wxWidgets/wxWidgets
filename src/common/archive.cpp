/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/archive.cpp
// Purpose:     Streams for archive formats
// Author:      Mike Wetherell
// RCS-ID:      $Id$
// Copyright:   (c) Mike Wetherell
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_STREAMS && wxUSE_ARCHIVE_STREAMS

#ifndef WX_PRECOMP
#endif

#include "wx/archive.h"
#include "wx/link.h"

IMPLEMENT_ABSTRACT_CLASS(wxArchiveEntry, wxObject)
IMPLEMENT_ABSTRACT_CLASS(wxArchiveClassFactory, wxObject)

#if wxUSE_ZIPSTREAM
wxFORCE_LINK_MODULE(zipstrm)
#endif


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

wxArchiveEntry& wxArchiveEntry::operator=(const wxArchiveEntry& WXUNUSED(e))
{
    m_notifier = NULL;
    return *this;
}

#endif // wxUSE_STREAMS && wxUSE_ARCHIVE_STREAMS
