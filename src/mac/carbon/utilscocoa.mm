/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/utils.mm
// Purpose:     various cocoa mixin utility functions
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include <Cocoa/Cocoa.h>

#include "wx/mac/private.h"

bool wxMacInitCocoa()
{
    bool cocoaLoaded = NSApplicationLoad();
    wxASSERT_MSG(cocoaLoaded,wxT("Couldn't load Cocoa in Carbon Environment")) ;
    return cocoaLoaded;
}

wxMacAutoreleasePool::wxMacAutoreleasePool()
{
    m_pool = [[NSAutoreleasePool alloc] init];
}

wxMacAutoreleasePool::~wxMacAutoreleasePool()
{
    [(NSAutoreleasePool*)m_pool release];
}
