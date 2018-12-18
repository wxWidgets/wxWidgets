/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/utilscocoa.mm
// Purpose:     various cocoa mixin utility functions
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/object.h"
#include "wx/math.h"
#endif

#if wxOSX_USE_COCOA_OR_CARBON
#include <Cocoa/Cocoa.h>
#else
#import <UIKit/UIKit.h>
#endif

#ifdef __WXMAC__
#include "wx/osx/private.h"
#endif

#include "wx/fontutil.h"

#ifdef __WXMAC__

wxMacAutoreleasePool::wxMacAutoreleasePool()
{
    m_pool = [[NSAutoreleasePool alloc] init];
}

wxMacAutoreleasePool::~wxMacAutoreleasePool()
{
    [(NSAutoreleasePool*)m_pool release];
}

#endif

#if wxOSX_USE_COCOA

CGContextRef wxOSXGetContextFromCurrentContext()
{
    CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext]
                                          graphicsPort];
    return context;
}

bool wxOSXLockFocus( WXWidget view)
{
    return [view lockFocusIfCanDraw];
}

void wxOSXUnlockFocus( WXWidget view)
{
    [view unlockFocus];
}

#endif

#if wxOSX_USE_IPHONE

CGContextRef wxOSXGetContextFromCurrentContext()
{
    CGContextRef context = UIGraphicsGetCurrentContext();
    return context;
}

#endif

// ----------------------------------------------------------------------------
// NSObject Utils
// ----------------------------------------------------------------------------

void wxMacCocoaRelease( void* obj )
{
    [(NSObject*)obj release];
}

void wxMacCocoaAutorelease( void* obj )
{
    [(NSObject*)obj autorelease];
}

void* wxMacCocoaRetain( void* obj )
{
    [(NSObject*)obj retain];
    return obj;
}

// ----------------------------------------------------------------------------
// NSFont Utils
// ----------------------------------------------------------------------------

#if wxOSX_USE_COCOA

NSFont* wxFont::OSXGetNSFont() const
{
    wxCHECK_MSG( m_refData != NULL , 0, wxT("invalid font") );

    // cast away constness otherwise lazy font resolution is not possible
    const_cast<wxFont *>(this)->RealizeResource();

    NSFont *font = const_cast<NSFont*>(reinterpret_cast<const NSFont*>(OSXGetCTFont()));

#if MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_12
    // There's a bug in OS X 10.11 (but not present in 10.10 or 10.12) where a
    // toll-free bridged font may have an attributed of private class __NSCFCharacterSet
    // that unlike NSCharacterSet doesn't conform to NSSecureCoding. This poses
    // a problem when such font is used in user-editable content, because some
    // Asian input methods then crash in 10.11 when editing the string.
    // As a workaround for this bug, don't use toll-free bridging, but
    // re-create NSFont from the descriptor instead on buggy OS X versions.
    int osMajor, osMinor;
    wxGetOsVersion(&osMajor, &osMinor, NULL);
    if (osMajor == 10 && osMinor == 11)
    {
        return [NSFont fontWithDescriptor:[font fontDescriptor] size:[font pointSize]];
    }
#endif // MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_12

    return font;
}

#endif

#if wxOSX_USE_IPHONE

UIFont* wxFont::OSXGetUIFont() const
{
    wxCHECK_MSG( m_refData != NULL , 0, wxT("invalid font") );

    // cast away constness otherwise lazy font resolution is not possible
    const_cast<wxFont *>(this)->RealizeResource();

    return const_cast<UIFont*>(reinterpret_cast<const UIFont*>(OSXGetCTFont()));
}

#endif

// ----------------------------------------------------------------------------
// NSWindow Utils
// ----------------------------------------------------------------------------

#if wxOSX_USE_COCOA

WXWindow wxOSXGetMainWindow()
{
    return [NSApp mainWindow];
}

WXWindow wxOSXGetKeyWindow()
{
    return [NSApp keyWindow];
}

#endif
// ----------------------------------------------------------------------------
// NSImage Utils
// ----------------------------------------------------------------------------

#if wxOSX_USE_IPHONE

wxBitmap wxOSXCreateSystemBitmap(const wxString& name, const wxString &client, const wxSize& size)
{
#if 1
    // unfortunately this only accesses images in the app bundle, not the system wide globals
    wxCFStringRef cfname(name);
    return wxBitmap( [[UIImage imageNamed:cfname.AsNSString()] CGImage] );
#else
    return wxBitmap();
#endif
}

#endif

#if wxOSX_USE_COCOA

WXImage wxOSXGetSystemImage(const wxString& name)
{
    wxCFStringRef cfname(name);
    NSImage* nsimage = [NSImage imageNamed:cfname.AsNSString()];
    return nsimage;
}

wxBitmap wxOSXCreateSystemBitmap(const wxString& name, const wxString &client, const wxSize& sizeHint)
{
    NSImage* nsimage = wxOSXGetSystemImage(name);
    if ( nsimage )
    {
        // if ( sizeHint != wxDefaultSize )
        //    [nsimage setSize:NSMakeSize(sizeHint.GetHeight(), sizeHint.GetWidth())];
        return wxBitmap( nsimage );
    }
    return wxNullBitmap;
}

#endif

WXImage  wxOSXGetImageFromCGImage( CGImageRef image, double scaleFactor, bool isTemplate )
{
#if wxOSX_USE_COCOA
    NSSize sz;
    sz.height = CGImageGetHeight(image)/scaleFactor;
    sz.width = CGImageGetWidth(image)/scaleFactor;
    NSImage* newImage = [[NSImage alloc] initWithCGImage:image size:sz];
    
    [newImage setTemplate:isTemplate];

    [newImage autorelease];
    return( newImage );
#else
    return  [UIImage imageWithCGImage:image scale:scaleFactor orientation:UIImageOrientationUp];
#endif
}

#if wxOSX_USE_COCOA
WXImage WXDLLIMPEXP_CORE wxOSXGetNSImageFromIconRef( WXHICON iconref )
{
    NSImage  *newImage = [[NSImage alloc] initWithIconRef:iconref];
    [newImage autorelease];
    return( newImage );
}
#endif

CGImageRef WXDLLIMPEXP_CORE wxOSXGetCGImageFromImage( WXImage nsimage, CGRect* r, CGContextRef cg)
{
#if wxOSX_USE_COCOA
    NSRect nsRect = NSRectFromCGRect(*r);
    return [nsimage CGImageForProposedRect:&nsRect
                               context:[NSGraphicsContext graphicsContextWithGraphicsPort:cg flipped:YES]
                                        hints:nil];
#else
    return [nsimage CGImage];
#endif
}

CGContextRef WXDLLIMPEXP_CORE wxOSXCreateBitmapContextFromImage( WXImage nsimage, bool *isTemplate)
{
    // based on http://www.mail-archive.com/cocoa-dev@lists.apple.com/msg18065.html
    
    CGContextRef hbitmap = NULL;
    if (nsimage != nil)
    {
        double scale = wxOSXGetMainScreenContentScaleFactor();

        CGSize imageSize = wxOSXGetImageSize(nsimage);
        
        hbitmap = CGBitmapContextCreate(NULL, imageSize.width*scale, imageSize.height*scale, 8, 0, wxMacGetGenericRGBColorSpace(), kCGImageAlphaPremultipliedFirst);
        CGContextScaleCTM( hbitmap, scale, scale );
        CGContextClearRect(hbitmap,CGRectMake(0, 0, imageSize.width, imageSize.height));

#if wxOSX_USE_COCOA
        NSGraphicsContext *previousContext = [NSGraphicsContext currentContext];
        NSGraphicsContext *nsGraphicsContext = [NSGraphicsContext graphicsContextWithGraphicsPort:hbitmap flipped:NO];
        [NSGraphicsContext setCurrentContext:nsGraphicsContext];
        [nsimage drawAtPoint:NSZeroPoint fromRect:NSZeroRect operation:NSCompositeCopy fraction:1.0];
        [NSGraphicsContext setCurrentContext:previousContext];

        if (isTemplate)
            *isTemplate = [nsimage isTemplate];
#else
        wxUnusedVar(isTemplate);
        CGContextDrawImage(hbitmap,CGRectMake(0, 0, imageSize.width, imageSize.height),[nsimage CGImage]);
#endif
    }
    return hbitmap;
}

void WXDLLIMPEXP_CORE wxOSXDrawNSImage(
                                          CGContextRef    inContext,
                                          const CGRect *  inBounds,
                                          WXImage      inImage)
{
    if (inImage != nil)
    {
        CGContextSaveGState(inContext);
        CGContextTranslateCTM(inContext, inBounds->origin.x, inBounds->origin.y + inBounds->size.height);
        CGRect r = *inBounds;
        r.origin.x = r.origin.y = 0;
        CGContextScaleCTM(inContext, 1, -1);

#if wxOSX_USE_COCOA
       NSGraphicsContext *previousContext = [NSGraphicsContext currentContext];
        NSGraphicsContext *nsGraphicsContext = [NSGraphicsContext graphicsContextWithGraphicsPort:inContext flipped:NO];
        [NSGraphicsContext setCurrentContext:nsGraphicsContext];
        [inImage drawInRect:NSRectFromCGRect(r) fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];
        [NSGraphicsContext setCurrentContext:previousContext];
#else
        CGContextDrawImage(inContext, *inBounds, [inImage CGImage]);
#endif
        CGContextRestoreGState(inContext);

    }
}

double wxOSXGetMainScreenContentScaleFactor()
{
#if wxOSX_USE_COCOA
    return [[NSScreen mainScreen] backingScaleFactor];
#else
    return [[UIScreen mainScreen] scale];
#endif
}

WXImage wxOSXGetIconForType(OSType type )
{
#if wxOSX_USE_COCOA
    return [[NSWorkspace sharedWorkspace] iconForFileType: NSFileTypeForHFSTypeCode(type)];
#else
    return NULL;
#endif
}

void wxOSXSetImageSize(WXImage image, CGFloat width, CGFloat height)
{
#if wxOSX_USE_COCOA
    [image setSize:NSMakeSize(width, height)];
#else
    // TODO
#endif
}

double wxOSXGetImageScaleFactor(WXImage image)
{
#if wxOSX_USE_COCOA
    NSSize imagesize = [image size];
    int width = [[image bestRepresentationForRect:NSMakeRect(0, 0, imagesize.width, imagesize.height) context:nil hints:nil] pixelsWide];
    if ( width == 0 ) // there are multi-res representations which return 0 for the pixel dimensions
        return wxOSXGetMainScreenContentScaleFactor();

    return width / [image size].width;
#else
    return [image scale];
#endif
}

CGSize wxOSXGetImageSize(WXImage image)
{
#if wxOSX_USE_COCOA
    return NSSizeToCGSize([image size]);
#else
    return [image size];
#endif
}

CGImageRef wxOSXCreateCGImageFromImage( WXImage nsimage, double *scaleptr )
{
    CGImageRef image = NULL;
    if (nsimage != nil)
    {
#if wxOSX_USE_COCOA
        image = [nsimage CGImageForProposedRect:nil context:nil hints:nil];
#else
        image = [nsimage CGImage];
#endif
        CFRetain(image);
        if ( scaleptr )
            *scaleptr = CGImageGetWidth(image)/[nsimage size].width;
    }
    return image;
 }

// ----------------------------------------------------------------------------
// NSCursor Utils
// ----------------------------------------------------------------------------

// copied from cursor.mm

#if wxOSX_USE_COCOA

static NSCursor* wxGetStockCursor( short sIndex )
{
    ClassicCursor* pCursor = &gMacCursors[sIndex];

    //Classic mac cursors are 1bps 16x16 black and white with a
    //identical mask that is 1 for on and 0 for off
    NSImage *theImage = [[NSImage alloc] initWithSize:NSMakeSize(16.0,16.0)];

    //NSCursor takes an NSImage takes a number of Representations - here
    //we need only one for the raw data
    NSBitmapImageRep *theRep = [[NSBitmapImageRep alloc]
        initWithBitmapDataPlanes: NULL  // Tell Cocoa to allocate the planes for us.
        pixelsWide: 16      // All classic cursors are 16x16
        pixelsHigh: 16
        bitsPerSample: 1    // All classic cursors are bitmaps with bitmasks
        samplesPerPixel: 2  // Sample 0:image 1:mask
        hasAlpha: YES       // Identify last sample as a mask
        isPlanar: YES       // Use a separate array for each sample
        colorSpaceName: NSCalibratedWhiteColorSpace // 0.0=black 1.0=white
        bytesPerRow: 2      // Rows in each plane are on 2-byte boundaries (no pad)
        bitsPerPixel: 1];   // same as bitsPerSample since data is planar

    // Ensure that Cocoa allocated 2 and only 2 of the 5 possible planes
    unsigned char *planes[5];
    [theRep getBitmapDataPlanes:planes];
    wxASSERT(planes[0] != NULL);
    wxASSERT(planes[1] != NULL);
    wxASSERT(planes[2] == NULL);
    wxASSERT(planes[3] == NULL);
    wxASSERT(planes[4] == NULL);

    // NOTE1: The Cursor's bits field is white=0 black=1.. thus the bitwise-not
    // Why not use NSCalibratedBlackColorSpace?  Because that reverses the
    // sense of the alpha (mask) plane.
    // NOTE2: The mask data is 0=off 1=on
    // NOTE3: Cocoa asks for "premultiplied" color planes.  Since we have a
    // 1-bit color plane and a 1-bit alpha plane we can just do a bitwise-and
    // on the two.  The original cursor bitmaps have 0 (white actually) for
    // any masked-off pixels.  Therefore every masked-off pixel would be wrong
    // since we bit-flip all of the picture bits.  In practice, Cocoa doesn't
    // seem to care, but we are following the documentation.

    // Fill in the color (black/white) plane
    for(int i=0; i<16; ++i)
    {
        planes[0][2*i  ] = (~pCursor->bits[i] & pCursor->mask[i]) >> 8 & 0xff;
        planes[0][2*i+1] = (~pCursor->bits[i] & pCursor->mask[i]) & 0xff;
    }
    // Fill in the alpha (i.e. mask) plane
    for(int i=0; i<16; ++i)
    {
        planes[1][2*i  ] = pCursor->mask[i] >> 8 & 0xff;
        planes[1][2*i+1] = pCursor->mask[i] & 0xff;
    }

    //add the representation (data) to the image
    [theImage addRepresentation:theRep];

    //create the new cursor
    NSCursor* theCursor =  [[NSCursor alloc]  initWithImage:theImage
                                    hotSpot:NSMakePoint(pCursor->hotspot[1], pCursor->hotspot[0])
                            ];

    //do the usual cleanups
    [theRep release];
    [theImage release];

    //return the new cursor
    return theCursor;
}

WX_NSCursor wxMacCocoaCreateStockCursor( int cursor_type )
{
    WX_NSCursor cursor = nil;
    switch (cursor_type)
    {
    case wxCURSOR_COPY_ARROW:
        cursor = [[NSCursor arrowCursor] retain];
        break;

    case wxCURSOR_WATCH:
    case wxCURSOR_WAIT:
        // an arrow should be displayed by the system when things are running
        // according to the HIG
        // cursor = [[NSCursor arrowCursor] retain];
        // but for crossplatform compatibility we display a watch cursor
        cursor = wxGetStockCursor(kwxCursorWatch);
        break;

    case wxCURSOR_IBEAM:
        cursor = [[NSCursor IBeamCursor] retain];
        break;

    case wxCURSOR_CROSS:
        cursor = [[NSCursor crosshairCursor] retain];
        break;

    case wxCURSOR_SIZENWSE:
        cursor = wxGetStockCursor(kwxCursorSizeNWSE);
        break;

    case wxCURSOR_SIZENESW:
        cursor = wxGetStockCursor(kwxCursorSizeNESW);
        break;

    case wxCURSOR_SIZEWE:
        cursor = [[NSCursor resizeLeftRightCursor] retain];
        break;

    case wxCURSOR_SIZENS:
        cursor = [[NSCursor resizeUpDownCursor] retain];
        break;

    case wxCURSOR_SIZING:
        cursor = wxGetStockCursor(kwxCursorSize);
        break;

    case wxCURSOR_HAND:
        cursor = [[NSCursor pointingHandCursor] retain];
        break;

    case wxCURSOR_BULLSEYE:
        cursor = wxGetStockCursor(kwxCursorBullseye);
        break;

    case wxCURSOR_PENCIL:
        cursor = wxGetStockCursor(kwxCursorPencil);
        break;

    case wxCURSOR_MAGNIFIER:
        cursor = wxGetStockCursor(kwxCursorMagnifier);
        break;

    case wxCURSOR_NO_ENTRY:
        cursor = wxGetStockCursor(kwxCursorNoEntry);
        break;

    case wxCURSOR_PAINT_BRUSH:
        cursor = wxGetStockCursor(kwxCursorPaintBrush);
        break;

    case wxCURSOR_POINT_LEFT:
        cursor = wxGetStockCursor(kwxCursorPointLeft);
        break;

    case wxCURSOR_POINT_RIGHT:
        cursor = wxGetStockCursor(kwxCursorPointRight);
        break;

    case wxCURSOR_QUESTION_ARROW:
        cursor = wxGetStockCursor(kwxCursorQuestionArrow);
        break;

    case wxCURSOR_BLANK:
        cursor = wxGetStockCursor(kwxCursorBlank);
        break;

    case wxCURSOR_RIGHT_ARROW:
        cursor = wxGetStockCursor(kwxCursorRightArrow);
        break;

    case wxCURSOR_SPRAYCAN:
        cursor = wxGetStockCursor(kwxCursorRoller);
        break;

    case wxCURSOR_OPEN_HAND:
        cursor = [[NSCursor openHandCursor] retain];
        break;

    case wxCURSOR_CLOSED_HAND:
        cursor = [[NSCursor closedHandCursor] retain];
        break;

    case wxCURSOR_CHAR:
    case wxCURSOR_ARROW:
    case wxCURSOR_LEFT_BUTTON:
    case wxCURSOR_RIGHT_BUTTON:
    case wxCURSOR_MIDDLE_BUTTON:
    default:
        cursor = [[NSCursor arrowCursor] retain];
        break;
    }
    return cursor;
}

//  C-based style wrapper routines around NSCursor
WX_NSCursor  wxMacCocoaCreateCursorFromCGImage( CGImageRef cgImageRef, float hotSpotX, float hotSpotY )
{
    static BOOL    firstTime  = YES;

    if ( firstTime )
    {
        //  Must first call [[[NSWindow alloc] init] release] to get the NSWindow machinery set up so that NSCursor can use a window to cache the cursor image
        [[[NSWindow alloc] init] release];
        firstTime = NO;
    }

    NSImage    *nsImage  = wxOSXGetImageFromCGImage( cgImageRef );
    NSCursor  *cursor    = [[NSCursor alloc] initWithImage:nsImage hotSpot:NSMakePoint( hotSpotX, hotSpotY )];

    return cursor;
}

void  wxMacCocoaSetCursor( WX_NSCursor cursor )
{
    [cursor set];
}

void  wxMacCocoaHideCursor()
{
    [NSCursor hide];
}

void  wxMacCocoaShowCursor()
{
    [NSCursor unhide];
}
#endif

//---------------------------------------------------------
// helper functions for NSString<->wxString conversion
//---------------------------------------------------------

wxString wxStringWithNSString(NSString *nsstring)
{
#if wxUSE_UNICODE
    return wxString([nsstring UTF8String], wxConvUTF8);
#else
    return wxString([nsstring lossyCString]);
#endif // wxUSE_UNICODE
}

NSString* wxNSStringWithWxString(const wxString &wxstring)
{
#if wxUSE_UNICODE
    return [NSString stringWithUTF8String: wxstring.mb_str(wxConvUTF8)];
#else
    return [NSString stringWithCString: wxstring.c_str() length:wxstring.Len()];
#endif // wxUSE_UNICODE
}

// ----------------------------------------------------------------------------
// helper class for getting the correct system colors according to the
// appearance in effect
// ----------------------------------------------------------------------------

#if wxOSX_USE_COCOA

wxOSXEffectiveAppearanceSetter::wxOSXEffectiveAppearanceSetter()
{
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_14
    if ( wxPlatformInfo::Get().CheckOSVersion(10, 14 ) )
    {
        formerAppearance = NSAppearance.currentAppearance;
        NSAppearance.currentAppearance = NSApp.effectiveAppearance;
    }
#else
    wxUnusedVar(formerAppearance);
#endif
}

wxOSXEffectiveAppearanceSetter::~wxOSXEffectiveAppearanceSetter()
{
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_14
    if ( wxPlatformInfo::Get().CheckOSVersion(10, 14 ) )
        NSAppearance.currentAppearance = (NSAppearance*) formerAppearance;
#endif
}

#endif

