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

#include "wx/fontutil.h"

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

WX_NSFont wxFont::OSXCreateNSFont(wxOSXSystemFont font, wxNativeFontInfo* info)
{
    NSFont* nsfont = nil;
    switch( font )
    {
        case wxOSX_SYSTEM_FONT_NORMAL:
            nsfont = [NSFont systemFontOfSize:[NSFont systemFontSize]];
            break;
        case wxOSX_SYSTEM_FONT_BOLD:
            nsfont = [NSFont boldSystemFontOfSize:[NSFont systemFontSize]];
            break;
        case wxOSX_SYSTEM_FONT_SMALL:
            nsfont = [NSFont systemFontOfSize:[NSFont smallSystemFontSize]];
            break;
        case wxOSX_SYSTEM_FONT_SMALL_BOLD:
            nsfont = [NSFont boldSystemFontOfSize:[NSFont smallSystemFontSize]];
            break;
        case wxOSX_SYSTEM_FONT_MINI:
            nsfont = [NSFont systemFontOfSize:
                [NSFont systemFontSizeForControlSize:NSMiniControlSize]];
            break;
       case wxOSX_SYSTEM_FONT_MINI_BOLD:
            nsfont = [NSFont boldSystemFontOfSize:
                [NSFont systemFontSizeForControlSize:NSMiniControlSize]];
            break;
        case wxOSX_SYSTEM_FONT_LABELS:
            nsfont = [NSFont labelFontOfSize:[NSFont labelFontSize]];
            break;
       case wxOSX_SYSTEM_FONT_VIEWS:
            nsfont = [NSFont controlContentFontOfSize:0];
            break;
        default:
            break;
    }
    [nsfont retain];
    NSFontDescriptor*desc = [[nsfont fontDescriptor] retain];
    if ( info->m_faceName.empty())
    {
        wxFontStyle fontstyle = wxFONTSTYLE_NORMAL;
        wxFontWeight fontweight = wxFONTWEIGHT_NORMAL;
        bool underlined = false;
        
        int size = (int) ([desc pointSize]+0.5);
        NSFontSymbolicTraits traits = [desc symbolicTraits];
            
        if ( traits & NSFontBoldTrait )
            fontweight = wxFONTWEIGHT_BOLD ;
        else
            fontweight = wxFONTWEIGHT_NORMAL ;
        if ( traits & NSFontItalicTrait )
            fontstyle = wxFONTSTYLE_ITALIC ;
             
        wxCFStringRef fontname( [desc postscriptName] );
        info->Init(size,wxFONTFAMILY_DEFAULT,fontstyle,fontweight,underlined,
            fontname.AsString(), wxFONTENCODING_DEFAULT);
        
    }
    info->m_nsFontDescriptor = desc;
    return nsfont;
}

void wxNativeFontInfo::OSXValidateNSFontDescriptor()
{
    NSFontDescriptor* desc  = nil;
    NSFontSymbolicTraits traits = 0;
    float weight = 0; 

    if (m_weight == wxFONTWEIGHT_BOLD)
    {
        traits |= NSFontBoldTrait;
        weight = 1.0;
    }
    else if (m_weight == wxFONTWEIGHT_LIGHT)
        weight = -1;
    
    if (m_style == wxFONTSTYLE_ITALIC || m_style == wxFONTSTYLE_SLANT)
        traits |= NSFontItalicTrait;

    desc = [NSFontDescriptor fontDescriptorWithFontAttributes:
        [[NSDictionary alloc] initWithObjectsAndKeys:
            wxCFStringRef(m_faceName).AsNSString(), NSFontFamilyAttribute, 
            [NSNumber numberWithFloat:m_pointSize], NSFontSizeAttribute, 
            [NSNumber numberWithUnsignedInt:traits], NSFontSymbolicTrait, 
            [NSNumber numberWithFloat:weight],NSFontWeightTrait,
            nil]];

    wxMacCocoaRetain(desc);
    m_nsFontDescriptor = desc;
}

WX_NSFont wxFont::OSXCreateNSFont(const wxNativeFontInfo* info)
{
    NSFont* nsFont;
    nsFont = [NSFont fontWithDescriptor:info->m_nsFontDescriptor size:info->m_pointSize];
    wxMacCocoaRetain(nsFont);
    return nsFont;
}

#endif

#if wxOSX_USE_IPHONE

WX_UIFont wxFont::OSXCreateUIFont(wxOSXSystemFont font, wxNativeFontInfo* info)
{
    UIFont* uifont;
    switch( font )
    {
        case wxOSX_SYSTEM_FONT_NORMAL:
            uifont = [UIFont systemFontOfSize:[UIFont systemFontSize]];
            break;
        case wxOSX_SYSTEM_FONT_BOLD:
            uifont = [UIFont boldSystemFontOfSize:[UIFont systemFontSize]];
            break;
        case wxOSX_SYSTEM_FONT_MINI:
        case wxOSX_SYSTEM_FONT_SMALL:
            uifont = [UIFont systemFontOfSize:[UIFont smallSystemFontSize]];
            break;
        case wxOSX_SYSTEM_FONT_MINI_BOLD:
        case wxOSX_SYSTEM_FONT_SMALL_BOLD:
            uifont = [UIFont boldSystemFontOfSize:[UIFont smallSystemFontSize]];
            break;
        case wxOSX_SYSTEM_FONT_VIEWS:
        case wxOSX_SYSTEM_FONT_LABELS:
            uifont = [UIFont systemFontOfSize:[UIFont labelFontSize]];
            break;
        default:
            break;
    }
    [uifont retain];
    if ( info->m_faceName.empty())
    {
        wxFontStyle fontstyle = wxFONTSTYLE_NORMAL;
        wxFontWeight fontweight = wxFONTWEIGHT_NORMAL;
        bool underlined = false;
        
        int size = (int) ([uifont pointSize]+0.5);
        /*
        NSFontSymbolicTraits traits = [desc symbolicTraits];
            
        if ( traits & NSFontBoldTrait )
            fontweight = wxFONTWEIGHT_BOLD ;
        else
            fontweight = wxFONTWEIGHT_NORMAL ;
        if ( traits & NSFontItalicTrait )
            fontstyle = wxFONTSTYLE_ITALIC ;
        */
        wxCFStringRef fontname( [uifont familyName] );
        info->Init(size,wxFONTFAMILY_DEFAULT,fontstyle,fontweight,underlined,
            fontname.AsString(), wxFONTENCODING_DEFAULT);
        
    }
    return uifont;
}

WX_UIFont wxFont::OSXCreateUIFont(const wxNativeFontInfo* info)
{
    UIFont* uiFont;
    uiFont = [UIFont fontWithName:wxCFStringRef(info->m_faceName).AsNSString() size:info->m_pointSize];
    wxMacCocoaRetain(uiFont);
    return uiFont;
}

#endif
// ----------------------------------------------------------------------------
// NSImage Utils
// ----------------------------------------------------------------------------

#if wxOSX_USE_COCOA

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

