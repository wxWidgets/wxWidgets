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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "cursor.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/bitmap.h"
    #include "wx/icon.h"
    #include "wx/cursor.h"
    #include "wx/settings.h"
    #include "wx/intl.h"
#endif

#include "wx/module.h"
#include "wx/image.h"
#include "wx/msw/private.h"

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

        return true;
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
#ifndef __WXWINCE__
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

#if wxUSE_IMAGE
wxCursor::wxCursor(const wxImage& image)
{
    // image has to be of the standard cursor size, otherwise we won't be able
    // to create it
    const int w = wxCursorRefData::GetStandardWidth();
    const int h = wxCursorRefData::GetStandardHeight();

    const int hotSpotX = image.GetOptionInt(wxIMAGE_OPTION_CUR_HOTSPOT_X);
    const int hotSpotY = image.GetOptionInt(wxIMAGE_OPTION_CUR_HOTSPOT_Y);

    wxASSERT_MSG( hotSpotX >= 0 && hotSpotX < w &&
                    hotSpotY >= 0 && hotSpotY < h,
                  _T("invalid cursor hot spot coordinates") );

    HCURSOR hcursor = wxBitmapToHCURSOR
                      (
                        wxBitmap(image.Scale(w, h)),
                        hotSpotX,
                        hotSpotY
                      );
    if ( !hcursor )
    {
        wxLogWarning(_("Failed to create cursor."));
        return;
    }

    m_refData = new wxCursorRefData(hcursor, true /* delete it later */);
}
#endif

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

#ifndef __WXWINCE__
        case wxBITMAP_TYPE_CUR:
            hcursor = ::LoadCursorFromFile(filename);
            break;
#endif

        case wxBITMAP_TYPE_ICO:
            hcursor = wxBitmapToHCURSOR
                      (
                       wxIcon(filename, wxBITMAP_TYPE_ICO),
                       hotSpotX,
                       hotSpotY
                      );
            break;

        case wxBITMAP_TYPE_BMP:
            hcursor = wxBitmapToHCURSOR
                      (
                       wxBitmap(filename, wxBITMAP_TYPE_BMP),
                       hotSpotX,
                       hotSpotY
                      );
            break;

        default:
            wxFAIL_MSG( _T("unknown cursor resource type") );

            hcursor = NULL;
    }

    if ( hcursor )
    {
        m_refData = new wxCursorRefData(hcursor, true /* delete it later */);
    }
}

// Cursors by stock number
wxCursor::wxCursor(int idCursor)
{
    // all wxWidgets standard cursors
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
        { false, _T("WXCURSOR_RIGHT_ARROW")  }, // wxCURSOR_RIGHT_ARROW
        { false, _T("WXCURSOR_BULLSEYE")     }, // wxCURSOR_BULLSEYE
        {  true, IDC_ARROW                   }, // WXCURSOR_CHAR
        {  true, IDC_CROSS                   }, // WXCURSOR_CROSS
        { false, _T("WXCURSOR_HAND")         }, // wxCURSOR_HAND
        {  true, IDC_IBEAM                   }, // WXCURSOR_IBEAM
        {  true, IDC_ARROW                   }, // WXCURSOR_LEFT_BUTTON
        { false, _T("WXCURSOR_MAGNIFIER")    }, // wxCURSOR_MAGNIFIER
        {  true, IDC_ARROW                   }, // WXCURSOR_MIDDLE_BUTTON
        {  true, IDC_NO                      }, // WXCURSOR_NO_ENTRY
        { false, _T("WXCURSOR_PBRUSH")       }, // wxCURSOR_PAINT_BRUSH
        { false, _T("WXCURSOR_PENCIL")       }, // wxCURSOR_PENCIL
        { false, _T("WXCURSOR_PLEFT")        }, // wxCURSOR_POINT_LEFT
        { false, _T("WXCURSOR_PRIGHT")       }, // wxCURSOR_POINT_RIGHT
        {  true, IDC_HELP                    }, // WXCURSOR_QUESTION_ARROW
        {  true, IDC_ARROW                   }, // WXCURSOR_RIGHT_BUTTON
        {  true, IDC_SIZENESW                }, // WXCURSOR_SIZENESW
        {  true, IDC_SIZENS                  }, // WXCURSOR_SIZENS
        {  true, IDC_SIZENWSE                }, // WXCURSOR_SIZENWSE
        {  true, IDC_SIZEWE                  }, // WXCURSOR_SIZEWE
        {  true, IDC_SIZEALL                 }, // WXCURSOR_SIZING
        { false, _T("WXCURSOR_PBRUSH")       }, // wxCURSOR_SPRAYCAN
        {  true, IDC_WAIT                    }, // WXCURSOR_WAIT
        { false, _T("WXCURSOR_WATCH")        }, // WXCURSOR_WATCH
        { false, _T("WXCURSOR_BLANK")        }, // wxCURSOR_BLANK
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


