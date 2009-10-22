///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/core/evtloop_cf.cpp
// Purpose:     wxEventLoop implementation common to both Carbon and Cocoa
// Author:      Vadim Zeitlin
// Created:     2009-10-18
// RCS-ID:      $Id: wxhead.cpp,v 1.10 2009-06-29 10:23:04 zeitlin Exp $
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/evtloop.h"

#if wxUSE_EVENTLOOP_SOURCE

#ifndef WX_PRECOMP
    #include "wx/log.h"
#endif

#include "wx/evtloopsrc.h"

#include "wx/scopedptr.h"

#include "wx/osx/private.h"
#include "wx/osx/core/cfref.h"

// ============================================================================
// wxCFEventLoopSource and wxCFEventLoop implementation
// ============================================================================

#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5
namespace
{

void EnableDescriptorCallBacks(CFFileDescriptorRef cffd, int flags)
{
    if ( flags & wxEVENT_SOURCE_INPUT )
        CFFileDescriptorEnableCallBacks(cffd, kCFFileDescriptorReadCallBack);
    if ( flags & wxEVENT_SOURCE_OUTPUT )
        CFFileDescriptorEnableCallBacks(cffd, kCFFileDescriptorWriteCallBack);
}

void
wx_cffiledescriptor_callback(CFFileDescriptorRef cffd,
                             CFOptionFlags flags,
                             void *ctxData)
{
    wxLogTrace(wxTRACE_EVT_SOURCE,
               "CFFileDescriptor callback, flags=%d", flags);

    wxCFEventLoopSource * const
        source = static_cast<wxCFEventLoopSource *>(ctxData);

    wxEventLoopSourceHandler * const
        handler = source->GetHandler();
    if ( flags & kCFFileDescriptorReadCallBack )
        handler->OnReadWaiting();
    if ( flags & kCFFileDescriptorWriteCallBack )
        handler->OnWriteWaiting();

    // we need to re-enable callbacks to be called again
    EnableDescriptorCallBacks(cffd, source->GetFlags());
}

} // anonymous namespace

wxEventLoopSource *
wxCFEventLoop::AddSourceForFD(int fd,
                              wxEventLoopSourceHandler *handler,
                              int flags)
{
    wxCHECK_MSG( fd != -1, NULL, "can't monitor invalid fd" );

    wxScopedPtr<wxCFEventLoopSource>
        source(new wxCFEventLoopSource(handler, flags));

    CFFileDescriptorContext ctx = { 0, source.get(), NULL, NULL, NULL };
    wxCFRef<CFFileDescriptorRef>
        cffd(CFFileDescriptorCreate
             (
                  kCFAllocatorDefault,
                  fd,
                  true,   // close on invalidate
                  wx_cffiledescriptor_callback,
                  &ctx
             ));
    if ( !cffd )
        return NULL;

    source->SetFileDescriptor(cffd.release());

    wxCFRef<CFRunLoopSourceRef>
        cfsrc(CFFileDescriptorCreateRunLoopSource(kCFAllocatorDefault, cffd, 0));
    if ( !cfsrc )
        return NULL;

    CFRunLoopRef cfloop = CFGetCurrentRunLoop();
    CFRunLoopAddSource(cfloop, cfsrc, kCFRunLoopDefaultMode);

    return source.release();
}

void wxCFEventLoopSource::SetFileDescriptor(CFFileDescriptorRef cffd)
{
    wxASSERT_MSG( !m_cffd, "shouldn't be called more than once" );

    m_cffd = cffd;
}

wxCFEventLoopSource::~wxCFEventLoopSource()
{
    if ( m_cffd )
        CFRelease(m_cffd);
}

#else // OS X < 10.5

wxEventLoopSource *
wxCFEventLoop::AddSourceForFD(int WXUNUSED(fd),
                              wxEventLoopSourceHandler * WXUNUSED(handler),
                              int WXUNUSED(flags))
{
    return NULL;
}

#endif // MAC_OS_X_VERSION_MAX_ALLOWED

#endif // wxUSE_EVENTLOOP_SOURCE
