///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/menuitem.cpp
// Purpose:     wxMenuItem implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     11.11.97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_MENUS

#include "wx/menuitem.h"
#include "wx/stockitem.h"

#ifndef WX_PRECOMP
    #include "wx/font.h"
    #include "wx/bitmap.h"
    #include "wx/settings.h"
    #include "wx/window.h"
    #include "wx/accel.h"
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/menu.h"
#endif

#if wxUSE_ACCEL
    #include "wx/accel.h"
#endif // wxUSE_ACCEL

#include "wx/msw/private.h"
#include "wx/msw/dc.h"

#ifdef __WXWINCE__
// Implemented in menu.cpp
UINT GetMenuState(HMENU hMenu, UINT id, UINT flags) ;
#endif

// ---------------------------------------------------------------------------
// macro
// ---------------------------------------------------------------------------

// hide the ugly cast
#define GetHMenuOf(menu)    ((HMENU)menu->GetHMenu())

// ============================================================================
// implementation
// ============================================================================

#if wxUSE_OWNER_DRAWN

#include "wx/fontutil.h"
#include "wx/msw/private/metrics.h"

#ifndef SPI_GETKEYBOARDCUES
#define SPI_GETKEYBOARDCUES 0x100A
#endif

#ifndef DSS_HIDEPREFIX
#define DSS_HIDEPREFIX  0x0200
#endif

#endif // wxUSE_OWNER_DRAWN

// ----------------------------------------------------------------------------
// dynamic classes implementation
// ----------------------------------------------------------------------------

#if wxUSE_EXTENDED_RTTI

bool wxMenuItemStreamingCallback( const wxObject *object, wxWriter * , wxPersister * , wxxVariantArray & )
{
    const wxMenuItem * mitem = dynamic_cast<const wxMenuItem*>(object) ;
    if ( mitem->GetMenu() && !mitem->GetMenu()->GetTitle().empty() )
    {
        // we don't stream out the first two items for menus with a title, they will be reconstructed
        if ( mitem->GetMenu()->FindItemByPosition(0) == mitem || mitem->GetMenu()->FindItemByPosition(1) == mitem )
            return false ;
    }
    return true ;
}

wxBEGIN_ENUM( wxItemKind )
    wxENUM_MEMBER( wxITEM_SEPARATOR )
    wxENUM_MEMBER( wxITEM_NORMAL )
    wxENUM_MEMBER( wxITEM_CHECK )
    wxENUM_MEMBER( wxITEM_RADIO )
wxEND_ENUM( wxItemKind )

IMPLEMENT_DYNAMIC_CLASS_XTI_CALLBACK(wxMenuItem, wxObject,"wx/menuitem.h",wxMenuItemStreamingCallback)

wxBEGIN_PROPERTIES_TABLE(wxMenuItem)
    wxPROPERTY( Parent,wxMenu*, SetMenu, GetMenu, EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group") )
    wxPROPERTY( Id,int, SetId, GetId, EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group") )
    wxPROPERTY( Text, wxString , SetText, GetText, wxString(), 0 /*flags*/ , wxT("Helpstring") , wxT("group") )
    wxPROPERTY( Help, wxString , SetHelp, GetHelp, wxString(), 0 /*flags*/ , wxT("Helpstring") , wxT("group") )
    wxREADONLY_PROPERTY( Kind, wxItemKind , GetKind , EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group") )
    wxPROPERTY( SubMenu,wxMenu*, SetSubMenu, GetSubMenu, EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group") )
    wxPROPERTY( Enabled , bool , Enable , IsEnabled , wxxVariant((bool)true) , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY( Checked , bool , Check , IsChecked , wxxVariant((bool)false) , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY( Checkable , bool , SetCheckable , IsCheckable , wxxVariant((bool)false) , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
wxEND_PROPERTIES_TABLE()

wxBEGIN_HANDLERS_TABLE(wxMenuItem)
wxEND_HANDLERS_TABLE()

wxDIRECT_CONSTRUCTOR_6( wxMenuItem , wxMenu* , Parent , int , Id , wxString , Text , wxString , Help , wxItemKind , Kind , wxMenu* , SubMenu  )
#else
IMPLEMENT_DYNAMIC_CLASS(wxMenuItem, wxObject)
#endif

// ----------------------------------------------------------------------------
// wxMenuItem
// ----------------------------------------------------------------------------

#if wxUSE_OWNER_DRAWN

// these static variables are from the wxMenuItem object for cache
// system settings returned by the Win32 API's SystemParametersInfo() call
wxFont wxMenuItem::ms_systemMenuFont;
size_t wxMenuItem::ms_systemMenuHeight = 0;
bool   wxMenuItem::ms_alwaysShowCues = false;

#endif // wxUSE_OWNER_DRAWN


// ctor & dtor
// -----------

wxMenuItem::wxMenuItem(wxMenu *pParentMenu,
                       int id,
                       const wxString& text,
                       const wxString& strHelp,
                       wxItemKind kind,
                       wxMenu *pSubMenu)
          : wxMenuItemBase(pParentMenu, id, text, strHelp, kind, pSubMenu)
{
    Init();
}

#if WXWIN_COMPATIBILITY_2_8
wxMenuItem::wxMenuItem(wxMenu *parentMenu,
                       int id,
                       const wxString& text,
                       const wxString& help,
                       bool isCheckable,
                       wxMenu *subMenu)
          : wxMenuItemBase(parentMenu, id, text, help,
                           isCheckable ? wxITEM_CHECK : wxITEM_NORMAL, subMenu)
{
    Init();
}
#endif

void wxMenuItem::Init()
{
    m_radioGroup.start = -1;
    m_isRadioGroupStart = false;

#if  wxUSE_OWNER_DRAWN

    // init static varaibles
    if ( !ms_systemMenuHeight )
    {
        const NONCLIENTMETRICS& metrics = wxMSWImpl::GetNonClientMetrics();

        ms_systemMenuFont = wxFont(wxNativeFontInfo(metrics.lfMenuFont));
        ms_systemMenuHeight = metrics.iMenuHeight;

        if ( ::SystemParametersInfo(SPI_GETKEYBOARDCUES, 0,
                                    &ms_alwaysShowCues, 0) == 0 )
        {
            // if it's not supported, we must be on an old Windows version
            // which always shows them
            ms_alwaysShowCues = true;
        }

    }

    // when the color is not valid, wxOwnerDraw takes the default ones.
    // If we set the colors here and they are changed by the user during
    // the execution, then the colors are not updated until the application
    // is restarted and our menus look bad
    SetTextColour(wxNullColour);
    SetBackgroundColour(wxNullColour);

    // setting default colors switched ownerdraw on: switch it off again
    SetOwnerDrawn(false);

    //  switch ownerdraw back on if using a non default margin
    if ( !IsSeparator() )
        SetMarginWidth(GetMarginWidth());

#endif // wxUSE_OWNER_DRAWN
}

wxMenuItem::~wxMenuItem()
{
}

// misc
// ----

// return the id for calling Win32 API functions
WXWPARAM wxMenuItem::GetMSWId() const
{
    // we must use ids in unsigned short range with Windows functions, if we
    // pass ids > USHRT_MAX to them they get very confused (e.g. start
    // generating WM_COMMAND messages with negative high word of wParam), so
    // use the cast to ensure the id is in range
    return m_subMenu ? wxPtrToUInt(m_subMenu->GetHMenu())
                     : static_cast<unsigned short>(GetId());
}

// get item state
// --------------

bool wxMenuItem::IsChecked() const
{
    // fix that RTTI is always getting the correct state (separators cannot be
    // checked, but the Windows call below returns true
    if ( IsSeparator() )
        return false;

    // the item might not be attached to a menu yet
    //
    // TODO: shouldn't we just always call the base class version? It seems
    //       like it ought to always be in sync
    if ( !m_parentMenu )
        return wxMenuItemBase::IsChecked();

    HMENU hmenu = GetHMenuOf(m_parentMenu);
    int flag = ::GetMenuState(hmenu, GetMSWId(), MF_BYCOMMAND);

    return (flag & MF_CHECKED) != 0;
}

// radio group stuff
// -----------------

void wxMenuItem::SetAsRadioGroupStart()
{
    m_isRadioGroupStart = true;
}

void wxMenuItem::SetRadioGroupStart(int start)
{
    wxASSERT_MSG( !m_isRadioGroupStart,
                  wxT("should only be called for the next radio items") );

    m_radioGroup.start = start;
}

void wxMenuItem::SetRadioGroupEnd(int end)
{
    wxASSERT_MSG( m_isRadioGroupStart,
                  wxT("should only be called for the first radio item") );

    m_radioGroup.end = end;
}

// change item state
// -----------------

void wxMenuItem::Enable(bool enable)
{
    if ( m_isEnabled == enable )
        return;

    if ( m_parentMenu )
    {
        long rc = EnableMenuItem(GetHMenuOf(m_parentMenu),
                                 GetMSWId(),
                                 MF_BYCOMMAND |
                                 (enable ? MF_ENABLED : MF_GRAYED));

        if ( rc == -1 )
        {
            wxLogLastError(wxT("EnableMenuItem"));
        }
    }

    wxMenuItemBase::Enable(enable);
}

void wxMenuItem::Check(bool check)
{
    wxCHECK_RET( IsCheckable(), wxT("only checkable items may be checked") );

    if ( m_isChecked == check )
        return;

    if ( m_parentMenu )
    {
        int flags = check ? MF_CHECKED : MF_UNCHECKED;
        HMENU hmenu = GetHMenuOf(m_parentMenu);

        if ( GetKind() == wxITEM_RADIO )
        {
            // it doesn't make sense to uncheck a radio item -- what would this
            // do?
            if ( !check )
                return;

            // get the index of this item in the menu
            const wxMenuItemList& items = m_parentMenu->GetMenuItems();
            int pos = items.IndexOf(this);
            wxCHECK_RET( pos != wxNOT_FOUND,
                         wxT("menuitem not found in the menu items list?") );

            // get the radio group range
            int start,
                end;

            if ( m_isRadioGroupStart )
            {
                // we already have all information we need
                start = pos;
                end = m_radioGroup.end;
            }
            else // next radio group item
            {
                // get the radio group end from the start item
                start = m_radioGroup.start;
                end = items.Item(start)->GetData()->m_radioGroup.end;
            }

#ifdef __WIN32__
            // calling CheckMenuRadioItem() with such parameters hangs my system
            // (NT4 SP6) and I suspect this could happen to the others as well,
            // so don't do it!
            wxCHECK_RET( start != -1 && end != -1,
                         wxT("invalid ::CheckMenuRadioItem() parameter(s)") );

            if ( !::CheckMenuRadioItem(hmenu,
                                       start,   // the first radio group item
                                       end,     // the last one
                                       pos,     // the one to check
                                       MF_BYPOSITION) )
            {
                wxLogLastError(wxT("CheckMenuRadioItem"));
            }
#endif // __WIN32__

            // also uncheck all the other items in this radio group
            wxMenuItemList::compatibility_iterator node = items.Item(start);
            for ( int n = start; n <= end && node; n++ )
            {
                if ( n != pos )
                {
                    node->GetData()->m_isChecked = false;
                }

                node = node->GetNext();
            }
        }
        else // check item
        {
            if ( ::CheckMenuItem(hmenu,
                                 GetMSWId(),
                                 MF_BYCOMMAND | flags) == (DWORD)-1 )
            {
                wxFAIL_MSG(wxT("CheckMenuItem() failed, item not in the menu?"));
            }
        }
    }

    wxMenuItemBase::Check(check);
}

void wxMenuItem::SetItemLabel(const wxString& txt)
{
    wxString text = txt;

    // don't do anything if label didn't change
    if ( m_text == txt )
        return;

    // wxMenuItemBase will do stock ID checks
    wxMenuItemBase::SetItemLabel(text);

    // the item can be not attached to any menu yet and SetItemLabel() is still
    // valid to call in this case and should do nothing else
    if ( !m_parentMenu )
        return;

#if wxUSE_ACCEL
    m_parentMenu->UpdateAccel(this);
#endif // wxUSE_ACCEL

    const UINT id = GetMSWId();
    HMENU hMenu = GetHMenuOf(m_parentMenu);
    if ( !hMenu || ::GetMenuState(hMenu, id, MF_BYCOMMAND) == (UINT)-1 )
        return;

#if wxUSE_OWNER_DRAWN
    if ( IsOwnerDrawn() )
    {
        // we don't need to do anything for owner drawn items, they will redraw
        // themselves using the new text the next time they're displayed
        return;
    }
#endif // owner drawn

    // update the text of the native menu item
    WinStruct<MENUITEMINFO> info;

    // surprisingly, calling SetMenuItemInfo() with just MIIM_STRING doesn't
    // work as it resets the menu bitmap, so we need to first get the old item
    // state and then modify it
    const bool isLaterThanWin95 = wxGetWinVersion() > wxWinVersion_95;
    info.fMask = MIIM_STATE |
                 MIIM_ID |
                 MIIM_SUBMENU |
                 MIIM_CHECKMARKS |
                 MIIM_DATA;
    if ( isLaterThanWin95 )
        info.fMask |= MIIM_BITMAP | MIIM_FTYPE;
    else
        info.fMask |= MIIM_TYPE;
    if ( !::GetMenuItemInfo(hMenu, id, FALSE, &info) )
    {
        wxLogLastError(wxT("GetMenuItemInfo"));
        return;
    }

    if ( isLaterThanWin95 )
        info.fMask |= MIIM_STRING;
    //else: MIIM_TYPE already specified
    info.dwTypeData = (LPTSTR)m_text.wx_str();
    info.cch = m_text.length();
    if ( !::SetMenuItemInfo(hMenu, id, FALSE, &info) )
    {
        wxLogLastError(wxT("SetMenuItemInfo"));
    }
}

#if wxUSE_OWNER_DRAWN

wxString wxMenuItem::GetName() const
{
    return GetItemLabelText();
}

bool wxMenuItem::OnMeasureItem(size_t *width, size_t *height)
{
    if ( IsOwnerDrawn() )
    {

        wxString str = GetName();

        // if we have a valid accel string, then pad out
        // the menu string so that the menu and accel string are not
        // placed on top of each other.
        wxString accel = GetItemLabel().AfterFirst(wxT('\t'));
        if ( !accel.empty() )
        {
            str.Pad(str.length()%8);
            str += accel;
        }

        wxMemoryDC dc;
        wxFont font;
        GetFontToUse(font);
        dc.SetFont(font);

        wxCoord w, h;
        dc.GetTextExtent(str, &w, &h);
        *width = w;
        *height = h;
    }
    else // don't draw the text, just the bitmap (if any)
    {
        *width = 0;
        *height = 0;
    }

    // increase size to accommodate bigger bitmaps if necessary
    if (m_bmpChecked.Ok())
    {
        // Is BMP height larger than text height?
        size_t adjustedHeight = m_bmpChecked.GetHeight();
        if ( *height < adjustedHeight )
            *height = adjustedHeight;

        const int widthBmp = m_bmpChecked.GetWidth();
        if ( IsOwnerDrawn() )
        {
            // widen the margin to fit the bitmap if necessary
            if ( GetMarginWidth() < widthBmp )
                SetMarginWidth(widthBmp);

        }
        else // we must allocate enough space for the bitmap
        {
            *width += widthBmp;
        }
    }

    // add a 4-pixel separator, otherwise menus look cluttered
    *width += 4;

    // notice that this adjustment must be done after (possibly) changing the
    // margin width above
    if ( IsOwnerDrawn() )
    {
        // add space at the end of the menu for the submenu expansion arrow
        // this will also allow offsetting the accel string from the right edge
        *width += GetMarginWidth() + 16;
    }

    // make sure that this item is at least as tall as the system menu height
    if ( *height < ms_systemMenuHeight )
      *height = ms_systemMenuHeight;

    return true;
}

bool wxMenuItem::OnDrawItem(wxDC& dc, const wxRect& rc,
                            wxODAction WXUNUSED(act), wxODStatus stat)
{

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
    wxColour colText1, colBack1;
    GetColourToUse(stat, colText1, colBack1);

    DWORD colText = wxColourToPalRGB(colText1);
    DWORD colBack = wxColourToPalRGB(colBack1);

    if ( IsOwnerDrawn() )
    {
        // don't draw an edge around the bitmap, if background is white ...
        DWORD menu_bg_color = GetSysColor(COLOR_MENU);
        if ( GetRValue( menu_bg_color ) >= 0xf0 &&
             GetGValue( menu_bg_color ) >= 0xf0 &&
             GetBValue( menu_bg_color ) >= 0xf0 )
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
    COLORREF colOldText = ::SetTextColor(hdc, colText);
    COLORREF colOldBack = ::SetBkColor(hdc, colBack);

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
        int prevMode = SetBkMode(hdc, TRANSPARENT);
        AutoHBRUSH hbr(colBack);
        SelectInHDC selBrush(hdc, hbr);

        RECT rectFill;
        wxCopyRectToRECT(rc, rectFill);

        if ( (stat & wxODSelected) && m_bmpChecked.Ok() && draw_bitmap_edge )
        {
            // only draw the highlight under the text, not under
            // the bitmap or checkmark
            rectFill.left = xText;
        }

        ::FillRect(hdc, &rectFill, hbr);

        // use default font if no font set
        wxFont font;
        GetFontToUse(font);
        SelectInHDC selFont(hdc, GetHfontOf(font));

        // item text name with menemonic
        wxString text = GetItemLabel().BeforeFirst('\t');

        xText += 3; // separate text from the highlight rectangle

        SIZE textRect;
        ::GetTextExtentPoint32(hdc, text.c_str(), text.length(), &textRect);

        int flags = DST_PREFIXTEXT;
        if ( (stat & wxODDisabled) && !(stat & wxODSelected) )
            flags |= DSS_DISABLED;

        if ( (stat & wxODHidePrefix) && !ms_alwaysShowCues )
            flags |= DSS_HIDEPREFIX;

        int x = xText;
        int y = rc.y + (rc.GetHeight() - textRect.cy) / 2;
        int cx = rc.GetWidth() - GetMarginWidth();
        int cy = textRect.cy;

        ::DrawState(hdc, NULL, NULL, (LPARAM)text.wx_str(),
                    text.length(), x, y, cx, cy, flags);

        // ::SetTextAlign(hdc, TA_RIGHT) doesn't work with DSS_DISABLED or DSS_MONO
        // as the last parameter in DrawState() (at least with Windows98). So we have
        // to take care of right alignment ourselves.
        wxString accel = GetItemLabel().AfterFirst(wxT('\t'));
        if ( !accel.empty() )
        {
            SIZE accelRect;
            ::GetTextExtentPoint32(hdc, accel.c_str(), accel.length(), &accelRect);

            int flags = DST_TEXT;
            if ( (stat & wxODDisabled) && !(stat & wxODSelected) )
                flags |= DSS_DISABLED;

            // right align accel string with right edge of menu
            // (offset by the margin width)

            int x = rc.GetWidth() - 16 - accelRect.cx;
            int y = rc.y + (rc.GetHeight() - accelRect.cy) / 2;
            ::DrawState(hdc, NULL, NULL, (LPARAM)accel.wx_str(),
                        accel.length(), x, y, 0, 0, flags);
        }

        ::SetBkMode(hdc, prevMode);
    }


    // draw the bitmap
    // ---------------
    if ( IsCheckable() && !m_bmpChecked.Ok() )
    {
        if ( stat & wxODChecked )
        {
            // what goes on: DrawFrameControl creates a b/w mask,
            // then we copy it to screen to have right colors

            // first create a monochrome bitmap in a memory DC
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hbmpCheck = CreateBitmap(margin, rc.GetHeight(), 1, 1, 0);
            SelectObject(hdcMem, hbmpCheck);

            // then draw a check mark into it
            RECT rect = { 0, 0, margin, rc.GetHeight() };
            if ( rc.GetHeight() > 0 )
            {
                ::DrawFrameControl(hdcMem, &rect, DFC_MENU, DFCS_MENUCHECK);
            }

            // finally copy it to screen DC and clean up
            BitBlt(hdc, rc.x, rc.y, margin, rc.GetHeight(), hdcMem, 0, 0, SRCCOPY);

            DeleteDC(hdcMem);
            DeleteObject(hbmpCheck);
        }
    }
    else
    {
        wxBitmap bmp;

        if ( stat & wxODDisabled )
        {
            bmp = GetDisabledBitmap();
        }

        if ( !bmp.Ok() )
        {
            // for not checkable bitmaps we should always use unchecked one
            // because their checked bitmap is not set
            bmp = GetBitmap(!IsCheckable() || (stat & wxODChecked));

#if wxUSE_IMAGE
            if ( bmp.Ok() && stat & wxODDisabled )
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

            int heightDiff = rc.GetHeight() - nBmpHeight;
            dc.Blit(rc.x + (margin - nBmpWidth) / 2,
                    rc.y + heightDiff / 2,
                    nBmpWidth, nBmpHeight,
                    &dcMem, 0, 0, wxCOPY, true /* use mask */);

            if ( ( stat & wxODSelected ) && !( stat & wxODDisabled ) && draw_bitmap_edge )
            {
                RECT rectBmp = { rc.GetLeft(), rc.GetTop(),
                                 rc.GetLeft() + margin,
                                 rc.GetTop() + rc.GetHeight() };
                SetBkColor(hdc, colBack);

                DrawEdge(hdc, &rectBmp, BDR_RAISEDINNER, BF_RECT);
            }
        }
    }

    ::SetTextColor(hdc, colOldText);
    ::SetBkColor(hdc, colOldBack);

    return true;

}

void wxMenuItem::GetFontToUse(wxFont& font) const
{
    font = GetFont();
    if ( !font.IsOk() )
        font = ms_systemMenuFont;
}

#endif // wxUSE_OWNER_DRAWN

// ----------------------------------------------------------------------------
// wxMenuItemBase
// ----------------------------------------------------------------------------

wxMenuItem *wxMenuItemBase::New(wxMenu *parentMenu,
                                int id,
                                const wxString& name,
                                const wxString& help,
                                wxItemKind kind,
                                wxMenu *subMenu)
{
    return new wxMenuItem(parentMenu, id, name, help, kind, subMenu);
}

#endif // wxUSE_MENUS
