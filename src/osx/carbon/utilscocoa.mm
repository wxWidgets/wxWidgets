/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/utils.mm
// Purpose:     various cocoa mixin utility functions
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: utilscocoa.mm 48805 2007-09-19 14:52:25Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxOSX_USE_COCOA_OR_CARBON
#include <Cocoa/Cocoa.h>
#else
#import <UIKit/UIKit.h>
#endif

#ifdef __WXMAC__
#include "wx/osx/private.h"
#endif

#ifdef __WXMAC__

#if wxOSX_USE_CARBON
bool wxMacInitCocoa()
{
    bool cocoaLoaded = NSApplicationLoad();
    wxASSERT_MSG(cocoaLoaded,wxT("Couldn't load Cocoa in Carbon Environment")) ;
    return cocoaLoaded;
}
#endif

wxMacAutoreleasePool::wxMacAutoreleasePool()
{
    m_pool = [[NSAutoreleasePool alloc] init];
}

wxMacAutoreleasePool::~wxMacAutoreleasePool()
{
    [(NSAutoreleasePool*)m_pool release];
}

#endif

#if wxOSX_USE_COCOA_OR_IPHONE

CGContextRef wxOSXGetContextFromCurrentNSContext()
{
    CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext]
                                          graphicsPort];
    return context;
}

void wxOSXLockFocus( WXWidget view) 
{
    [view lockFocus];
}

void wxOSXUnlockFocus( WXWidget view) 
{
    [view unlockFocus];
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

void wxMacCocoaRetain( void* obj )
{
    [(NSObject*)obj retain];
}

#if wxOSX_USE_COCOA

// ----------------------------------------------------------------------------
// NSImage Utils
// ----------------------------------------------------------------------------

//  From "Cocoa Drawing Guide:Working with Images"
WX_NSImage  wxOSXCreateNSImageFromCGImage( CGImageRef image )
{
    NSRect      imageRect    = NSMakeRect(0.0, 0.0, 0.0, 0.0);
    
    // Get the image dimensions.
    imageRect.size.height = CGImageGetHeight(image);
    imageRect.size.width = CGImageGetWidth(image);
    
    // Create a new image to receive the Quartz image data.
    NSImage  *newImage = [[NSImage alloc] initWithSize:imageRect.size]; 
    [newImage lockFocus];
    
    // Get the Quartz context and draw.
    CGContextRef  imageContext = (CGContextRef) [[NSGraphicsContext currentContext] graphicsPort];
    CGContextDrawImage( imageContext, *(CGRect*)&imageRect, image );
    [newImage unlockFocus];
    
    /*
        // Create a bitmap rep from the image...
        NSBitmapImageRep *bitmapRep = [[NSBitmapImageRep alloc] initWithCGImage:cgImage];
        // Create an NSImage and add the bitmap rep to it...
        NSImage *image = [[NSImage alloc] init];
        [image addRepresentation:bitmapRep];
        [bitmapRep release];
    */
    [newImage autorelease];
    return( newImage );
}

// ----------------------------------------------------------------------------
// NSCursor Utils
// ----------------------------------------------------------------------------

// copied from cursor.mm

static NSCursor* wxGetStockCursor( short sIndex )
{
    ClassicCursor* pCursor = &gMacCursors[sIndex];

    //Classic mac cursors are 1bps 16x16 black and white with a
    //identical mask that is 1 for on and 0 for off
    NSImage *theImage = [[NSImage alloc] initWithSize:NSMakeSize(16.0,16.0)];

    //NSCursor takes an NSImage takes a number of Representations - here
    //we need only one for the raw data
    NSBitmapImageRep *theRep =
    [[NSBitmapImageRep alloc]
      initWithBitmapDataPlanes:nil  // Allocate the buffer for us :)
      pixelsWide:16
      pixelsHigh:16
      bitsPerSample:1
      samplesPerPixel:2
      hasAlpha:YES                  // Well, more like a mask...
      isPlanar:NO
      colorSpaceName:NSCalibratedWhiteColorSpace // Normal B/W - 0 black 1 white
      bytesPerRow:0     // I don't care - figure it out for me :)
      bitsPerPixel:2];  // bitsPerSample * samplesPerPixel

    //unsigned int is better to put data in then a void*
    //note that working with bitfields would be a lot better here -
    //but since it breaks some compilers...
    wxUint32 *data = (wxUint32 *)[theRep bitmapData];

    //traverse through the bitmap data
    for (int i = 0; i < 16; ++i)
    {
        //bit alpha bit alpha ... :D

        //Notice the = instead of |= -
        //this is to avoid doing a memset earlier
        data[i] = 0;

        //do the rest of those bits and alphas :)
        for (int shift = 0; shift < 32; ++shift)
        {
            const int bit = 1 << (shift >> 1);
            data[i] |= ( !!( (pCursor->mask[i] & bit) ) ) << shift;
            data[i] |= ( !( (pCursor->bits[i] & bit) ) ) << ++shift;
        }
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
        // should be displayed by the system when things are running
        cursor = [[NSCursor arrowCursor] retain];
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
    
    NSImage    *nsImage  = wxOSXCreateNSImageFromCGImage( cgImageRef );
    NSCursor  *cursor    = [[NSCursor alloc] initWithImage:nsImage hotSpot:NSMakePoint( hotSpotX, hotSpotY )];
    
    [nsImage release];
    
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

