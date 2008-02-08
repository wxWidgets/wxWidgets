/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/dc.mm
// Purpose:     wxDC
// Author:      David Elliott
// Modified by:
// Created:     2003/04/01
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/dc.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/math.h" //math constants
#endif //WX_PRECOMP

#include "wx/cocoa/autorelease.h"
#include "wx/cocoa/string.h"
#include "wx/cocoa/ObjcRef.h"

#import <AppKit/NSBezierPath.h>
#import <AppKit/NSTextStorage.h>
#import <AppKit/NSLayoutManager.h>
#import <AppKit/NSTextContainer.h>
#import <AppKit/NSGraphicsContext.h>
#import <AppKit/NSAffineTransform.h>
#import <AppKit/NSColor.h>
#import <AppKit/NSTypesetter.h>
#import <AppKit/NSImage.h>

#include "wx/listimpl.cpp"
WX_DEFINE_LIST(wxCocoaDCStack);

IMPLEMENT_ABSTRACT_CLASS(wxDC, wxObject)
WX_NSTextStorage wxDC::sm_cocoaNSTextStorage = nil;
WX_NSLayoutManager wxDC::sm_cocoaNSLayoutManager = nil;
WX_NSTextContainer wxDC::sm_cocoaNSTextContainer = nil;
wxCocoaDCStack wxDC::sm_cocoaDCStack;

inline void CocoaSetPenForNSBezierPath(wxPen &pen, NSBezierPath *bezpath)
{
    [pen.GetNSColor() set];
    const CGFloat *pattern;
    [bezpath setLineDash:pattern count:pen.GetCocoaLineDash(&pattern) phase:0.0];
    [bezpath setLineWidth:pen.GetWidth()];
    switch(pen.GetJoin())
    {
    case wxJOIN_BEVEL:
        [bezpath setLineJoinStyle:NSBevelLineJoinStyle];
        break;
    case wxJOIN_ROUND:
        [bezpath setLineJoinStyle:NSRoundLineJoinStyle];
        break;
    case wxJOIN_MITER:
        [bezpath setLineJoinStyle:NSMiterLineJoinStyle];
        break;
    }
    switch(pen.GetCap())
    {
    case wxCAP_ROUND:
        [bezpath setLineCapStyle:NSRoundLineCapStyle];
        break;
    case wxCAP_PROJECTING:
        [bezpath setLineCapStyle:NSSquareLineCapStyle];
        break;
    case wxCAP_BUTT:
        [bezpath setLineCapStyle:NSButtLineCapStyle];
        break;
    }
}

void wxDC::CocoaInitializeTextSystem()
{
    wxAutoNSAutoreleasePool pool;

    wxASSERT_MSG(!sm_cocoaNSTextStorage && !sm_cocoaNSLayoutManager && !sm_cocoaNSTextContainer,wxT("Text system already initalized!  BAD PROGRAMMER!"));

    // FIXME: Never released
    sm_cocoaNSTextStorage = wxGCSafeRetain([[[NSTextStorage alloc] init] autorelease]);

    // FIXME: Never released
    sm_cocoaNSLayoutManager = wxGCSafeRetain([[[NSLayoutManager alloc] init] autorelease]);

    [sm_cocoaNSTextStorage addLayoutManager:sm_cocoaNSLayoutManager];
    // NSTextStorage retains NSLayoutManager, but so do we
    // [sm_cocoaNSLayoutManager release]; [sm_cocoaNSLayoutManager retain];

    // NOTE:  initWithContainerSize is the designated initializer, but the
    // Apple CircleView sample gets away with just calling init, which
    // is all we really need for our purposes.
    // FIXME: Never released
    sm_cocoaNSTextContainer = wxGCSafeRetain([[[NSTextContainer alloc] init] autorelease]);
    [sm_cocoaNSLayoutManager addTextContainer:sm_cocoaNSTextContainer];
    // NSLayoutManager retains NSTextContainer, but so do we
    // [sm_cocoaNSTextContainer release]; [sm_cocoaNSTextContainer retain];
}

void wxDC::CocoaShutdownTextSystem()
{
    [sm_cocoaNSTextContainer release]; sm_cocoaNSTextContainer = nil;
    [sm_cocoaNSLayoutManager release]; sm_cocoaNSLayoutManager = nil;
    [sm_cocoaNSTextStorage release]; sm_cocoaNSTextStorage = nil;
}

void wxDC::CocoaUnwindStackAndLoseFocus()
{
    wxCocoaDCStack::compatibility_iterator ourNode=sm_cocoaDCStack.Find(this);
    if(ourNode)
    {
        wxCocoaDCStack::compatibility_iterator node=sm_cocoaDCStack.GetFirst();
        for(;node!=ourNode; node=sm_cocoaDCStack.GetFirst())
        {
            wxDC *dc = node->GetData();
            wxASSERT(dc);
            wxASSERT(dc!=this);
            if(!dc->CocoaUnlockFocus())
            {
                wxFAIL_MSG(wxT("Unable to unlock focus on higher-level DC!"));
            }
            sm_cocoaDCStack.Erase(node);
        }
        wxASSERT(node==ourNode);
        wxASSERT(ourNode->GetData() == this);
        ourNode->GetData()->CocoaUnlockFocus();
        sm_cocoaDCStack.Erase(ourNode);
    }
}

bool wxDC::CocoaUnwindStackAndTakeFocus()
{
    wxCocoaDCStack::compatibility_iterator node=sm_cocoaDCStack.GetFirst();
    for(;node;node = sm_cocoaDCStack.GetFirst())
    {
        wxDC *dc = node->GetData();
        wxASSERT(dc);
        // If we're on the stack, then it's unwound enough and we have focus
        if(dc==this)
            return true;
        // If unable to unlockFocus (e.g. wxPaintDC) stop here
        if(!dc->CocoaUnlockFocus())
            break;
        sm_cocoaDCStack.Erase(node);
    }
    return CocoaLockFocus();
}

wxDC::wxDC(void)
{
    m_cocoaWxToBoundsTransform = nil;
    m_pen = *wxBLACK_PEN;
}

wxDC::~wxDC(void)
{
    [m_cocoaWxToBoundsTransform release];
}

bool wxDC::CocoaLockFocus()
{
    return false;
}

bool wxDC::CocoaUnlockFocus()
{
    return false;
}

/*static*/ WX_NSAffineTransform wxDC::CocoaGetWxToBoundsTransform(bool isFlipped, float height)
{
    NSAffineTransform *transform = nil;
    // This transform flips the graphics since wxDC uses top-left origin
    if(!isFlipped)
    {
        // The transform is auto released
        transform = [NSAffineTransform transform];
        /*  x' = 1x + 0y + 0
            y' = 0x + -1y + window's height
        */
        NSAffineTransformStruct matrix = {
            1,  0
        ,   0, -1
        ,   0, height
        };
        [transform setTransformStruct: matrix];
    }
    return transform;
}

void wxDC::CocoaApplyTransformations()
{
    [m_cocoaWxToBoundsTransform concat];
    // TODO: Apply device/logical/user position/scaling transformations
}

void wxDC::CocoaUnapplyTransformations()
{
    // NOTE: You *must* call this with focus held.
    // Undo all transforms so we're back in true Cocoa coords with
    // no scaling or flipping.
    NSAffineTransform *invertTransform;
    invertTransform = [m_cocoaWxToBoundsTransform copy];
    [invertTransform invert];
    [invertTransform concat];
    [invertTransform release];
}

bool wxDC::CocoaGetBounds(void *rectData)
{
    // We don't know what we are so we can't return anything.
    return false;
}

void wxDC::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    wxAutoNSAutoreleasePool pool;
    if(!CocoaTakeFocus()) return;
    NSBezierPath *bezpath = [NSBezierPath bezierPathWithRect:NSMakeRect(x,y,width,height)];
    CocoaSetPenForNSBezierPath(m_pen,bezpath);
    [bezpath stroke];
    [m_brush.GetNSColor() set];
    [bezpath fill];
}

void wxDC::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{
    wxAutoNSAutoreleasePool pool;
    if(!CocoaTakeFocus()) return;
    NSBezierPath *bezpath = [NSBezierPath bezierPath];
    [bezpath moveToPoint:NSMakePoint(x1,y1)];
    [bezpath lineToPoint:NSMakePoint(x2,y2)];

    CocoaSetPenForNSBezierPath(m_pen,bezpath);
    [bezpath stroke];
}

void wxDC::DoGetTextExtent(const wxString& text, wxCoord *x, wxCoord *y, wxCoord *descent, wxCoord *externalLeading, wxFont *theFont) const
{
    wxAutoNSAutoreleasePool pool;
// FIXME: Cache this so it can be used for DoDrawText
    wxASSERT_MSG(sm_cocoaNSTextStorage && sm_cocoaNSLayoutManager && sm_cocoaNSTextContainer, wxT("Text system has not been initialized.  BAD PROGRAMMER!"));
    NSAttributedString *attributedString = [[NSAttributedString alloc]
            initWithString:wxNSStringWithWxString(text.c_str())];
    [sm_cocoaNSTextStorage setAttributedString:attributedString];
    [attributedString release];

    NSRange glyphRange = [sm_cocoaNSLayoutManager glyphRangeForTextContainer:sm_cocoaNSTextContainer];
    NSRect usedRect = [sm_cocoaNSLayoutManager usedRectForTextContainer:sm_cocoaNSTextContainer];
    if(x)
        *x=(int)usedRect.size.width;
    if(y)
        *y=(int)usedRect.size.height;
    if(descent)
        *descent=0;
    if(externalLeading)
        *externalLeading=0;
}

void wxDC::DoDrawText(const wxString& text, wxCoord x, wxCoord y)
{
    wxAutoNSAutoreleasePool pool;
    if(!CocoaTakeFocus()) return;
    wxASSERT_MSG(sm_cocoaNSTextStorage && sm_cocoaNSLayoutManager && sm_cocoaNSTextContainer, wxT("Text system has not been initialized.  BAD PROGRAMMER!"));
    NSAttributedString *attributedString = [[NSAttributedString alloc]
            initWithString:wxNSStringWithWxString(text.c_str())];
    [sm_cocoaNSTextStorage setAttributedString:attributedString];
    [attributedString release];

    // Set the color (and later font) attributes
    NSColor *fgColor = m_textForegroundColour.GetNSColor();
    NSColor *bgColor = m_textBackgroundColour.GetNSColor();
    if(!fgColor)
        fgColor = [NSColor clearColor];
    if(!bgColor)
        bgColor = [NSColor clearColor];
    NSDictionary *attrDict = [[NSDictionary alloc] initWithObjectsAndKeys:
            fgColor, NSForegroundColorAttributeName,
            bgColor, NSBackgroundColorAttributeName,
            nil];
    [sm_cocoaNSTextStorage addAttributes: attrDict range:NSMakeRange(0,[sm_cocoaNSTextStorage length])];
    [attrDict release];

    NSRange glyphRange = [sm_cocoaNSLayoutManager glyphRangeForTextContainer:sm_cocoaNSTextContainer];
    NSRect usedRect = [sm_cocoaNSLayoutManager usedRectForTextContainer:sm_cocoaNSTextContainer];
    // NOTE: We'll crash trying to get the location of glyphAtIndex:0 if
    // there is no length or we don't start at zero
    if(!glyphRange.length)
        return;
    wxASSERT_MSG(glyphRange.location==0,wxT("glyphRange must begin at zero"));

    NSAffineTransform *transform = [NSAffineTransform transform];
    [transform translateXBy:x yBy:y];

    NSAffineTransform *flipTransform = [NSAffineTransform transform];
    /*  x' = 1x + 0y + 0
        y' = 0x + -1y + window's height
    */
    NSAffineTransformStruct matrix = {
        1,  0
    ,   0, -1
    ,   0, usedRect.size.height
    };
    [flipTransform setTransformStruct: matrix];

    NSGraphicsContext *context = [NSGraphicsContext currentContext];
    [context saveGraphicsState];
    [transform concat];
    [flipTransform concat];
    #if 0
    // Draw+fill a rectangle so we can see where the shit is supposed to be.
    wxLogTrace(wxTRACE_COCOA,wxT("(%f,%f) (%fx%f)"),usedRect.origin.x,usedRect.origin.y,usedRect.size.width,usedRect.size.height);
    NSBezierPath *bezpath = [NSBezierPath bezierPathWithRect:NSMakeRect(0,0,usedRect.size.width,usedRect.size.height)];
    [[NSColor blackColor] set];
    [bezpath stroke];
    [[NSColor blueColor] set];
    [bezpath fill];
    #endif

    NSPoint layoutLocation = [sm_cocoaNSLayoutManager locationForGlyphAtIndex:0];
    layoutLocation.x = 0.0;
    layoutLocation.y *= -1.0;

    // Save the location as is for underlining
    NSPoint underlineLocation = layoutLocation;

    // Offset the location by the baseline for drawing the glyphs.
    layoutLocation.y += [[sm_cocoaNSLayoutManager typesetter] baselineOffsetInLayoutManager:sm_cocoaNSLayoutManager glyphIndex:0];

    if(m_backgroundMode==wxSOLID)
        [sm_cocoaNSLayoutManager drawBackgroundForGlyphRange:glyphRange  atPoint:NSZeroPoint];
    [sm_cocoaNSLayoutManager drawGlyphsForGlyphRange:glyphRange  atPoint:layoutLocation];

    int underlineStyle = GetFont().GetUnderlined() ? NSUnderlineStyleSingle : NSUnderlineStyleNone;
    NSRange lineGlyphRange;
    NSRect lineRect = [sm_cocoaNSLayoutManager lineFragmentRectForGlyphAtIndex:0 effectiveRange:&lineGlyphRange];

    [sm_cocoaNSLayoutManager underlineGlyphRange:glyphRange underlineType:underlineStyle
        lineFragmentRect:lineRect lineFragmentGlyphRange:lineGlyphRange
        containerOrigin:underlineLocation];

    [context restoreGraphicsState];
}

// wxDCBase functions
int wxDCBase::DeviceToLogicalX(int x) const
{
    return x;
}

int wxDCBase::DeviceToLogicalY(int y) const
{
    return y;
}

int wxDCBase::DeviceToLogicalXRel(int x) const
{
    return x;
}

int wxDCBase::DeviceToLogicalYRel(int y) const
{
    return y;
}

int wxDCBase::LogicalToDeviceX(int x) const
{
    return x;
}

int wxDCBase::LogicalToDeviceY(int y) const
{
    return y;
}

int wxDCBase::LogicalToDeviceXRel(int x) const
{
    return x;
}

int wxDCBase::LogicalToDeviceYRel(int y) const
{
    return y;
}

///////////////////////////////////////////////////////////////////////////
// cut here, the rest is stubs
///////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// wxDC
//-----------------------------------------------------------------------------

void wxDC::DoDrawIcon( const wxIcon &WXUNUSED(icon), int WXUNUSED(x), int WXUNUSED(y) )
{
};

void wxDC::DoDrawPoint( int x, int y )
{
};

void wxDC::DoDrawPolygon( int, wxPoint *, int, int, int)
{
};

void wxDC::DoDrawLines( int, wxPoint *, int, int )
{
}

int wxDC::GetDepth() const
{
    return 0;
}

wxSize wxDC::GetPPI() const
{
    return wxSize(0,0);
}

bool wxDC::CanGetTextExtent() const
{
    return false;
}

wxCoord wxDC::GetCharHeight() const
{
    return 0;
}

wxCoord wxDC::GetCharWidth() const
{
    return 0;
}

bool wxDC::CanDrawBitmap() const
{
    return true;
}

bool wxDC::DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const
{
    return false;
}

void wxDC::DoDrawArc(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2, wxCoord xc, wxCoord yc)
{
}

void wxDC::SetFont(const wxFont& font)
{
    m_font = font;
}

void wxDC::SetPen(const wxPen& pen)
{
    m_pen = pen;
}

void wxDC::SetBrush(const wxBrush& brush)
{
    m_brush = brush;
}

void wxDC::DoSetClippingRegionAsRegion(const wxRegion& region)
{
}

void wxDC::DoSetClippingRegion(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
}

void wxDC::DestroyClippingRegion()
{
}

void wxDC::DoDrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius)
{
}

void wxDC::DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y, double angle)
{
}

void wxDC::DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h, double sa, double ea)
{
}

void wxDC::DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
}

void wxDC::DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y, bool useMask)
{
    wxAutoNSAutoreleasePool pool;
    if(!CocoaTakeFocus()) return;
    if(!bmp.Ok())
        return;

#if 0
    // Draw a rect so we can see where it's supposed to be
    wxLogTrace(wxTRACE_COCOA,wxT("image at (%d,%d) size %dx%d"),x,y,bmp.GetWidth(),bmp.GetHeight());
    NSBezierPath *bezpath = [NSBezierPath bezierPathWithRect:NSMakeRect(x,y,bmp.GetWidth(),bmp.GetHeight())];
    [[NSColor blackColor] set];
    [bezpath stroke];
    [[NSColor blueColor] set];
    [bezpath fill];
#endif // 0

    NSAffineTransform *transform = [NSAffineTransform transform];
    [transform translateXBy:x yBy:y];

    NSAffineTransform *flipTransform = [NSAffineTransform transform];
    /*  x' = 1x + 0y + 0
        y' = 0x + -1y + window's height
    */
    NSAffineTransformStruct matrix = {
        1,  0
    ,   0, -1
    ,   0, bmp.GetHeight()
    };
    [flipTransform setTransformStruct: matrix];

    NSGraphicsContext *context = [NSGraphicsContext currentContext];
    [context saveGraphicsState];
    [transform concat];
    [flipTransform concat];

    NSImage *nsimage = [bmp.GetNSImage(useMask) retain];

    [nsimage drawAtPoint: NSMakePoint(0,0)
        fromRect: NSMakeRect(0.0,0.0,bmp.GetWidth(),bmp.GetHeight())
        operation: NSCompositeSourceOver
        fraction: 1.0];

    [nsimage release];
    [context restoreGraphicsState];
}

bool wxDC::DoFloodFill(wxCoord x, wxCoord y, const wxColour& col, int style)
{
    return false;
}

void wxDC::DoCrossHair(wxCoord x, wxCoord y)
{
}


bool wxDC::DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height, wxDC *source, wxCoord xsrc, wxCoord ysrc, int rop, bool useMask , wxCoord xsrcMask, wxCoord ysrcMask)
{
    if(!CocoaTakeFocus()) return false;
    if(!source) return false;
    return source->CocoaDoBlitOnFocusedDC(xdest,ydest,width,height,
        xsrc, ysrc, rop, useMask, xsrcMask, ysrcMask);
}

bool wxDC::CocoaDoBlitOnFocusedDC(wxCoord xdest, wxCoord ydest,
    wxCoord width, wxCoord height, wxCoord xsrc, wxCoord ysrc,
    int logicalFunc, bool useMask, wxCoord xsrcMask, wxCoord ysrcMask)
{
    return false;
}

void wxDC::DoGetSize( int* width, int* height ) const
{
  *width = m_maxX-m_minX;
  *height = m_maxY-m_minY;
};

void wxDC::DoGetSizeMM( int* width, int* height ) const
{
  int w = 0;
  int h = 0;
  GetSize( &w, &h );
};

void wxDC::SetTextForeground( const wxColour &col )
{
  if (!Ok()) return;
  m_textForegroundColour = col;
};

void wxDC::SetTextBackground( const wxColour &col )
{
  if (!Ok()) return;
  m_textBackgroundColour = col;
};

void wxDC::Clear()
{
    if(!CocoaTakeFocus()) return;

    NSRect boundsRect;
    if(!CocoaGetBounds(&boundsRect)) return;

    NSGraphicsContext *context = [NSGraphicsContext currentContext];
    [context saveGraphicsState];

    // Undo all transforms so when we draw our bounds rect we
    // really overwrite our bounds rect.
    CocoaUnapplyTransformations();

    [m_backgroundBrush.GetNSColor() set];
    [NSBezierPath fillRect:boundsRect];

    [context restoreGraphicsState];
}

void wxDC::SetBackground(const wxBrush& brush)
{
    m_backgroundBrush = brush;
}

void wxDC::SetPalette(const wxPalette&)
{
}

void wxDC::SetLogicalFunction(int)
{
}


void wxDC::SetMapMode( int mode )
{
  switch (mode)
  {
    case wxMM_TWIPS:
      break;
    case wxMM_POINTS:
      break;
    case wxMM_METRIC:
      break;
    case wxMM_LOMETRIC:
      break;
    default:
    case wxMM_TEXT:
      SetLogicalScale( 1.0, 1.0 );
      break;
  };
  if (mode != wxMM_TEXT)
  {
  };
};

void wxDC::SetUserScale( double x, double y )
{
  // allow negative ? -> no
  m_userScaleX = x;
  m_userScaleY = y;
  ComputeScaleAndOrigin();
};

void wxDC::SetLogicalScale( double x, double y )
{
  // allow negative ?
  m_logicalScaleX = x;
  m_logicalScaleY = y;
  ComputeScaleAndOrigin();
};

void wxDC::SetLogicalOrigin( wxCoord x, wxCoord y )
{
  m_logicalOriginX = x * m_signX;   // is this still correct ?
  m_logicalOriginY = y * m_signY;
  ComputeScaleAndOrigin();
};

void wxDC::SetDeviceOrigin( wxCoord x, wxCoord y )
{
  ComputeScaleAndOrigin();
};

void wxDC::SetAxisOrientation( bool xLeftRight, bool yBottomUp )
{
  m_signX = (xLeftRight ?  1 : -1);
  m_signY = (yBottomUp  ? -1 :  1);
  ComputeScaleAndOrigin();
};

void wxDC::ComputeScaleAndOrigin(void)
{
  // CMB: copy scale to see if it changes
  double origScaleX = m_scaleX;
  double origScaleY = m_scaleY;

  m_scaleX = m_logicalScaleX * m_userScaleX;
  m_scaleY = m_logicalScaleY * m_userScaleY;

  // CMB: if scale has changed call SetPen to recalulate the line width
  if (m_scaleX != origScaleX || m_scaleY != origScaleY)
  {
    // this is a bit artificial, but we need to force wxDC to think
    // the pen has changed
    const wxPen* pen = & GetPen();
    wxPen tempPen;
    m_pen = tempPen;
    SetPen(* pen);
  }
};
