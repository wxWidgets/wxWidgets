///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/ownerdrw.cpp
// Purpose:     implementation of wxOwnerDrawn class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     13.11.97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_OWNER_DRAWN

#ifndef WX_PRECOMP
    #include "wx/window.h"
    #include "wx/font.h"
    #include "wx/bitmap.h"
    #include "wx/image.h"
    #include "wx/dcmemory.h"
    #include "wx/menu.h"
    #include "wx/utils.h"
    #include "wx/settings.h"
    #include "wx/menuitem.h"
    #include "wx/module.h"
    #include "wx/msw/wrapcctl.h"
#endif

#include "wx/ownerdrw.h"
#include "wx/fontutil.h"

#include "wx/msw/private.h"
#include "wx/msw/private/metrics.h"
#include "wx/msw/dc.h"

#ifndef SPI_GETKEYBOARDCUES
#define SPI_GETKEYBOARDCUES 0x100A
#endif

#ifndef DSS_HIDEPREFIX
#define DSS_HIDEPREFIX  0x0200
#endif

class wxMSWSystemMenuFontModule : public wxModule
{
public:
    virtual bool OnInit()
    {
        return true;
    }

    virtual void OnExit()
    {
        if ( ms_systemMenuFont )
        {
            delete ms_systemMenuFont;
            ms_systemMenuFont = NULL;
        }
    }

    static const wxFont& GetSystemMenuFont()
    {
        if ( !ms_systemMenuFont )
            DoInitFont();

        return *ms_systemMenuFont;
    }

    static int GetSystemMenuHeight()
    {
        if ( !ms_systemMenuHeight )
            DoInitMetrics();

        return ms_systemMenuHeight;
    }

    static bool AlwaysShowCues()
    {
        if ( !ms_systemMenuHeight )
            DoInitMetrics();

        return ms_alwaysShowCues;
    }

private:
    static void DoInitMetrics()
    {
        // iMenuHeight is the menu bar height and the menu items are less tall,
        // although I don't know by how much -- below is the value for my system
        ms_systemMenuHeight = wxMSWImpl::GetNonClientMetrics().iMenuHeight - 4;

        wxASSERT_MSG( ms_systemMenuHeight > 0,
                        "menu height should be positive" );

        if ( ::SystemParametersInfo(SPI_GETKEYBOARDCUES, 0,
                                    &ms_alwaysShowCues, 0) == 0 )
        {
            // if it's not supported, we must be on an old Windows version
            // which always shows them
            ms_alwaysShowCues = true;
        }
    }

    static void DoInitFont()
    {
        ms_systemMenuFont = new
          wxFont(wxNativeFontInfo(wxMSWImpl::GetNonClientMetrics().lfMenuFont));
    }

    static wxFont* ms_systemMenuFont;
    static int ms_systemMenuHeight;
    static bool ms_alwaysShowCues;


    DECLARE_DYNAMIC_CLASS(wxMSWSystemMenuFontModule)
};

// these static variables are from the wxMSWSystemMenuFontModule object
// and reflect the system settings returned by the Win32 API's
// SystemParametersInfo() call.

wxFont* wxMSWSystemMenuFontModule::ms_systemMenuFont = NULL;
int wxMSWSystemMenuFontModule::ms_systemMenuHeight = 0;
bool wxMSWSystemMenuFontModule::ms_alwaysShowCues = false;

IMPLEMENT_DYNAMIC_CLASS(wxMSWSystemMenuFontModule, wxModule)


// VC++ 6 gives a warning here:
//
//      return type for 'OwnerDrawnSet_wxImplementation_HashTable::iterator::
//      operator ->' is 'class wxOwnerDrawn ** ' (ie; not a UDT or reference to
//      a UDT.  Will produce errors if applied using infix notation.
//
// shut it down
#if defined __VISUALC__ && __VISUALC__ <= 1300
    #if __VISUALC__ >= 1200
        #pragma warning(push)
        #define POP_WARNINGS
    #endif
    #pragma warning(disable: 4284)
#endif

#include "wx/hashset.h"
WX_DECLARE_HASH_SET(wxOwnerDrawn*, wxPointerHash, wxPointerEqual, OwnerDrawnSet);

#ifdef POP_WARNINGS
    #pragma warning(pop)
#endif

// ============================================================================
// implementation of wxOwnerDrawn class
// ============================================================================

// ctor
// ----
wxOwnerDrawn::wxOwnerDrawn(const wxString& str,
                           bool bCheckable,
                           bool bMenuItem)
            : m_strName(str)
{
    if ( ms_nDefaultMarginWidth == 0 )
    {
       ms_nDefaultMarginWidth = ::GetSystemMetrics(SM_CXMENUCHECK) +
                                wxSystemSettings::GetMetric(wxSYS_EDGE_X);
       ms_nLastMarginWidth = ms_nDefaultMarginWidth;
    }

    m_bCheckable   = bCheckable;
    m_bOwnerDrawn  = false;
    m_isMenuItem   = bMenuItem;
    m_nHeight      = 0;
    m_nMarginWidth = ms_nLastMarginWidth;
}

wxOwnerDrawn::~wxOwnerDrawn()
{
}

bool wxOwnerDrawn::IsMenuItem() const
{
    return m_isMenuItem;
}


// these items will be set during the first invocation of the ctor,
// because the values will be determined by checking the system settings,
// which is a chunk of code
size_t wxOwnerDrawn::ms_nDefaultMarginWidth = 0;
size_t wxOwnerDrawn::ms_nLastMarginWidth = 0;


// drawing
// -------

wxFont wxOwnerDrawn::GetFontToUse() const
{
    wxFont font = m_font;
    if ( !font.Ok() )
    {
        if ( IsMenuItem() )
            font = wxMSWSystemMenuFontModule::GetSystemMenuFont();

        if ( !font.Ok() )
            font = *wxNORMAL_FONT;
    }

    return font;
}

// get size of the item
// The item size includes the menu string, the accel string,
// the bitmap and size for a submenu expansion arrow...
bool wxOwnerDrawn::OnMeasureItem(size_t *pwidth, size_t *pheight)
{
    if ( IsOwnerDrawn() )
    {
        wxMemoryDC dc;

        wxString str = wxStripMenuCodes(m_strName);

        // if we have a valid accel string, then pad out
        // the menu string so that the menu and accel string are not
        // placed on top of each other.
        if ( !m_strAccel.empty() )
        {
            str.Pad(str.length()%8);
            str += m_strAccel;
        }

        dc.SetFont(GetFontToUse());

        wxCoord w, h;
        dc.GetTextExtent(str, &w, &h);
        *pwidth = w;
        *pheight = h;

        // add space at the end of the menu for the submenu expansion arrow
        // this will also allow offsetting the accel string from the right edge
        *pwidth += GetMarginWidth() + 16;
    }
    else // don't draw the text, just the bitmap (if any)
    {
        *pwidth =
        *pheight = 0;
    }

    // increase size to accommodate bigger bitmaps if necessary
    if (m_bmpChecked.Ok())
    {
        // Is BMP height larger than text height?
        size_t adjustedHeight = m_bmpChecked.GetHeight();
        if ( *pheight < adjustedHeight )
            *pheight = adjustedHeight;

        const size_t widthBmp = m_bmpChecked.GetWidth();
        if ( IsOwnerDrawn() )
        {
            // widen the margin to fit the bitmap if necessary
            if ((size_t)GetMarginWidth() < widthBmp)
                SetMarginWidth(widthBmp);
        }
        else // we must allocate enough space for the bitmap
        {
            *pwidth += widthBmp;
        }
    }

    // add a 4-pixel separator, otherwise menus look cluttered
    *pwidth += 4;

    // make sure that this item is at least as tall as the system menu height
    const size_t heightStd = wxMSWSystemMenuFontModule::GetSystemMenuHeight();
    if ( *pheight < heightStd )
      *pheight = heightStd;

    // remember height for use in OnDrawItem
    m_nHeight = *pheight;

    return true;
}

// draw the item
bool wxOwnerDrawn::OnDrawItem(wxDC& dc,
                              const wxRect& rc,
                              wxODAction act,
                              wxODStatus st)
{
    // we do nothing on focus change
    if ( act == wxODFocusChanged )
        return true;


    // this flag determines whether or not an edge will
    // be drawn around the bitmap. In most "windows classic"
    // applications, a 1-pixel highlight edge is drawn around
    // the bitmap of an item when it is selected.  However,
    // with the new "luna" theme, no edge is drawn around
    // the bitmap because the background is white (this applies
    // only to "non-XP style" menus w/ bitmaps --
    // see IE 6 menus for an example)

    bool draw_bitmap_edge = true;

    // set the colors
    // --------------
    DWORD colBack, colText;
    if ( st & wxODSelected )
    {
        colBack = GetSysColor(COLOR_HIGHLIGHT);
        if (!(st & wxODDisabled))
        {
            colText = GetSysColor(COLOR_HIGHLIGHTTEXT);
        }
        else
        {
            colText = GetSysColor(COLOR_GRAYTEXT);
        }
    }
    else
    {
        // fall back to default colors if none explicitly specified
        colBack = m_colBack.Ok() ? wxColourToPalRGB(m_colBack)
                                 : GetSysColor(COLOR_MENU);
        colText = m_colText.Ok() ? wxColourToPalRGB(m_colText)
                                 : GetSysColor(COLOR_MENUTEXT);
    }

    if ( IsOwnerDrawn() )
    {
        // don't draw an edge around the bitmap, if background is white ...
        DWORD menu_bg_color = GetSysColor(COLOR_MENU);
        if (    ( GetRValue( menu_bg_color ) >= 0xf0 &&
                  GetGValue( menu_bg_color ) >= 0xf0 &&
                  GetBValue( menu_bg_color ) >= 0xf0 )
          )
        {
            draw_bitmap_edge = false;
        }
    }
    else // edge doesn't look well with default Windows drawing
    {
        draw_bitmap_edge = false;
    }


    wxMSWDCImpl *impl = (wxMSWDCImpl*) dc.GetImpl();
    HDC hdc = GetHdcOf(*impl);
    COLORREF colOldText = ::SetTextColor(hdc, colText),
             colOldBack = ::SetBkColor(hdc, colBack);

    // *2, as in wxSYS_EDGE_Y
    int margin = GetMarginWidth() + 2 * wxSystemSettings::GetMetric(wxSYS_EDGE_X);

    // select the font and draw the text
    // ---------------------------------


    // determine where to draw and leave space for a check-mark.
    // + 1 pixel to separate the edge from the highlight rectangle
    int xText = rc.x + margin + 1;


    // using native API because it recognizes '&'
    if ( IsOwnerDrawn() )
    {
        int nPrevMode = SetBkMode(hdc, TRANSPARENT);
        AutoHBRUSH hbr(colBack);
        SelectInHDC selBrush(hdc, hbr);

        RECT rectFill = { rc.GetLeft(), rc.GetTop(),
                            rc.GetRight() + 1, rc.GetBottom() + 1 };

        if ( (st & wxODSelected) && m_bmpChecked.Ok() && draw_bitmap_edge )
        {
            // only draw the highlight under the text, not under
            // the bitmap or checkmark
            rectFill.left = xText;
        }

        FillRect(hdc, &rectFill, hbr);

        // use default font if no font set
        wxFont fontToUse = GetFontToUse();
        SelectInHDC selFont(hdc, GetHfontOf(fontToUse));

        wxString strMenuText = m_strName.BeforeFirst('\t');

        xText += 3; // separate text from the highlight rectangle

        SIZE sizeRect;
        ::GetTextExtentPoint32(hdc, strMenuText.c_str(), strMenuText.length(), &sizeRect);

        int flags = DST_PREFIXTEXT;
        if ( (st & wxODDisabled) && !(st & wxODSelected) )
            flags |= DSS_DISABLED;

        if ( (st & wxODHidePrefix) &&
                !wxMSWSystemMenuFontModule::AlwaysShowCues() )
            flags |= DSS_HIDEPREFIX;

        ::DrawState
        (
            hdc,
            NULL,
            NULL,
            (LPARAM)strMenuText.wx_str(),
            strMenuText.length(),
            xText,
            rc.y + (rc.GetHeight() - sizeRect.cy + 1)/2, // centre vertically
            rc.GetWidth() - margin,
            sizeRect.cy,
            flags
        );

        // ::SetTextAlign(hdc, TA_RIGHT) doesn't work with DSS_DISABLED or DSS_MONO
        // as the last parameter in DrawState() (at least with Windows98). So we have
        // to take care of right alignment ourselves.
        if ( !m_strAccel.empty() )
        {
            int accel_width, accel_height;
            dc.GetTextExtent(m_strAccel, &accel_width, &accel_height);
            // right align accel string with right edge of menu ( offset by the
            // margin width )
            ::DrawState(hdc, NULL, NULL,
                    (LPARAM)m_strAccel.wx_str(),
                    m_strAccel.length(),
                    rc.GetWidth()-16-accel_width, rc.y+(int) ((rc.GetHeight()-sizeRect.cy)/2.0),
                    0, 0,
                    DST_TEXT |
                    (((st & wxODDisabled) && !(st & wxODSelected)) ? DSS_DISABLED : 0));
        }

        (void)SetBkMode(hdc, nPrevMode);
    }


    // draw the bitmap
    // ---------------
    if ( IsCheckable() && !m_bmpChecked.Ok() )
    {
        if ( st & wxODChecked )
        {
            // what goes on: DrawFrameControl creates a b/w mask,
            // then we copy it to screen to have right colors

            // first create a monochrome bitmap in a memory DC
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hbmpCheck = CreateBitmap(margin, m_nHeight, 1, 1, 0);
            SelectObject(hdcMem, hbmpCheck);

            // then draw a check mark into it
            RECT rect = { 0, 0, margin, m_nHeight };
            if ( m_nHeight > 0 )
            {
                ::DrawFrameControl(hdcMem, &rect, DFC_MENU, DFCS_MENUCHECK);
            }

            // finally copy it to screen DC and clean up
            BitBlt(hdc, rc.x, rc.y, margin, m_nHeight, hdcMem, 0, 0, SRCCOPY);

            DeleteDC(hdcMem);
            DeleteObject(hbmpCheck);
        }
    }
    else
    {
        wxBitmap bmp;

        if ( st & wxODDisabled )
        {
            bmp = GetDisabledBitmap();
        }

        if ( !bmp.Ok() )
        {
            // for not checkable bitmaps we should always use unchecked one
            // because their checked bitmap is not set
            bmp = GetBitmap(!IsCheckable() || (st & wxODChecked));

#if wxUSE_IMAGE
            if ( bmp.Ok() && st & wxODDisabled )
            {
                // we need to grey out the bitmap as we don't have any specific
                // disabled bitmap
                wxImage imgGrey = bmp.ConvertToImage().ConvertToGreyscale();
                if ( imgGrey.Ok() )
                    bmp = wxBitmap(imgGrey);
            }
#endif // wxUSE_IMAGE
        }

        if ( bmp.Ok() )
        {
            wxMemoryDC dcMem(&dc);
            dcMem.SelectObjectAsSource(bmp);

            // center bitmap
            int nBmpWidth = bmp.GetWidth(),
                nBmpHeight = bmp.GetHeight();

            // there should be enough space!
            wxASSERT((nBmpWidth <= rc.GetWidth()) && (nBmpHeight <= rc.GetHeight()));

            int heightDiff = m_nHeight - nBmpHeight;
            dc.Blit(rc.x + (margin - nBmpWidth) / 2,
                    rc.y + heightDiff / 2,
                    nBmpWidth, nBmpHeight,
                    &dcMem, 0, 0, wxCOPY, true /* use mask */);

            if ( ( st & wxODSelected ) && !( st & wxODDisabled ) && draw_bitmap_edge )
            {
                RECT rectBmp = { rc.GetLeft(), rc.GetTop(),
                                 rc.GetLeft() + margin,
                                 rc.GetTop() + m_nHeight };
                SetBkColor(hdc, colBack);

                DrawEdge(hdc, &rectBmp, BDR_RAISEDINNER, BF_RECT);
            }
        }
    }

    ::SetTextColor(hdc, colOldText);
    ::SetBkColor(hdc, colOldBack);

    return true;
}


// ----------------------------------------------------------------------------
// global helper functions implemented here
// ----------------------------------------------------------------------------

BOOL wxDrawStateBitmap(HDC hDC, HBITMAP hBitmap, int x, int y, UINT uState)
{
    // determine size of bitmap image
    BITMAP bmp;
    if ( !::GetObject(hBitmap, sizeof(BITMAP), &bmp) )
        return FALSE;

    BOOL result;

    switch ( uState )
    {
        case wxDSB_NORMAL:
        case wxDSB_SELECTED:
            {
                // uses image list functions to draw
                //  - normal bitmap with support transparency
                //    (image list internally create mask etc.)
                //  - blend bitmap with the background colour
                //    (like default selected items)
                HIMAGELIST hIml = ::ImageList_Create(bmp.bmWidth, bmp.bmHeight,
                                                     ILC_COLOR32 | ILC_MASK, 1, 1);
                ::ImageList_Add(hIml, hBitmap, NULL);
                UINT fStyle = uState == wxDSB_SELECTED ? ILD_SELECTED : ILD_NORMAL;
                result = ::ImageList_Draw(hIml, 0, hDC, x, y, fStyle);
                ::ImageList_Destroy(hIml);
            }
            break;

        case wxDSB_DISABLED:
            result = ::DrawState(hDC, NULL, NULL, (LPARAM)hBitmap, 0, x, y,
                                 bmp.bmWidth, bmp.bmHeight,
                                 DST_BITMAP | DSS_DISABLED);
            break;

        default:
            wxFAIL_MSG( _T("DrawStateBitmap: unknown wxDSBStates value") );
            result = FALSE;
    }

    return result;
}

#endif // wxUSE_OWNER_DRAWN
