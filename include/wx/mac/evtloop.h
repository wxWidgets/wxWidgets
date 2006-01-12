///////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/mac/evtloop.h
// Purpose:     simply forwards to wx/mac/carbon/evtloop.h for consistency with
//              the other Mac headers
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2006-01-12
// RCS-ID:      $Id$
// Copyright:   (c) 2006 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __WXMAC_CLASSIC__
    #error "wxEventLoop is not implemented for Classic build."
#else
    #include "wx/mac/carbon/evtloop.h"
#endif

