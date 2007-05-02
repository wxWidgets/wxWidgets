/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/dc.cpp
// Purpose:     wxDC class
// Author:      Stefan Csomor
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/dc.h"

#if !wxMAC_USE_CORE_GRAPHICS

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/app.h"
    #include "wx/dcmemory.h"
    #include "wx/dcprint.h"
    #include "wx/region.h"
    #include "wx/image.h"
#endif

#include "wx/mac/uma.h"

#ifdef __MSL__
    #if __MSL__ >= 0x6000
        namespace std {}
        using namespace std ;
    #endif
#endif

#include "wx/mac/private.h"
#ifndef __DARWIN__
#include <ATSUnicode.h>
#include <TextCommon.h>
#include <TextEncodingConverter.h>
#endif


// set to 0 if problems arise
#define wxMAC_EXPERIMENTAL_DC 1


IMPLEMENT_ABSTRACT_CLASS(wxDC, wxObject)

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

const double RAD2DEG  = 180.0 / M_PI;
const short kEmulatedMode = -1 ;
const short kUnsupportedMode = -2 ;

wxMacPortSetter::wxMacPortSetter( const wxDC* dc ) :
    m_ph( (GrafPtr) dc->m_macPort )
{
    wxASSERT( dc->Ok() ) ;
    m_dc = dc ;
    dc->MacSetupPort(&m_ph) ;
}

wxMacPortSetter::~wxMacPortSetter()
{
    m_dc->MacCleanupPort(&m_ph) ;
}

#if wxMAC_EXPERIMENTAL_DC
class wxMacFastPortSetter
{
public :
    wxMacFastPortSetter( const wxDC *dc )
    {
        wxASSERT( dc->Ok() ) ;
        m_swapped = QDSwapPort( (GrafPtr) dc->m_macPort , &m_oldPort ) ;
        m_clipRgn = NewRgn() ;
        GetClip( m_clipRgn ) ;
        m_dc = dc ;
        dc->MacSetupPort( NULL ) ;
    }

    ~wxMacFastPortSetter()
    {
        // SetPort( (GrafPtr) m_dc->m_macPort ) ;
        SetClip( m_clipRgn ) ;
        if ( m_swapped )
            SetPort( m_oldPort ) ;
        m_dc->MacCleanupPort( NULL ) ;
        DisposeRgn( m_clipRgn ) ;
    }

private :
    bool m_swapped ;
    RgnHandle m_clipRgn ;
    GrafPtr m_oldPort ;
    const wxDC*   m_dc ;
} ;

#else
typedef wxMacPortSetter wxMacFastPortSetter ;
#endif

wxMacWindowClipper::wxMacWindowClipper( const wxWindow* win ) :
    wxMacPortSaver( (GrafPtr) GetWindowPort((WindowRef) win->MacGetTopLevelWindowRef()) )
{
    m_newPort =(GrafPtr) GetWindowPort((WindowRef) win->MacGetTopLevelWindowRef()) ;
    m_formerClip = NewRgn() ;
    m_newClip = NewRgn() ;
    GetClip( m_formerClip ) ;

    if ( win )
    {
        // guard against half constructed objects, this just leads to a empty clip
        if ( win->GetPeer() )
        {
            int x = 0 , y = 0;
            win->MacWindowToRootWindow( &x, &y ) ;

            // get area including focus rect
            CopyRgn( (RgnHandle) ((wxWindow*)win)->MacGetVisibleRegion(true).GetWXHRGN() , m_newClip ) ;
            if ( !EmptyRgn( m_newClip ) )
                OffsetRgn( m_newClip , x , y ) ;
        }

        SetClip( m_newClip ) ;
    }
}

wxMacWindowClipper::~wxMacWindowClipper()
{
    SetPort( m_newPort ) ;
    SetClip( m_formerClip ) ;
    DisposeRgn( m_newClip ) ;
    DisposeRgn( m_formerClip ) ;
}

wxMacWindowStateSaver::wxMacWindowStateSaver( const wxWindow* win ) :
    wxMacWindowClipper( win )
{
    // the port is already set at this point
    m_newPort = (GrafPtr) GetWindowPort((WindowRef) win->MacGetTopLevelWindowRef()) ;
    GetThemeDrawingState( &m_themeDrawingState ) ;
}

wxMacWindowStateSaver::~wxMacWindowStateSaver()
{
    SetPort( m_newPort ) ;
    SetThemeDrawingState( m_themeDrawingState , true ) ;
}

//-----------------------------------------------------------------------------
// wxDC
//-----------------------------------------------------------------------------
// this function emulates all wx colour manipulations, used to verify the implementation
// by setting the mode in the blitting functions to kEmulatedMode
void wxMacCalculateColour( int logical_func , const RGBColor &srcColor , RGBColor &dstColor ) ;

void wxMacCalculateColour( int logical_func , const RGBColor &srcColor , RGBColor &dstColor )
{
    switch ( logical_func )
    {
        case wxAND:        // src AND dst
            dstColor.red = dstColor.red & srcColor.red ;
            dstColor.green = dstColor.green & srcColor.green ;
            dstColor.blue = dstColor.blue & srcColor.blue ;
            break ;

        case wxAND_INVERT: // (NOT src) AND dst
            dstColor.red = dstColor.red & ~srcColor.red ;
            dstColor.green = dstColor.green & ~srcColor.green ;
            dstColor.blue = dstColor.blue & ~srcColor.blue ;
            break ;

        case wxAND_REVERSE:// src AND (NOT dst)
            dstColor.red = ~dstColor.red & srcColor.red ;
            dstColor.green = ~dstColor.green & srcColor.green ;
            dstColor.blue = ~dstColor.blue & srcColor.blue ;
            break ;

        case wxCLEAR:      // 0
            dstColor.red = 0 ;
            dstColor.green = 0 ;
            dstColor.blue = 0 ;
            break ;

        case wxCOPY:       // src
            dstColor.red = srcColor.red ;
            dstColor.green = srcColor.green ;
            dstColor.blue = srcColor.blue ;
            break ;

        case wxEQUIV:      // (NOT src) XOR dst
            dstColor.red = dstColor.red ^ ~srcColor.red ;
            dstColor.green = dstColor.green ^ ~srcColor.green ;
            dstColor.blue = dstColor.blue ^ ~srcColor.blue ;
            break ;

        case wxINVERT:     // NOT dst
            dstColor.red = ~dstColor.red ;
            dstColor.green = ~dstColor.green ;
            dstColor.blue = ~dstColor.blue ;
            break ;

        case wxNAND:       // (NOT src) OR (NOT dst)
            dstColor.red = ~dstColor.red | ~srcColor.red ;
            dstColor.green = ~dstColor.green | ~srcColor.green ;
            dstColor.blue = ~dstColor.blue | ~srcColor.blue ;
            break ;

        case wxNOR:        // (NOT src) AND (NOT dst)
            dstColor.red = ~dstColor.red & ~srcColor.red ;
            dstColor.green = ~dstColor.green & ~srcColor.green ;
            dstColor.blue = ~dstColor.blue & ~srcColor.blue ;
            break ;

        case wxOR:         // src OR dst
            dstColor.red = dstColor.red | srcColor.red ;
            dstColor.green = dstColor.green | srcColor.green ;
            dstColor.blue = dstColor.blue | srcColor.blue ;
            break ;

        case wxOR_INVERT:  // (NOT src) OR dst
            dstColor.red = dstColor.red | ~srcColor.red ;
            dstColor.green = dstColor.green | ~srcColor.green ;
            dstColor.blue = dstColor.blue | ~srcColor.blue ;
            break ;

        case wxOR_REVERSE: // src OR (NOT dst)
            dstColor.red = ~dstColor.red | srcColor.red ;
            dstColor.green = ~dstColor.green | srcColor.green ;
            dstColor.blue = ~dstColor.blue | srcColor.blue ;
            break ;

        case wxSET:        // 1
            dstColor.red = 0xFFFF ;
            dstColor.green = 0xFFFF ;
            dstColor.blue = 0xFFFF ;
            break ;

        case wxSRC_INVERT: // (NOT src)
            dstColor.red = ~srcColor.red ;
            dstColor.green = ~srcColor.green ;
            dstColor.blue = ~srcColor.blue ;
            break ;

        case wxXOR:        // src XOR dst
            dstColor.red = dstColor.red ^ srcColor.red ;
            dstColor.green = dstColor.green ^ srcColor.green ;
            dstColor.blue = dstColor.blue ^ srcColor.blue ;
            break ;

        case wxNO_OP:      // dst
        default:
            break ;
    }
}

wxDC::wxDC()
{
    m_ok = false;
    m_colour = true;
    m_macPort = NULL ;
    m_macMask = NULL ;
    m_macFontInstalled = false ;
    m_macBrushInstalled = false ;
    m_macPenInstalled = false ;
    m_macBoundaryClipRgn = NewRgn() ;
    m_macCurrentClipRgn = NewRgn() ;
    SetRectRgn( (RgnHandle) m_macBoundaryClipRgn , -32000 , -32000 , 32000 , 32000 ) ;
    SetRectRgn( (RgnHandle) m_macCurrentClipRgn , -32000 , -32000 , 32000 , 32000 ) ;
    m_pen = *wxBLACK_PEN;
    m_font = *wxNORMAL_FONT;
    m_brush = *wxWHITE_BRUSH;

#ifdef __WXDEBUG__
    // needed to debug possible errors with two active drawing methods at the same time on
    // the same DC
    m_macCurrentPortStateHelper = NULL ;
#endif

    m_macATSUIStyle = NULL ;
    m_macAliasWasEnabled = false;
    m_macForegroundPixMap = NULL ;
    m_macBackgroundPixMap = NULL ;
}

wxDC::~wxDC(void)
{
    DisposeRgn( (RgnHandle) m_macBoundaryClipRgn ) ;
    DisposeRgn( (RgnHandle) m_macCurrentClipRgn ) ;
}

void wxDC::MacSetupPort(wxMacPortStateHelper* help) const
{
#ifdef __WXDEBUG__
    wxASSERT( m_macCurrentPortStateHelper == NULL ) ;
    m_macCurrentPortStateHelper = help ;
#endif

    SetClip( (RgnHandle) m_macCurrentClipRgn);

#if ! wxMAC_EXPERIMENTAL_DC
    m_macFontInstalled = false ;
    m_macBrushInstalled = false ;
    m_macPenInstalled = false ;
#endif
}

void wxDC::MacCleanupPort(wxMacPortStateHelper* help) const
{
#ifdef __WXDEBUG__
    wxASSERT( m_macCurrentPortStateHelper == help ) ;
    m_macCurrentPortStateHelper = NULL ;
#endif

    if ( m_macATSUIStyle )
    {
        ::ATSUDisposeStyle((ATSUStyle)m_macATSUIStyle);
        m_macATSUIStyle = NULL ;
    }

    if ( m_macAliasWasEnabled )
    {
        SetAntiAliasedTextEnabled(m_macFormerAliasState, m_macFormerAliasSize);
        m_macAliasWasEnabled = false ;
    }

    if ( m_macForegroundPixMap )
    {
        Pattern blackColor ;
        ::PenPat(GetQDGlobalsBlack(&blackColor));
        DisposePixPat( (PixPatHandle) m_macForegroundPixMap ) ;
        m_macForegroundPixMap = NULL ;
    }

    if ( m_macBackgroundPixMap )
    {
        Pattern whiteColor ;
        ::BackPat(GetQDGlobalsWhite(&whiteColor));
        DisposePixPat( (PixPatHandle) m_macBackgroundPixMap ) ;
        m_macBackgroundPixMap = NULL ;
    }
}

void wxDC::DoDrawBitmap( const wxBitmap &bmp, wxCoord x, wxCoord y, bool useMask )
{
     wxCHECK_RET( Ok(), wxT("wxDC::DoDrawBitmap - invalid DC") );
     wxCHECK_RET( bmp.Ok(), wxT("wxDC::DoDrawBitmap - invalid bitmap") );

     wxMacFastPortSetter helper(this) ;
     wxCoord xx = XLOG2DEVMAC(x);
     wxCoord yy = YLOG2DEVMAC(y);
     wxCoord w = bmp.GetWidth();
     wxCoord h = bmp.GetHeight();
     wxCoord ww = XLOG2DEVREL(w);
     wxCoord hh = YLOG2DEVREL(h);

     // Set up drawing mode
     short  mode = (m_logicalFunction == wxCOPY ? srcCopy :
                    //m_logicalFunction == wxCLEAR ? WHITENESS :
                    //m_logicalFunction == wxSET ? BLACKNESS :
                    m_logicalFunction == wxINVERT ? hilite :
                   //m_logicalFunction == wxAND ? MERGECOPY :
                    m_logicalFunction == wxOR ? srcOr :
                    m_logicalFunction == wxSRC_INVERT ? notSrcCopy :
                    m_logicalFunction == wxXOR ? srcXor :
                    m_logicalFunction == wxOR_REVERSE ? notSrcOr :
                    //m_logicalFunction == wxAND_REVERSE ? SRCERASE :
                    //m_logicalFunction == wxSRC_OR ? srcOr :
                    //m_logicalFunction == wxSRC_AND ? SRCAND :
                    srcCopy );

     GWorldPtr maskworld = NULL ;
     GWorldPtr bmapworld = MAC_WXHBITMAP( bmp.GetHBITMAP((WXHBITMAP*)&maskworld) );
     PixMapHandle bmappixels ;

     // Set foreground and background colours (for bitmaps depth = 1)
     if (bmp.GetDepth() == 1)
     {
         RGBColor fore = MAC_WXCOLORREF(m_textForegroundColour.GetPixel());
         RGBColor back = MAC_WXCOLORREF(m_textBackgroundColour.GetPixel());
         RGBForeColor(&fore);
         RGBBackColor(&back);
     }
     else
     {
         RGBColor white = { 0xFFFF, 0xFFFF, 0xFFFF } ;
         RGBColor black = { 0, 0, 0 } ;
         RGBForeColor( &black ) ;
         RGBBackColor( &white ) ;
     }
     bmappixels = GetGWorldPixMap( bmapworld ) ;

     wxCHECK_RET(LockPixels(bmappixels),
                 wxT("wxDC::DoDrawBitmap - failed to lock pixels"));

     Rect source = { 0, 0, h, w };
     Rect dest   = { yy, xx, yy + hh, xx + ww };
     if ( useMask && maskworld )
     {
         if ( LockPixels(GetGWorldPixMap(MAC_WXHBITMAP(maskworld))))
         {
             CopyDeepMask
                 (
                  GetPortBitMapForCopyBits(bmapworld),
                  GetPortBitMapForCopyBits(MAC_WXHBITMAP(maskworld)),
                  GetPortBitMapForCopyBits( MAC_WXHBITMAP(m_macPort) ),
                  &source, &source, &dest, mode, NULL
                  );
             UnlockPixels(GetGWorldPixMap(MAC_WXHBITMAP(maskworld)));
         }
     }
     else
     {
         CopyBits( GetPortBitMapForCopyBits( bmapworld ),
                   GetPortBitMapForCopyBits( MAC_WXHBITMAP(m_macPort) ),
                   &source, &dest, mode, NULL ) ;
     }
     UnlockPixels( bmappixels ) ;

     m_macPenInstalled = false ;
     m_macBrushInstalled = false ;
     m_macFontInstalled = false ;
}

void wxDC::DoDrawIcon( const wxIcon &icon, wxCoord x, wxCoord y )
{
    wxCHECK_RET(Ok(), wxT("wxDC::DoDrawIcon - invalid DC"));
    wxCHECK_RET(icon.Ok(), wxT("wxDC::DoDrawIcon - invalid icon"));

    wxMacFastPortSetter helper(this) ;

    wxCoord xx = XLOG2DEVMAC(x);
    wxCoord yy = YLOG2DEVMAC(y);
    wxCoord w = icon.GetWidth();
    wxCoord h = icon.GetHeight();
    wxCoord ww = XLOG2DEVREL(w);
    wxCoord hh = YLOG2DEVREL(h);

    Rect r = { yy , xx, yy + hh, xx + ww } ;
    PlotIconRef( &r , kAlignNone , kTransformNone , kPlotIconRefNormalFlags , MAC_WXHICON( icon.GetHICON() ) ) ;
}

void wxDC::DoSetClippingRegion( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    wxCHECK_RET(Ok(), wxT("wxDC::DoSetClippingRegion - invalid DC"));

    wxCoord xx, yy, ww, hh;
    xx = XLOG2DEVMAC(x);
    yy = YLOG2DEVMAC(y);
    ww = XLOG2DEVREL(width);
    hh = YLOG2DEVREL(height);

    SetRectRgn( (RgnHandle) m_macCurrentClipRgn , xx , yy , xx + ww , yy + hh ) ;
    SectRgn( (RgnHandle) m_macCurrentClipRgn , (RgnHandle) m_macBoundaryClipRgn , (RgnHandle) m_macCurrentClipRgn ) ;
    if ( m_clipping )
    {
        m_clipX1 = wxMax( m_clipX1 , xx );
        m_clipY1 = wxMax( m_clipY1 , yy );
        m_clipX2 = wxMin( m_clipX2, (xx + ww) );
        m_clipY2 = wxMin( m_clipY2, (yy + hh) );
    }
    else
    {
        m_clipping = true;
        m_clipX1 = xx;
        m_clipY1 = yy;
        m_clipX2 = xx + ww;
        m_clipY2 = yy + hh;
    }
}

void wxDC::DoSetClippingRegionAsRegion( const wxRegion &region )
{
    wxCHECK_RET(Ok(), wxT("wxDC::DoSetClippingRegionAsRegion - invalid DC"));

    wxMacFastPortSetter helper(this) ;
    wxCoord x, y, w, h;
    region.GetBox( x, y, w, h );
    wxCoord xx, yy, ww, hh;
    xx = XLOG2DEVMAC(x);
    yy = YLOG2DEVMAC(y);
    ww = XLOG2DEVREL(w);
    hh = YLOG2DEVREL(h);

    // if we have a scaling that we cannot map onto native regions
    // we must use the box
    if ( ww != w || hh != h )
    {
        wxDC::DoSetClippingRegion( x, y, w, h );
    }
    else
    {
        CopyRgn( (RgnHandle) region.GetWXHRGN() , (RgnHandle) m_macCurrentClipRgn ) ;
        if ( xx != x || yy != y )
            OffsetRgn( (RgnHandle) m_macCurrentClipRgn , xx - x , yy - y ) ;

        SectRgn( (RgnHandle) m_macCurrentClipRgn , (RgnHandle) m_macBoundaryClipRgn , (RgnHandle) m_macCurrentClipRgn ) ;
        if ( m_clipping )
        {
            m_clipX1 = wxMax( m_clipX1 , xx );
            m_clipY1 = wxMax( m_clipY1 , yy );
            m_clipX2 = wxMin( m_clipX2, (xx + ww) );
            m_clipY2 = wxMin( m_clipY2, (yy + hh) );
        }
        else
        {
            m_clipping = true;
            m_clipX1 = xx;
            m_clipY1 = yy;
            m_clipX2 = xx + ww;
            m_clipY2 = yy + hh;
        }
    }
}

void wxDC::DestroyClippingRegion()
{
    wxMacFastPortSetter helper(this) ;

    CopyRgn( (RgnHandle) m_macBoundaryClipRgn , (RgnHandle) m_macCurrentClipRgn ) ;
    ResetClipping();
}

void wxDC::DoGetSizeMM( int* width, int* height ) const
{
    int w = 0, h = 0;

    GetSize( &w, &h );
    if (width)
        *width = long( double(w) / (m_scaleX * m_mm_to_pix_x) );
    if (height)
        *height = long( double(h) / (m_scaleY * m_mm_to_pix_y) );
}

void wxDC::SetTextForeground( const wxColour &col )
{
    wxCHECK_RET(Ok(), wxT("wxDC::SetTextForeground - invalid DC"));

    m_textForegroundColour = col;
    m_macFontInstalled = false ;
}

void wxDC::SetTextBackground( const wxColour &col )
{
    wxCHECK_RET(Ok(), wxT("wxDC::SetTextBackground - invalid DC"));

    m_textBackgroundColour = col;
    m_macFontInstalled = false ;
}

wxSize wxDC::GetPPI() const
{
    return wxSize(72, 72);
}

int wxDC::GetDepth() const
{
    if ( IsPortColor( (CGrafPtr) m_macPort ) )
        return ( (**GetPortPixMap( (CGrafPtr) m_macPort)).pixelSize ) ;

    return 1 ;
}

void wxDC::SetPalette( const wxPalette& palette )
{
}

void wxDC::SetBackgroundMode( int mode )
{
    m_backgroundMode = mode ;
}

void wxDC::SetFont( const wxFont &font )
{
    m_font = font;
    m_macFontInstalled = false ;
}

void wxDC::SetPen( const wxPen &pen )
{
    if ( m_pen == pen )
        return ;

    m_pen = pen;
    m_macPenInstalled = false ;
}

void wxDC::SetBrush( const wxBrush &brush )
{
    if (m_brush == brush)
        return;

    m_brush = brush;
    m_macBrushInstalled = false ;
}

void wxDC::SetBackground( const wxBrush &brush )
{
    if (m_backgroundBrush == brush)
        return;

    m_backgroundBrush = brush;
    if (m_backgroundBrush.Ok())
        m_macBrushInstalled = false ;
}

void wxDC::SetLogicalFunction( int function )
{
    if (m_logicalFunction == function)
        return;

    m_logicalFunction = function ;
    m_macFontInstalled = false ;
    m_macBrushInstalled = false ;
    m_macPenInstalled = false ;
}

extern bool wxDoFloodFill(wxDC *dc, wxCoord x, wxCoord y,
                          const wxColour & col, int style);

bool wxDC::DoFloodFill(wxCoord x, wxCoord y,
                       const wxColour& col, int style)
{
    return wxDoFloodFill(this, x, y, col, style);
}

bool wxDC::DoGetPixel( wxCoord x, wxCoord y, wxColour *col ) const
{
    wxCHECK_MSG( Ok(), false, wxT("wxDC::DoGetPixel - invalid DC") );

    wxMacFastPortSetter helper(this) ;

    // NOTE: Get/SetCPixel are slow!
    RGBColor colour;
    GetCPixel( XLOG2DEVMAC(x), YLOG2DEVMAC(y), &colour );

    // convert from Mac colour to wx
    *col = colour;

    return true ;
}

void wxDC::DoDrawLine( wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2 )
{
    wxCHECK_RET(Ok(), wxT("wxDC::DoDrawLine - invalid DC"));

    wxMacFastPortSetter helper(this) ;

    if (m_pen.GetStyle() != wxTRANSPARENT)
    {
        MacInstallPen() ;
        wxCoord offset = ( (m_pen.GetWidth() == 0 ? 1 :
            m_pen.GetWidth() ) * (wxCoord)m_scaleX - 1) / 2;
        wxCoord xx1 = XLOG2DEVMAC(x1) - offset;
        wxCoord yy1 = YLOG2DEVMAC(y1) - offset;
        wxCoord xx2 = XLOG2DEVMAC(x2) - offset;
        wxCoord yy2 = YLOG2DEVMAC(y2) - offset;

        if ((m_pen.GetCap() == wxCAP_ROUND) &&
            (m_pen.GetWidth() <= 1))
        {
            // Implement LAST_NOT for MAC at least for
            // orthogonal lines. RR.
            if (xx1 == xx2)
            {
                if (yy1 < yy2)
                    yy2--;
                if (yy1 > yy2)
                    yy2++;
            }

            if (yy1 == yy2)
            {
                if (xx1 < xx2)
                    xx2--;
                if (xx1 > xx2)
                    xx2++;
            }
        }

        ::MoveTo(xx1, yy1);
        ::LineTo(xx2, yy2);
    }
}

void wxDC::DoCrossHair( wxCoord x, wxCoord y )
{
    wxCHECK_RET(Ok(), wxT("wxDC::DoCrossHair - invalid DC"));

    wxMacFastPortSetter helper(this) ;

    if (m_pen.GetStyle() != wxTRANSPARENT)
    {
        int w = 0, h = 0;

        GetSize( &w, &h );
        wxCoord xx = XLOG2DEVMAC(x);
        wxCoord yy = YLOG2DEVMAC(y);

        MacInstallPen();
        ::MoveTo( XLOG2DEVMAC(0), yy );
        ::LineTo( XLOG2DEVMAC(w), yy );
        ::MoveTo( xx, YLOG2DEVMAC(0) );
        ::LineTo( xx, YLOG2DEVMAC(h) );
        CalcBoundingBox(x, y);
        CalcBoundingBox(x + w, y + h);
    }
}

/*
* To draw arcs properly the angles need to be converted from the WX style:
* Angles start on the +ve X axis and go anti-clockwise (As you would draw on
* a normal axis on paper).
* TO
* the Mac style:
* Angles start on the +ve y axis and go clockwise.
*/

static double wxConvertWXangleToMACangle(double angle)
{
    double newAngle = 90 - angle ;

    while ( newAngle > 360 )
        newAngle -= 360 ;
    while ( newAngle < 0 )
        newAngle += 360 ;

    return newAngle ;
}

void wxDC::DoDrawArc( wxCoord x1, wxCoord y1,
                      wxCoord x2, wxCoord y2,
                      wxCoord xc, wxCoord yc )
{
    wxCHECK_RET(Ok(), wxT("wxDC::DoDrawArc - invalid DC"));

    wxMacFastPortSetter helper(this) ;

    wxCoord xx1 = XLOG2DEVMAC(x1);
    wxCoord yy1 = YLOG2DEVMAC(y1);
    wxCoord xx2 = XLOG2DEVMAC(x2);
    wxCoord yy2 = YLOG2DEVMAC(y2);
    wxCoord xxc = XLOG2DEVMAC(xc);
    wxCoord yyc = YLOG2DEVMAC(yc);

    double dx = xx1 - xxc;
    double dy = yy1 - yyc;
    double radius = sqrt((double)(dx * dx + dy * dy));
    wxCoord rad = (wxCoord)radius;
    double radius1, radius2;

    if (xx1 == xx2 && yy1 == yy2)
    {
        radius1 = 0.0;
        radius2 = 360.0;
    }
    else if (radius == 0.0)
    {
        radius1 = radius2 = 0.0;
    }
    else
    {
        radius1 = (xx1 - xxc == 0) ?
            (yy1 - yyc < 0) ? 90.0 : -90.0 :
        -atan2(double(yy1 - yyc), double(xx1 - xxc)) * RAD2DEG;
        radius2 = (xx2 - xxc == 0) ?
            (yy2 - yyc < 0) ? 90.0 : -90.0 :
        -atan2(double(yy2 - yyc), double(xx2 - xxc)) * RAD2DEG;
    }

    wxCoord alpha2 = wxCoord(radius2 - radius1);
    wxCoord alpha1 = wxCoord(wxConvertWXangleToMACangle(radius1));
    while ( alpha2 < 0 )
        alpha2 += 360 ;
    alpha2 = -alpha2 ;
    Rect r = { yyc - rad, xxc - rad, yyc + rad, xxc + rad };

    if (m_brush.GetStyle() != wxTRANSPARENT)
    {
        MacInstallBrush();
        PaintArc(&r, alpha1, alpha2);
    }

    if (m_pen.GetStyle() != wxTRANSPARENT)
    {
        MacInstallPen();
        FrameArc(&r, alpha1, alpha2);
    }
}

void wxDC::DoDrawEllipticArc( wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                              double sa, double ea )
{
    wxCHECK_RET(Ok(), wxT("wxDC::DoDrawEllepticArc - invalid DC"));
    wxMacFastPortSetter helper(this) ;
    Rect r;

    // Order important Mac in opposite direction to wx
    // we have to make sure that the filling is always counter-clockwise
    double angle = sa - ea;
    if ( angle > 0 )
        angle -= 360 ;

    wxCoord xx = XLOG2DEVMAC(x);
    wxCoord yy = YLOG2DEVMAC(y);
    wxCoord ww = m_signX * XLOG2DEVREL(w);
    wxCoord hh = m_signY * YLOG2DEVREL(h);

    // handle -ve width and/or height
    if (ww < 0)
    {
        ww = -ww;
        xx = xx - ww;
    }

    if (hh < 0)
    {
        hh = -hh;
        yy = yy - hh;
    }

    sa = wxConvertWXangleToMACangle(sa);
    r.top    = yy;
    r.left   = xx;
    r.bottom = yy + hh;
    r.right  = xx + ww;

    if (m_brush.GetStyle() != wxTRANSPARENT)
    {
        MacInstallBrush();
        PaintArc(&r, (short)sa, (short)angle);
    }

    if (m_pen.GetStyle() != wxTRANSPARENT)
    {
        MacInstallPen();
        FrameArc(&r, (short)sa, (short)angle);
    }
}

void wxDC::DoDrawPoint( wxCoord x, wxCoord y )
{
    wxCHECK_RET(Ok(), wxT("wxDC::DoDrawPoint - invalid DC"));

    wxMacFastPortSetter helper(this) ;

    if (m_pen.GetStyle() != wxTRANSPARENT)
    {
        wxCoord xx1 = XLOG2DEVMAC(x);
        wxCoord yy1 = YLOG2DEVMAC(y);
        RGBColor pencolor = MAC_WXCOLORREF( m_pen.GetColour().GetPixel());

        // NOTE: Get/SetCPixel are slow!
        ::SetCPixel( xx1, yy1, &pencolor) ;
        CalcBoundingBox(x, y);
    }
}

void wxDC::DoDrawLines(int n, wxPoint points[],
                        wxCoord xoffset, wxCoord yoffset)
{
    wxCHECK_RET(Ok(), wxT("wxDC::DoDrawLines - invalid DC"));

    if (m_pen.GetStyle() == wxTRANSPARENT)
        return;

    wxMacFastPortSetter helper(this) ;

    MacInstallPen() ;
    wxCoord offset = ( (m_pen.GetWidth() == 0 ? 1 :
    m_pen.GetWidth() ) * (wxCoord)m_scaleX - 1) / 2 ;

    wxCoord x1, x2 , y1 , y2 ;
    x1 = XLOG2DEVMAC(points[0].x + xoffset);
    y1 = YLOG2DEVMAC(points[0].y + yoffset);

    ::MoveTo( x1 - offset, y1 - offset );
    for (int i = 0; i < n-1; i++)
    {
        x2 = XLOG2DEVMAC(points[i + 1].x + xoffset);
        y2 = YLOG2DEVMAC(points[i + 1].y + yoffset);
        ::LineTo( x2 - offset, y2 - offset );
    }
}

void wxDC::DoDrawPolygon(int n, wxPoint points[],
                          wxCoord xoffset, wxCoord yoffset,
                          int fillStyle )
{
    wxCHECK_RET(Ok(), wxT("wxDC::DoDrawPolygon - invalid DC"));

    if ( m_brush.GetStyle() == wxTRANSPARENT && m_pen.GetStyle() == wxTRANSPARENT )
        return ;

    wxMacFastPortSetter helper(this) ;

    wxCoord x1, x2 , y1 , y2 ;
    PolyHandle polygon = OpenPoly();
    x2 = x1 = XLOG2DEVMAC(points[0].x + xoffset);
    y2 = y1 = YLOG2DEVMAC(points[0].y + yoffset);

    ::MoveTo(x1, y1);
    for (int i = 1; i < n; i++)
    {
        x2 = XLOG2DEVMAC(points[i].x + xoffset);
        y2 = YLOG2DEVMAC(points[i].y + yoffset);
        ::LineTo(x2, y2);
    }

    // close the polyline if necessary
    if ( x1 != x2 || y1 != y2 )
        ::LineTo( x1, y1 ) ;
    ClosePoly();

    if (m_brush.GetStyle() != wxTRANSPARENT)
    {
        MacInstallBrush();
        ::PaintPoly( polygon );
    }

    if (m_pen.GetStyle() != wxTRANSPARENT)
    {
        MacInstallPen() ;
        ::FramePoly( polygon ) ;
    }

    KillPoly( polygon );
}

void wxDC::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    wxCHECK_RET(Ok(), wxT("wxDC::DoDrawRectangle - invalid DC"));

    wxMacFastPortSetter helper(this) ;

    wxCoord xx = XLOG2DEVMAC(x);
    wxCoord yy = YLOG2DEVMAC(y);
    wxCoord ww = m_signX * XLOG2DEVREL(width);
    wxCoord hh = m_signY * YLOG2DEVREL(height);

    // CMB: draw nothing if transformed w or h is 0
    if (ww == 0 || hh == 0)
        return;

    // CMB: handle -ve width and/or height
    if (ww < 0)
    {
        ww = -ww;
        xx = xx - ww;
    }

    if (hh < 0)
    {
        hh = -hh;
        yy = yy - hh;
    }

    Rect rect = { yy , xx , yy + hh , xx + ww } ;

    if (m_brush.GetStyle() != wxTRANSPARENT)
    {
        MacInstallBrush() ;
        ::PaintRect( &rect ) ;
    }

    if (m_pen.GetStyle() != wxTRANSPARENT)
    {
        MacInstallPen() ;
        ::FrameRect( &rect ) ;
    }
}

void wxDC::DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                   wxCoord width, wxCoord height,
                                   double radius)
{
    wxCHECK_RET(Ok(), wxT("wxDC::DoDrawRoundedRectangle - invalid DC"));

    wxMacFastPortSetter helper(this) ;
    if (radius < 0.0)
        radius = - radius * ((width < height) ? width : height);
    wxCoord xx = XLOG2DEVMAC(x);
    wxCoord yy = YLOG2DEVMAC(y);
    wxCoord ww = m_signX * XLOG2DEVREL(width);
    wxCoord hh = m_signY * YLOG2DEVREL(height);

    // CMB: draw nothing if transformed w or h is 0
    if (ww == 0 || hh == 0)
        return;

    // CMB: handle -ve width and/or height
    if (ww < 0)
    {
        ww = -ww;
        xx = xx - ww;
    }

    if (hh < 0)
    {
        hh = -hh;
        yy = yy - hh;
    }

    Rect rect = { yy , xx , yy + hh , xx + ww } ;

    if (m_brush.GetStyle() != wxTRANSPARENT)
    {
        MacInstallBrush() ;
        ::PaintRoundRect( &rect , int(radius * 2) , int(radius * 2) ) ;
    }

    if (m_pen.GetStyle() != wxTRANSPARENT)
    {
        MacInstallPen() ;
        ::FrameRoundRect( &rect , int(radius * 2) , int(radius * 2) ) ;
    }
}

void wxDC::DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    wxCHECK_RET(Ok(), wxT("wxDC::DoDrawEllipse - invalid DC"));

    wxMacFastPortSetter helper(this) ;

    wxCoord xx = XLOG2DEVMAC(x);
    wxCoord yy = YLOG2DEVMAC(y);
    wxCoord ww = m_signX * XLOG2DEVREL(width);
    wxCoord hh = m_signY * YLOG2DEVREL(height);

    // CMB: draw nothing if transformed w or h is 0
    if (ww == 0 || hh == 0)
        return;

    // CMB: handle -ve width and/or height
    if (ww < 0)
    {
        ww = -ww;
        xx = xx - ww;
    }

    if (hh < 0)
    {
        hh = -hh;
        yy = yy - hh;
    }

    Rect rect = { yy , xx , yy + hh , xx + ww } ;

    if (m_brush.GetStyle() != wxTRANSPARENT)
    {
        MacInstallBrush() ;
        ::PaintOval( &rect ) ;
    }

    if (m_pen.GetStyle() != wxTRANSPARENT)
    {
        MacInstallPen() ;
        ::FrameOval( &rect ) ;
    }
}

bool wxDC::CanDrawBitmap(void) const
{
    return true ;
}

bool wxDC::DoBlit(wxCoord xdest, wxCoord ydest, wxCoord dstWidth, wxCoord dstHeight,
                   wxDC *source, wxCoord xsrc, wxCoord ysrc, int logical_func , bool useMask,
                   wxCoord xsrcMask, wxCoord ysrcMask )
{
    return DoStretchBlit( xdest, ydest, dstWidth, dstHeight,
                           source, xsrc, ysrc, dstWidth, dstHeight, 
                           logical_func, useMask,
                           xsrcMask, ysrcMask );
}

bool wxDC::DoStretchBlit(wxCoord xdest, wxCoord ydest,
                         wxCoord dstWidth, wxCoord dstHeight,
                         wxDC *source,
                         wxCoord xsrc, wxCoord ysrc,
                         wxCoord srcWidth, wxCoord srcHeight,
                         int logical_func, bool useMask,
                         wxCoord xsrcMask, wxCoord ysrcMask)
{
    wxCHECK_MSG(Ok(), false, wxT("wxDC::DoStretchBlit - invalid DC"));
    wxCHECK_MSG(source->Ok(), false, wxT("wxDC::DoStretchBlit - invalid source DC"));

    if ( logical_func == wxNO_OP )
        return true ;

    if (xsrcMask == -1 && ysrcMask == -1)
    {
        xsrcMask = xsrc;
        ysrcMask = ysrc;
    }

    // correct the parameter in case this dc does not have a mask at all
    if ( useMask && !source->m_macMask )
        useMask = false ;

    Rect srcrect , dstrect ;
    srcrect.top = source->YLOG2DEVMAC(ysrc) ;
    srcrect.left = source->XLOG2DEVMAC(xsrc)  ;
    srcrect.right = source->XLOG2DEVMAC(xsrc + srcWidth ) ;
    srcrect.bottom = source->YLOG2DEVMAC(ysrc + srcHeight) ;
    dstrect.top = YLOG2DEVMAC(ydest) ;
    dstrect.left = XLOG2DEVMAC(xdest) ;
    dstrect.bottom = YLOG2DEVMAC(ydest + dstHeight )  ;
    dstrect.right = XLOG2DEVMAC(xdest + dstWidth ) ;
    short mode = kUnsupportedMode ;
    bool invertDestinationFirst = false ;

    switch ( logical_func )
    {
    case wxAND:        // src AND dst
        mode = adMin ; // ok
        break ;

    case wxAND_INVERT: // (NOT src) AND dst
        mode = notSrcOr  ; // ok
        break ;

    case wxAND_REVERSE:// src AND (NOT dst)
        invertDestinationFirst = true ;
        mode = srcOr ;
        break ;

    case wxCLEAR:      // 0
        mode = kEmulatedMode ;
        break ;

    case wxCOPY:       // src
        mode = srcCopy ; // ok
        break ;

    case wxEQUIV:      // (NOT src) XOR dst
        mode = srcXor ; // ok
        break ;

    case wxINVERT:     // NOT dst
        mode = kEmulatedMode ; //or hilite ;
        break ;

    case wxNAND:       // (NOT src) OR (NOT dst)
        invertDestinationFirst = true ;
        mode = srcBic ;
        break ;

    case wxNOR:        // (NOT src) AND (NOT dst)
        invertDestinationFirst = true ;
        mode = notSrcOr ;
        break ;

    case wxNO_OP:      // dst
        mode = kEmulatedMode ; // this has already been handled upon entry
        break ;

    case wxOR:         // src OR dst
        mode = notSrcBic ;
        break ;

    case wxOR_INVERT:  // (NOT src) OR dst
        mode = srcBic ;
        break ;

    case wxOR_REVERSE: // src OR (NOT dst)
        invertDestinationFirst = true ;
        mode = notSrcBic ;
        break ;

    case wxSET:        // 1
        mode = kEmulatedMode ;
        break ;

    case wxSRC_INVERT: // (NOT src)
        mode = notSrcCopy ; // ok
        break ;

    case wxXOR:        // src XOR dst
        mode = notSrcXor ; // ok
        break ;

    default :
        break ;
    }

    if ( mode == kUnsupportedMode )
    {
        wxFAIL_MSG(wxT("unsupported blitting mode" ));

        return false ;
    }

    CGrafPtr            sourcePort = (CGrafPtr) source->m_macPort ;
    PixMapHandle    bmappixels =  GetGWorldPixMap( sourcePort ) ;
    if ( LockPixels(bmappixels) )
    {
        wxMacFastPortSetter helper(this) ;

        if ( source->GetDepth() == 1 )
        {
            RGBForeColor( &MAC_WXCOLORREF(m_textForegroundColour.GetPixel()) ) ;
            RGBBackColor( &MAC_WXCOLORREF(m_textBackgroundColour.GetPixel()) ) ;
        }
        else
        {
            // the modes need this, otherwise we'll end up having really nice colors...
            RGBColor white = { 0xFFFF, 0xFFFF, 0xFFFF } ;
            RGBColor black = { 0, 0, 0 } ;

            RGBForeColor( &black ) ;
            RGBBackColor( &white ) ;
        }

        if ( useMask && source->m_macMask )
        {
            if ( mode == srcCopy )
            {
                if ( LockPixels( GetGWorldPixMap( MAC_WXHBITMAP(source->m_macMask) ) ) )
                {
                    CopyMask( GetPortBitMapForCopyBits( sourcePort ) ,
                        GetPortBitMapForCopyBits( MAC_WXHBITMAP(source->m_macMask) ) ,
                        GetPortBitMapForCopyBits( MAC_WXHBITMAP(m_macPort) ) ,
                        &srcrect, &srcrect , &dstrect ) ;
                    UnlockPixels( GetGWorldPixMap( MAC_WXHBITMAP(source->m_macMask) )  ) ;
                }
            }
            else
            {
                RgnHandle clipRgn = NewRgn() ;
                LockPixels( GetGWorldPixMap( MAC_WXHBITMAP(source->m_macMask) ) ) ;
                BitMapToRegion( clipRgn , (BitMap*) *GetGWorldPixMap( MAC_WXHBITMAP(source->m_macMask) ) ) ;
                UnlockPixels( GetGWorldPixMap( MAC_WXHBITMAP(source->m_macMask) ) ) ;
                OffsetRgn( clipRgn , -srcrect.left + dstrect.left, -srcrect.top +  dstrect.top ) ;

                if ( mode == kEmulatedMode )
                {
                    Pattern pat ;

                    ::PenPat(GetQDGlobalsBlack(&pat));
                    if ( logical_func == wxSET )
                    {
                        RGBColor col= { 0xFFFF, 0xFFFF, 0xFFFF } ;
                        ::RGBForeColor( &col  ) ;
                        ::PaintRgn( clipRgn ) ;
                    }
                    else if ( logical_func == wxCLEAR )
                    {
                        RGBColor col= { 0x0000, 0x0000, 0x0000 } ;
                        ::RGBForeColor( &col  ) ;
                        ::PaintRgn( clipRgn ) ;
                    }
                    else if ( logical_func == wxINVERT )
                    {
                        MacInvertRgn( clipRgn ) ;
                    }
                    else
                    {
                        for ( int y = 0 ; y < srcrect.right - srcrect.left ; ++y )
                        {
                            for ( int x = 0 ; x < srcrect.bottom - srcrect.top ; ++x )
                            {
                                Point dstPoint = { dstrect.top + y , dstrect.left + x } ;
                                Point srcPoint = { srcrect.top + y , srcrect.left + x } ;
                                if ( PtInRgn( dstPoint , clipRgn ) )
                                {
                                    RGBColor srcColor, dstColor ;

                                    // NOTE: Get/SetCPixel are slow!
                                    SetPort( (GrafPtr) sourcePort ) ;
                                    GetCPixel( srcPoint.h , srcPoint.v , &srcColor ) ;
                                    SetPort( (GrafPtr) m_macPort ) ;
                                    GetCPixel( dstPoint.h , dstPoint.v , &dstColor ) ;
                                    wxMacCalculateColour( logical_func , srcColor , dstColor ) ;
                                    SetCPixel( dstPoint.h , dstPoint.v , &dstColor ) ;
                                }
                            }
                        }
                    }
                }
                else
                {
                    if ( invertDestinationFirst )
                        MacInvertRgn( clipRgn ) ;

                    CopyBits( GetPortBitMapForCopyBits( sourcePort ) ,
                        GetPortBitMapForCopyBits( MAC_WXHBITMAP(m_macPort) ) ,
                        &srcrect, &dstrect, mode, clipRgn ) ;
                }

                DisposeRgn( clipRgn ) ;
            }
        }
        else
        {
            RgnHandle clipRgn = NewRgn() ;
            SetRectRgn( clipRgn , dstrect.left , dstrect.top , dstrect.right , dstrect.bottom ) ;

            if ( mode == kEmulatedMode )
            {
                Pattern pat ;

                ::PenPat(GetQDGlobalsBlack(&pat));
                if ( logical_func == wxSET )
                {
                    RGBColor col= { 0xFFFF, 0xFFFF, 0xFFFF } ;
                    ::RGBForeColor( &col  ) ;
                    ::PaintRgn( clipRgn ) ;
                }
                else if ( logical_func == wxCLEAR )
                {
                    RGBColor col = { 0x0000, 0x0000, 0x0000 } ;

                    ::RGBForeColor( &col  ) ;
                    ::PaintRgn( clipRgn ) ;
                }
                else if ( logical_func == wxINVERT )
                {
                    MacInvertRgn( clipRgn ) ;
                }
                else
                {
                    for ( int y = 0 ; y < srcrect.right - srcrect.left ; ++y )
                    {
                        for ( int x = 0 ; x < srcrect.bottom - srcrect.top ; ++x )
                        {
                            Point dstPoint = { dstrect.top + y , dstrect.left + x } ;
                            Point srcPoint = { srcrect.top + y , srcrect.left + x } ;
                            {
                                RGBColor srcColor, dstColor ;

                                // NOTE: Get/SetCPixel are slow!
                                SetPort( (GrafPtr) sourcePort ) ;
                                GetCPixel( srcPoint.h , srcPoint.v , &srcColor) ;
                                SetPort( (GrafPtr) m_macPort ) ;
                                GetCPixel( dstPoint.h , dstPoint.v , &dstColor ) ;
                                wxMacCalculateColour( logical_func , srcColor , dstColor ) ;
                                SetCPixel( dstPoint.h , dstPoint.v , &dstColor ) ;
                            }
                        }
                    }
                }
            }
            else
            {
                if ( invertDestinationFirst )
                    MacInvertRgn( clipRgn ) ;

                CopyBits( GetPortBitMapForCopyBits( sourcePort ) ,
                    GetPortBitMapForCopyBits( MAC_WXHBITMAP(m_macPort) ) ,
                    &srcrect, &dstrect, mode, NULL ) ;
            }

            DisposeRgn( clipRgn ) ;
        }

        UnlockPixels( bmappixels ) ;
    }

    m_macPenInstalled = false ;
    m_macBrushInstalled = false ;
    m_macFontInstalled = false ;

    return true;
}

void wxDC::DoDrawRotatedText(const wxString& str, wxCoord x, wxCoord y,
                              double angle)
{
    wxCHECK_RET( Ok(), wxT("wxDC::DoDrawRotatedText - invalid DC") );

    if ( str.empty() )
        return ;

    wxMacFastPortSetter helper(this) ;
    MacInstallFont() ;

    OSStatus status = noErr ;
    ATSUTextLayout atsuLayout ;

    wxMacUniCharBuffer unibuf( str ) ;
    UniCharCount chars = unibuf.GetChars() ;

    status = ::ATSUCreateTextLayoutWithTextPtr( unibuf.GetBuffer() , 0 , chars , chars , 1 ,
        &chars , (ATSUStyle*) &m_macATSUIStyle , &atsuLayout ) ;

    wxASSERT_MSG( status == noErr , wxT("couldn't create the layout of the rotated text") );

    status = ::ATSUSetTransientFontMatching( atsuLayout , true ) ;
    wxASSERT_MSG( status == noErr , wxT("couldn't setup transient font matching") );

    int iAngle = int( angle );
    int drawX = XLOG2DEVMAC(x) ;
    int drawY = YLOG2DEVMAC(y) ;

    ATSUTextMeasurement textBefore, textAfter ;
    ATSUTextMeasurement ascent, descent ;

    ATSLineLayoutOptions layoutOptions = kATSLineNoLayoutOptions ;

    if (m_font.GetNoAntiAliasing())
    {
        layoutOptions |= kATSLineNoAntiAliasing ;
    }

    Fixed atsuAngle = IntToFixed( iAngle ) ;

    ATSUAttributeTag atsuTags[] =
    {
        kATSULineLayoutOptionsTag ,
        kATSULineRotationTag ,
    } ;

    ByteCount atsuSizes[sizeof(atsuTags)/sizeof(ATSUAttributeTag)] =
    {
        sizeof( ATSLineLayoutOptions ) ,
        sizeof( Fixed ) ,
    } ;

    ATSUAttributeValuePtr    atsuValues[sizeof(atsuTags)/sizeof(ATSUAttributeTag)] =
    {
        &layoutOptions ,
        &atsuAngle ,
    } ;

    status = ::ATSUSetLayoutControls(atsuLayout , sizeof(atsuTags)/sizeof(ATSUAttributeTag) - ( abs(iAngle) > 0.001 ? 0 : 1),
            atsuTags, atsuSizes, atsuValues ) ;

    status = ::ATSUMeasureText( atsuLayout, kATSUFromTextBeginning, kATSUToTextEnd,
        &textBefore , &textAfter, &ascent , &descent );
    wxASSERT_MSG( status == noErr , wxT("couldn't measure the rotated text") );

    if ( m_backgroundMode == wxSOLID )
    {
        // background painting must be done by hand, cannot be done by ATSUI
        wxCoord x2 , y2 ;
        PolyHandle polygon = OpenPoly();

        ::MoveTo(drawX, drawY);

        x2 = (int) (drawX + sin(angle / RAD2DEG) * FixedToInt(ascent + descent)) ;
        y2 = (int) (drawY + cos(angle / RAD2DEG) * FixedToInt(ascent + descent)) ;
        ::LineTo(x2, y2);

        x2 = (int) (drawX + sin(angle / RAD2DEG) * FixedToInt(ascent + descent ) + cos(angle / RAD2DEG) * FixedToInt(textAfter)) ;
        y2 = (int) (drawY + cos(angle / RAD2DEG) * FixedToInt(ascent + descent) - sin(angle / RAD2DEG) * FixedToInt(textAfter)) ;
        ::LineTo(x2, y2);

        x2 = (int) (drawX + cos(angle / RAD2DEG) * FixedToInt(textAfter)) ;
        y2 = (int) (drawY - sin(angle / RAD2DEG) * FixedToInt(textAfter)) ;
        ::LineTo(x2, y2);

        ::LineTo( drawX, drawY) ;
        ClosePoly();

        ::ErasePoly( polygon );
        KillPoly( polygon );
    }

    drawX += (int)(sin(angle / RAD2DEG) * FixedToInt(ascent));
    drawY += (int)(cos(angle / RAD2DEG) * FixedToInt(ascent));
    status = ::ATSUDrawText( atsuLayout, kATSUFromTextBeginning, kATSUToTextEnd,
        IntToFixed(drawX) , IntToFixed(drawY) );

    wxASSERT_MSG( status == noErr , wxT("couldn't draw the rotated text") );

    Rect rect ;
    status = ::ATSUMeasureTextImage( atsuLayout, kATSUFromTextBeginning, kATSUToTextEnd,
        IntToFixed(drawX) , IntToFixed(drawY) , &rect );
    wxASSERT_MSG( status == noErr , wxT("couldn't measure the rotated text") );

    OffsetRect( &rect , -m_deviceLocalOriginX , -m_deviceLocalOriginY ) ;
    CalcBoundingBox(XDEV2LOG(rect.left), YDEV2LOG(rect.top) );
    CalcBoundingBox(XDEV2LOG(rect.right), YDEV2LOG(rect.bottom) );
    ::ATSUDisposeTextLayout(atsuLayout);
}

void wxDC::DoDrawText(const wxString& strtext, wxCoord x, wxCoord y)
{
    DoDrawRotatedText( strtext , x , y , 0) ;
}

bool wxDC::CanGetTextExtent() const
{
    wxCHECK_MSG(Ok(), false, wxT("wxDC::CanGetTextExtent - invalid DC"));

    return true ;
}


void wxDC::DoGetTextExtent( const wxString &str, wxCoord *width, wxCoord *height,
                            wxCoord *descent, wxCoord *externalLeading ,
                            const wxFont *theFont ) const
{
    wxCHECK_RET(Ok(), wxT("wxDC::DoGetTextExtent - invalid DC"));

    wxMacFastPortSetter helper(this) ;
    wxFont formerFont = m_font ;
    if ( theFont )
    {
        // work around the constness
        *((wxFont*)(&m_font)) = *theFont ;
    }

    MacInstallFont() ;

    OSStatus status = noErr ;
    ATSUTextLayout atsuLayout ;

    wxMacUniCharBuffer unibuf( str ) ;
    UniCharCount chars = unibuf.GetChars() ;

    status = ::ATSUCreateTextLayoutWithTextPtr( unibuf.GetBuffer() , 0 , chars , chars , 1 ,
        &chars , (ATSUStyle*) &m_macATSUIStyle , &atsuLayout ) ;

    wxASSERT_MSG( status == noErr , wxT("couldn't create the layout of the text") );

    status = ::ATSUSetTransientFontMatching( atsuLayout , true ) ;
    wxASSERT_MSG( status == noErr , wxT("couldn't setup transient font matching") );

    ATSLineLayoutOptions layoutOptions = kATSLineNoLayoutOptions ;

    if (m_font.GetNoAntiAliasing())
    {
        layoutOptions |= kATSLineNoAntiAliasing ;
    }

    ATSUAttributeTag atsuTags[] =
    {
        kATSULineLayoutOptionsTag ,
    } ;

    ByteCount atsuSizes[sizeof(atsuTags)/sizeof(ATSUAttributeTag)] =
    {
        sizeof( ATSLineLayoutOptions ) ,
    } ;

    ATSUAttributeValuePtr    atsuValues[sizeof(atsuTags)/sizeof(ATSUAttributeTag)] =
    {
        &layoutOptions ,
    } ;

    status = ::ATSUSetLayoutControls(atsuLayout , sizeof(atsuTags)/sizeof(ATSUAttributeTag) ,
            atsuTags, atsuSizes, atsuValues ) ;

    ATSUTextMeasurement textBefore, textAfter ;
    ATSUTextMeasurement textAscent, textDescent ;

    status = ::ATSUGetUnjustifiedBounds( atsuLayout, kATSUFromTextBeginning, kATSUToTextEnd,
        &textBefore , &textAfter, &textAscent , &textDescent );

    if ( height )
        *height = YDEV2LOGREL( FixedToInt(textAscent + textDescent) ) ;
    if ( descent )
        *descent =YDEV2LOGREL( FixedToInt(textDescent) );
    if ( externalLeading )
        *externalLeading = 0 ;
    if ( width )
        *width = XDEV2LOGREL( FixedToInt(textAfter - textBefore) ) ;

    ::ATSUDisposeTextLayout(atsuLayout);


    if ( theFont )
    {
        // work around the constness
        *((wxFont*)(&m_font)) = formerFont ;
        m_macFontInstalled = false ;
    }
}

bool wxDC::DoGetPartialTextExtents(const wxString& text, wxArrayInt& widths) const
{
    wxCHECK_MSG(Ok(), false, wxT("wxDC::DoGetPartialTextExtents - invalid DC"));

    widths.Empty();
    widths.Add(0, text.length());

    if (text.length() == 0)
        return false;

    wxMacFastPortSetter helper(this) ;
    MacInstallFont() ;

    OSStatus status = noErr ;
    ATSUTextLayout atsuLayout ;

    wxMacUniCharBuffer unibuf( text ) ;
    UniCharCount chars = unibuf.GetChars() ;

    status = ::ATSUCreateTextLayoutWithTextPtr( unibuf.GetBuffer() , 0 , chars , chars , 1 ,
        &chars , (ATSUStyle*) &m_macATSUIStyle , &atsuLayout ) ;

    wxASSERT_MSG( status == noErr , wxT("couldn't create the layout of the text") );

    status = ::ATSUSetTransientFontMatching( atsuLayout , true ) ;
    wxASSERT_MSG( status == noErr , wxT("couldn't setup transient font matching") );

    ATSLineLayoutOptions layoutOptions = kATSLineNoLayoutOptions ;

    if (m_font.GetNoAntiAliasing())
    {
        layoutOptions |= kATSLineNoAntiAliasing ;
    }

    ATSUAttributeTag atsuTags[] =
    {
        kATSULineLayoutOptionsTag ,
    } ;

    ByteCount atsuSizes[sizeof(atsuTags)/sizeof(ATSUAttributeTag)] =
    {
        sizeof( ATSLineLayoutOptions ) ,
    } ;

    ATSUAttributeValuePtr    atsuValues[sizeof(atsuTags)/sizeof(ATSUAttributeTag)] =
    {
        &layoutOptions ,
    } ;

    status = ::ATSUSetLayoutControls(atsuLayout , sizeof(atsuTags)/sizeof(ATSUAttributeTag) ,
            atsuTags, atsuSizes, atsuValues ) ;

    for ( int pos = 0; pos < (int)chars ; pos ++ )
    {
        unsigned long actualNumberOfBounds = 0;
        ATSTrapezoid glyphBounds;

        // We get a single bound, since the text should only require one. If it requires more, there is an issue
        OSStatus result;
        result = ATSUGetGlyphBounds( atsuLayout, 0, 0, kATSUFromTextBeginning, pos + 1,
            kATSUseDeviceOrigins, 1, &glyphBounds, &actualNumberOfBounds );
        if (result != noErr || actualNumberOfBounds != 1 )
            return false;

        widths[pos] = XDEV2LOGREL(FixedToInt( glyphBounds.upperRight.x - glyphBounds.upperLeft.x ));
    }

    ::ATSUDisposeTextLayout(atsuLayout);

    return true;
}

wxCoord wxDC::GetCharWidth(void) const
{
    wxCoord width = 0 ;
    DoGetTextExtent( wxT("g"), &width , NULL , NULL , NULL , NULL ) ;
    return width ;
}

wxCoord wxDC::GetCharHeight(void) const
{
    wxCoord height ;
    DoGetTextExtent( wxT("g") , NULL , &height , NULL , NULL , NULL ) ;
    return height ;
}

void wxDC::Clear(void)
{
    wxCHECK_RET(Ok(), wxT("wxDC::Clear - invalid DC"));

    wxMacFastPortSetter helper(this) ;
    Rect rect = { -31000 , -31000 , 31000 , 31000 } ;

    if ( m_backgroundBrush.Ok() && m_backgroundBrush.GetStyle() != wxTRANSPARENT)
    {
        ::PenNormal() ;
        MacSetupBackgroundForCurrentPort( m_backgroundBrush ) ;
        ::EraseRect( &rect ) ;
    }
}

void wxDC::MacInstallFont() const
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));

    //    if ( m_macFontInstalled )
    //        return ;

    Pattern blackColor ;
    MacSetupBackgroundForCurrentPort(m_backgroundBrush) ;
    if ( m_backgroundMode != wxTRANSPARENT )
    {
        Pattern whiteColor ;
        ::BackPat(GetQDGlobalsWhite(&whiteColor));
    }

    wxASSERT( m_font.Ok() ) ;

    ::TextFont( m_font.MacGetFontNum() ) ;
    ::TextSize( (short)(m_scaleY * m_font.MacGetFontSize()) ) ;
    ::TextFace( m_font.MacGetFontStyle() ) ;
    m_macFontInstalled = true ;
    m_macBrushInstalled = false ;
    m_macPenInstalled = false ;
    RGBColor forecolor = MAC_WXCOLORREF( m_textForegroundColour.GetPixel());
    RGBColor backcolor = MAC_WXCOLORREF( m_textBackgroundColour.GetPixel());
    ::RGBForeColor( &forecolor );
    ::RGBBackColor( &backcolor );

    // TODO:
    short mode = patCopy ;
    switch ( m_logicalFunction )
    {
    case wxCOPY:       // src
        mode = patCopy ;
        break ;

    case wxINVERT:     // NOT dst
        ::PenPat(GetQDGlobalsBlack(&blackColor));
        mode = patXor ;
        break ;

    case wxXOR:        // src XOR dst
        mode = patXor ;
        break ;

    case wxOR_REVERSE: // src OR (NOT dst)
        mode = notPatOr ;
        break ;

    case wxSRC_INVERT: // (NOT src)
        mode = notPatCopy ;
        break ;

    case wxAND: // src AND dst
        mode = adMin ;
        break ;

    // TODO: unsupported
    case wxCLEAR:      // 0
    case wxAND_REVERSE:// src AND (NOT dst)
    case wxAND_INVERT: // (NOT src) AND dst
    case wxNO_OP:      // dst
    case wxNOR:        // (NOT src) AND (NOT dst)
    case wxEQUIV:      // (NOT src) XOR dst
    case wxOR_INVERT:  // (NOT src) OR dst
    case wxNAND:       // (NOT src) OR (NOT dst)
    case wxOR:         // src OR dst
    case wxSET:        // 1
        //        case wxSRC_OR:     // source _bitmap_ OR destination
        //        case wxSRC_AND:     // source _bitmap_ AND destination
        break ;

    default:
        break ;
    }

    ::PenMode( mode ) ;
    OSStatus status = noErr ;
    status = ATSUCreateAndCopyStyle( (ATSUStyle) m_font.MacGetATSUStyle() , (ATSUStyle*) &m_macATSUIStyle ) ;
    wxASSERT_MSG( status == noErr , wxT("couldn't set create ATSU style") ) ;

    Fixed atsuSize = IntToFixed( int(m_scaleY * m_font.MacGetFontSize()) ) ;
    ATSUAttributeTag atsuTags[] =
    {
            kATSUSizeTag ,
    } ;
    ByteCount atsuSizes[sizeof(atsuTags)/sizeof(ATSUAttributeTag)] =
    {
            sizeof( Fixed ) ,
    } ;
    ATSUAttributeValuePtr    atsuValues[sizeof(atsuTags)/sizeof(ATSUAttributeTag)] =
    {
            &atsuSize ,
    } ;

    status = ::ATSUSetAttributes((ATSUStyle)m_macATSUIStyle, sizeof(atsuTags)/sizeof(ATSUAttributeTag) ,
        atsuTags, atsuSizes, atsuValues);

    wxASSERT_MSG( status == noErr , wxT("couldn't modify ATSU style") ) ;
}

Pattern gPatterns[] =
{
    // hatch patterns
    { { 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF } } ,
    { { 0x01 , 0x02 , 0x04 , 0x08 , 0x10 , 0x20 , 0x40 , 0x80 } } ,
    { { 0x80 , 0x40 , 0x20 , 0x10 , 0x08 , 0x04 , 0x02 , 0x01 } } ,
    { { 0x10 , 0x10 , 0x10 , 0xFF , 0x10 , 0x10 , 0x10 , 0x10 } } ,
    { { 0x00 , 0x00 , 0x00 , 0xFF , 0x00 , 0x00 , 0x00 , 0x00 } } ,
    { { 0x10 , 0x10 , 0x10 , 0x10 , 0x10 , 0x10 , 0x10 , 0x10 } } ,
    { { 0x81 , 0x42 , 0x24 , 0x18 , 0x18 , 0x24 , 0x42 , 0x81 } } ,

    // dash patterns
    { { 0xCC , 0x99 , 0x33 , 0x66 , 0xCC , 0x99 , 0x33 , 0x66 } } , // DOT
    { { 0xFE , 0xFD , 0xFB , 0xF7 , 0xEF , 0xDF , 0xBF , 0x7F } } , // LONG_DASH
    { { 0xEE , 0xDD , 0xBB , 0x77 , 0xEE , 0xDD , 0xBB , 0x77 } } , // SHORT_DASH
    { { 0xDE , 0xBD , 0x7B , 0xF6 , 0xED , 0xDB , 0xB7 , 0x6F } } , // DOT_DASH
} ;

static void wxMacGetPattern(int penStyle, Pattern *pattern)
{
    int index = 0;  // solid pattern by default
    switch (penStyle)
    {
        // hatches
        case wxBDIAGONAL_HATCH:     index = 1; break;
        case wxFDIAGONAL_HATCH:     index = 2; break;
        case wxCROSS_HATCH:         index = 3; break;
        case wxHORIZONTAL_HATCH:    index = 4; break;
        case wxVERTICAL_HATCH:      index = 5; break;
        case wxCROSSDIAG_HATCH:     index = 6; break;

        // dashes
        case wxDOT:                 index = 7; break;
        case wxLONG_DASH:           index = 8; break;
        case wxSHORT_DASH:          index = 9; break;
        case wxDOT_DASH:            index = 10; break;

        default:
            break;
    }

    *pattern = gPatterns[index];
}

void wxDC::MacInstallPen() const
{
    wxCHECK_RET(Ok(), wxT("wxDC::MacInstallPen - invalid DC"));

    //Pattern     blackColor;
    //    if ( m_macPenInstalled )
    //        return ;

    RGBColor forecolor = MAC_WXCOLORREF( m_pen.GetColour().GetPixel());
    RGBColor backcolor = MAC_WXCOLORREF( m_backgroundBrush.GetColour().GetPixel());
    ::RGBForeColor( &forecolor );
    ::RGBBackColor( &backcolor );
    ::PenNormal() ;

    // null means only one pixel, at whatever resolution
    int penWidth = (int) (m_pen.GetWidth() * m_scaleX) ;
    if ( penWidth == 0 )
        penWidth = 1 ;
    ::PenSize(penWidth, penWidth);

    Pattern pat;
    int penStyle = m_pen.GetStyle();
    if (penStyle == wxUSER_DASH)
    {
        // FIXME: there should be exactly 8 items in the dash
        wxDash* dash ;
        int number = m_pen.GetDashes(&dash) ;
        int index = 0;
        for ( int i = 0 ; i < 8 ; ++i )
        {
            pat.pat[i] = dash[index] ;
            if (index < number - 1)
                index++;
        }
    }
    else
    {
        wxMacGetPattern(penStyle, &pat);
    }
    ::PenPat(&pat);

    // TODO:
    short mode = patCopy ;
    switch ( m_logicalFunction )
    {
    case wxCOPY:       // only foreground color, leave background (thus not patCopy)
        mode = patOr ;
        break ;

    case wxINVERT:     // NOT dst
        //            ::PenPat(GetQDGlobalsBlack(&blackColor));
        mode = patXor ;
        break ;

    case wxXOR:        // src XOR dst
        mode = patXor ;
        break ;

    case wxOR_REVERSE: // src OR (NOT dst)
        mode = notPatOr ;
        break ;

    case wxSRC_INVERT: // (NOT src)
        mode = notPatCopy ;
        break ;

    case wxAND: // src AND dst
        mode = adMin ;
        break ;

    // TODO: unsupported
    case wxCLEAR:      // 0
    case wxAND_REVERSE:// src AND (NOT dst)
    case wxAND_INVERT: // (NOT src) AND dst
    case wxNO_OP:      // dst
    case wxNOR:        // (NOT src) AND (NOT dst)
    case wxEQUIV:      // (NOT src) XOR dst
    case wxOR_INVERT:  // (NOT src) OR dst
    case wxNAND:       // (NOT src) OR (NOT dst)
    case wxOR:         // src OR dst
    case wxSET:        // 1
        //        case wxSRC_OR:     // source _bitmap_ OR destination
        //        case wxSRC_AND:     // source _bitmap_ AND destination
        break ;

    default:
        break ;
    }

    ::PenMode( mode ) ;
    m_macPenInstalled = true ;
    m_macBrushInstalled = false ;
    m_macFontInstalled = false ;
}

void wxDC::MacSetupBackgroundForCurrentPort(const wxBrush& background )
{
    Pattern whiteColor ;

    if ( background.Ok() )
    {
        switch ( background.MacGetBrushKind() )
        {
            case kwxMacBrushTheme :
                ::SetThemeBackground( background.MacGetTheme() , wxDisplayDepth() , true ) ;
                break ;

            case kwxMacBrushThemeBackground :
            {
                Rect extent ;
                ThemeBackgroundKind bg = background.MacGetThemeBackground( &extent ) ;
                ::ApplyThemeBackground( bg , &extent, kThemeStateActive , wxDisplayDepth() , true ) ;
            }
                break ;

            case kwxMacBrushColour :
            {
                ::RGBBackColor( &MAC_WXCOLORREF( background.GetColour().GetPixel()) );
                int brushStyle = background.GetStyle();
                if (brushStyle == wxSOLID)
                    ::BackPat(GetQDGlobalsWhite(&whiteColor));
                else if (background.IsHatch())
                {
                    Pattern pat ;
                    wxMacGetPattern(brushStyle, &pat);
                    ::BackPat(&pat);
                }
                else
                {
                    ::BackPat(GetQDGlobalsWhite(&whiteColor));
                }
            }
                break ;

            default:
                break ;
        }
    }
}

void wxDC::MacInstallBrush() const
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));

    //    if ( m_macBrushInstalled )
    //        return ;

    Pattern     blackColor ;
    bool backgroundTransparent = (GetBackgroundMode() == wxTRANSPARENT) ;
    ::RGBForeColor( &MAC_WXCOLORREF( m_brush.GetColour().GetPixel()) );
    ::RGBBackColor( &MAC_WXCOLORREF( m_backgroundBrush.GetColour().GetPixel()) );

    int brushStyle = m_brush.GetStyle();
    if (brushStyle == wxSOLID)
    {
        switch ( m_brush.MacGetBrushKind() )
        {
            case kwxMacBrushTheme :
                {
                    Pattern whiteColor ;
                    ::BackPat(GetQDGlobalsWhite(&whiteColor));
                    ::SetThemePen( m_brush.MacGetTheme() , wxDisplayDepth() , true ) ;
                }
            break ;

            default :
                ::PenPat(GetQDGlobalsBlack(&blackColor));
                break ;

        }
    }
    else if (m_brush.IsHatch())
    {
        Pattern pat ;

        wxMacGetPattern(brushStyle, &pat);
        ::PenPat(&pat);
    }
    else if ( m_brush.GetStyle() == wxSTIPPLE || m_brush.GetStyle() == wxSTIPPLE_MASK_OPAQUE )
    {
        // we force this in order to be compliant with wxMSW
        backgroundTransparent = false ;

        // for these the text fore (and back for MASK_OPAQUE) colors are used
        wxBitmap* bitmap = m_brush.GetStipple() ;
        int width = bitmap->GetWidth() ;
        int height = bitmap->GetHeight() ;
        GWorldPtr gw = NULL ;

        if ( m_brush.GetStyle() == wxSTIPPLE )
            gw = MAC_WXHBITMAP(bitmap->GetHBITMAP(NULL))  ;
        else
            gw = MAC_WXHBITMAP(bitmap->GetMask()->GetHBITMAP()) ;

        PixMapHandle gwpixmaphandle = GetGWorldPixMap( gw ) ;
        LockPixels( gwpixmaphandle ) ;
        bool isMonochrome = !IsPortColor( gw ) ;
        if ( !isMonochrome )
        {
            if ( (**gwpixmaphandle).pixelSize == 1 )
                isMonochrome = true ;
        }

        if ( isMonochrome && width == 8 && height == 8 )
        {
            ::RGBForeColor( &MAC_WXCOLORREF( m_textForegroundColour.GetPixel()) );
            ::RGBForeColor( &MAC_WXCOLORREF( m_textBackgroundColour.GetPixel()) );
            BitMap* gwbitmap = (BitMap*) *gwpixmaphandle ; // since the color depth is 1 it is a BitMap
            UInt8 *gwbits = (UInt8*) gwbitmap->baseAddr ;
            int alignment = gwbitmap->rowBytes & 0x7FFF ;
            Pattern pat ;

            for ( int i = 0 ; i < 8 ; ++i )
            {
                pat.pat[i] = gwbits[i * alignment + 0] ;
            }

            UnlockPixels( GetGWorldPixMap( gw ) ) ;
            ::PenPat( &pat ) ;
        }
        else
        {
            // this will be the code to handle power of 2 patterns, we will have to arrive at a nice
            // caching scheme before putting this into production
            Handle      image;
            long        imageSize;

            PixPatHandle pixpat = NewPixPat() ;
            CopyPixMap(gwpixmaphandle, (**pixpat).patMap);
            imageSize = GetPixRowBytes((**pixpat).patMap) *
                ((**(**pixpat).patMap).bounds.bottom -
                (**(**pixpat).patMap).bounds.top);
            PtrToHand( (**gwpixmaphandle).baseAddr, &image, imageSize );
            (**pixpat).patData = image;

            if ( isMonochrome )
            {
                CTabHandle ctable = ((**((**pixpat).patMap)).pmTable) ;
                ColorSpecPtr ctspec = (ColorSpecPtr) &(**ctable).ctTable ;
                if ( ctspec[0].rgb.red == 0x0000 )
                {
                    ctspec[1].rgb = MAC_WXCOLORREF( m_textBackgroundColour.GetPixel()) ;
                    ctspec[0].rgb = MAC_WXCOLORREF( m_textForegroundColour.GetPixel()) ;
                }
                else
                {
                    ctspec[0].rgb = MAC_WXCOLORREF( m_textBackgroundColour.GetPixel()) ;
                    ctspec[1].rgb = MAC_WXCOLORREF( m_textForegroundColour.GetPixel()) ;
                }
                ::CTabChanged( ctable ) ;
            }

            ::PenPixPat(pixpat);
            m_macForegroundPixMap = pixpat ;
        }

        UnlockPixels( gwpixmaphandle ) ;
    }
    else
    {
        ::PenPat(GetQDGlobalsBlack(&blackColor));
    }

    short mode = patCopy ;
    switch ( m_logicalFunction )
    {
    case wxCOPY:       // src
        if ( backgroundTransparent )
            mode = patOr ;
        else
            mode = patCopy ;
        break ;

    case wxINVERT:     // NOT dst
        if ( !backgroundTransparent )
            ::PenPat(GetQDGlobalsBlack(&blackColor));
        mode = patXor ;
        break ;

    case wxXOR:        // src XOR dst
        mode = patXor ;
        break ;

    case wxOR_REVERSE: // src OR (NOT dst)
        mode = notPatOr ;
        break ;

    case wxSRC_INVERT: // (NOT src)
        mode = notPatCopy ;
        break ;

    case wxAND: // src AND dst
        mode = adMin ;
        break ;

    // TODO: unsupported
    case wxCLEAR:      // 0
    case wxAND_REVERSE:// src AND (NOT dst)
    case wxAND_INVERT: // (NOT src) AND dst
    case wxNO_OP:      // dst
    case wxNOR:        // (NOT src) AND (NOT dst)
    case wxEQUIV:      // (NOT src) XOR dst
    case wxOR_INVERT:  // (NOT src) OR dst
    case wxNAND:       // (NOT src) OR (NOT dst)
    case wxOR:         // src OR dst
    case wxSET:        // 1
        //        case wxSRC_OR:     // source _bitmap_ OR destination
        //        case wxSRC_AND:     // source _bitmap_ AND destination
        break ;

    default:
        break ;
    }

    ::PenMode( mode ) ;
    m_macBrushInstalled = true ;
    m_macPenInstalled = false ;
    m_macFontInstalled = false ;
}

// ---------------------------------------------------------------------------
// coordinates transformations
// ---------------------------------------------------------------------------

wxCoord wxDC::DeviceToLogicalX(wxCoord x) const
{
    return XDEV2LOG(x);
}

wxCoord wxDC::DeviceToLogicalY(wxCoord y) const
{
    return YDEV2LOG(y);
}

wxCoord wxDC::DeviceToLogicalXRel(wxCoord x) const
{
    return XDEV2LOGREL(x);
}

wxCoord wxDC::DeviceToLogicalYRel(wxCoord y) const
{
    return YDEV2LOGREL(y);
}

wxCoord wxDC::LogicalToDeviceX(wxCoord x) const
{
    return XLOG2DEV(x);
}

wxCoord wxDC::LogicalToDeviceY(wxCoord y) const
{
    return YLOG2DEV(y);
}

wxCoord wxDC::LogicalToDeviceXRel(wxCoord x) const
{
    return XLOG2DEVREL(x);
}

wxCoord wxDC::LogicalToDeviceYRel(wxCoord y) const
{
    return YLOG2DEVREL(y);
}

#endif // !wxMAC_USE_CORE_GRAPHICS
