///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/taskdlg.cpp
// Purpose:     Dark mode support for native TaskDialog.
// Author:      Mohmed Abdel-Fattah
// Created:     2026-06-07
// Copyright:   (c) 2026 wxWidgets development team
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

#ifndef wxUSE_DARK_MODE
    // Otherwise enable it by default.
    #define wxUSE_DARK_MODE 1
#endif

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/msw/private/taskdlg.h"

#if wxUSE_DARK_MODE

#include "wx/dynlib.h"
#include "wx/log.h"
#include "wx/module.h"

#include "wx/msw/uxtheme.h"

#include "wx/msw/private/darkmode.h"

#include "wx/msw/private/comptr.h"
#include <uiautomation.h>

// ============================================================================
// TaskDialog dark mode — implementation detail types
// ============================================================================

// Colour palette used for the dark TaskDialog panels.
// Values match wxDarkModeSettings::GetColour() so dialogs blend with the app.
namespace TDDarkCol
{
    static constexpr COLORREF kPrimary = RGB(0x20, 0x20, 0x20);
    static constexpr COLORREF kSecondary = RGB(0x2c, 0x2c, 0x2c);
    static constexpr COLORREF kFootnote = RGB(0x2c, 0x2c, 0x2c);
    static constexpr COLORREF kSeparator = RGB(0x3c, 0x3c, 0x3c);

    static constexpr COLORREF kTextNormal = RGB(0xe0, 0xe0, 0xe0);
    static constexpr COLORREF kTextInstruct = RGB(0x00, 0x99, 0xff);
    static constexpr COLORREF kTextContent = RGB(0xe0, 0xe0, 0xe0);
    static constexpr COLORREF kTextExpando = RGB(0xe0, 0xe0, 0xe0);
    static constexpr COLORREF kTextVerify = RGB(0xe0, 0xe0, 0xe0);
    static constexpr COLORREF kTextFootnote = RGB(0xb0, 0xb0, 0xb0);
    static constexpr COLORREF kTextFtrExp = RGB(0xb0, 0xb0, 0xb0);
    static constexpr COLORREF kTextRadio = RGB(0xe0, 0xe0, 0xe0);
}
namespace
{

// Helper function to get an HWND from IUIAutomationElement.
HWND GetHWNDFromElement(IUIAutomationElement* element)
{
    UIA_HWND hwnd = 0;
    HRESULT hr = element->get_CurrentNativeWindowHandle(&hwnd);
    if ( FAILED(hr) )
    {
        wxLogApiError("get_CurrentNativeWindowHandle", hr);
        return 0;
    }

    return reinterpret_cast<HWND>(hwnd);
}

// Cached bounding rect + metadata for a single TaskDialog UI element.
struct TDLayoutElement
{
    RECT         rect = {};
    std::wstring automationId;
    std::wstring name;        // visible text (used for overdraw)
    LONG         legacyState = 0; // STATE_SYSTEM_* flags
};

// All per-dialog rendering state, stored in a thread_local map.
struct TDPageState
{
    wxUxThemeHandle hTD ; // TaskDialog panel + glyph parts
    wxUxThemeHandle hTDS ; // TaskDialogStyle / text fonts
    wxUxThemeHandle hButton ; // Button (checkbox glyph)
    bool   isDark = false;   // native dark theme available
    bool   themesOk = false;

    AutoHBRUSH brPrimary{TDDarkCol::kPrimary};
    AutoHBRUSH brSecondary{TDDarkCol::kSecondary};
    AutoHBRUSH brFootnote{TDDarkCol::kFootnote};

    std::vector<TDLayoutElement> elements;
    bool elemsOk = false;

    // Mouse interaction (message-driven, no polling)
    bool tracking = false;
    bool pressing = false;
    int  hotIdx = -1;

    // Logical dialog state
    bool isExpanded = false;
    bool isChecked = false;
    bool defExpanded = false;
    bool defChecked = false;

    const TASKDIALOGCONFIG* pCfg = nullptr;

    void CloseThemes()
    {
        themesOk = false;
    }

    using TDStateMap = std::unordered_map<HWND, TDPageState>;

    static TDStateMap& GetAll()
    {
        static TDStateMap s_states;
        return s_states;
    }

    static TDPageState& Get(HWND h)
    {
        return GetAll()[h];
    }

    static void Destroy(HWND h)
    {
        auto& states = GetAll();
        auto const it = states.find(h);
        if ( it != states.end() )
        {
            states.erase(it);
        }
    }
};

// ----------------------------------------------------------------------------
// Module for cleaning up resources used by the code in this file
// ----------------------------------------------------------------------------

// Function to get IUIAutomation instance
class wxTaskDialogDarkModule : public wxModule
{
public:
    virtual bool OnInit() override { return true; }
    virtual void OnExit() override
    {
        ms_uiAutomation.reset();
        TDPageState::GetAll().clear();
    }

    static IUIAutomation* GetUIAutomation()
    {
        if ( !ms_uiAutomation )
        {
            HRESULT hr = CoCreateInstance
                         (
                            CLSID_CUIAutomation,
                            nullptr,
                            CLSCTX_INPROC_SERVER,
                            wxIID_PPV_ARGS(IUIAutomation, &ms_uiAutomation)
                         );
            if ( FAILED(hr) )
            {
                wxLogLastError("CoCreateInstance(IUIAutomation)");
            }
        }

        return ms_uiAutomation.get();
    }

private:
    static wxCOMPtr<IUIAutomation> ms_uiAutomation;

    wxDECLARE_DYNAMIC_CLASS(wxTaskDialogDarkModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxTaskDialogDarkModule, wxModule);

wxCOMPtr<IUIAutomation> wxTaskDialogDarkModule::ms_uiAutomation;


// Subclass IDs
static constexpr UINT_PTR kTDMainSubclassId = 0xDEADBEEFul;
static constexpr UINT_PTR kTDPageSubclassId = 0xBADF00Dul;
static constexpr UINT_PTR kTDCtrlSubclassId = 0xC0FFEE01ul;

static int GetWindowDPI(HWND hwnd)
{
    typedef UINT(WINAPI* GetDpiForWindow_t)(HWND hwnd);
    static GetDpiForWindow_t s_pfnGetDpiForWindow = nullptr;
    static bool s_initDone = false;

    if (!s_initDone)
    {
        wxLoadedDLL dllUser32("user32.dll");
        wxDL_INIT_FUNC(s_pfn, GetDpiForWindow, dllUser32);
        s_initDone = true;
    }

    if (s_pfnGetDpiForWindow)
    {
        const int dpi = static_cast<int>(s_pfnGetDpiForWindow(hwnd));
        return dpi;
    }

    return 0;
}


// ============================================================================
// TaskDialog theme helpers
// ============================================================================

// Cached probe: does the OS have a native dark TaskDialog theme (Win11+)?
static bool TDHasNativeDarkTheme()
{
    static int s_cached = -1;
    if (s_cached == -1)
    {
       wxUxThemeHandle hD (wxUxThemeHandle::NewAtStdDPI( L"DarkMode_DarkTheme::TaskDialog"));
       wxUxThemeHandle hB(wxUxThemeHandle::NewAtStdDPI(L"TaskDialog"));
        s_cached = (hD && hD != hB) ? 1 : 0;
    }
    return s_cached == 1;
}

static void TDRefreshThemes(HWND hwnd, TDPageState& s)
{
    s.CloseThemes();
    s.isDark = TDHasNativeDarkTheme();
    int dpi = GetWindowDPI(hwnd);

    const wchar_t* mainClass = s.isDark
        ? L"DarkMode_Explorer::TaskDialog"
        : L"TaskDialog";
    const wchar_t* btnClass = s.isDark
        ? L"DarkMode_Explorer::Button" : L"Button";
    // For TaskDialog style, likely same as mainClass; could be different.
    const wchar_t* mainClassDark = s.isDark ? L"DarkMode_Explorer::TaskDialog" : nullptr; // or nullptr if not needed

    s.hTD = wxUxThemeHandle::NewAtDPI(hwnd, mainClass, mainClassDark, dpi);
    s.hTDS = wxUxThemeHandle::NewAtDPI(hwnd, mainClass, mainClassDark, dpi);
    s.hButton = wxUxThemeHandle::NewAtDPI(hwnd, btnClass, dpi);

    s.themesOk = true;
}

static COLORREF TDGetTextColour(const TDPageState& s, int uiPart)
{
    if (s.isDark && s.hTDS)
    {
        COLORREF c = TDDarkCol::kTextNormal;
        if (SUCCEEDED(GetThemeColor(s.hTDS, uiPart, 0, TMT_TEXTCOLOR, &c)))
            return c;
    }
    switch (uiPart)
    {
    case TDLG_MAININSTRUCTIONPANE:
        return TDDarkCol::kTextInstruct;
    case TDLG_CONTENTPANE:
        return TDDarkCol::kTextContent;
    case TDLG_EXPANDOTEXT:
        return TDDarkCol::kTextExpando;
    case TDLG_VERIFICATIONTEXT:
        return TDDarkCol::kTextVerify;
    case TDLG_FOOTNOTEPANE:
        return TDDarkCol::kTextFootnote;
    case TDLG_EXPANDEDFOOTERAREA:
        return TDDarkCol::kTextFtrExp;
    case TDLG_RADIOBUTTONPANE:
        return TDDarkCol::kTextRadio;
    default:
        return TDDarkCol::kTextNormal;
    }
}

// ============================================================================
// Icon loading
// ============================================================================

static HICON TDLoadStockIcon(const TASKDIALOGCONFIG* cfg, bool isMain)
{
    if (!cfg)
        return nullptr;

    if (isMain && (cfg->dwFlags & TDF_USE_HICON_MAIN))
        return cfg->hMainIcon;
    if (!isMain && (cfg->dwFlags & TDF_USE_HICON_FOOTER))
        return cfg->hFooterIcon;

    LPCWSTR res = isMain ? cfg->pszMainIcon : cfg->pszFooterIcon;
    if (!res || !IS_INTRESOURCE(res))
        return nullptr;

    auto Stock = [](SHSTOCKICONID id) -> HICON
        {
         SHSTOCKICONINFO sii = { sizeof(sii)
        };
        return SUCCEEDED(SHGetStockIconInfo(id,
            SHGSI_ICON | SHGSI_LARGEICON, &sii))
            ? sii.hIcon : nullptr;
        };
    if (res == TD_WARNING_ICON)
        return Stock(SIID_WARNING);
    if (res == TD_ERROR_ICON)
        return Stock(SIID_ERROR);
    if (res == TD_INFORMATION_ICON)
        return Stock(SIID_INFO);
    if (res == TD_SHIELD_ICON)
        return Stock(SIID_SHIELD);

    return nullptr;
}

// ============================================================================
// UIA layout cache
// ============================================================================

static void TDBuildLayoutCache(HWND hwnd, std::vector<TDLayoutElement>& out)
{
    out.clear();
    IUIAutomation* const pAuto = wxTaskDialogDarkModule::GetUIAutomation();
    if (!pAuto)
        return;

   wxCOMPtr <IUIAutomationElement> pRoot;
    if (FAILED(pAuto->ElementFromHandle(hwnd, &pRoot)))
        return;

    wxCOMPtr<IUIAutomationTreeWalker> pWalker;
    pAuto->get_ContentViewWalker(&pWalker);
    if (!pWalker)
        return;

    wxCOMPtr<IUIAutomationElement> pChild;
    pWalker->GetFirstChildElement(pRoot, &pChild);

    while (pChild)
    {
        TDLayoutElement info{};
        pChild->get_CurrentBoundingRectangle(&info.rect);
        ScreenToClient(hwnd, reinterpret_cast<POINT*>(&info.rect.left));
        ScreenToClient(hwnd, reinterpret_cast<POINT*>(&info.rect.right));

        {
            BSTR b;
            pChild->get_CurrentAutomationId(&b);
            if (b) info.automationId = static_cast<LPCWSTR>(b);
        }
        {
            BSTR b;
            pChild->get_CurrentName(&b);
            if (b) info.name = static_cast<LPCWSTR>(b);
        }

        if (info.automationId == L"VerificationCheckBox")
        {
            VARIANT v; VariantInit(&v);
            if (SUCCEEDED(pChild->GetCurrentPropertyValue(
                UIA_LegacyIAccessibleStatePropertyId, &v)) && v.vt == VT_I4)
                info.legacyState = v.lVal;
            VariantClear(&v);
        }

        if (!info.automationId.empty() && !IsRectEmpty(&info.rect))
        {
            const std::wstring& id = info.automationId;
            if (id == L"MainIcon" || id == L"FootnoteIcon" ||
                id == L"MainInstruction" || id == L"ContentText" ||
                id == L"ExpandedFooterText" || id == L"FootnoteText" ||
                id == L"ExpandoButton" || id == L"VerificationCheckBox" ||
                id.find(L"RadioButton_") == 0 ||
                id.find(L"CommandLink_") == 0 ||
                id.find(L"CommandButton_") == 0)
            {
                out.push_back(std::move(info));
            }
        }

        wxCOMPtr<IUIAutomationElement> pNext;
        pWalker->GetNextSiblingElement(pChild, &pNext);
        pChild = pNext;
    }
}

static void TDUpdateLayoutCache(HWND hwnd, TDPageState& s)
{
    if (!s.elemsOk)
    {
        TDBuildLayoutCache(hwnd, s.elements);
        s.elemsOk = true;
    }
    for (const auto& el : s.elements)
        if (el.automationId == L"VerificationCheckBox")
        {
            s.isChecked = (el.legacyState & STATE_SYSTEM_CHECKED) != 0;
            break;
        }
    if (s.defChecked)
        s.isChecked = true;
}

static int TDHitTest(const std::vector<TDLayoutElement>& els, POINT pt)
{
    for (int i = 0; i < static_cast<int>(els.size()); ++i)
        if (PtInRect(&els[i].rect, pt)) return i;
    return -1;
}

// ============================================================================
// Paint routines
// ============================================================================

static void TDPaintPixelSwap(HPAINTBUFFER hbp, int w, int h)
{
    RGBQUAD* pPx = nullptr; int rw = 0;
    if (FAILED(GetBufferedPaintBits(hbp, &pPx, &rw))) return;

    wxColour srcPri(0xff, 0xff, 0xff),
             srcSec(0xf0, 0xf0, 0xf0),
             srcSep(0x80, 0x80, 0x80),
             srcSp2(0xdf, 0xdf, 0xdf);

    // Get the primary colour from the theme in case it's different.
    //
    // Note that under Windows 10 the theme doesn't define the other colours
    // and under Windows 11 this code is not executed at all because it has
    // native task dialog dark theme.
    wxUxThemeHandle theme(wxUxThemeHandle::NewAtStdDPI(L"TaskDialog"));
    if ( theme )
        srcPri = theme.GetColour(TDLG_PRIMARYPANEL, TMT_FILLCOLOR);

    // Define our own macros doing explicit cast because the standard
    // Get[RGB]Value() result in warnings about "truncating constant value".
#define GET_B(c) static_cast<BYTE>((c) & 0xFF)
#define GET_G(c) static_cast<BYTE>(((c) >> 8) & 0xFF)
#define GET_R(c) static_cast<BYTE>(((c) >> 16) & 0xFF)

    struct Rule { BYTE sR, sG, sB, dR, dG, dB; };
    const Rule rules[] =
    {
        { srcPri.Red(), srcPri.Green(), srcPri.Blue(),
          GET_R(TDDarkCol::kPrimary), GET_G(TDDarkCol::kPrimary), GET_B(TDDarkCol::kPrimary) },
        { srcSec.Red(), srcSec.Green(), srcSec.Blue(),
          GET_R(TDDarkCol::kSecondary), GET_G(TDDarkCol::kSecondary), GET_B(TDDarkCol::kSecondary) },
        { srcSep.Red(), srcSep.Green(), srcSep.Blue(),
          GET_R(TDDarkCol::kSeparator), GET_G(TDDarkCol::kSeparator), GET_B(TDDarkCol::kSeparator) },
        { srcSp2.Red(), srcSp2.Green(), srcSp2.Blue(),
          GET_R(TDDarkCol::kSeparator), GET_G(TDDarkCol::kSeparator), GET_B(TDDarkCol::kSeparator) },
    };

#undef GET_B
#undef GET_G
#undef GET_R

    for (int y = 0; y < h; ++y)
    {
        RGBQUAD* row = pPx + static_cast<ptrdiff_t>(y) * rw;
        for (int x = 0; x < w; ++x)
        {
            RGBQUAD& p = row[x];
            for (const Rule& r : rules)
                if (p.rgbRed == r.sR && p.rgbGreen == r.sG && p.rgbBlue == r.sB)
                {
                    p.rgbRed = r.dR; p.rgbGreen = r.dG; p.rgbBlue = r.dB;
                    p.rgbReserved = 55;
                    break;
                }
        }
    }
}

static void TDPaintIcons(HDC hdc, const TDPageState& s)
{
    if (!s.pCfg || TDHasNativeDarkTheme()) return;
    for (const auto& el : s.elements)
    {
        if (IsRectEmpty(&el.rect)) continue;
        HICON hIcon = nullptr; HBRUSH brBg = nullptr;
        if (el.automationId == L"MainIcon")
        {
            hIcon = TDLoadStockIcon(s.pCfg, true);
            brBg = s.brPrimary;
        }
        else if (el.automationId == L"FootnoteIcon")
        {
            hIcon = TDLoadStockIcon(s.pCfg, false);
            brBg = s.brFootnote;
        }
        if (!hIcon || !brBg) continue;
        FillRect(hdc, &el.rect, brBg);
        DrawIconEx(hdc, el.rect.left, el.rect.top, hIcon,
            el.rect.right - el.rect.left, el.rect.bottom - el.rect.top,
            0, nullptr, DI_NORMAL);
    }
}

static void TDPaintGlyphs(HDC hdc, TDPageState& s)
{
    if (!s.hTD && !s.hButton)
        return;
    const bool native = TDHasNativeDarkTheme();

    for (int i = 0; i < static_cast<int>(s.elements.size()); ++i)
    {
        const TDLayoutElement& el = s.elements[i];
        if (IsRectEmpty(&el.rect))
            continue;
        const bool hot = (i == s.hotIdx), press = hot && s.pressing;

        if (el.automationId == L"ExpandoButton" && s.hTD)
        {
            SIZE sz = {}; GetThemePartSize(s.hTD, hdc, TDLG_EXPANDOBUTTON, TDLGEBS_NORMAL, nullptr, TS_TRUE, &sz);
            RECT rcG = el.rect; rcG.right = el.rect.left + sz.cx + 3;
            int st = (press && s.isExpanded) ? TDLGEBS_EXPANDEDPRESSED :
                press ? TDLGEBS_PRESSED :
                (hot && s.isExpanded) ? TDLGEBS_EXPANDEDHOVER :
                hot ? TDLGEBS_HOVER :
                s.isExpanded ? TDLGEBS_EXPANDEDNORMAL : TDLGEBS_NORMAL;
            if (!native) {
                FillRect(hdc, &rcG, s.brSecondary);
                DrawThemeBackground(s.hTD, hdc, TDLG_EXPANDOBUTTON, st, &rcG, &el.rect);
            }
        }
        else if (el.automationId == L"VerificationCheckBox" && s.hButton)
        {
            SIZE cs = {}; GetThemePartSize(s.hButton, hdc, BP_CHECKBOX, CBS_UNCHECKEDNORMAL, nullptr, TS_DRAW, &cs);
            int mg = (el.rect.bottom - el.rect.top - cs.cy) / 3;
            RECT rcG = { el.rect.left + mg + 1,el.rect.top + mg + 1,el.rect.left + mg + 1 + cs.cx,el.rect.bottom };
            int st = (press && s.isChecked) ? CBS_CHECKEDPRESSED :
                press ? CBS_UNCHECKEDPRESSED :
                (hot && s.isChecked) ? CBS_CHECKEDHOT :
                hot ? CBS_UNCHECKEDHOT :
                s.isChecked ? CBS_CHECKEDNORMAL : CBS_UNCHECKEDNORMAL;
            if (!native) {
                FillRect(hdc, &rcG, s.brSecondary);
                DrawThemeBackground(s.hButton, hdc, BP_CHECKBOX, st, &rcG, nullptr);
            }
        }
    }
}

static void TDPaintText(HDC hdc, const TDPageState& s)
{
    if (!s.hTDS && !s.hTD)
        return;
     const bool native = TDHasNativeDarkTheme();

    for (const auto& el : s.elements)
    {
        if (IsRectEmpty(&el.rect))
            continue;
        RECT   rcT = el.rect;
        int    part = 0;
        HBRUSH brBg = s.brPrimary;
        DWORD  dtF = DT_LEFT | DT_VCENTER | DT_WORDBREAK | DT_NOPREFIX;

        if (el.automationId == L"MainInstruction")
        {
            part = TDLG_MAININSTRUCTIONPANE;
            brBg = s.brPrimary;
        }
        else if (el.automationId == L"ContentText")
        {
            part = TDLG_CONTENTPANE;
            brBg = s.brPrimary;
        }
        else if (el.automationId == L"ExpandedFooterText")
        {
            part = TDLG_EXPANDEDFOOTERAREA;
            brBg = s.brFootnote;
        }
        else if (el.automationId == L"FootnoteText")
        {
            part = TDLG_FOOTNOTEPANE;
            brBg = s.brFootnote;
        }
        else if (el.automationId == L"ExpandoButton" && s.hTD)
        {
            SIZE sz = {};
            ::GetThemePartSize(s.hTD, hdc, TDLG_EXPANDOBUTTON, TDLGEBS_NORMAL, nullptr, TS_TRUE, &sz);
            MARGINS vm = {};
            ::GetThemeMargins(s.hTD, hdc, TDLG_VERIFICATIONTEXT, 0, TMT_CONTENTMARGINS, nullptr, &vm);
            rcT.left += sz.cx + vm.cxLeftWidth - 2; rcT.top += 1;
            part = TDLG_EXPANDOTEXT; brBg = s.brSecondary;
            dtF = DT_LEFT | DT_VCENTER | DT_NOPREFIX;
        }
        else if (el.automationId == L"VerificationCheckBox" && s.hButton && s.hTD)
        {
            SIZE cs = {};
            ::GetThemePartSize(s.hButton, hdc, BP_CHECKBOX, CBS_UNCHECKEDNORMAL, nullptr, TS_DRAW, &cs);
            MARGINS tm = {};
            ::GetThemeMargins(s.hTD, hdc, TDLG_VERIFICATIONTEXT, 0, TMT_CONTENTMARGINS, nullptr, &tm);
            rcT.left = el.rect.left + cs.cx + tm.cxLeftWidth + 3; rcT.top += 5;
            part = TDLG_VERIFICATIONTEXT; brBg = s.brSecondary;
            dtF = DT_LEFT | DT_VCENTER | DT_NOPREFIX;
        }
        if (!part)
            continue;

        if (!native)
        {
            DTTOPTS opts = { sizeof(opts) }; opts.dwFlags = DTT_COMPOSITED | DTT_TEXTCOLOR;
            opts.crText = TDGetTextColour(s, part);
            FillRect(hdc, &rcT, brBg);
            ::DrawThemeTextEx(s.hTDS ? s.hTDS : s.hTD, hdc, part, 0, el.name.c_str(), -1, dtF, &rcT, &opts);
        }
        else
        {

            ::DrawThemeText(s.hTDS ? s.hTDS : s.hTD, hdc, part, 0, el.name.c_str(), -1, dtF, 0, &rcT);
        }
    }
}

static void TDPaintPage(HWND hwnd, HDC hdcWin, TDPageState& s)
{
    if (!s.themesOk)
        TDRefreshThemes(hwnd, s);

    RECT rc; GetClientRect(hwnd, &rc);
    HDC hdcBuf = hdcWin;
    HPAINTBUFFER hbp = BeginBufferedPaint(hdcWin, &rc, BPBF_TOPDOWNDIB, nullptr, &hdcBuf);
    if (!hbp)
    {
        DefSubclassProc(hwnd, WM_PRINTCLIENT, reinterpret_cast<WPARAM>(hdcWin), PRF_CLIENT);
        return;
    }

    DefSubclassProc(hwnd, WM_PRINTCLIENT, reinterpret_cast<WPARAM>(hdcBuf), PRF_CLIENT);

    if (!TDHasNativeDarkTheme())
    {
        RECT rcBuf; GetBufferedPaintTargetRect(hbp, &rcBuf);
        TDPaintPixelSwap(hbp, rcBuf.right - rcBuf.left, rcBuf.bottom - rcBuf.top);
    }

    TDPaintIcons(hdcBuf, s);
    TDPaintGlyphs(hdcBuf, s);
    TDPaintText(hdcBuf, s);

    EndBufferedPaint(hbp, TRUE);
}

// ============================================================================
// Subclass procedures
// ============================================================================

static LRESULT CALLBACK TDPageSubclassProc(
    HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
    UINT_PTR uId, DWORD_PTR)
{
    switch (msg)
    {
    case WM_ERASEBKGND:
        return TRUE;

    case WM_PAINT:
    {
        PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
        TDPageState& s = TDPageState::Get(hwnd);
        s.isExpanded = !!GetProp(GetParent(hwnd), L"IsExpanded");
        s.elemsOk = false;
        TDUpdateLayoutCache(hwnd, s);
        TDPaintPage(hwnd, hdc, s);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_MOUSEMOVE:
    {
        TDPageState& s = TDPageState::Get(hwnd);
        if (!s.tracking)
        {
            TRACKMOUSEEVENT tme = { sizeof(tme),TME_LEAVE,hwnd,0 };
            TrackMouseEvent(&tme); s.tracking = true;
        }
        POINT pt = { (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam) };
        int nh = TDHitTest(s.elements, pt);
        if (nh != s.hotIdx)
        {
            s.hotIdx = nh;
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        break;
    }
    case WM_MOUSELEAVE:
    {
        TDPageState& s = TDPageState::Get(hwnd);
        s.tracking = false;
        s.pressing = false;
        if (s.hotIdx != -1)
        {
            s.hotIdx = -1;
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        break;
    }
    case WM_LBUTTONDOWN:
        TDPageState::Get(hwnd).pressing = true;
        TDUpdateLayoutCache(hwnd, TDPageState::Get(hwnd));
        InvalidateRect(hwnd, nullptr, FALSE);
        break;
    case WM_LBUTTONUP:
        TDPageState::Get(hwnd).pressing = false;
        TDUpdateLayoutCache(hwnd, TDPageState::Get(hwnd));
        InvalidateRect(hwnd, nullptr, FALSE);
        break;
    case WM_THEMECHANGED:
    {
        TDPageState& s = TDPageState::Get(hwnd);
        s.CloseThemes(); s.elemsOk = false;
        InvalidateRect(hwnd, nullptr, FALSE);
        break;
    }
    case WM_DESTROY:
        TDPageState::Destroy(hwnd);
        RemoveWindowSubclass(hwnd, TDPageSubclassProc, uId);
        break;
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK TDCtrlContainerSubclassProc(
    HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
    UINT_PTR uId, DWORD_PTR dwRef)
{
    switch (msg)
    {
    case WM_ERASEBKGND:
        if (dwRef)
        {
            HDC hdc = reinterpret_cast<HDC>(wParam); RECT rc; GetClientRect(hwnd, &rc);
            FillRect(hdc, &rc, reinterpret_cast<HBRUSH>(dwRef)); return 1;
        }
        break;
    case WM_CTLCOLORMSGBOX:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
    {
        HDC hdc = reinterpret_cast<HDC>(wParam);
        COLORREF bg = TDDarkCol::kSecondary;
        if (dwRef) { LOGBRUSH lb = {}; GetObject(reinterpret_cast<HBRUSH>(dwRef), sizeof(lb), &lb); if (lb.lbStyle == BS_SOLID) bg = lb.lbColor; }
        SetBkColor(hdc, bg); SetTextColor(hdc, TDDarkCol::kTextNormal);
        return reinterpret_cast<LRESULT>(dwRef ? reinterpret_cast<HBRUSH>(dwRef) : CreateSolidBrush(TDDarkCol::kSecondary));
    }
    case WM_DESTROY:
        if (dwRef) DeleteObject(reinterpret_cast<HBRUSH>(dwRef));
        RemoveWindowSubclass(hwnd, TDCtrlContainerSubclassProc, uId);
        return DefSubclassProc(hwnd, msg, wParam, lParam);
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK TDRadioButtonSubclassProc(
    HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
    UINT_PTR uId, DWORD_PTR)
{
    if (msg == WM_PAINT)
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        int dpi = GetWindowDPI(hwnd);
        auto hStyle = wxUxThemeHandle::NewAtStdDPI(L"TaskDialogStyle");
        auto  hBtn = wxUxThemeHandle::NewAtDPI(nullptr, L"Button",dpi);
        RECT rcC; GetClientRect(hwnd, &rcC);
        HDC hdcBuf = hdc;
        HPAINTBUFFER hbp = BeginBufferedPaint(hdc, &rcC, BPBF_TOPDOWNDIB, nullptr, &hdcBuf);
        DefSubclassProc(hwnd, WM_PRINTCLIENT, reinterpret_cast<WPARAM>(hdcBuf), PRF_CLIENT);
        wchar_t text[512] = {};
        GetWindowTextW(hwnd, text, static_cast<int>(std::size(text)));
        auto gs = hBtn.GetTrueSize(BP_RADIOBUTTON, RBS_UNCHECKEDNORMAL);
        RECT rcT = { gs.x + 2,0,rcC.right,rcC.bottom };
        DTTOPTS opts = { sizeof(opts) };
        opts.dwFlags = DTT_COMPOSITED | DTT_TEXTCOLOR;
        opts.crText = TDDarkCol::kTextNormal;
        LOGFONT lf = {};
        if (SUCCEEDED(GetThemeFont(hStyle, hdcBuf, TDLG_RADIOBUTTONPANE, 0, TMT_FONT, &lf)))
        {
            HFONT hF = CreateFontIndirect(&lf), hO = static_cast<HFONT>(SelectObject(hdcBuf, hF));
            DrawThemeTextEx(hStyle, hdcBuf, TDLG_RADIOBUTTONPANE, 0, text, -1, DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS, &rcT, &opts);
            SelectObject(hdcBuf, hO); DeleteObject(hF);
        }
        if (hbp)
            EndBufferedPaint(hbp, TRUE);
        EndPaint(hwnd, &ps); return 0;
    }
    if (msg == WM_DESTROY)
        RemoveWindowSubclass(hwnd, TDRadioButtonSubclassProc, uId);
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

// ============================================================================
// Attachment helpers
// ============================================================================

static void TDSubclassContainer(HWND hP, COLORREF bg)
{
    if (!hP) return;
    DWORD_PTR ex = 0;
    if (!GetWindowSubclass(hP, TDCtrlContainerSubclassProc, kTDCtrlSubclassId, &ex))
        SetWindowSubclass(hP, TDCtrlContainerSubclassProc, kTDCtrlSubclassId,
            reinterpret_cast<DWORD_PTR>(CreateSolidBrush(bg)));
}

static void TDApplyToChildren(IUIAutomationElement* pEl)
{
    IUIAutomation* const uiAuto = wxTaskDialogDarkModule::GetUIAutomation();
    if ( !uiAuto )
        return;

    const bool native = TDHasNativeDarkTheme();

    wxCOMPtr<IUIAutomationTreeWalker> pW;
    uiAuto->get_ContentViewWalker(&pW);
    if (!pW) return;
    wxCOMPtr<IUIAutomationElement> pChild;
    pW->GetFirstChildElement(pEl, &pChild);

    while (pChild)
    {
        CONTROLTYPEID ct = 0;
        pChild->get_CurrentControlType(&ct);
        if (ct == UIA_ButtonControlTypeId || ct == UIA_RadioButtonControlTypeId ||
            ct == UIA_ProgressBarControlTypeId || ct == UIA_HyperlinkControlTypeId ||
            ct == UIA_ScrollBarControlTypeId || ct == UIA_PaneControlTypeId)
        {
            if ( const HWND hBtn = GetHWNDFromElement(pChild) )
            {
                BSTR bId; pChild->get_CurrentAutomationId(&bId);
                const std::wstring id(bId ? static_cast<LPCWSTR>(bId) : L"");
                HWND hP = GetParent(hBtn);

                if (ct == UIA_ProgressBarControlTypeId)
                {
                    wxUxThemeHandle hCE = wxUxThemeHandle::NewAtStdDPI(L"DarkMode_CopyEngine::Progress");
                    wxUxThemeHandle hBase = wxUxThemeHandle::NewAtStdDPI(L"Progress");
                    bool hasCE = (hCE && hCE != hBase);
                    SetWindowTheme(hBtn, hasCE ? L"DarkMode_CopyEngine" : L"DarkMode_Explorer", nullptr);
                }
                else if (ct == UIA_RadioButtonControlTypeId || id.find(L"RadioButton_") == 0 || ct == UIA_HyperlinkControlTypeId)
                {
                    if (native)
                    {
                        SetWindowTheme(hBtn, L"DarkMode_DarkTheme", nullptr);
                    }
                    else
                    {
                        DWORD_PTR ex = 0;
                        if (!GetWindowSubclass(hBtn, TDRadioButtonSubclassProc, kTDCtrlSubclassId, &ex))
                            SetWindowSubclass(hBtn, TDRadioButtonSubclassProc, kTDCtrlSubclassId, 0);
                    }
                    TDSubclassContainer(hP, native ? TDDarkCol::kSecondary : TDDarkCol::kPrimary);
                }
                else if (id.find(L"CommandLink_") == 0 || id.find(L"CommandButton_") == 0)
                {
                    SetWindowTheme(hBtn, L"DarkMode_Explorer", nullptr);
                    TDSubclassContainer(hP, TDDarkCol::kSecondary);
                }
                else
                {
                    SetWindowTheme(hBtn, L"DarkMode_Explorer", nullptr);
                }
            }
        }
        wxCOMPtr<IUIAutomationElement> pNext; pW->GetNextSiblingElement(pChild, &pNext);
        pChild = pNext;
    }
}

// EnumData is used by TDEnumAttachProc below to get the input parameters and
// return the "found" result.
struct TDEnumData
{
    HWND hwndTD = 0;
    const TASKDIALOGCONFIG* pCfg = nullptr;
    bool found = false;
};

static BOOL CALLBACK TDEnumAttachProc(HWND hwndChild, LPARAM lparam)
{
    IUIAutomation* const uiAuto = wxTaskDialogDarkModule::GetUIAutomation();
    if ( !uiAuto )
        return FALSE;

    wxCOMPtr<IUIAutomationElement> pEl;
    if ( FAILED(uiAuto->ElementFromHandle(hwndChild, &pEl)) )
        return TRUE;

    BSTR cls;
    pEl->get_CurrentClassName(&cls);

    // SysLink controls (footnote / content hyperlinks)
    if ( wcscmp(cls, L"CCSysLink") == 0 )
    {
        if ( const HWND hL = GetHWNDFromElement(pEl) )
        {
            BSTR bId;
            pEl->get_CurrentAutomationId(&bId);
            const std::wstring id(bId ? static_cast<LPCWSTR>(bId) : L"");
            bool isFn = id.find(L"Footnote") != std::wstring::npos || id.find(L"ExpandedFooter") != std::wstring::npos;
            TDSubclassContainer(GetParent(hL), (isFn && !TDHasNativeDarkTheme()) ? TDDarkCol::kFootnote : TDDarkCol::kPrimary);
        }
        return TRUE;
    }

    // Main TaskPage (DirectUI "TaskDialog" class)
    if ( wcscmp(cls, L"TaskDialog") != 0 )
        return TRUE;

    const HWND hDUI = GetHWNDFromElement(pEl);
    if ( !hDUI )
        return TRUE;

    // Class background brush
    {
        HBRUSH nb = CreateSolidBrush(TDDarkCol::kSecondary);
        HBRUSH ob = reinterpret_cast<HBRUSH>(SetClassLongPtr(hDUI, GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(nb)));
        if (ob && ob != GetSysColorBrush(COLOR_WINDOW) && ob != GetSysColorBrush(COLOR_BTNFACE)) DeleteObject(ob);
    }

    TDApplyToChildren(pEl);
    SetWindowTheme(hDUI, L"DarkMode_Explorer", nullptr);

    // Initialise per-page state
    TDEnumData* const d = reinterpret_cast<TDEnumData*>(lparam);
    TDPageState& s = TDPageState::Get(hDUI);
    s.pCfg = d->pCfg;
    s.defExpanded = d->pCfg && (d->pCfg->dwFlags & TDF_EXPANDED_BY_DEFAULT);
    s.defChecked = d->pCfg && (d->pCfg->dwFlags & TDF_VERIFICATION_FLAG_CHECKED);
    s.isExpanded = GetProp(d->hwndTD, L"IsExpanded");
    s.isChecked = s.defChecked || GetProp(d->hwndTD, L"IsChecked");
    s.elemsOk = false;
    TDUpdateLayoutCache(hDUI, s);

    DWORD_PTR ex = 0;
    if (!GetWindowSubclass(hDUI, TDPageSubclassProc, kTDPageSubclassId, &ex))
        SetWindowSubclass(hDUI, TDPageSubclassProc, kTDPageSubclassId, 0);

    d->found = true;
    return TRUE;
}

static BOOL CALLBACK TDEnumSysColorProc(HWND h, LPARAM)
{
    SendMessage(h, WM_SYSCOLORCHANGE, 0, 0);
    return TRUE;
}

static BOOL CALLBACK TDEnumDetachProc(HWND hChild, LPARAM)
{
    DWORD_PTR ex = 0;
    if (GetWindowSubclass(hChild, TDPageSubclassProc, kTDPageSubclassId, &ex))
    {
        RemoveWindowSubclass(hChild, TDPageSubclassProc, kTDPageSubclassId);
        TDPageState::Destroy(hChild);
    }
    if (GetWindowSubclass(hChild, TDCtrlContainerSubclassProc, kTDCtrlSubclassId, &ex))
        RemoveWindowSubclass(hChild, TDCtrlContainerSubclassProc, kTDCtrlSubclassId);
    if (GetWindowSubclass(hChild, TDRadioButtonSubclassProc, kTDCtrlSubclassId, &ex))
        RemoveWindowSubclass(hChild, TDRadioButtonSubclassProc, kTDCtrlSubclassId);
    SetWindowTheme(hChild, nullptr, nullptr);
    return TRUE;
}

static void TDAttach(HWND hwndTD, const TASKDIALOGCONFIG* pCfg)
{
    const bool native = TDHasNativeDarkTheme();

    TDEnumData data;
    data.hwndTD = hwndTD;
    data.pCfg = pCfg;

    EnumChildWindows(hwndTD, TDEnumAttachProc, reinterpret_cast<LPARAM>(&data));

    if (!data.found)
        return;

    if (native)
        wxMSWDarkMode::AllowForWindow(hwndTD, L"DarkMode_Explorer", nullptr);
    DWORD_PTR ex;
    if (!GetWindowSubclass(hwndTD, TDCtrlContainerSubclassProc, kTDMainSubclassId, &ex))
        SetWindowSubclass(hwndTD, TDCtrlContainerSubclassProc, kTDMainSubclassId, 0);
    // Dark title bar
    wxMSWDarkMode::ConfigureTLW(hwndTD);
    HBRUSH nb = CreateSolidBrush(TDDarkCol::kPrimary);
    SetClassLongPtr(hwndTD, GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(nb));
    EnumChildWindows(hwndTD, TDEnumSysColorProc, 0);
    SendMessage(hwndTD, WM_THEMECHANGED, 0, 0);
}

static void TDDetach(HWND hwndTD)
{
    EnumChildWindows(hwndTD, TDEnumDetachProc, 0);

} // TDDetach
} // anonymous namespace

#endif // wxUSE_DARK_MODE

// ----------------------------------------------------------------------------
// TaskDialog dark mode — public entry points
// ----------------------------------------------------------------------------

void wxMSWDarkMode::AllowForTaskDialog(HWND hwnd, const TASKDIALOGCONFIG* pCfg)
{
#if wxUSE_DARK_MODE
    if ( !wxMSWDarkMode::IsActive() )
        return;

    // Ensure COM is initialised for UIA on this thread.
    // S_FALSE means already initialised by the caller — that is fine.
    const HRESULT hr = CoInitializeEx(
        nullptr,
        COINIT_APARTMENTTHREADED |
        COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr) && hr != S_FALSE)
        return;

    TDAttach(hwnd, pCfg);

    CoUninitialize();
#endif // wxUSE_DARK_MODE
}

void wxMSWDarkMode::RemoveFromTaskDialog(HWND hwnd)
{
#if wxUSE_DARK_MODE
    TDDetach(hwnd);
#endif // wxUSE_DARK_MODE
}
