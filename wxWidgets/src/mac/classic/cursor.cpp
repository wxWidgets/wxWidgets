/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/cursor.cpp
// Purpose:     wxCursor class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/cursor.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/icon.h"
    #include "wx/image.h"
#endif // WX_PRECOMP

#include "wx/xpmdecod.h"

#include "wx/mac/private.h"

IMPLEMENT_DYNAMIC_CLASS(wxCursor, wxBitmap)

const short kwxCursorBullseye = 10 ;
const short kwxCursorBlank = 11 ;
const short kwxCursorPencil = 12 ;
const short kwxCursorMagnifier = 13 ;
const short kwxCursorNoEntry = 14 ;
const short kwxCursorPaintBrush = 15 ;
const short kwxCursorPointRight = 16 ;
const short kwxCursorPointLeft = 17 ;
const short kwxCursorQuestionArrow = 18 ;
const short kwxCursorRightArrow = 19 ;
const short kwxCursorSizeNS = 20 ;
const short kwxCursorSize = 21 ;
const short kwxCursorSizeNESW = 22 ;
const short kwxCursorSizeNWSE = 23 ;
const short kwxCursorRoller = 24 ;

wxCursor    gMacCurrentCursor ;

wxCursorRefData::wxCursorRefData()
{
    m_width = 16;
    m_height = 16;
    m_hCursor = NULL ;
    m_disposeHandle = false ;
    m_releaseHandle = false ;
    m_isColorCursor = false ;
    m_themeCursor = -1 ;
}

wxCursorRefData::~wxCursorRefData()
{
    if ( m_isColorCursor )
    {
        ::DisposeCCursor( (CCrsrHandle) m_hCursor ) ;
    }
    else if ( m_disposeHandle )
    {
        ::DisposeHandle( (Handle ) m_hCursor ) ;
    }
    else if ( m_releaseHandle )
    {
        // we don't release the resource since it may already
        // be in use again
    }
}

// Cursors
wxCursor::wxCursor()
{
}

wxCursor::wxCursor(const char WXUNUSED(bits)[], int WXUNUSED(width), int WXUNUSED(height),
    int WXUNUSED(hotSpotX), int WXUNUSED(hotSpotY), const char WXUNUSED(maskBits)[])
{
}

wxCursor::wxCursor( const wxImage &image )
{
    CreateFromImage( image ) ;
}

wxCursor::wxCursor(const char **bits)
{
    (void) CreateFromXpm(bits);
}

wxCursor::wxCursor(char **bits)
{
    (void) CreateFromXpm((const char **)bits);
}

bool wxCursor::CreateFromXpm(const char **bits)
{
    wxCHECK_MSG( bits != NULL, false, wxT("invalid cursor data") );
    wxXPMDecoder decoder;
    wxImage img = decoder.ReadData(bits);
    wxCHECK_MSG( img.Ok(), false, wxT("invalid cursor data") );
    CreateFromImage( img ) ;
    return true;
}

short GetCTabIndex( CTabHandle colors , RGBColor *col )
{
    short retval = 0 ;
    unsigned long bestdiff = 0xFFFF ;
    for ( int i = 0 ; i < (**colors).ctSize ; ++i )
    {
        unsigned long diff = abs(col->red -  (**colors).ctTable[i].rgb.red ) +
            abs(col->green -  (**colors).ctTable[i].rgb.green ) +
            abs(col->blue -  (**colors).ctTable[i].rgb.blue ) ;
        if ( diff < bestdiff )
        {
            bestdiff = diff ;
            retval = (**colors).ctTable[i].value ;
        }
    }
    return retval ;
}

void wxCursor::CreateFromImage(const wxImage & image)
{
    m_refData = new wxCursorRefData;

    int w = 16;
    int h = 16;

    int hotSpotX = image.GetOptionInt(wxIMAGE_OPTION_CUR_HOTSPOT_X);
    int hotSpotY = image.GetOptionInt(wxIMAGE_OPTION_CUR_HOTSPOT_Y);
    int image_w = image.GetWidth();
    int image_h = image.GetHeight();

    wxASSERT_MSG( hotSpotX >= 0 && hotSpotX < image_w &&
                  hotSpotY >= 0 && hotSpotY < image_h,
                  _T("invalid cursor hot spot coordinates") );

    wxImage image16(image); // final image of correct size

    // if image is too small then place it in the center, resize it if too big
    if ((w > image_w) && (h > image_h))
    {
        wxPoint offset((w - image_w)/2, (h - image_h)/2);
        hotSpotX = hotSpotX + offset.x;
        hotSpotY = hotSpotY + offset.y;

        image16 = image.Size(wxSize(w, h), offset);
    }
    else if ((w != image_w) || (h != image_h))
    {
        hotSpotX = int(hotSpotX * double(w) / double(image_w));
        hotSpotY = int(hotSpotY * double(h) / double(image_h));

        image16 = image.Scale(w, h);
    }

    unsigned char * rgbBits = image16.GetData();
    bool bHasMask = image16.HasMask() ;

#if 0
    // monochrome implementation
    M_CURSORDATA->m_hCursor = NewHandle( sizeof( Cursor ) ) ;
    M_CURSORDATA->m_disposeHandle = true ;
    HLock( (Handle) M_CURSORDATA->m_hCursor ) ;
    CursPtr cp = *(CursHandle)M_CURSORDATA->m_hCursor ;
    memset( cp->data , 0 , sizeof( Bits16 ) ) ;
    memset( cp->mask , 0 , sizeof( Bits16 ) ) ;

    unsigned char mr = image16.GetMaskRed() ;
    unsigned char mg = image16.GetMaskGreen() ;
    unsigned char mb = image16.GetMaskBlue() ;
    for ( int y = 0 ; y < h ; ++y )
    {
        short rowbits = 0 ;
        short maskbits = 0 ;

        for ( int x = 0 ; x < w ; ++x )
        {
            long pos = (y * w + x) * 3;

            unsigned char r = rgbBits[pos] ;
            unsigned char g = rgbBits[pos+1] ;
            unsigned char b = rgbBits[pos+2] ;
            if ( bHasMask && r==mr && g==mg && b==mb )
            {
                // masked area, does not appear anywhere
            }
            else
            {
                if ( (int)r + (int)g + (int)b < 0x0200 )
                {
                    rowbits |= ( 1 << (15-x) ) ;
                }
                maskbits |= ( 1 << (15-x) ) ;
            }
        }
        cp->data[y] = rowbits ;
        cp->mask[y] = maskbits ;
    }
    if ( !bHasMask )
    {
        memcpy( cp->mask , cp->data , sizeof( Bits16) ) ;
    }
    cp->hotSpot.h = hotSpotX ;
    cp->hotSpot.v = hotSpotY ;
    HUnlock( (Handle) M_CURSORDATA->m_hCursor ) ;
#else
    PixMapHandle pm = (PixMapHandle) NewHandleClear( sizeof (PixMap))  ;
    short extent = 16 ;
    short bytesPerPixel = 1 ;
    short depth = 8 ;
    Rect bounds = { 0 , 0 , extent , extent } ;
    CCrsrHandle ch = (CCrsrHandle) NewHandleClear ( sizeof( CCrsr ) ) ;
    CTabHandle newColors = GetCTable( 8 ) ;
    HandToHand((Handle *) &newColors);
    // set the values to the indices
    for ( int i = 0 ; i < (**newColors).ctSize ; ++i )
    {
        (**newColors).ctTable[i].value = i ;
    }
    HLock( (Handle) ch) ;
    (**ch).crsrType = 0x8001 ; // color cursors
    (**ch).crsrMap = pm ;
    short bytesPerRow = bytesPerPixel * extent ;

    (**pm).baseAddr = 0;
    (**pm).rowBytes = bytesPerRow | 0x8000;
    (**pm).bounds = bounds;
    (**pm).pmVersion = 0;
    (**pm).packType = 0;
    (**pm).packSize = 0;
    (**pm).hRes = 0x00480000; /* 72 DPI default res */
    (**pm).vRes = 0x00480000; /* 72 DPI default res */
    (**pm).pixelSize = depth;
    (**pm).pixelType = 0;
    (**pm).cmpCount = 1;
    (**pm).cmpSize = depth;
    (**pm).pmTable = newColors;

    (**ch).crsrData = NewHandleClear( extent * bytesPerRow ) ;
    (**ch).crsrXData = NULL ;
    (**ch).crsrXValid = 0;
    (**ch).crsrXHandle = NULL;

    (**ch).crsrHotSpot.h = hotSpotX ;
    (**ch).crsrHotSpot.v = hotSpotY ;
    (**ch).crsrXTable = NULL ;
    (**ch).crsrID = GetCTSeed() ;

    memset( (**ch).crsr1Data  , 0 , sizeof( Bits16 ) ) ;
    memset( (**ch).crsrMask , 0 , sizeof( Bits16 ) ) ;

    unsigned char mr = image16.GetMaskRed() ;
    unsigned char mg = image16.GetMaskGreen() ;
    unsigned char mb = image16.GetMaskBlue() ;
    for ( int y = 0 ; y < h ; ++y )
    {
        short rowbits = 0 ;
        short maskbits = 0 ;

        for ( int x = 0 ; x < w ; ++x )
        {
            long pos = (y * w + x) * 3;

            unsigned char r = rgbBits[pos] ;
            unsigned char g = rgbBits[pos+1] ;
            unsigned char b = rgbBits[pos+2] ;
            RGBColor col = { 0xFFFF ,0xFFFF, 0xFFFF } ;

            if ( bHasMask && r==mr && g==mg && b==mb )
            {
                // masked area, does not appear anywhere
            }
            else
            {
                if ( (int)r + (int)g + (int)b < 0x0200 )
                {
                    rowbits |= ( 1 << (15-x) ) ;
                }
                maskbits |= ( 1 << (15-x) ) ;

                col = *((RGBColor*) wxColor( r , g , b ).GetPixel()) ;
            }
            *((*(**ch).crsrData) + y * bytesPerRow + x) =
                GetCTabIndex( newColors , &col) ;
        }
        (**ch).crsr1Data[y] = rowbits ;
        (**ch).crsrMask[y] = maskbits ;
    }
    if ( !bHasMask )
    {
        memcpy( (**ch).crsrMask , (**ch).crsr1Data , sizeof( Bits16) ) ;
    }

    HUnlock((Handle) ch) ;
    M_CURSORDATA->m_hCursor = ch ;
    M_CURSORDATA->m_isColorCursor = true ;
#endif
}

wxCursor::wxCursor(const wxString& cursor_file, long flags, int hotSpotX, int hotSpotY)
{
    m_refData = new wxCursorRefData;
    if ( flags == wxBITMAP_TYPE_MACCURSOR_RESOURCE )
    {
        Str255 theName ;
        wxMacStringToPascal( cursor_file , theName ) ;

        wxStAppResource resload ;
        Handle resHandle = ::GetNamedResource( 'crsr' , theName ) ;
        if ( resHandle )
        {
            short theId = -1 ;
            OSType theType ;
            GetResInfo( resHandle , &theId , &theType , theName ) ;
            ReleaseResource( resHandle ) ;
            M_CURSORDATA->m_hCursor = GetCCursor( theId ) ;
            if ( M_CURSORDATA->m_hCursor )
                M_CURSORDATA->m_isColorCursor = true ;
        }
        else
        {
            Handle resHandle = ::GetNamedResource( 'CURS' , theName ) ;
            if ( resHandle )
            {
                short theId = -1 ;
                OSType theType ;
                GetResInfo( resHandle , &theId , &theType , theName ) ;
                ReleaseResource( resHandle ) ;
                 M_CURSORDATA->m_hCursor = GetCursor( theId ) ;
                if ( M_CURSORDATA->m_hCursor )
                    M_CURSORDATA->m_releaseHandle = true ;
            }
        }
    }
    else
    {
        wxImage image ;
        image.LoadFile( cursor_file , flags ) ;
        if( image.Ok() )
        {
            image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X,hotSpotX ) ;
            image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y,hotSpotY ) ;
            delete m_refData ;
            CreateFromImage(image) ;
        }
    }
}

// Cursors by stock number
wxCursor::wxCursor(int cursor_type)
{
    m_refData = new wxCursorRefData;

    switch (cursor_type)
    {
    case wxCURSOR_COPY_ARROW:
        M_CURSORDATA->m_themeCursor = kThemeCopyArrowCursor ;
        break;
    case wxCURSOR_WAIT:
        M_CURSORDATA->m_themeCursor = kThemeWatchCursor ;
        break;
    case wxCURSOR_IBEAM:
        M_CURSORDATA->m_themeCursor = kThemeIBeamCursor ;
        break;
    case wxCURSOR_CROSS:
        M_CURSORDATA->m_themeCursor = kThemeCrossCursor;
        break;
    case wxCURSOR_SIZENWSE:
        {
            wxStAppResource resload ;
            M_CURSORDATA->m_hCursor = ::GetCursor(kwxCursorSizeNWSE);
        }
        break;
    case wxCURSOR_SIZENESW:
        {
            wxStAppResource resload ;
            M_CURSORDATA->m_hCursor = ::GetCursor(kwxCursorSizeNESW);
        }
        break;
    case wxCURSOR_SIZEWE:
        {
            M_CURSORDATA->m_themeCursor = kThemeResizeLeftRightCursor;
        }
        break;
    case wxCURSOR_SIZENS:
        {
            wxStAppResource resload ;
            M_CURSORDATA->m_hCursor = ::GetCursor(kwxCursorSizeNS);
        }
        break;
    case wxCURSOR_SIZING:
        {
            wxStAppResource resload ;
            M_CURSORDATA->m_hCursor = ::GetCursor(kwxCursorSize);
        }
        break;
    case wxCURSOR_HAND:
        {
            M_CURSORDATA->m_themeCursor = kThemePointingHandCursor;
        }
        break;
    case wxCURSOR_BULLSEYE:
        {
            wxStAppResource resload ;
            M_CURSORDATA->m_hCursor = ::GetCursor(kwxCursorBullseye);
        }
        break;
    case wxCURSOR_PENCIL:
        {
            wxStAppResource resload ;
            M_CURSORDATA->m_hCursor = ::GetCursor(kwxCursorPencil);
        }
        break;
    case wxCURSOR_MAGNIFIER:
        {
            wxStAppResource resload ;
            M_CURSORDATA->m_hCursor = ::GetCursor(kwxCursorMagnifier);
        }
        break;
    case wxCURSOR_NO_ENTRY:
        {
            wxStAppResource resload ;
            M_CURSORDATA->m_hCursor = ::GetCursor(kwxCursorNoEntry);
        }
        break;
    case wxCURSOR_WATCH:
        {
            M_CURSORDATA->m_themeCursor = kThemeWatchCursor;
            break;
        }
    case wxCURSOR_PAINT_BRUSH:
        {
            wxStAppResource resload ;
            M_CURSORDATA->m_hCursor = ::GetCursor(kwxCursorPaintBrush);
            break;
        }
    case wxCURSOR_POINT_LEFT:
        {
            wxStAppResource resload ;
            M_CURSORDATA->m_hCursor = ::GetCursor(kwxCursorPointLeft);
            break;
        }
    case wxCURSOR_POINT_RIGHT:
        {
            wxStAppResource resload ;
            M_CURSORDATA->m_hCursor = ::GetCursor(kwxCursorPointRight);
            break;
        }
    case wxCURSOR_QUESTION_ARROW:
        {
            wxStAppResource resload ;
            M_CURSORDATA->m_hCursor = ::GetCursor(kwxCursorQuestionArrow);
            break;
        }
    case wxCURSOR_BLANK:
        {
            wxStAppResource resload ;
            M_CURSORDATA->m_hCursor = ::GetCursor(kwxCursorBlank);
            break;
        }
    case wxCURSOR_RIGHT_ARROW:
        {
            wxStAppResource resload ;
            M_CURSORDATA->m_hCursor = ::GetCursor(kwxCursorRightArrow);
            break;
        }
    case wxCURSOR_SPRAYCAN:
        {
            wxStAppResource resload ;
            M_CURSORDATA->m_hCursor = ::GetCursor(kwxCursorRoller);
            break;
        }
    case wxCURSOR_CHAR:
    case wxCURSOR_ARROW:
    case wxCURSOR_LEFT_BUTTON:
    case wxCURSOR_RIGHT_BUTTON:
    case wxCURSOR_MIDDLE_BUTTON:
    default:
        M_CURSORDATA->m_themeCursor = kThemeArrowCursor ;
        break;
    }
    if ( M_CURSORDATA->m_themeCursor == -1 )
        M_CURSORDATA->m_releaseHandle = true ;
}

void wxCursor::MacInstall() const
{
    gMacCurrentCursor = *this ;
    if ( m_refData && M_CURSORDATA->m_themeCursor != -1 )
    {
        SetThemeCursor( M_CURSORDATA->m_themeCursor ) ;
    }
    else if ( m_refData && M_CURSORDATA->m_hCursor )
    {
        if ( M_CURSORDATA->m_isColorCursor )
            ::SetCCursor( (CCrsrHandle) M_CURSORDATA->m_hCursor ) ;
        else
            ::SetCursor( * (CursHandle) M_CURSORDATA->m_hCursor ) ;
    }
    else
    {
        SetThemeCursor( kThemeArrowCursor ) ;
    }
}

wxCursor::~wxCursor()
{
}

// Global cursor setting
void wxSetCursor(const wxCursor& cursor)
{
    cursor.MacInstall() ;
}
