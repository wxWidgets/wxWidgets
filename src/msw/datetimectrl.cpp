///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/datetimectrl.cpp
// Purpose:     Implementation of wxDateTimePickerCtrl for MSW.
// Author:      Vadim Zeitlin
// Created:     2011-09-22 (extracted from src/msw/datectrl.cpp)
// Copyright:   (c) 2005-2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/datetimectrl.h"
#include "wx/uilocale.h"

#ifdef wxNEEDS_DATETIMEPICKCTRL

#ifndef WX_PRECOMP
    #include "wx/msw/wrapwin.h"
    #include "wx/msw/wrapcctl.h" // include <commctrl.h> "properly"
    #include "wx/msw/private.h"
    #include "wx/dcclient.h"
#endif // WX_PRECOMP

#include "wx/msw/private/datecontrols.h"
#include "wx/msw/private/uilocale.h"
#include "wx/msw/private/darkmode.h"
#include "wx/msw/private/custompaint.h"
#include "wx/msw/uxtheme.h"
// apparently some versions of mingw define these macros erroneously
#ifndef DateTime_GetSystemtime
    #define DateTime_GetSystemtime DateTime_GetSystemTime
#endif

#ifndef DateTime_SetSystemtime
    #define DateTime_SetSystemtime DateTime_SetSystemTime
#endif

#ifndef DTM_GETIDEALSIZE
    #define DTM_GETIDEALSIZE 0x100f
#endif
namespace
{
// Pre-Win11 22H2 fallback: let stock control paint in light theme into a
// buffered DIB and remap pixels to dark palette. "DTP" prefix used for helpers.
// Gray levels and thresholds for DTPRemapPixels() and hot-state tuning.
    namespace DTPDarkGray
    {
        // Inverts and maps stock luma: white face -> kWhiteMapsTo, black text/arrows -> kBlackMapsTo.
        constexpr int kWhiteMapsTo = 40;
        constexpr int kBlackMapsTo = 235;

        // Detect hot/pressed blue states via pixel chroma (no mouse tracking needed).
        constexpr int kHotChromaFull = 24;      // Chroma >= value means fully "hot"
        constexpr int kHotBorderChroma = 128;   // >= value means saturated border; 1 to value is pale fill

        // Blend hot pixels towards white by lift/255 after remapping.
        constexpr int kHotFillLift = 50;
        constexpr int kHotBorderLift = 0;
    }

    constexpr UINT_PTR kDTPChildSubclassId = 0x77780020ul;
    constexpr DWORD_PTR kDTPMonthCal = 1;
    void DTPRemapMonthCalPixels(HWND hwnd, HPAINTBUFFER hbp, int w, int h);
    void DTPRemapPixels(HPAINTBUFFER hbp, int w, int h);
    // Native dark theme for child controls supported since Windows 11 22H2.
    bool DTPHasNativeDarkChildTheme()
    {
        static const bool s_hasNativeTheme = wxCheckOsVersion(10, 0, 22621);
        return s_hasNativeTheme;
    }

    inline BYTE DTPLuma(const RGBQUAD& p)
    {
        return static_cast<BYTE>((p.rgbRed * 299 + p.rgbGreen * 587 + p.rgbBlue * 114) / 1000);
    }

    // max(R,G,B) - min(R,G,B): 0 for gray, high for colored hot-state pixels.
    inline int DTPChroma(const RGBQUAD& p)
    {
        return wxMax(p.rgbRed, wxMax(p.rgbGreen, p.rgbBlue)) -
            wxMin(p.rgbRed, wxMin(p.rgbGreen, p.rgbBlue));
    }

    inline BYTE DTPBlendToWhite(BYTE gray, int amount)
    {
        return static_cast<BYTE>(gray + (255 - gray) * amount / 255);
    }

    void DTPRemapPixels(HPAINTBUFFER hbp, int w, int h)
    {
        using namespace DTPDarkGray;

        RGBQUAD* pPx = nullptr;
        int rw = 0; // Row width in pixels, not bytes.
        if (FAILED(::GetBufferedPaintBits(hbp, &pPx, &rw)) || !pPx)
            return;

        for (int y = 0; y < h; ++y)
        {
            RGBQUAD* row = pPx + static_cast<ptrdiff_t>(y) * rw;
            for (int x = 0; x < w; ++x)
            {
                RGBQUAD& p = row[x];
                const int chroma = DTPChroma(p); // Must calculate before overwrite

                const int inv = 255 - DTPLuma(p);
                BYTE gray = static_cast<BYTE>(
                    kWhiteMapsTo + inv * (kBlackMapsTo - kWhiteMapsTo) / 255
                    );

                if (chroma > 0)
                {
                    const int weight = wxMin(chroma * 255 / kHotChromaFull, 255);
                    const int lift = chroma >= kHotBorderChroma ? kHotBorderLift : kHotFillLift;
                    gray = DTPBlendToWhite(gray, lift * weight / 255);
                }

                p.rgbRed = p.rgbGreen = p.rgbBlue = gray;
                p.rgbReserved = 255;
            }
        }
    }

    void DTPPaintChild(HWND hwnd, bool isMonthCal)
    {
        PAINTSTRUCT ps;
        HDC hdc = ::BeginPaint(hwnd, &ps);

        const RECT rc = wxGetClientRect(hwnd);
        HDC hdcBuf = 0;
        HPAINTBUFFER hbp = ::BeginBufferedPaint(hdc, &rc, BPBF_TOPDOWNDIB, nullptr, &hdcBuf);
        if (hbp)
        {
            ::FillRect(hdcBuf, &rc, ::GetSysColorBrush(COLOR_BTNFACE));
            ::DefSubclassProc(hwnd, WM_PRINTCLIENT, reinterpret_cast<WPARAM>(hdcBuf), PRF_CLIENT | PRF_ERASEBKGND);

            const int w = rc.right - rc.left;
            const int h = rc.bottom - rc.top;

            if (isMonthCal)
                DTPRemapMonthCalPixels(hwnd, hbp, w, h);
            else
                DTPRemapPixels(hbp, w, h);

            ::EndBufferedPaint(hbp, TRUE);
        }
        else // Fallback to unfiltered stock look
        {
            ::DefSubclassProc(hwnd, WM_PRINTCLIENT, reinterpret_cast<WPARAM>(hdc), PRF_CLIENT | PRF_ERASEBKGND);
        }

        ::EndPaint(hwnd, &ps);
    }

    LRESULT CALLBACK DTPChildSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uId, DWORD_PTR dwRef)
    {
        switch (msg)
        {
        case WM_ERASEBKGND:
            if (wxMSWDarkMode::IsActive())
                return TRUE;
            break;

        case WM_PAINT:
            if (wxMSWDarkMode::IsActive())
            {
                DTPPaintChild(hwnd, dwRef == kDTPMonthCal);
                return 0;
            }
            break;

        case WM_NCDESTROY:
            ::RemoveWindowSubclass(hwnd, DTPChildSubclassProc, uId);
            break;
        }

        return ::DefSubclassProc(hwnd, msg, wParam, lParam);
    }

    // Apply dark or light mode to a single child window.
    BOOL CALLBACK DTPEnumChildProc(HWND hwnd, LPARAM WXUNUSED(lParam))
    {
        const wxString className = wxGetWindowClass(hwnd);
        if (!className.IsSameAs(UPDOWN_CLASS, false) && !className.IsSameAs(WC_EDIT, false))
            return TRUE;

        if (!wxMSWDarkMode::IsActive())
        {
            ::RemoveWindowSubclass(hwnd, DTPChildSubclassProc, kDTPChildSubclassId);
            wxMSWDarkMode::SetTheme(hwnd, nullptr, nullptr);
        }
        else if (DTPHasNativeDarkChildTheme())
        {
            wxMSWDarkMode::SetTheme(
                hwnd,
                wxMSWDarkMode::HasDarkTheme() ? L"DarkMode_DarkTheme" : L"DarkMode_Explorer",
                nullptr
            );
        }
        else // Keep stock light theme as painting source
        {
            ::SetWindowSubclass(hwnd, DTPChildSubclassProc, kDTPChildSubclassId, 0);
        }

        ::InvalidateRect(hwnd, nullptr, TRUE);
        return TRUE;
    }
   // WinUI 3 (CalendarView, dark) look via full-RGB pixel remapping (retains accent blue, avoids text fringes).
    namespace DTPWinUI
    {
        constexpr COLORREF kBody = RGB(0x23, 0x23, 0x23);     // Days and header background
        constexpr COLORREF kBand = RGB(0x1f, 0x1f, 0x1f);     // Weekday names strip background
        constexpr COLORREF kText = RGB(0xff, 0xff, 0xff);     // Standard text color
        constexpr COLORREF kTrailing = RGB(0xcd, 0xcd, 0xcd); // Other months' days
        constexpr COLORREF kAccent = RGB(0x40, 0xbd, 0xff);   // Selected day and today ring
        constexpr COLORREF kOnAccent = RGB(0x00, 0x00, 0x00); // Text on accent
    }

    // Light gray levels for DateTime_SetMonthCalColor() to maximize anti-aliasing range.
    namespace DTPMonthCalLight
    {
        constexpr int kMonthBk = 255;   // MCSC_BACKGROUND & MCSC_MONTHBK
        constexpr int kText = 0;      // MCSC_TEXT
        constexpr int kTrailing = 160;// MCSC_TRAILINGTEXT
        constexpr int kTitleBk = 96;  // MCSC_TITLEBK (header, weekdays, selected day)
        constexpr int kTitleText = 0; // MCSC_TITLETEXT
        constexpr int kNavFace = 240; // COLOR_BTNFACE
        constexpr int kNavGlyph = 0;  // COLOR_BTNTEXT
    }

    // Calendar region mapping: stock background/foreground levels to target colors.
    struct DTPRegion
    {
        int stockBg, stockFg;
        COLORREF bg, fg;
        bool ring;       // Convert red "today" ring to custom accent color
        bool dropDarker; // Map pixels darker than text to background
    };

    struct DTPCell
    {
        RECT rc;
        bool selected;
        WORD month;
    };

    inline int DTPRed(COLORREF c) { return c & 0xff; }
    inline int DTPGreen(COLORREF c) { return (c >> 8) & 0xff; }
    inline int DTPBlue(COLORREF c) { return (c >> 16) & 0xff; }

    inline int DTPMix(int a, int b, int t256)
    {
        return (a * (256 - t256) + b * t256) / 256;
    }

    inline int DTPClamp256(int t)
    {
        return wxMax(0, wxMin(256, t));
    }

    bool DTPGetGridRect(HWND hwnd, DWORD part, RECT& rc)
    {
        MCGRIDINFO gi = {};
        gi.cbSize = sizeof(gi);
        gi.dwFlags = MCGIF_RECT;
        gi.dwPart = part;
        gi.iCalendar = 0;

        if (!::SendMessage(hwnd, MCM_GETCALENDARGRIDINFO, 0, reinterpret_cast<LPARAM>(&gi)))
            return false;

        rc = gi.rc;
        return true;
    }

    bool DTPGetCell(HWND hwnd, int row, int col, DTPCell& cell)
    {
        MCGRIDINFO gi = {};
        gi.cbSize = sizeof(gi);
        gi.dwFlags = MCGIF_RECT | MCGIF_DATE;
        gi.dwPart = MCGIP_CALENDARCELL;
        gi.iCalendar = 0;
        gi.iRow = row;
        gi.iCol = col;

        if (!::SendMessage(hwnd, MCM_GETCALENDARGRIDINFO, 0, reinterpret_cast<LPARAM>(&gi)))
            return false;

        cell.rc = gi.rc;
        cell.selected = gi.bSelected != FALSE;
        cell.month = gi.stStart.wMonth;
        return true;
    }

    void DTPSetMonthCalLightColours(HWND hwndDTP)
    {
        using namespace DTPMonthCalLight;
        const auto gray = [](int g) { return RGB(g, g, g); };

        DateTime_SetMonthCalColor(hwndDTP, MCSC_BACKGROUND, gray(kMonthBk));
        DateTime_SetMonthCalColor(hwndDTP, MCSC_MONTHBK, gray(kMonthBk));
        DateTime_SetMonthCalColor(hwndDTP, MCSC_TEXT, gray(kText));
        DateTime_SetMonthCalColor(hwndDTP, MCSC_TRAILINGTEXT, gray(kTrailing));
        DateTime_SetMonthCalColor(hwndDTP, MCSC_TITLEBK, gray(kTitleBk));
        DateTime_SetMonthCalColor(hwndDTP, MCSC_TITLETEXT, gray(kTitleText));
    }

    void DTPMapRect(RGBQUAD* dst, const RGBQUAD* src, int rw, int w, int h, RECT rc, const DTPRegion& r)
    {
        rc.left = wxMax(rc.left, 0);
        rc.top = wxMax(rc.top, 0);
        rc.right = wxMin(rc.right, w);
        rc.bottom = wxMin(rc.bottom, h);

        const int denom = r.stockFg - r.stockBg;
        if (!denom)
            return;

        for (int y = rc.top; y < rc.bottom; ++y)
        {
            const RGBQUAD* in = src + static_cast<ptrdiff_t>(y) * rw;
            RGBQUAD* row = dst + static_cast<ptrdiff_t>(y) * rw;
            for (int x = rc.left; x < rc.right; ++x)
            {
                const RGBQUAD& s = in[x];
                RGBQUAD& p = row[x];

                const int luma = DTPLuma(s);
                int t = DTPClamp256((luma - r.stockBg) * 256 / denom);
                if (r.dropDarker && luma < r.stockFg - 8)
                    t = 0;

                int red = DTPMix(DTPRed(r.bg), DTPRed(r.fg), t);
                int green = DTPMix(DTPGreen(r.bg), DTPGreen(r.fg), t);
                int blue = DTPMix(DTPBlue(r.bg), DTPBlue(r.fg), t);

                if (r.ring)
                {
                    const int redness = s.rgbRed - wxMax(s.rgbGreen, s.rgbBlue);
                    if (redness > 32)
                    {
                        const int wt = DTPClamp256(redness * 256 / 255);
                        red = DTPMix(red, DTPRed(DTPWinUI::kAccent), wt);
                        green = DTPMix(green, DTPGreen(DTPWinUI::kAccent), wt);
                        blue = DTPMix(blue, DTPBlue(DTPWinUI::kAccent), wt);
                    }
                }

                p.rgbRed = static_cast<BYTE>(red);
                p.rgbGreen = static_cast<BYTE>(green);
                p.rgbBlue = static_cast<BYTE>(blue);
                p.rgbReserved = 255;
            }
        }
    }

    void DTPRemapMonthCalPixels(HWND hwnd, HPAINTBUFFER hbp, int w, int h)
    {
        namespace L = DTPMonthCalLight;
        namespace C = DTPWinUI;

        RECT rcHeader, rcPrev, rcNext;
        DTPCell cells[6][7] = {};
        bool ok = DTPGetGridRect(hwnd, MCGIP_CALENDARHEADER, rcHeader) &&
            DTPGetGridRect(hwnd, MCGIP_PREV, rcPrev) &&
            DTPGetGridRect(hwnd, MCGIP_NEXT, rcNext);
        for (int row = 0; ok && row < 6; ++row)
        {
            for (int col = 0; ok && col < 7; ++col)
                ok = DTPGetCell(hwnd, row, col, cells[row][col]);
        }

        if (!ok)
        {
            DTPRemapPixels(hbp, w, h);
            return;
        }

        RGBQUAD* pPx = nullptr;
        int rw = 0;
        if (FAILED(::GetBufferedPaintBits(hbp, &pPx, &rw)) || !pPx)
            return;

        const std::vector<RGBQUAD> src(pPx, pPx + static_cast<size_t>(rw) * h);
        const RGBQUAD* const pSrc = src.data();
        const WORD month = cells[2][3].month;

        const RECT rcAll = { 0, 0, w, h };
        const DTPRegion body = { 255, L::kText, C::kBody, C::kText, false, false };
        DTPMapRect(pPx, pSrc, rw, w, h, rcAll, body);

        const DTPRegion header = { L::kTitleBk, L::kTitleText, C::kBody, C::kText, false, false };
        DTPMapRect(pPx, pSrc, rw, w, h, rcHeader, header);

        const DTPRegion flat = { 255, 0, C::kBody, C::kBody, false, false };
        const DTPRegion glyph = { L::kNavFace, L::kNavGlyph, C::kBody, C::kText, false, false };
        for (const RECT* rc : { &rcPrev, &rcNext })
        {
            DTPMapRect(pPx, pSrc, rw, w, h, *rc, flat);
            RECT rcGlyph = *rc;
            ::InflateRect(&rcGlyph, -3, -3);
            DTPMapRect(pPx, pSrc, rw, w, h, rcGlyph, glyph);
        }

        const RECT rcNames = { rcHeader.left, rcHeader.bottom, rcHeader.right, cells[0][0].rc.top };
        const DTPRegion names = { L::kMonthBk, L::kTitleBk, C::kBand, C::kText, false, true };
        DTPMapRect(pPx, pSrc, rw, w, h, rcNames, names);

        const DTPRegion selected = { L::kTitleBk, L::kTitleText, C::kAccent, C::kOnAccent, false, false };
        const DTPRegion normal = { L::kMonthBk, L::kText, C::kBody, C::kText, true, false };
        const DTPRegion trailing = { L::kMonthBk, L::kTrailing, C::kBody, C::kTrailing, true, false };

        bool isRTL = ::GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_LAYOUTRTL;
        for (int row = 0; row < 6; ++row)
        {
            for (int col = 0; col < 7; ++col)
            {
                const DTPCell& cell = cells[row][col];
                const RECT rc = isRTL
                    ? RECT{ w - cell.rc.right, cell.rc.top, w - cell.rc.left, cell.rc.bottom }
                : cell.rc;

                DTPMapRect(pPx, pSrc, rw, w, h, rc,
                    cell.selected ? selected :
                    cell.month != month ? trailing : normal);
            }
        }
    }
} // namespace

// ============================================================================
// wxDateTimePickerCtrl implementation
// ============================================================================

bool
wxDateTimePickerCtrl::MSWCreateDateTimePicker(wxWindow *parent,
                                              wxWindowID id,
                                              const wxDateTime& dt,
                                              const wxPoint& pos,
                                              const wxSize& size,
                                              long style,
                                              const wxValidator& validator,
                                              const wxString& name)
{
    if ( !wxMSWDateControls::CheckInitialization() )
        return false;

    // initialize the base class
    if ( !CreateControl(parent, id, pos, size, style, validator, name) )
        return false;

    // create the native control
    if ( !MSWCreateControl(DATETIMEPICK_CLASS, wxString(), pos, size) )
        return false;

    if ( dt.IsValid() || MSWAllowsNone() )
        SetValue(dt);
    else
        SetValue(wxDateTime::Now());

    return true;
}

#if wxUSE_INTL

void wxDateTimePickerCtrl::MSWSetTimeFormat(wxLocaleInfo index)
{
    const wxString format = wxGetMSWDateTimeFormat(index);
    if ( !format.empty() )
    {
        DateTime_SetFormat(GetHwnd(),
                           static_cast<const wchar_t*>(format.t_str()));
    }
}

#endif // wxUSE_INTL

void wxDateTimePickerCtrl::SetValue(const wxDateTime& dt)
{
    wxCHECK_RET( dt.IsValid() || MSWAllowsNone(),
                    wxT("this control requires a valid date") );

    SYSTEMTIME st;
    if ( dt.IsValid() )
        dt.GetAsMSWSysTime(&st);

    if ( !DateTime_SetSystemtime(GetHwnd(),
                                 dt.IsValid() ? GDT_VALID : GDT_NONE,
                                 &st) )
    {
        // The only expected failure is when the date is out of range but we
        // already checked for this above.
        wxFAIL_MSG( wxT("Setting the calendar date unexpectedly failed.") );

        // In any case, skip updating m_date below.
        return;
    }

    MSWUpdateFormatIfNeeded(dt.IsValid());

    m_date = dt;
}

wxDateTime wxDateTimePickerCtrl::GetValue() const
{
    return m_date;
}

void wxDateTimePickerCtrl::MSWUpdateFormatIfNeeded(bool valid)
{
    if ( MSWAllowsNone() && !m_nullText.empty() && valid != m_date.IsValid() )
        MSWUpdateFormat(valid);
}

void wxDateTimePickerCtrl::MSWUpdateFormat(bool valid)
{
    // We just use nullptr to reset to the default format when the date is valid,
    // as the control seems to remember whichever format was used when it was
    // created, i.e. this works both with and without wxDP_SHOWCENTURY.

    // Use a temporary variable to ensure that the code compiles in
    // wxUSE_UNICODE_UTF8 case, where t_str() doesn't return a pointer.
    const TCHAR* format;
    if ( valid )
        format = nullptr;
    else
        format = m_nullText.t_str();

    DateTime_SetFormat(GetHwnd(), format);
}

void wxDateTimePickerCtrl::SetNullText(const wxString& text)
{
    m_nullText = text;
    if ( m_nullText.empty() )
    {
        // Using empty format doesn't work with the native control, it just
        // uses the default short date format in this case, so set the format
        // to the single space which is more or less guaranteed to work as it's
        // the semi-official way to clear the control contents when it doesn't
        // have any valid value, according to Microsoft's old KB document
        // Q238077, which can still be found online by searching for its
        // number, even if it's not available on Microsoft web site any more.
        //
        // Coincidentally, it's also convenient for us, as we can just check if
        // null text is empty to see if we need to use it elsewhere in the code.
        m_nullText = wxS(" ");
    }
    else
    {
        // We need to quote the text, as otherwise format specifiers (e.g.
        // "d", "m" etc) would be interpreted specially by the control. To make
        // things simple, we just quote it entirely and always.
        m_nullText.Replace("'", "''");
        m_nullText.insert(0, "'");
        m_nullText.append("'");
    }

    // Apply it immediately if we don't have any value right now.
    if ( !m_date.IsValid() )
        MSWUpdateFormat(false);
}

wxSize wxDateTimePickerCtrl::DoGetBestSize() const
{
    wxSize size;

    // Use DTM_GETIDEALSIZE to ask the control itself to compute its ideal
    // size, but we can't use it with DTS_SHOWNONE because handling of
    // this flag is completely broken (up to at least Window 10 20H2): it's
    // not just ignored, but we get completely wrong results when this flag
    // is on, e.g. the returned width is less than the width without it or
    // much greater than the real value after a DPI change (and growing
    // with every new change, even when repeatedly switching between the
    // same DPI values, e.g. dragging a window between 2 monitors with
    // different scaling). Moreover, note that even without DTS_SHOWNONE,
    // DTM_GETIDEALSIZE still returns wrong results for the height after a
    // DPI change, so we never use the vertical component of the value
    // returned by it.
    //
    // Unfortunately, resetting this style doesn't work either, so we have
    // to create a whole new window just for this, which is pretty wasteful
    // but seems unavoidable.
    HWND hwnd;
    if ( MSWAllowsNone() )
    {
        hwnd = ::CreateWindow
                 (
                    DATETIMEPICK_CLASS,
                    wxT(""),
                    ::GetWindowLong(GetHwnd(), GWL_STYLE) & ~DTS_SHOWNONE,
                    0, 0, 1, 1,
                    GetHwndOf(m_parent),
                    0,
                    wxGetInstance(),
                    nullptr
                 );
        wxCHECK_MSG( hwnd, wxSize(),
                     wxS("SysDateTimePick32 creation unexpected failed") );

        wxSetWindowFont(hwnd, GetFont());
    }
    else
    {
        hwnd = GetHwnd();
    }

    // Also work around https://bugs.winehq.org/show_bug.cgi?id=44680 by
    // checking for the return value: even if all "real" MSW systems do support
    // this message, Wine does not, even when it's configured to return Vista
    // or later version to the application, and returns FALSE for it.
    SIZE idealSize = { 0, 0 };
    if ( ::SendMessage(hwnd, DTM_GETIDEALSIZE, 0, (LPARAM)&idealSize) )
    {
        size.x = idealSize.cx;
        size.y = GetCharHeight();
    }

    if ( hwnd != GetHwnd() )
    {
        ::DestroyWindow(hwnd);
    }

    if ( !size.x ) // Compute the size ourselves.
    {
        // Use the same native format as the underlying native control.
#if wxUSE_INTL
        wxString s = wxDateTime::Now().Format(wxUILocale::GetCurrent().GetInfo(MSWGetFormat()));
#else // !wxUSE_INTL
        wxString s("XXX-YYY-ZZZZ");
#endif // wxUSE_INTL/!wxUSE_INTL

        // the best size for the control is bigger than just the string
        // representation of the current value because the control must accommodate
        // any date and while the widths of all digits are usually about the same,
        // the width of the month string varies a lot, so try to account for it
        s += wxS("W");

        size = GetTextExtent(s);

        // Account for the drop-down arrow or spin arrows.
        size.x += wxSystemSettings::GetMetric(wxSYS_HSCROLL_ARROW_X, m_parent);
    }

    // Account for the checkbox.
    if ( MSWAllowsNone() )
    {
        // The extra 8px here was determined heuristically as the value which
        // results in the same layout with and without DTS_SHOWNONE under
        // Windows 7 and Windows 10 with 100%, 150% and 200% scaling.
        size.x += wxGetSystemMetrics(SM_CXMENUCHECK, m_parent) + 8;
    }

    int scrollY = wxSystemSettings::GetMetric(wxSYS_HSCROLL_ARROW_Y, m_parent);
    size.y = wxMax(size.y, scrollY);

    // In any case, adjust the height to be the same as for the text controls.
    size.y = EDIT_HEIGHT_FROM_CHAR_HEIGHT(size.y);

    return size;
}

WXLRESULT
wxDateTimePickerCtrl::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    if (nMsg == WM_PAINT && wxMSWDarkMode::IsActive())
    {
        LONG_PTR style = ::GetWindowLong(GetHwnd(), GWL_STYLE);
        // Skip in the up-down (spinner) variant – no dropdown button there.
        if ((!(style & DTS_UPDOWN)) || (style & DTS_SHOWNONE))
        {
            wxMSWImpl::CustomPaint
            (
                GetHwnd(),
                [this](HWND hwnd, WPARAM wParamPaint)
                {
                    m_oldWndProc(hwnd, WM_PAINT, wParamPaint, 0);
                },
                [this](const wxBitmap& bmpOrig)
                {
                    wxBitmap bmp(bmpOrig);
                    wxMemoryDC dc(bmp);
                    dc.SetLayoutDirection(this->GetLayoutDirection());
                    HDC hdc = GetHdcOf(dc);
                    // 1. Ask the control where its elements are.
                    DATETIMEPICKERINFO dtpi = {};
                    dtpi.cbSize = sizeof(DATETIMEPICKERINFO);
                    ::SendMessage(GetHwnd(),
                        DTM_GETDATETIMEPICKERINFO,
                        0,
                        reinterpret_cast<LPARAM>(&dtpi));

                    RECT rcButton = dtpi.rcButton;
                    RECT rcCheck = dtpi.rcCheck;

                    auto theme = wxUxThemeHandle::NewAtDPI(0, L"DATEPICKER", GetDPI().y);
                    auto chTheme = wxUxThemeHandle::NewAtStdDPI(0, L"Button", L"DarkMode_Explorer::Button");
                    // --- DRAW DROPDOWN CALENDAR BUTTON ---
                    if (rcButton.right > rcButton.left && rcButton.bottom > rcButton.top)
                    {
                        int state = DPSCBR_NORMAL;

                        POINT ptCursor;
                        GetCursorPos(&ptCursor);
                        // 2. Convert screen coordinates to coordinates relative to this control's window
                        MapWindowPoints(HWND_DESKTOP, GetHwnd(), (LPPOINT)&ptCursor, 1);

                        if (PtInRect(&rcButton, ptCursor))
                        {
                            int alpha = dtpi.stateButton & STATE_SYSTEM_PRESSED ? 116 : 126;
                            dc.SetBrush(wxBrush(wxColour(GetPixel(hdc, rcButton.left, rcButton.top)).ChangeLightness(alpha)));
                            dc.SetPen(wxNullPen);
                            dc.DrawRectangle(rcButton.left, rcButton.top, rcButton.right - rcButton.left, rcButton.bottom - rcButton.top);
                        }

                        theme.DrawBackground(hdc, rcButton, (int)DP_SHOWCALENDARBUTTONRIGHT, state);

                        // Draw a hairline separator on the left edge so the
                        // button visually separates from the date field.
                        dc.SetPen(wxPen(wxColour(80, 80, 80)));
                        dc.DrawLine(rcButton.left, rcButton.top + 1,
                            rcButton.left, rcButton.bottom - 1);
                    }

                    // --- DRAW CHECKBOX ---
                    if (rcCheck.right > rcCheck.left && rcCheck.bottom > rcCheck.top)
                    {
                        const int edge = wxSystemSettings::GetMetric(wxSYS_EDGE_X);
                        rcCheck.left += edge;
                        rcCheck.right -= edge;
                        rcCheck.top += edge;
                        rcCheck.bottom -= edge;
                        SYSTEMTIME st;
                        LRESULT result = SendMessage(GetHwnd(), DTM_GETSYSTEMTIME, 0, (LPARAM)&st);

                        bool isChecked = (result == GDT_VALID);
                        // Determine the theme part state index (BP_CHECKBOX)
                        int chState = isChecked ? CBS_CHECKEDNORMAL : CBS_UNCHECKEDNORMAL;

                        // Draw the custom theme background over the double-buffered DC
                        const int BP_CHECKBOX = 3;
                        chTheme.DrawBackground(hdc, rcCheck, BP_CHECKBOX, chState);
                    }

                    return bmp;
                }
            );

            return 0;
        }
    }
    return wxDateTimePickerCtrlBase::MSWWindowProc(nMsg, wParam, lParam);
}

void wxDateTimePickerCtrl::MSWSetDarkOrLightMode()
{
    wxDateTimePickerCtrlBase::MSWSetDarkOrLightMode();

    if (wxMSWDarkMode::IsActive())
    {
        wxMSWDarkMode::AllowForWindow(GetHwnd(), nullptr,
            L"DarkMode::FileExplorerBannerContainer");
    }
    else
    {
        wxMSWDarkMode::AllowForWindow(GetHwnd());
    }

    ::EnumChildWindows(GetHwnd(), DTPEnumChildProc, 0);
}

bool
wxDateTimePickerCtrl::MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result)
{
    NMHDR* hdr = (NMHDR *)lParam;
    switch ( hdr->code )
    {
        case DTN_DROPDOWN:
        {
            HWND hwndMonthCal = (HWND)SendMessage(hdr->hwndFrom, DTM_GETMONTHCAL, 0, 0);
            if (hwndMonthCal)
            {
               // Disable native uxtheme to prevent animation glitches during header clicks in dark mode custom painting.
               const wchar_t* themeName = wxMSWDarkMode::IsActive() ? L"" : nullptr;
               wxMSWDarkMode::SetTheme(hwndMonthCal, themeName, themeName);
               DTPSetMonthCalLightColours(hdr->hwndFrom);
               ::SetWindowSubclass(hwndMonthCal, DTPChildSubclassProc, kDTPChildSubclassId, kDTPMonthCal);
               RECT rcIdeal = {};
               ::SendMessage(hwndMonthCal, MCM_GETMINREQRECT, 0, (LPARAM)&rcIdeal);
               // Expand width if the "Today" text string is wider than the grid.
               DWORD dwMaxTodayWidth = (DWORD)::SendMessage(hwndMonthCal, MCM_GETMAXTODAYWIDTH, 0, 0);
               if (dwMaxTodayWidth > (DWORD)rcIdeal.right)
               {
                   rcIdeal.right = dwMaxTodayWidth;
               }
                // Add slight padding around the calendar rectangle.
                ::InflateRect(&rcIdeal, 3, 3);
                 // Retrieve the host popup window, adjust for borders, and apply Win11 rounded corners.
                HWND hwndPopup = ::GetParent(hwndMonthCal);
                if (hwndPopup)
                {
                    WINDOWINFO wi = {};
                    wi.cbSize = sizeof(WINDOWINFO);
                    ::GetWindowInfo(hwndPopup, &wi);
                    ::AdjustWindowRectEx(&rcIdeal, wi.dwStyle, FALSE, wi.dwExStyle);

                    DWORD borderColor = wxMSWDarkMode::IsActive()
                        ? DTPWinUI::kAccent
                        : wxSystemSettings::GetColour(wxSYS_COLOUR_HOTLIGHT).GetPixel();
                    // Enable rounded corners on the popup window to align with modern Windows 11 design language.
                    wxMSWImpl::EnableRoundCorners(hwndPopup, borderColor);
                    ::SetWindowPos(hwndPopup, nullptr, 0, 0,
                        rcIdeal.right - rcIdeal.left,
                        rcIdeal.bottom - rcIdeal.top,
                        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
                }

            }
        }
        break;

         case DTN_DATETIMECHANGE:
            const NMDATETIMECHANGE& dtch = *(NMDATETIMECHANGE*)(hdr);

            // Update the format before showing the new date if necessary.
            MSWUpdateFormatIfNeeded(dtch.dwFlags == GDT_VALID);

            if ( MSWOnDateTimeChange(dtch) )
            {
                *result = 0;
                return true;
            }
            break;
    }

    return wxDateTimePickerCtrlBase::MSWOnNotify(idCtrl, lParam, result);
}

#endif // wxNEEDS_DATETIMEPICKCTRL
