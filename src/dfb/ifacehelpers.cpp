/////////////////////////////////////////////////////////////////////////////
// Name:        src/dfb/ifacehelpers.cpp
// Purpose:     helpers for dealing with DFB interfaces
// Author:      Vaclav Slavik
// Created:     2006-08-09
// RCS-ID:      $Id$
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/dfb/ifacehelpers.h"
#include <directfb.h>

//-----------------------------------------------------------------------------
// wxDfbPtr
//-----------------------------------------------------------------------------

// NB: We can't call AddRef() and Release() directly from wxDfbPtr<T> because
//     we don't have full type declarations for them if we don't include
//     <directfb.h> (which user wx headers can't). Fortunately, all DirectFB
//     interfaces are structs with common beginning that contains pointers to
//     AddRef and Release, so we can safely cast interface pointer to void* and
//     then back to arbitrary interface pointer and these calls will still work
//     correctly. For this purpose, we declare a dummy DFB interface here.

DECLARE_INTERFACE(wxDummyDFBInterface)
DEFINE_INTERFACE(wxDummyDFBInterface, )

/* static */
void wxDfbPtrBase::DoAddRef(void *ptr)
{
    wxDummyDFBInterface *p = (wxDummyDFBInterface*)ptr;
    p->AddRef(p);
}

/* static */
void wxDfbPtrBase::DoRelease(void *ptr)
{
    wxDummyDFBInterface *p = (wxDummyDFBInterface*)ptr;
    p->Release(p);
}
