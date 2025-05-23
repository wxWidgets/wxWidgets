/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/cursor.cpp
// Purpose:     wxCursor class
// Author:      Julian Smart
// Created:     01/02/97
// Copyright:   (c) 1997-2003 Julian Smart and Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/cursor.h"

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/bitmap.h"
    #include "wx/icon.h"
    #include "wx/settings.h"
    #include "wx/intl.h"
    #include "wx/image.h"
#endif

#include "wx/display.h"

#include "wx/msw/private.h"
#include "wx/msw/missing.h" // IDC_HAND

#include "wx/private/rescale.h"

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

    virtual void Free() override;


    // return the size of the standard cursor: notice that the system only
    // supports the cursors of this size
    static wxSize GetStandardSize();

private:
    bool m_destroyCursor;
};

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxCursor, wxGDIObject);

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// Current cursor, in order to hang on to cursor handle when setting the cursor
// globally
static wxCursor gs_globalCursor;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxCursorRefData
// ----------------------------------------------------------------------------


wxSize wxCursorRefData::GetStandardSize()
{
    const wxWindow* win = wxApp::GetMainTopWindow();
    // Cursors are always square, so using just wxSYS_CURSOR_X is good enough.
    const int size = wxSystemSettings::GetMetric(wxSYS_CURSOR_X, win);
    return wxSize(size, size);
}

wxCursorRefData::wxCursorRefData(HCURSOR hcursor, bool destroy)
{
    m_hCursor = (WXHCURSOR)hcursor;

    if ( m_hCursor )
    {
        const wxSize size = GetStandardSize();
        m_width = size.x;
        m_height = size.y;
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

wxCursor::wxCursor(const wxBitmap& bitmap, const wxPoint& hotSpot)
{
    InitFromBitmap(bitmap, hotSpot);
}

void wxCursor::InitFromBitmap(const wxBitmap& bmp, const wxPoint& hotSpot)
{
    HCURSOR hcursor = wxBitmapToHCURSOR( bmp, hotSpot.x, hotSpot.y );

    if ( !hcursor )
    {
        wxLogWarning(_("Failed to create cursor."));
        return;
    }

    m_refData = new wxCursorRefData(hcursor, true /* delete it later */);
}

#if wxUSE_IMAGE
wxCursor::wxCursor(const wxImage& image)
{
    InitFromImage(image);
}

wxCursor::wxCursor(const char* const* xpmData)
{
    InitFromImage(wxImage(xpmData));
}

void wxCursor::InitFromImage(const wxImage& image)
{
    InitFromBitmap
    (
        image,
        wxPoint(image.GetOptionInt(wxIMAGE_OPTION_CUR_HOTSPOT_X),
                image.GetOptionInt(wxIMAGE_OPTION_CUR_HOTSPOT_Y))
    );
}
#endif // wxUSE_IMAGE

wxCursor::wxCursor(const wxString& filename,
                   wxBitmapType kind,
                   int hotSpotX,
                   int hotSpotY)
{
    HCURSOR hcursor;
    switch ( kind )
    {
        case wxBITMAP_TYPE_CUR_RESOURCE:
            hcursor = ::LoadCursor(wxGetInstance(), filename.t_str());
            break;

        case wxBITMAP_TYPE_ANI:
        case wxBITMAP_TYPE_CUR:
            hcursor = ::LoadCursorFromFile(filename.t_str());
            break;

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
            wxLogError( wxT("unknown cursor resource type '%d'"), kind );

            hcursor = nullptr;
    }

    if ( hcursor )
    {
        m_refData = new wxCursorRefData(hcursor, true /* delete it later */);
    }
}

wxPoint wxCursor::GetHotSpot() const
{
    if ( !GetGDIImageData() )
        return wxDefaultPosition;

    AutoIconInfo ii;
    if ( !ii.GetFrom((HICON)GetGDIImageData()->m_hCursor) )
        return wxDefaultPosition;

    return wxPoint(ii.xHotspot, ii.yHotspot);
}

namespace
{

wxSize ScaleAndReverseBitmap(HBITMAP& bitmap, float scale)
{
    BITMAP bmp;
    if ( !::GetObject(bitmap, sizeof(bmp), &bmp) )
        return wxSize();
    wxSize cs(bmp.bmWidth * scale, bmp.bmHeight * scale);

    MemoryHDC hdc;
    SelectInHDC selBitmap(hdc, bitmap);
    if ( scale != 1 )
        ::SetStretchBltMode(hdc, HALFTONE);
    ::StretchBlt(hdc, cs.x - 1, 0, -cs.x, cs.y, hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);

    return cs;
}

HCURSOR CreateReverseCursor(HCURSOR cursor)
{
    AutoIconInfo info;
    if ( !info.GetFrom(cursor) )
        return nullptr;

    const unsigned displayID = (unsigned)wxDisplay::GetFromPoint(wxGetMousePosition());
    wxDisplay disp(displayID == 0u || displayID < wxDisplay::GetCount() ? displayID : 0u);
    const float scale = (float)disp.GetPPI().y / wxGetDisplayPPI().y;

    wxSize cursorSize = ScaleAndReverseBitmap(info.hbmMask, scale);
    if ( info.hbmColor )
        ScaleAndReverseBitmap(info.hbmColor, scale);
    info.xHotspot = (DWORD)(cursorSize.x - 1 - info.xHotspot * scale);
    info.yHotspot = (DWORD)(info.yHotspot * scale);

    return ::CreateIconIndirect(&info);
}

} // anonymous namespace

// Cursors by stock number
void wxCursor::InitFromStock(wxStockCursor idCursor)
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
        {  true, nullptr                        }, // wxCURSOR_NONE
        {  true, IDC_ARROW                   }, // wxCURSOR_ARROW
        { false, wxT("WXCURSOR_RIGHT_ARROW")  }, // wxCURSOR_RIGHT_ARROW
        { false, wxT("WXCURSOR_BULLSEYE")     }, // wxCURSOR_BULLSEYE
        {  true, IDC_ARROW                   }, // WXCURSOR_CHAR
        {  true, IDC_CROSS                   }, // WXCURSOR_CROSS
        {  true, IDC_HAND                    }, // wxCURSOR_HAND
        {  true, IDC_IBEAM                   }, // WXCURSOR_IBEAM
        {  true, IDC_ARROW                   }, // WXCURSOR_LEFT_BUTTON
        { false, wxT("WXCURSOR_MAGNIFIER")    }, // wxCURSOR_MAGNIFIER
        {  true, IDC_ARROW                   }, // WXCURSOR_MIDDLE_BUTTON
        {  true, IDC_NO                      }, // WXCURSOR_NO_ENTRY
        { false, wxT("WXCURSOR_PBRUSH")       }, // wxCURSOR_PAINT_BRUSH
        { false, wxT("WXCURSOR_PENCIL")       }, // wxCURSOR_PENCIL
        { false, wxT("WXCURSOR_PLEFT")        }, // wxCURSOR_POINT_LEFT
        { false, wxT("WXCURSOR_PRIGHT")       }, // wxCURSOR_POINT_RIGHT
        {  true, IDC_HELP                    }, // WXCURSOR_QUESTION_ARROW
        {  true, IDC_ARROW                   }, // WXCURSOR_RIGHT_BUTTON
        {  true, IDC_SIZENESW                }, // WXCURSOR_SIZENESW
        {  true, IDC_SIZENS                  }, // WXCURSOR_SIZENS
        {  true, IDC_SIZENWSE                }, // WXCURSOR_SIZENWSE
        {  true, IDC_SIZEWE                  }, // WXCURSOR_SIZEWE
        {  true, IDC_SIZEALL                 }, // WXCURSOR_SIZING
        { false, wxT("WXCURSOR_PBRUSH")       }, // wxCURSOR_SPRAYCAN
        {  true, IDC_WAIT                    }, // WXCURSOR_WAIT
        {  true, IDC_WAIT                    }, // WXCURSOR_WATCH
        { false, wxT("WXCURSOR_BLANK")        }, // wxCURSOR_BLANK
        {  true, IDC_APPSTARTING             }, // wxCURSOR_ARROWWAIT

        // no entry for wxCURSOR_MAX
    };

    wxCOMPILE_TIME_ASSERT( WXSIZEOF(stdCursors) == wxCURSOR_MAX,
                           CursorsIdArrayMismatch );

    wxCHECK_RET( idCursor > 0 && (size_t)idCursor < WXSIZEOF(stdCursors),
                 wxT("invalid cursor id in wxCursor() ctor") );

    const StdCursor& stdCursor = stdCursors[idCursor];
    bool deleteLater = !stdCursor.isStd;

    HCURSOR hcursor = ::LoadCursor(stdCursor.isStd ? nullptr : wxGetInstance(),
                                   stdCursor.name);

    // IDC_HAND may not be available on some versions of Windows.
    if ( !hcursor && idCursor == wxCURSOR_HAND)
    {
        hcursor = ::LoadCursor(wxGetInstance(), wxT("WXCURSOR_HAND"));
        deleteLater = true;
    }

    if ( !hcursor && idCursor == wxCURSOR_RIGHT_ARROW)
    {
        hcursor = ::LoadCursor(nullptr, IDC_ARROW);
        if ( hcursor )
        {
            hcursor = CreateReverseCursor(hcursor);
            deleteLater = true;
        }
    }

    if ( !hcursor )
    {
        if ( !stdCursor.isStd )
        {
            // it may be not obvious to the programmer why did loading fail,
            // try to help by pointing to the by far the most probable reason
            wxFAIL_MSG(wxT("Loading a cursor defined by wxWidgets failed, ")
                       wxT("did you include include/wx/msw/wx.rc file from ")
                       wxT("your resource file?"));
        }

        wxLogLastError(wxT("LoadCursor"));
    }
    else
    {
        m_refData = new wxCursorRefData(hcursor, deleteLater);
    }
}

// ----------------------------------------------------------------------------
// other wxCursor functions
// ----------------------------------------------------------------------------

wxGDIImageRefData *wxCursor::CreateData() const
{
    return new wxCursorRefData;
}

// ----------------------------------------------------------------------------
// Global cursor setting
// ----------------------------------------------------------------------------

const wxCursor *wxGetGlobalCursor()
{
    return &gs_globalCursor;
}

void wxSetCursor(const wxCursorBundle& cursors)
{
    const wxCursor& cursor = cursors.GetCursorForMainWindow();
    if ( cursor.IsOk() )
        ::SetCursor(GetHcursorOf(cursor));

    gs_globalCursor = cursor;
}
