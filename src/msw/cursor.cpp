/////////////////////////////////////////////////////////////////////////////
// Name:        cursor.cpp
// Purpose:     wxCursor class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "cursor.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/list.h"
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/bitmap.h"
    #include "wx/icon.h"
    #include "wx/cursor.h"
#endif

#include "wx/module.h"
#include "wx/image.h"
#include "wx/msw/private.h"
#ifndef __WXMICROWIN__
#include "wx/msw/dib.h"
#endif

#if wxUSE_RESOURCE_LOADING_IN_MSW
    #include "wx/msw/curico.h"
    #include "wx/msw/curicop.h"
#endif

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxCursor, wxGDIObject)

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// Current cursor, in order to hang on to cursor handle when setting the cursor
// globally
static wxCursor *gs_globalCursor = NULL;

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class wxCursorModule : public wxModule
{
public:
    virtual bool OnInit()
    {
        gs_globalCursor = new wxCursor;

        return TRUE;
    }

    virtual void OnExit()
    {
        delete gs_globalCursor;
        gs_globalCursor = (wxCursor *)NULL;
    }
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxCursorRefData
// ----------------------------------------------------------------------------

wxCursorRefData::wxCursorRefData()
{
  m_width = 32;
  m_height = 32;

  m_destroyCursor = TRUE;
}

void wxCursorRefData::Free()
{
    if ( m_hCursor )
    {
#ifndef __WXMICROWIN__
        if ( m_destroyCursor )
            ::DestroyCursor((HCURSOR)m_hCursor);
#endif

        m_hCursor = 0;
    }
}

// ----------------------------------------------------------------------------
// Cursors
// ----------------------------------------------------------------------------

wxCursor::wxCursor()
{
}

wxCursor::wxCursor( const wxImage & image )
{
    //image has to be 32x32
    wxImage image32 = image.Scale(32,32);
    unsigned char * rgbBits = image32.GetData();
    int w = image32.GetWidth()  ;
    int h = image32.GetHeight() ;
    bool bHasMask = image32.HasMask() ;
    int imagebitcount = (w*h)/8;

    unsigned char r, g, b ;
    unsigned char * bits = new unsigned char [imagebitcount];
    unsigned char * maskBits = new unsigned char [imagebitcount];

    int i,j, i8; unsigned char c, cMask;
    for (i=0; i<imagebitcount; i++)
        {
        bits[i] = 0;
        i8 = i * 8;
//unlike gtk, the pixels go in the opposite order in the bytes
        cMask = 128;
        for (j=0; j<8; j++)
           {
           // possible overflow if we do the summation first ?
           c = rgbBits[(i8+j)*3]/3 + rgbBits[(i8+j)*3+1]/3 + rgbBits[(i8+j)*3+2]/3 ;
           //if average value is > mid grey
           if (c>127)
              bits[i] = bits[i] | cMask ;
           cMask = cMask / 2 ;
           }
        }
    if (bHasMask)
        {
        r = image32.GetMaskRed() ;
        g = image32.GetMaskGreen() ;
        b = image32.GetMaskBlue() ;

        for (i=0; i<imagebitcount; i++)
        {
        maskBits[i] = 0x0;
        i8 = i * 8;

        cMask = 128;
        for (j=0; j<8; j++)
           {
           if (rgbBits[(i8+j)*3] == r && rgbBits[(i8+j)*3+1] == g && rgbBits[(i8+j)*3+2] == b)
              maskBits[i] = maskBits[i] | cMask ;
           cMask = cMask / 2 ;
           }
        }
        }
      else
        {
        for (i=0; i<imagebitcount; i++)
            maskBits[i]= 0x0 ;
        }

    int hotSpotX = image32.GetOptionInt(wxCUR_HOTSPOT_X);
    int hotSpotY = image32.GetOptionInt(wxCUR_HOTSPOT_Y);
    if (hotSpotX < 0 || hotSpotX >= w)
            hotSpotX = 0;
    if (hotSpotY < 0 || hotSpotY >= h)
            hotSpotY = 0;

    wxCursorRefData *refData = new wxCursorRefData;
    m_refData = refData;
    refData->m_hCursor = (WXHCURSOR) CreateCursor ( wxGetInstance(), hotSpotX, hotSpotY, w, h, /*AND*/ maskBits, /*XOR*/ bits   );

    delete [] bits ;
    delete [] maskBits;

}

wxCursor::wxCursor(const char WXUNUSED(bits)[],
                   int WXUNUSED(width),
                   int WXUNUSED(height),
                   int WXUNUSED(hotSpotX), int WXUNUSED(hotSpotY),
                   const char WXUNUSED(maskBits)[])
{
}

wxCursor::wxCursor(const wxString& cursor_file,
                   long flags,
                   int hotSpotX, int hotSpotY)
{
#ifdef __WXMICROWIN__
    m_refData = NULL;
#else
    wxCursorRefData *refData = new wxCursorRefData;
    m_refData = refData;

    if (flags == wxBITMAP_TYPE_CUR_RESOURCE)
    {
#ifdef __WIN95__
        refData->m_hCursor = (WXHCURSOR) LoadImage(wxGetInstance(), cursor_file, IMAGE_CURSOR, 0, 0, 0);
#else
        refData->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), cursor_file);
#endif
    }
    else if (flags == wxBITMAP_TYPE_CUR)
    {
#ifdef __WIN95__
        refData->m_hCursor = (WXHCURSOR) LoadImage(wxGetInstance(), cursor_file, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
#else
#if wxUSE_RESOURCE_LOADING_IN_MSW
        refData->m_hCursor = (WXHCURSOR) ReadCursorFile(WXSTRINGCAST cursor_file, wxGetInstance(), &refData->m_width, &refData->m_height);
#endif
#endif
    }
    else if (flags == wxBITMAP_TYPE_ICO)
    {
#if wxUSE_RESOURCE_LOADING_IN_MSW
        refData->m_hCursor = (WXHCURSOR) IconToCursor(WXSTRINGCAST cursor_file, wxGetInstance(), hotSpotX, hotSpotY, &refData->m_width, &refData->m_height);
#endif
    }
    else if (flags == wxBITMAP_TYPE_BMP)
    {
#if wxUSE_RESOURCE_LOADING_IN_MSW
        HBITMAP hBitmap = 0;
        HPALETTE hPalette = 0;
        bool success = wxReadDIB(WXSTRINGCAST cursor_file, &hBitmap, &hPalette) != 0;
        if (!success)
            return;
        if (hPalette)
            DeleteObject(hPalette);
        POINT pnt;
        pnt.x = hotSpotX;
        pnt.y = hotSpotY;
        refData->m_hCursor = (WXHCURSOR) MakeCursorFromBitmap(wxGetInstance(), hBitmap, &pnt);
        DeleteObject(hBitmap);
#endif
    }

#if WXWIN_COMPATIBILITY_2
    refData->SetOk();
#endif // WXWIN_COMPATIBILITY_2

#endif
}

// Cursors by stock number
wxCursor::wxCursor(int cursor_type)
{
#ifdef __WXMICROWIN__
    m_refData = NULL;
#else
  wxCursorRefData *refData = new wxCursorRefData;
  m_refData = refData;

  switch (cursor_type)
  {
    case wxCURSOR_ARROWWAIT:
#ifndef __WIN16__
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_APPSTARTING);
      break;
#endif
    case wxCURSOR_WAIT:
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_WAIT);
      break;
    case wxCURSOR_IBEAM:
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_IBEAM);
      break;
    case wxCURSOR_CROSS:
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_CROSS);
      break;
    case wxCURSOR_SIZENWSE:
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_SIZENWSE);
      break;
    case wxCURSOR_SIZENESW:
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_SIZENESW);
      break;
    case wxCURSOR_SIZEWE:
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_SIZEWE);
      break;
    case wxCURSOR_SIZENS:
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_SIZENS);
      break;
    case wxCURSOR_CHAR:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_ARROW);
      break;
    }
    case wxCURSOR_HAND:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_HAND"));
      break;
    }
    case wxCURSOR_BULLSEYE:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_BULLSEYE"));
      break;
    }
    case wxCURSOR_PENCIL:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_PENCIL"));
      break;
    }
    case wxCURSOR_MAGNIFIER:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_MAGNIFIER"));
      break;
    }
    case wxCURSOR_NO_ENTRY:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_NO);
      break;
    }
    case wxCURSOR_LEFT_BUTTON:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_ARROW);
      break;
    }
    case wxCURSOR_RIGHT_BUTTON:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_ARROW);
      break;
    }
    case wxCURSOR_MIDDLE_BUTTON:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_ARROW);
      break;
    }
    case wxCURSOR_SIZING:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_SIZING"));
      break;
    }
    case wxCURSOR_WATCH:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_WAIT);
      break;
    }
    case wxCURSOR_SPRAYCAN:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_ROLLER"));
      break;
    }
    case wxCURSOR_PAINT_BRUSH:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_PBRUSH"));
      break;
    }
    case wxCURSOR_POINT_LEFT:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_PLEFT"));
      break;
    }
    case wxCURSOR_POINT_RIGHT:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_PRIGHT"));
      break;
    }
    case wxCURSOR_QUESTION_ARROW:
    {
//      refData->m_hCursor = (WXHCURSOR) LoadImage(wxGetInstance(), wxT("wxCURSOR_QARROW"), IMAGE_CURSOR, 16, 16, LR_MONOCHROME);
//      refData->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_QARROW"));
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_HELP);
      break;
    }
    case wxCURSOR_BLANK:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_BLANK"));
      break;
    }
    case wxCURSOR_RIGHT_ARROW:
    {
        refData->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_RIGHT_ARROW"));
        break;
    }
    default:
    case wxCURSOR_ARROW:
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_ARROW);
      break;
  }

  // no need to destroy the stock cursors
  ((wxCursorRefData *)m_refData)->m_destroyCursor = FALSE;
#endif
}

wxCursor::~wxCursor()
{
}

// ----------------------------------------------------------------------------
// Global cursor setting
// ----------------------------------------------------------------------------

const wxCursor *wxGetGlobalCursor()
{
    return gs_globalCursor;
}

void wxSetCursor(const wxCursor& cursor)
{
    if ( cursor.Ok() )
    {
#ifndef __WXMICROWIN__
        ::SetCursor(GetHcursorOf(cursor));
#endif

        if ( gs_globalCursor )
            *gs_globalCursor = cursor;
    }
}


