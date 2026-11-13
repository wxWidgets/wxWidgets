/////////////////////////////////////////////////////////////////////////////
// Name:        src/retro68/cursor.cpp
// Purpose:     wxCursor implementation
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/cursor.h"

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
    #include "wx/image.h"
    #include "wx/utils.h"
    #include "wx/window.h"
#endif

#include "wx/retro68/private/qd.h"

#include <string.h>

// Implemented in src/retro68/evtloop.cpp.
extern wxWindow *wxRetro68GetWindowUnderMouse();

//-----------------------------------------------------------------------------
// wxCursorRefData
//-----------------------------------------------------------------------------

class wxCursorRefData : public wxGDIRefData
{
public:
    wxCursorRefData()
    {
        memset(&m_cursor, 0, sizeof(m_cursor));
    }

    wxCursorRefData(const wxCursorRefData& data)
        : wxGDIRefData(),
          m_cursor(data.m_cursor),
          m_isArrow(data.m_isArrow)
    {
    }

    virtual bool IsOk() const override { return true; }

    Cursor m_cursor;

    // true if this is the standard arrow cursor, which can't be accessed
    // under Carbon in the same way as the other ones.
    bool m_isArrow = false;
};

#define M_CURSOR ((wxCursorRefData *)m_refData)

//-----------------------------------------------------------------------------
// helpers
//-----------------------------------------------------------------------------

namespace
{

// Bitmap of the horizontal resizing cursor, the vertical one is obtained by
// transposing it.
const unsigned short wxCursorSizeWE[16] =
{
    0x0000, 0x0000, 0x0000, 0x0000,
    0x0810, 0x1818, 0x3ffc, 0x7ffe,
    0x3ffc, 0x1818, 0x0810, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000
};

// Bitmap of the pointing hand cursor.
const unsigned short wxCursorHand[16] =
{
    0x0180, 0x0240, 0x0240, 0x0240,
    0x0258, 0x0256, 0x0255, 0x3255,
    0x4c01, 0x2401, 0x1001, 0x0802,
    0x0802, 0x0404, 0x0204, 0x03fc
};

// Bitmap of the "no entry" cursor.
const unsigned short wxCursorNoEntry[16] =
{
    0x07e0, 0x1ff8, 0x381c, 0x703e,
    0x6076, 0xe0e7, 0xc1c3, 0xc383,
    0xc703, 0xce07, 0xdc06, 0x7806,
    0x700e, 0x381c, 0x1ff8, 0x07e0
};

// Compute the mask for the given cursor data by dilating it by one pixel.
void ComputeMask(const Bits16 data, Bits16 mask)
{
    for ( int y = 0; y < 16; y++ )
    {
        unsigned short m = 0;
        for ( int dy = -1; dy <= 1; dy++ )
        {
            const int yy = y + dy;
            if ( yy < 0 || yy > 15 )
                continue;

            const unsigned short row = data[yy];
            m |= row | (row << 1) | (row >> 1);
        }

        mask[y] = m;
    }
}

void InitFromBits(Cursor& cursor, const unsigned short bits[16],
                  short hotX, short hotY, bool transpose = false)
{
    for ( int y = 0; y < 16; y++ )
    {
        if ( transpose )
        {
            unsigned short row = 0;
            for ( int x = 0; x < 16; x++ )
            {
                if ( bits[x] & (0x8000 >> y) )
                    row |= 0x8000 >> x;
            }
            cursor.data[y] = row;
        }
        else
        {
            cursor.data[y] = bits[y];
        }
    }

    ComputeMask(cursor.data, cursor.mask);

    cursor.hotSpot.h = hotX;
    cursor.hotSpot.v = hotY;
}

} // anonymous namespace

//-----------------------------------------------------------------------------
// wxCursor
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxCursor, wxObject);

void wxCursor::InitFromStock(wxStockCursor cursorId)
{
    wxCursorRefData* const data = new wxCursorRefData;
    m_refData = data;

    short sysCursor = 0;
    switch ( cursorId )
    {
        case wxCURSOR_IBEAM:
        case wxCURSOR_CHAR:
            sysCursor = iBeamCursor;
            break;

        case wxCURSOR_CROSS:
            sysCursor = crossCursor;
            break;

        case wxCURSOR_SIZING:
        case wxCURSOR_SIZENWSE:
        case wxCURSOR_SIZENESW:
            sysCursor = plusCursor;
            break;

        case wxCURSOR_WAIT:
        case wxCURSOR_WATCH:
        case wxCURSOR_ARROWWAIT:
            sysCursor = watchCursor;
            break;

        case wxCURSOR_SIZEWE:
            InitFromBits(data->m_cursor, wxCursorSizeWE, 7, 7);
            return;

        case wxCURSOR_SIZENS:
            InitFromBits(data->m_cursor, wxCursorSizeWE, 7, 7, true);
            return;

        case wxCURSOR_HAND:
            InitFromBits(data->m_cursor, wxCursorHand, 0, 7);
            return;

        case wxCURSOR_NO_ENTRY:
            InitFromBits(data->m_cursor, wxCursorNoEntry, 8, 8);
            return;

        case wxCURSOR_BLANK:
            // All zeros data and mask: nothing is shown.
            return;

        default:
            break;
    }

    if ( sysCursor )
    {
        CursHandle h = GetCursor(sysCursor);
        if ( h && *h )
        {
            data->m_cursor = **h;
            return;
        }
    }

    data->m_isArrow = true;
#if !TARGET_API_MAC_CARBON
    data->m_cursor = qd.arrow;
#endif
}

#if wxUSE_IMAGE

wxCursor::wxCursor(const wxBitmap& bitmap, int hotSpotX, int hotSpotY)
{
    wxImage image = bitmap.ConvertToImage();
    image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, hotSpotX);
    image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, hotSpotY);
    InitFromImage(image);
}

wxCursor::wxCursor(const wxImage& image)
{
    InitFromImage(image);
}

wxCursor::wxCursor(const char* const* xpmData)
{
    InitFromImage(wxImage(xpmData));
}

void wxCursor::InitFromImage(const wxImage& imageOrig)
{
    wxCHECK_RET( imageOrig.IsOk(), "invalid image" );

    int hotX = imageOrig.GetOptionInt(wxIMAGE_OPTION_CUR_HOTSPOT_X);
    int hotY = imageOrig.GetOptionInt(wxIMAGE_OPTION_CUR_HOTSPOT_Y);

    // Classic Mac OS cursors are always 16x16.
    wxImage image(imageOrig);
    if ( image.GetWidth() != 16 || image.GetHeight() != 16 )
    {
        hotX = hotX * 16 / image.GetWidth();
        hotY = hotY * 16 / image.GetHeight();
        image.Rescale(16, 16);
    }

    wxCursorRefData* const data = new wxCursorRefData;
    m_refData = data;

    const bool hasMask = image.HasMask();
    const unsigned char mr = image.GetMaskRed(),
                        mg = image.GetMaskGreen(),
                        mb = image.GetMaskBlue();

    for ( int y = 0; y < 16; y++ )
    {
        unsigned short dataRow = 0,
                       maskRow = 0;
        for ( int x = 0; x < 16; x++ )
        {
            const unsigned char r = image.GetRed(x, y),
                                g = image.GetGreen(x, y),
                                b = image.GetBlue(x, y);

            bool opaque = true;
            if ( hasMask && r == mr && g == mg && b == mb )
                opaque = false;
            if ( image.HasAlpha() && image.GetAlpha(x, y) < 128 )
                opaque = false;

            if ( opaque )
            {
                maskRow |= 0x8000 >> x;
                if ( (r * 30 + g * 59 + b * 11) / 100 < 128 )
                    dataRow |= 0x8000 >> x;
            }
        }

        data->m_cursor.data[y] = dataRow;
        data->m_cursor.mask[y] = maskRow;
    }

    data->m_cursor.hotSpot.h = static_cast<short>(wxClip(hotX, 0, 15));
    data->m_cursor.hotSpot.v = static_cast<short>(wxClip(hotY, 0, 15));
}

#endif // wxUSE_IMAGE

wxCursor::wxCursor(const wxString& cursor_file,
                   wxBitmapType type,
                   int hotSpotX, int hotSpotY)
{
#if wxUSE_IMAGE
    wxImage image;
    if ( !image.LoadFile(cursor_file, type) )
        return;

    if ( hotSpotX != 0 || hotSpotY != 0 ||
            !image.HasOption(wxIMAGE_OPTION_CUR_HOTSPOT_X) )
    {
        image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, hotSpotX);
        image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, hotSpotY);
    }

    InitFromImage(image);
#else
    wxUnusedVar(cursor_file);
    wxUnusedVar(type);
    wxUnusedVar(hotSpotX);
    wxUnusedVar(hotSpotY);
#endif
}

wxPoint wxCursor::GetHotSpot() const
{
    if ( !IsOk() )
        return wxDefaultPosition;

    return wxPoint(M_CURSOR->m_cursor.hotSpot.h, M_CURSOR->m_cursor.hotSpot.v);
}

const Cursor* wxCursor::GetMacCursor() const
{
    wxCHECK_MSG( IsOk(), nullptr, "invalid cursor" );

    return &M_CURSOR->m_cursor;
}

void wxCursor::MacInstall() const
{
    if ( !IsOk() || M_CURSOR->m_isArrow )
    {
        InitCursor();
        return;
    }

    SetCursor(&M_CURSOR->m_cursor);
}

wxGDIRefData *wxCursor::CreateGDIRefData() const
{
    return new wxCursorRefData;
}

wxGDIRefData *wxCursor::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxCursorRefData(*(wxCursorRefData *)data);
}

// ----------------------------------------------------------------------------
// Global cursor setting
// ----------------------------------------------------------------------------

void wxSetCursor(const wxCursor& cursor)
{
    if ( cursor.IsOk() )
        cursor.MacInstall();
    else
        InitCursor();
}

// ----------------------------------------------------------------------------
// busy cursor routines
// ----------------------------------------------------------------------------

static int gs_busyCount = 0;

void wxBeginBusyCursor(const wxCursor *cursor)
{
    if ( gs_busyCount++ > 0 )
        return;

    if ( cursor && cursor->IsOk() )
        cursor->MacInstall();
    else
        wxCursor(wxCURSOR_WATCH).MacInstall();
}

void wxEndBusyCursor()
{
    if ( !gs_busyCount )
        return;

    if ( --gs_busyCount > 0 )
        return;

    wxWindow* const win = wxRetro68GetWindowUnderMouse();
    if ( win )
        win->UpdateCursor();
    else
        InitCursor();
}

bool wxIsBusy()
{
    return gs_busyCount > 0;
}
