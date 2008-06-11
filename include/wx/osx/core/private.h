/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/corefoundation/private.h
// Purpose:     Private declarations: as this header is only included by
//              wxWidgets itself, it may contain identifiers which don't start
//              with "wx".
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: private.h 53819 2008-05-29 14:11:45Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_H_
#define _WX_PRIVATE_H_

#include "wx/defs.h"

#include <CoreFoundation/CoreFoundation.h>

#include "wx/osx/core/cfstring.h"
#include "wx/osx/core/cfdataref.h"

#if wxUSE_GUI

#include <CoreGraphics/CoreGraphics.h>

class WXDLLIMPEXP_CORE wxMacCGContextStateSaver
{
    DECLARE_NO_COPY_CLASS(wxMacCGContextStateSaver)

public:
    wxMacCGContextStateSaver( CGContextRef cg )
    {
        m_cg = cg;
        CGContextSaveGState( cg );
    }
    ~wxMacCGContextStateSaver()
    {
        CGContextRestoreGState( m_cg );
    }
private:
    CGContextRef m_cg;
};


// Quartz

WXDLLIMPEXP_CORE CGImageRef wxMacCreateCGImageFromBitmap( const wxBitmap& bitmap );

WXDLLIMPEXP_CORE CGDataProviderRef wxMacCGDataProviderCreateWithCFData( CFDataRef data );
WXDLLIMPEXP_CORE CGDataConsumerRef wxMacCGDataConsumerCreateWithCFData( CFMutableDataRef data );
WXDLLIMPEXP_CORE CGDataProviderRef wxMacCGDataProviderCreateWithMemoryBuffer( const wxMemoryBuffer& buf );

CGColorSpaceRef WXDLLIMPEXP_CORE wxMacGetGenericRGBColorSpace(void);

#endif // wxUSE_GUI

//---------------------------------------------------------------------------
// cocoa bridging utilities
//---------------------------------------------------------------------------

bool wxMacInitCocoa();

class WXDLLIMPEXP_CORE wxMacAutoreleasePool
{
public :
    wxMacAutoreleasePool();
    ~wxMacAutoreleasePool();
private :
    void* m_pool;
};

// NSObject

void wxMacCocoaRelease( void* obj );
void wxMacCocoaAutorelease( void* obj );
void wxMacCocoaRetain( void* obj );


#endif
    // _WX_PRIVATE_H_
