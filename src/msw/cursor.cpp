/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/cursor.cpp
// Purpose:     wxCursor class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) 1997-2003 Julian Smart and Vadim Zeitlin
// Licence:     wxWindows licence
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

#include "wx/ptr_scpd.h"

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

// define functions missing in MicroWin
#ifdef __WXMICROWIN__
    static inline void DestroyCursor(HCURSOR) { }
    static inline void SetCursor(HCURSOR) { }
#endif // __WXMICROWIN__

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxCursorRefData : public wxGDIImageRefData
{
public:
    // the second parameter is used to tell us to delete the cursor when we're
    // done with it (normally we shouldn't call DestroyCursor() this is why it
    // doesn't happen by default)
    wxCursorRefData(HCURSOR hcursor = 0, bool takeOwnership = false);

    virtual ~wxCursorRefData() { Free(); }

    virtual void Free();


    // return the size of the standard cursor: notice that the system only
    // supports the cursors of this size
    static wxCoord GetStandardWidth();
    static wxCoord GetStandardHeight();

private:
    bool m_destroyCursor;

    // standard cursor size, computed on first use
    static wxSize ms_sizeStd;
};

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

// ----------------------------------------------------------------------------
// other types
// ----------------------------------------------------------------------------

wxDECLARE_SCOPED_ARRAY(unsigned char, ByteArray);
wxDEFINE_SCOPED_ARRAY(unsigned char, ByteArray);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxCursorRefData
// ----------------------------------------------------------------------------

wxSize wxCursorRefData::ms_sizeStd;

wxCoord wxCursorRefData::GetStandardWidth()
{
    if ( !ms_sizeStd.x )
        ms_sizeStd.x = wxSystemSettings::GetMetric(wxSYS_CURSOR_X);

    return ms_sizeStd.x;
}

wxCoord wxCursorRefData::GetStandardHeight()
{
    if ( !ms_sizeStd.y )
        ms_sizeStd.y = wxSystemSettings::GetMetric(wxSYS_CURSOR_Y);

    return ms_sizeStd.y;
}

wxCursorRefData::wxCursorRefData(HCURSOR hcursor, bool destroy)
{
    m_hCursor = (WXHCURSOR)hcursor;

    if ( m_hCursor )
    {
        m_width = GetStandardWidth();
        m_height = GetStandardHeight();
    }

    m_destroyCursor = destroy;
}

void wxCursorRefData::Free()
{
    if ( m_hCursor )
    {
        if ( m_destroyCursor )
            ::DestroyCursor((HCURSOR)m_hCursor);

        m_hCursor = 0;
    }
}

// ----------------------------------------------------------------------------
// Cursors
// ----------------------------------------------------------------------------

wxCursor::wxCursor()
{
}

wxCursor::wxCursor(const wxImage& image)
{
    // image has to be of the standard cursor size, otherwise we won't be able
    // to create it
    const int w = wxCursorRefData::GetStandardWidth();
    const int h = wxCursorRefData::GetStandardHeight();

    wxImage image32 = image.Scale(w, h);

    const int imagebitcount = (w*h)/8;

    ByteArray bits(new unsigned char [imagebitcount]),
              maskBits(new unsigned char [imagebitcount]);

    int i, j, i8;
    unsigned char c, cMask;

    const unsigned char * const rgbBits = image32.GetData();

    // first create the XOR mask
    for ( i = 0; i < imagebitcount; i++ )
    {
        bits[i] = 0;
        i8 = i * 8;
        // unlike gtk, the pixels go in the opposite order in the bytes
        cMask = 128;
        for ( j = 0; j < 8; j++ )
        {
            // possible overflow if we do the summation first ?
            c = rgbBits[(i8+j)*3]/3 +
                rgbBits[(i8+j)*3+1]/3 +
                rgbBits[(i8+j)*3+2]/3;

            // if average value is > mid grey
            if ( c > 127 )
                bits[i] = bits[i] | cMask;
            cMask = cMask / 2;
        }
    }

    // now the AND one
    if ( image32.HasMask() )
    {
        unsigned char r = image32.GetMaskRed(),
                      g = image32.GetMaskGreen(),
                      b = image32.GetMaskBlue();

        for ( i = 0; i < imagebitcount; i++ )
        {
            maskBits[i] = 0x0;
            i8 = i * 8;

            cMask = 128;
            for ( j = 0; j < 8; j++ )
            {
                if ( rgbBits[(i8+j)*3] == r &&
                        rgbBits[(i8+j)*3+1] == g &&
                            rgbBits[(i8+j)*3+2] == b )
                {
                    maskBits[i] = maskBits[i] | cMask;
                }

                cMask = cMask / 2;
            }
        }
    }
    else // no mask in the image
    {
        memset(maskBits.get(), 0, sizeof(unsigned char)*imagebitcount);
    }

    // determine where should the cursors hot spot be
    int hotSpotX = image32.GetOptionInt(wxCUR_HOTSPOT_X);
    int hotSpotY = image32.GetOptionInt(wxCUR_HOTSPOT_Y);
    if (hotSpotX < 0 || hotSpotX >= w)
        hotSpotX = 0;
    if (hotSpotY < 0 || hotSpotY >= h)
        hotSpotY = 0;

    // do create cursor now
    HCURSOR hcursor = ::CreateCursor
                        (
                            wxGetInstance(),
                            hotSpotX, hotSpotY,
                            w, h,
                            /* AND */ maskBits.get(),
                            /* XOR */ bits.get()
                        );

    if ( !hcursor )
    {
        wxLogLastError(_T("CreateCursor"));
    }
    else
    {
        m_refData = new wxCursorRefData(hcursor, true /* delete it */);
    }
}

wxCursor::wxCursor(const char WXUNUSED(bits)[],
                   int WXUNUSED(width),
                   int WXUNUSED(height),
                   int WXUNUSED(hotSpotX), int WXUNUSED(hotSpotY),
                   const char WXUNUSED(maskBits)[])
{
}

// MicroWin doesn't have support needed for the other ctors
#ifdef __WXMICROWIN__

wxCursor::wxCursor(const wxString& WXUNUSED(filename),
                   long WXUNUSED(kind),
                   int WXUNUSED(hotSpotX),
                   int WXUNUSED(hotSpotY))
{
}

wxCursor::wxCursor(int WXUNUSED(cursor_type))
{
}

#else // !__WXMICROWIN__

wxCursor::wxCursor(const wxString& filename,
                   long kind,
                   int hotSpotX,
                   int hotSpotY)
{
    HCURSOR hcursor;
    switch ( kind )
    {
        case wxBITMAP_TYPE_CUR_RESOURCE:
            hcursor = ::LoadCursor(wxGetInstance(), filename);
            break;

        case wxBITMAP_TYPE_CUR:
            hcursor = ::LoadCursorFromFile(filename);
            break;

        case wxBITMAP_TYPE_ICO:
#if wxUSE_RESOURCE_LOADING_IN_MSW
            hcursor = IconToCursor((wxChar *)filename.c_str(),
                                   wxGetInstance(),
                                   hotSpotX, hotSpotY,
                                   NULL, NULL);
#endif // wxUSE_RESOURCE_LOADING_IN_MSW
            break;

        case wxBITMAP_TYPE_BMP:
            {
#if wxUSE_RESOURCE_LOADING_IN_MSW
                HBITMAP hBitmap = 0;
                HPALETTE hPalette = 0;
                if ( wxReadDIB((wxChar *)filename.c_str(), &hBitmap, &hPalette) )
                {
                    if (hPalette)
                        DeleteObject(hPalette);

                    POINT pt;
                    pt.x = hotSpotX;
                    pt.y = hotSpotY;
                    hcursor = MakeCursorFromBitmap(wxGetInstance(), hBitmap, &pt);
                    DeleteObject(hBitmap);
                }
                else
#endif // wxUSE_RESOURCE_LOADING_IN_MSW
                {
                    hcursor = NULL;
                }
            }
            break;

        default:
            wxFAIL_MSG( _T("unknown cursor resource type") );

            hcursor = NULL;
    }

    if ( hcursor )
    {
        m_refData = new wxCursorRefData(hcursor, true /* delete it later */);

#if WXWIN_COMPATIBILITY_2
        refData->SetOk();
#endif // WXWIN_COMPATIBILITY_2
    }
}

// Cursors by stock number
wxCursor::wxCursor(int idCursor)
{
    // all wxWindows standard cursors
    static const struct StdCursor
    {
        // is this a standard Windows cursor?
        bool isStd;

        // the cursor name or id
        LPCTSTR name;
    } stdCursors[] =
    {
        {  true, NULL                        }, // wxCURSOR_NONE
        {  true, IDC_ARROW                   }, // wxCURSOR_ARROW
        { false, _T("wxCURSOR_RIGHT_ARROW")  }, // wxCURSOR_RIGHT_ARROW
        { false, _T("wxCURSOR_BULLSEYE")     }, // wxCURSOR_BULLSEYE
        {  true, IDC_ARROW                   }, // wxCURSOR_CHAR
        {  true, IDC_CROSS                   }, // wxCURSOR_CROSS
        { false, _T("wxCURSOR_HAND")         }, // wxCURSOR_HAND
        {  true, IDC_IBEAM                   }, // wxCURSOR_IBEAM
        {  true, IDC_ARROW                   }, // wxCURSOR_LEFT_BUTTON
        { false, _T("wxCURSOR_MAGNIFIER")    }, // wxCURSOR_MAGNIFIER
        {  true, IDC_ARROW                   }, // wxCURSOR_MIDDLE_BUTTON
        {  true, IDC_NO                      }, // wxCURSOR_NO_ENTRY
        { false, _T("wxCURSOR_PAINT_BRUSH")  }, // wxCURSOR_PAINT_BRUSH
        { false, _T("wxCURSOR_PENCIL")       }, // wxCURSOR_PENCIL
        { false, _T("wxCURSOR_POINT_LEFT")   }, // wxCURSOR_POINT_LEFT
        { false, _T("wxCURSOR_POINT_RIGHT")  }, // wxCURSOR_POINT_RIGHT
        {  true, IDC_HELP                    }, // wxCURSOR_QUESTION_ARROW
        {  true, IDC_ARROW                   }, // wxCURSOR_RIGHT_BUTTON
        {  true, IDC_SIZENESW                }, // wxCURSOR_SIZENESW
        {  true, IDC_SIZENS                  }, // wxCURSOR_SIZENS
        {  true, IDC_SIZENWSE                }, // wxCURSOR_SIZENWSE
        {  true, IDC_SIZEWE                  }, // wxCURSOR_SIZEWE
        {  true, IDC_SIZEALL                 }, // wxCURSOR_SIZING
        { false, _T("wxCURSOR_SPRAYCAN")     }, // wxCURSOR_SPRAYCAN
        {  true, IDC_WAIT                    }, // wxCURSOR_WAIT
        {  true, IDC_WAIT                    }, // wxCURSOR_WATCH
        { false, _T("wxCURSOR_BLANK")        }, // wxCURSOR_BLANK
        {  true, IDC_APPSTARTING             }, // wxCURSOR_ARROWWAIT

        // no entry for wxCURSOR_MAX
    };

    wxCOMPILE_TIME_ASSERT( WXSIZEOF(stdCursors) == wxCURSOR_MAX,
                           CursorsIdArrayMismatch );

    wxCHECK_RET( idCursor > 0 && (size_t)idCursor < WXSIZEOF(stdCursors),
                 _T("invalid cursor id in wxCursor() ctor") );

    const StdCursor& stdCursor = stdCursors[idCursor];

    HCURSOR hcursor = ::LoadCursor(stdCursor.isStd ? NULL : wxGetInstance(),
                                   stdCursor.name);

    if ( !hcursor )
    {
        wxLogLastError(_T("LoadCursor"));
    }
    else
    {
        m_refData = new wxCursorRefData(hcursor);
    }
}

#endif // __WXMICROWIN__/!__WXMICROWIN__

wxCursor::~wxCursor()
{
}

// ----------------------------------------------------------------------------
// other wxCursor functions
// ----------------------------------------------------------------------------

bool wxCursor::operator==(const wxCursor& cursor) const
{
    if ( !m_refData )
        return !cursor.m_refData;

    return cursor.m_refData &&
                ((wxCursorRefData *)m_refData)->m_hCursor ==
                ((wxCursorRefData *)cursor.m_refData)->m_hCursor;
}

wxGDIImageRefData *wxCursor::CreateData() const
{
    return new wxCursorRefData;
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
        ::SetCursor(GetHcursorOf(cursor));

        if ( gs_globalCursor )
            *gs_globalCursor = cursor;
    }
}


