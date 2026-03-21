///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/darkmode.cpp
// Purpose:     Support for dark mode in wxMSW
// Author:      Vadim Zeitlin
// Created:     2022-06-24
// Copyright:   (c) 2022 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/*
    The code in this file is based on the following sources:

    - win32-darkmode by Richard Yu (https://github.com/ysc3839/win32-darkmode)
    - UAH menu by adzm (https://github.com/adzm/win32-custom-menubar-aero-theme)
 */

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

// Allow predefining this as 0 to disable dark mode support completely.
#ifndef wxUSE_DARK_MODE
    // Otherwise enable it by default.
    #define wxUSE_DARK_MODE 1
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/settings.h"
#endif

#include "wx/dynlib.h"
#include "wx/module.h"

#include "wx/msw/darkmode.h"
#include "wx/msw/uxtheme.h"

#include "wx/msw/private/darkmode.h"

#include <UIAutomationClient.h>
#include <UIAutomationCore.h>
#include <vssym32.h>
#include <commctrl.h>
#include <atlbase.h>    // CComPtr, CComBSTR
#include <dwmapi.h>
#include <windows.h>

// ----------------------------------------------------------------------------
// Module keeping dark mode-related data and wrapping DwmSetWindowAttribute()
// ----------------------------------------------------------------------------

namespace
{

// This function is documented, but we still load it dynamically to avoid
// having to link with dwmapi.lib.
typedef HRESULT
(WINAPI *DwmSetWindowAttribute_t)(HWND, DWORD, const void*, DWORD);

} // anonymous namespace

class wxDarkModeModule : public wxModule
{
public:
    virtual bool OnInit() override { return true; }
    virtual void OnExit() override
    {
        ms_settings.reset();

        ms_pfnDwmSetWindowAttribute = (DwmSetWindowAttribute_t)-1;
        ms_dllDWM.Unload();
    }

    // Takes ownership of the provided pointer.
    static void SetSettings(wxDarkModeSettings* settings)
    {
        ms_settings.reset(settings);
    }

    // Returns the currently used settings: may only be called when the dark
    // mode is on.
    static wxDarkModeSettings& GetSettings()
    {
        return *ms_settings;
    }

    static DwmSetWindowAttribute_t GetDwmSetWindowAttribute()
    {
        if ( ms_pfnDwmSetWindowAttribute == (DwmSetWindowAttribute_t)-1 )
        {
            ms_dllDWM.Load(wxS("dwmapi.dll"), wxDL_VERBATIM | wxDL_QUIET);
            wxDL_INIT_FUNC(ms_pfn, DwmSetWindowAttribute, ms_dllDWM);
        }

        return ms_pfnDwmSetWindowAttribute;
    }

private:
    static wxDynamicLibrary ms_dllDWM;
    static DwmSetWindowAttribute_t ms_pfnDwmSetWindowAttribute;

    static std::unique_ptr<wxDarkModeSettings> ms_settings;

    wxDECLARE_DYNAMIC_CLASS(wxDarkModeModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxDarkModeModule, wxModule);

wxDynamicLibrary wxDarkModeModule::ms_dllDWM;
std::unique_ptr<wxDarkModeSettings> wxDarkModeModule::ms_settings;

DwmSetWindowAttribute_t
wxDarkModeModule::ms_pfnDwmSetWindowAttribute = (DwmSetWindowAttribute_t)-1;

#if wxUSE_DARK_MODE

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
    #include "wx/dcmemory.h"
    #include "wx/image.h"
    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/msw/dc.h"

#include "wx/msw/private/custompaint.h"
#include "wx/msw/private/menu.h"
#include "wx/msw/private/metrics.h"
#include <memory>

#if wxUSE_LOG_TRACE
static const char* TRACE_DARKMODE = "msw-darkmode";
#endif // wxUSE_LOG_TRACE
#include <atlwin.h>

#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#define GET_B(c) static_cast<BYTE>((c) & 0xFF)
#define GET_G(c) static_cast<BYTE>(((c) >> 8) & 0xFF)
#define GET_R(c) static_cast<BYTE>(((c) >> 16) & 0xFF)

namespace
{

// Constants for use with SetPreferredAppMode().
enum PreferredAppMode
{
    AppMode_Default,
    AppMode_AllowDark,
    AppMode_ForceDark,
    AppMode_ForceLight
};

PreferredAppMode gs_appMode = AppMode_Default;

template <typename T>
bool TryLoadByOrd(T& func, const wxDynamicLibrary& lib, int ordinal)
{
    func = (T)::GetProcAddress(lib.GetLibHandle(), MAKEINTRESOURCEA(ordinal));
    if ( !func )
    {
        wxLogTrace(TRACE_DARKMODE,
                   "Required function with ordinal %d not found", ordinal);
        return false;
    }

    return true;
}

} // anonymous namespace


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
    HTHEME hTD = nullptr; // TaskDialog panel + glyph parts
    HTHEME hTDS = nullptr; // TaskDialogStyle / text fonts
    HTHEME hButton = nullptr; // Button (checkbox glyph)
    bool   isDark = false;   // native dark theme available
    bool   themesOk = false;

    HBRUSH brPrimary = nullptr;
    HBRUSH brSecondary = nullptr;
    HBRUSH brFootnote = nullptr;

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
        if (hTD) { CloseThemeData(hTD);     hTD = nullptr; }
        if (hTDS) { CloseThemeData(hTDS);    hTDS = nullptr; }
        if (hButton) { CloseThemeData(hButton); hButton = nullptr; }
        themesOk = false;
    }
    void DestroyBrushes()
    {
        if (brPrimary) { DeleteObject(brPrimary);   brPrimary = nullptr; }
        if (brSecondary) { DeleteObject(brSecondary); brSecondary = nullptr; }
        if (brFootnote) { DeleteObject(brFootnote);  brFootnote = nullptr; }
    }
    void Destroy() { CloseThemes(); DestroyBrushes(); elements.clear(); elemsOk = false; }
};

// Thread-local state map and UIA singleton
static thread_local std::unordered_map<HWND, TDPageState> tls_tdStates;
static thread_local CComPtr<IUIAutomation>                 tls_tdAutomation;

static TDPageState& GetTDPageState(HWND h) { return tls_tdStates[h]; }

static void DestroyTDPageState(HWND h)
{
    auto it = tls_tdStates.find(h);
    if (it != tls_tdStates.end()) { it->second.Destroy(); tls_tdStates.erase(it); }
}

static IUIAutomation* GetTDAutomation()
{
    if (!tls_tdAutomation)
        tls_tdAutomation.CoCreateInstance(__uuidof(CUIAutomation));
    return tls_tdAutomation;
}

// Subclass IDs
static constexpr UINT_PTR kTDMainSubclassId = 0xDEADBEEFul;
static constexpr UINT_PTR kTDPageSubclassId = 0xBADF00Dul;
static constexpr UINT_PTR kTDCtrlSubclassId = 0xC0FFEE01ul;

// ============================================================================
// TaskDialog theme helpers
// ============================================================================

// Cached probe: does the OS have a native dark TaskDialog theme (Win11+)?
static bool TDHasNativeDarkTheme()
{
    static int s_cached = -1;
    if (s_cached == -1)
    {
        HTHEME hD = OpenThemeData(nullptr, L"DarkMode_DarkTheme::TaskDialog");
        HTHEME hB = OpenThemeData(nullptr, L"TaskDialog");
        s_cached = (hD && hD != hB) ? 1 : 0;
        if (hD) CloseThemeData(hD);
        if (hB) CloseThemeData(hB);
    }
    return s_cached == 1;
}

static void TDRefreshThemes(HWND hwnd, TDPageState& s)
{
    s.CloseThemes();
    const UINT dpi = GetDpiForWindow(hwnd);

    auto Open = [&](const wchar_t* cls) -> HTHEME {
        HTHEME h = OpenThemeDataForDpi(hwnd, cls, dpi);
        return h ? h : OpenThemeData(hwnd, cls);
        };

    s.isDark = TDHasNativeDarkTheme();

    if (s.isDark) s.hTD = Open(L"DarkMode_DarkTheme::TaskDialog");
    if (!s.hTD)   s.hTD = Open(L"DarkMode_Explorer::TaskDialog");
    if (!s.hTD)   s.hTD = Open(L"TaskDialog");

    s.hTDS = Open(s.isDark ? L"DarkMode_Explorer::TaskDialog" : L"TaskDialog");
    s.hButton = Open(L"Button");
    s.themesOk = true;
}

static void TDEnsureBrushes(TDPageState& s)
{
    if (!s.brPrimary)   s.brPrimary = CreateSolidBrush(TDDarkCol::kPrimary);
    if (!s.brSecondary) s.brSecondary = CreateSolidBrush(TDDarkCol::kSecondary);
    if (!s.brFootnote)  s.brFootnote = CreateSolidBrush(TDDarkCol::kFootnote);
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
    case TDLG_MAININSTRUCTIONPANE: return TDDarkCol::kTextInstruct;
    case TDLG_CONTENTPANE:         return TDDarkCol::kTextContent;
    case TDLG_EXPANDOTEXT:         return TDDarkCol::kTextExpando;
    case TDLG_VERIFICATIONTEXT:    return TDDarkCol::kTextVerify;
    case TDLG_FOOTNOTEPANE:        return TDDarkCol::kTextFootnote;
    case TDLG_EXPANDEDFOOTERAREA:  return TDDarkCol::kTextFtrExp;
    case TDLG_RADIOBUTTONPANE:     return TDDarkCol::kTextRadio;
    default:                       return TDDarkCol::kTextNormal;
    }
}

// ============================================================================
// Icon loading
// ============================================================================

static HICON TDLoadStockIcon(const TASKDIALOGCONFIG* cfg, bool isMain)
{
    if (!cfg) return nullptr;

    if (isMain && (cfg->dwFlags & TDF_USE_HICON_MAIN))   return cfg->hMainIcon;
    if (!isMain && (cfg->dwFlags & TDF_USE_HICON_FOOTER)) return cfg->hFooterIcon;

    LPCWSTR res = isMain ? cfg->pszMainIcon : cfg->pszFooterIcon;
    if (!res || !IS_INTRESOURCE(res)) return nullptr;

    auto Stock = [](SHSTOCKICONID id) -> HICON {
        SHSTOCKICONINFO sii = { sizeof(sii) };
        return SUCCEEDED(SHGetStockIconInfo(id,
            SHGSI_ICON | SHGSI_LARGEICON, &sii))
            ? sii.hIcon : nullptr;
        };
    if (res == TD_WARNING_ICON)     return Stock(SIID_WARNING);
    if (res == TD_ERROR_ICON)       return Stock(SIID_ERROR);
    if (res == TD_INFORMATION_ICON) return Stock(SIID_INFO);
    if (res == TD_SHIELD_ICON)      return Stock(SIID_SHIELD);

    return nullptr;
}

// ============================================================================
// UIA layout cache
// ============================================================================

static void TDBuildLayoutCache(HWND hwnd, std::vector<TDLayoutElement>& out)
{
    out.clear();
    IUIAutomation* pAuto = GetTDAutomation();
    if (!pAuto) return;

    CComPtr<IUIAutomationElement> pRoot;
    if (FAILED(pAuto->ElementFromHandle(hwnd, &pRoot))) return;

    CComPtr<IUIAutomationTreeWalker> pWalker;
    pAuto->get_ContentViewWalker(&pWalker);
    if (!pWalker) return;

    CComPtr<IUIAutomationElement> pChild;
    pWalker->GetFirstChildElement(pRoot, &pChild);

    while (pChild)
    {
        TDLayoutElement info{};
        pChild->get_CurrentBoundingRectangle(&info.rect);
        ScreenToClient(hwnd, reinterpret_cast<POINT*>(&info.rect.left));
        ScreenToClient(hwnd, reinterpret_cast<POINT*>(&info.rect.right));

        {
            CComBSTR b; pChild->get_CurrentAutomationId(&b);
            if (b) info.automationId = static_cast<LPCWSTR>(b);
        }
        {
            CComBSTR b; pChild->get_CurrentName(&b);
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

        CComPtr<IUIAutomationElement> pNext;
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
    if (s.defChecked) s.isChecked = true;
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

    COLORREF srcPri = RGB(255, 255, 255), srcSec = RGB(240, 240, 240);
    COLORREF srcSep = RGB(128, 128, 128), srcSp2 = RGB(223, 223, 223);

    HTHEME hL = OpenThemeData(nullptr, L"TaskDialog");
    if (hL)
    {
        GetThemeColor(hL, TDLG_PRIMARYPANEL, 0, TMT_FILLCOLOR, &srcPri);
        GetThemeColor(hL, TDLG_SECONDARYPANEL, 0, TMT_FILLCOLOR, &srcSec);
        GetThemeColor(hL, TDLG_FOOTNOTESEPARATOR, 0, TMT_FILLCOLOR, &srcSep);
        CloseThemeData(hL);
    }
    struct Rule { BYTE sR, sG, sB, dR, dG, dB; };
    const Rule rules[] =
    {
        { GET_R(srcPri), GET_G(srcPri), GET_B(srcPri),
          GET_R(TDDarkCol::kPrimary), GET_G(TDDarkCol::kPrimary), GET_B(TDDarkCol::kPrimary) },
        { GET_R(srcSec), GET_G(srcSec), GET_B(srcSec),
          GET_R(TDDarkCol::kSecondary), GET_G(TDDarkCol::kSecondary), GET_B(TDDarkCol::kSecondary) },
        { GET_R(srcSep), GET_G(srcSep), GET_B(srcSep),
          GET_R(TDDarkCol::kSeparator), GET_G(TDDarkCol::kSeparator), GET_B(TDDarkCol::kSeparator) },
        { GET_R(srcSp2), GET_G(srcSp2), GET_B(srcSp2),
          GET_R(TDDarkCol::kSeparator), GET_G(TDDarkCol::kSeparator), GET_B(TDDarkCol::kSeparator) },
    };

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
            hIcon = TDLoadStockIcon(s.pCfg, true);  brBg = s.brPrimary;
        }
        else if (el.automationId == L"FootnoteIcon")
        {
            hIcon = TDLoadStockIcon(s.pCfg, false); brBg = s.brFootnote;
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
    if (!s.hTD && !s.hButton) return;
    const bool native = TDHasNativeDarkTheme();

    for (int i = 0; i < static_cast<int>(s.elements.size()); ++i)
    {
        const TDLayoutElement& el = s.elements[i];
        if (IsRectEmpty(&el.rect)) continue;
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
    if (!s.hTDS && !s.hTD) return;
    HTHEME hThm = s.hTDS ? s.hTDS : s.hTD;
    const bool native = TDHasNativeDarkTheme();

    for (const auto& el : s.elements)
    {
        if (IsRectEmpty(&el.rect)) continue;
        RECT   rcT = el.rect;
        int    part = 0;
        HBRUSH brBg = s.brPrimary;
        DWORD  dtF = DT_LEFT | DT_VCENTER | DT_WORDBREAK | DT_NOPREFIX;

        if (el.automationId == L"MainInstruction") { part = TDLG_MAININSTRUCTIONPANE; brBg = s.brPrimary; }
        else if (el.automationId == L"ContentText") { part = TDLG_CONTENTPANE;         brBg = s.brPrimary; }
        else if (el.automationId == L"ExpandedFooterText") { part = TDLG_EXPANDEDFOOTERAREA;  brBg = s.brFootnote; }
        else if (el.automationId == L"FootnoteText") { part = TDLG_FOOTNOTEPANE;        brBg = s.brFootnote; }
        else if (el.automationId == L"ExpandoButton" && s.hTD)
        {
            SIZE sz = {}; GetThemePartSize(s.hTD, hdc, TDLG_EXPANDOBUTTON, TDLGEBS_NORMAL, nullptr, TS_TRUE, &sz);
            MARGINS vm = {}; GetThemeMargins(s.hTD, hdc, TDLG_VERIFICATIONTEXT, 0, TMT_CONTENTMARGINS, &el.rect, &vm);
            rcT.left += sz.cx + vm.cxLeftWidth - 2; rcT.top += 1;
            part = TDLG_EXPANDOTEXT; brBg = s.brSecondary;
            dtF = DT_LEFT | DT_VCENTER | DT_NOPREFIX;
        }
        else if (el.automationId == L"VerificationCheckBox" && s.hButton && s.hTD)
        {
            SIZE cs = {}; GetThemePartSize(s.hButton, hdc, BP_CHECKBOX, CBS_UNCHECKEDNORMAL, nullptr, TS_DRAW, &cs);
            MARGINS tm = {}; GetThemeMargins(s.hTD, hdc, TDLG_VERIFICATIONTEXT, 0, TMT_CONTENTMARGINS, &el.rect, &tm);
            rcT.left = el.rect.left + cs.cx + tm.cxLeftWidth + 3; rcT.top += 5;
            part = TDLG_VERIFICATIONTEXT; brBg = s.brSecondary;
            dtF = DT_LEFT | DT_VCENTER | DT_NOPREFIX;
        }
        if (!part) continue;

        if (!native)
        {
            DTTOPTS opts = { sizeof(opts) }; opts.dwFlags = DTT_COMPOSITED | DTT_TEXTCOLOR;
            opts.crText = TDGetTextColour(s, part);
            FillRect(hdc, &rcT, brBg);
            DrawThemeTextEx(hThm, hdc, part, 0, el.name.c_str(), -1, dtF, &rcT, &opts);
        }
        else
        {
            DrawThemeText(hThm, hdc, part, 0, el.name.c_str(), -1, dtF, 0, &rcT);
        }
    }
}

static void TDPaintPage(HWND hwnd, HDC hdcWin, TDPageState& s)
{
    if (!s.themesOk) TDRefreshThemes(hwnd, s);
    TDEnsureBrushes(s);

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
    case WM_ERASEBKGND: return 1;

    case WM_PAINT:
    {
        PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
        TDPageState& s = GetTDPageState(hwnd);
        s.isExpanded = !!GetProp(GetParent(hwnd), L"IsExpanded");
        s.elemsOk = false;
        TDUpdateLayoutCache(hwnd, s);
        TDPaintPage(hwnd, hdc, s);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_MOUSEMOVE:
    {
        TDPageState& s = GetTDPageState(hwnd);
        if (!s.tracking) { TRACKMOUSEEVENT tme = { sizeof(tme),TME_LEAVE,hwnd,0 }; TrackMouseEvent(&tme); s.tracking = true; }
        POINT pt = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) };
        int nh = TDHitTest(s.elements, pt);
        if (nh != s.hotIdx) { s.hotIdx = nh; InvalidateRect(hwnd, nullptr, FALSE); }
        break;
    }
    case WM_MOUSELEAVE:
    {
        TDPageState& s = GetTDPageState(hwnd);
        s.tracking = false; s.pressing = false;
        if (s.hotIdx != -1) { s.hotIdx = -1; InvalidateRect(hwnd, nullptr, FALSE); }
        break;
    }
    case WM_LBUTTONDOWN:
        GetTDPageState(hwnd).pressing = true;
        TDUpdateLayoutCache(hwnd, GetTDPageState(hwnd));
        InvalidateRect(hwnd, nullptr, FALSE);
        break;
    case WM_LBUTTONUP:
        GetTDPageState(hwnd).pressing = false;
        TDUpdateLayoutCache(hwnd, GetTDPageState(hwnd));
        InvalidateRect(hwnd, nullptr, FALSE);
        break;
    case WM_THEMECHANGED:
    {
        TDPageState& s = GetTDPageState(hwnd);
        s.CloseThemes(); s.elemsOk = false;
        InvalidateRect(hwnd, nullptr, FALSE);
        break;
    }
    case WM_DESTROY:
        DestroyTDPageState(hwnd);
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
    case WM_CTLCOLORMSGBOX: case WM_CTLCOLOREDIT:   case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORBTN:    case WM_CTLCOLORDLG:    case WM_CTLCOLORSCROLLBAR:
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
        PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
        HTHEME hStyle = OpenThemeData(nullptr, L"TaskDialogStyle");
        HTHEME hBtn = OpenThemeDataForDpi(hwnd, L"BUTTON", GetDpiForWindow(hwnd));
        if (!hBtn) hBtn = OpenThemeData(nullptr, L"Button");

        RECT rcC; GetClientRect(hwnd, &rcC);
        HDC hdcBuf = hdc;
        HPAINTBUFFER hbp = BeginBufferedPaint(hdc, &rcC, BPBF_TOPDOWNDIB, nullptr, &hdcBuf);
        DefSubclassProc(hwnd, WM_PRINTCLIENT, reinterpret_cast<WPARAM>(hdcBuf), PRF_CLIENT);

        wchar_t text[512] = {}; GetWindowTextW(hwnd, text, static_cast<int>(std::size(text)));
        SIZE gs = {}; GetThemePartSize(hBtn, hdcBuf, BP_RADIOBUTTON, RBS_UNCHECKEDNORMAL, &rcC, TS_TRUE, &gs);
        RECT rcT = { gs.cx + 2,0,rcC.right,rcC.bottom };
        DTTOPTS opts = { sizeof(opts) }; opts.dwFlags = DTT_COMPOSITED | DTT_TEXTCOLOR; opts.crText = TDDarkCol::kTextNormal;

        LOGFONT lf = {}; if (SUCCEEDED(GetThemeFont(hStyle, hdcBuf, TDLG_RADIOBUTTONPANE, 0, TMT_FONT, &lf)))
        {
            HFONT hF = CreateFontIndirect(&lf), hO = static_cast<HFONT>(SelectObject(hdcBuf, hF));
            DrawThemeTextEx(hStyle, hdcBuf, TDLG_RADIOBUTTONPANE, 0, text, -1, DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS, &rcT, &opts);
            SelectObject(hdcBuf, hO); DeleteObject(hF);
        }
        if (hStyle) CloseThemeData(hStyle);
        if (hBtn)   CloseThemeData(hBtn);
        if (hbp)    EndBufferedPaint(hbp, TRUE);
        EndPaint(hwnd, &ps); return 0;
    }
    if (msg == WM_DESTROY) RemoveWindowSubclass(hwnd, TDRadioButtonSubclassProc, uId);
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

static void TDApplyToChildren(IUIAutomationElement* pEl, IUIAutomation* pAuto)
{
    const bool native = TDHasNativeDarkTheme();

    CComPtr<IUIAutomationTreeWalker> pW; pAuto->get_ContentViewWalker(&pW);
    if (!pW) return;
    CComPtr<IUIAutomationElement> pChild; pW->GetFirstChildElement(pEl, &pChild);

    while (pChild)
    {
        CONTROLTYPEID ct = 0; pChild->get_CurrentControlType(&ct);
        if (ct == UIA_ButtonControlTypeId || ct == UIA_RadioButtonControlTypeId ||
            ct == UIA_ProgressBarControlTypeId || ct == UIA_HyperlinkControlTypeId ||
            ct == UIA_ScrollBarControlTypeId || ct == UIA_PaneControlTypeId)
        {
            HWND hBtn = nullptr;
            pChild->get_CurrentNativeWindowHandle(reinterpret_cast<UIA_HWND*>(&hBtn));
            if (hBtn)
            {
                CComBSTR bId; pChild->get_CurrentAutomationId(&bId);
                const std::wstring id(bId ? static_cast<LPCWSTR>(bId) : L"");
                HWND hP = GetParent(hBtn);

                if (ct == UIA_ProgressBarControlTypeId)
                {
                    HTHEME hCE = OpenThemeData(nullptr, L"DarkMode_CopyEngine::Progress");
                    HTHEME hBase = OpenThemeData(nullptr, L"Progress");
                    bool hasCE = (hCE && hCE != hBase);
                    if (hCE) CloseThemeData(hCE); if (hBase) CloseThemeData(hBase);
                    SetWindowTheme(hBtn, hasCE ? L"DarkMode_CopyEngine" : L"DarkMode_Explorer", nullptr);
                }
                else if (ct == UIA_RadioButtonControlTypeId || id.find(L"RadioButton_") == 0 || ct == UIA_HyperlinkControlTypeId)
                {
                    if (native) { SetWindowTheme(hBtn, L"DarkMode_DarkTheme", nullptr); }
                    else { DWORD_PTR ex = 0; if (!GetWindowSubclass(hBtn, TDRadioButtonSubclassProc, kTDCtrlSubclassId, &ex)) SetWindowSubclass(hBtn, TDRadioButtonSubclassProc, kTDCtrlSubclassId, 0); }
                    TDSubclassContainer(hP, native ? TDDarkCol::kSecondary : TDDarkCol::kPrimary);
                }
                else if (id.find(L"CommandLink_") == 0 || id.find(L"CommandButton_") == 0)
                {
                    SetWindowTheme(hBtn, L"DarkMode_Explorer", nullptr); TDSubclassContainer(hP, TDDarkCol::kSecondary);
                }
                else
                {
                    SetWindowTheme(hBtn, L"DarkMode_Explorer", nullptr);
                }
            }
        }
        CComPtr<IUIAutomationElement> pNext; pW->GetNextSiblingElement(pChild, &pNext);
        pChild = pNext;
    }
}

static void TDAttach(HWND hwndTD, const TASKDIALOGCONFIG* pCfg)
{
    IUIAutomation* pAuto = GetTDAutomation();
    if (!pAuto) return;
    const bool native = TDHasNativeDarkTheme();

    struct EnumData { HWND hwndTD; const TASKDIALOGCONFIG* pCfg; IUIAutomation* pAuto; bool found; };
    EnumData data = { hwndTD,pCfg,pAuto,false };

    EnumChildWindows(hwndTD, [](HWND hwndChild, LPARAM lp)->BOOL
        {
            EnumData* d = reinterpret_cast<EnumData*>(lp);
            CComPtr<IUIAutomationElement> pEl;
            if (FAILED(d->pAuto->ElementFromHandle(hwndChild, &pEl))) return TRUE;

            CComBSTR cls; pEl->get_CurrentClassName(&cls);

            // SysLink controls (footnote / content hyperlinks)
            if (cls == L"CCSysLink")
            {
                HWND hL = nullptr; pEl->get_CurrentNativeWindowHandle(reinterpret_cast<UIA_HWND*>(&hL));
                if (hL) {
                    CComBSTR bId; pEl->get_CurrentAutomationId(&bId);
                    const std::wstring id(bId ? static_cast<LPCWSTR>(bId) : L"");
                    bool isFn = id.find(L"Footnote") != std::wstring::npos || id.find(L"ExpandedFooter") != std::wstring::npos;
                    TDSubclassContainer(GetParent(hL), (isFn && !TDHasNativeDarkTheme()) ? TDDarkCol::kFootnote : TDDarkCol::kPrimary);
                }
                return TRUE;
            }

            // Main TaskPage (DirectUI "TaskDialog" class)
            if (cls != L"TaskDialog") return TRUE;
            HWND hDUI = nullptr;
            if (FAILED(pEl->get_CurrentNativeWindowHandle(reinterpret_cast<UIA_HWND*>(&hDUI))) || !hDUI) return TRUE;

            // Class background brush
            {
                HBRUSH nb = CreateSolidBrush(TDDarkCol::kSecondary);
                HBRUSH ob = reinterpret_cast<HBRUSH>(SetClassLongPtr(hDUI, GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(nb)));
                if (ob && ob != GetSysColorBrush(COLOR_WINDOW) && ob != GetSysColorBrush(COLOR_BTNFACE)) DeleteObject(ob);
            }

            TDApplyToChildren(pEl, d->pAuto);
            SetWindowTheme(hDUI, L"DarkMode_Explorer", nullptr);

            // Initialise per-page state
            TDPageState& s = GetTDPageState(hDUI);
            s.pCfg = d->pCfg;
            s.defExpanded = d->pCfg && (d->pCfg->dwFlags & TDF_EXPANDED_BY_DEFAULT);
            s.defChecked = d->pCfg && (d->pCfg->dwFlags & TDF_VERIFICATION_FLAG_CHECKED);
            s.isExpanded = !!GetProp(d->hwndTD, L"IsExpanded");
            s.isChecked = s.defChecked || !!GetProp(d->hwndTD, L"IsChecked");
            s.elemsOk = false;
            TDUpdateLayoutCache(hDUI, s);

            DWORD_PTR ex = 0;
            if (!GetWindowSubclass(hDUI, TDPageSubclassProc, kTDPageSubclassId, &ex))
                SetWindowSubclass(hDUI, TDPageSubclassProc, kTDPageSubclassId, 0);

            d->found = true;
            return TRUE;
        }, reinterpret_cast<LPARAM>(&data));

    if (!data.found) return;

    if (native) wxMSWDarkMode::AllowForWindow(hwndTD, L"DarkMode_Explorer", nullptr);
    DWORD_PTR ex;
    if (!GetWindowSubclass(hwndTD, TDCtrlContainerSubclassProc, kTDMainSubclassId, &ex))
        SetWindowSubclass(hwndTD, TDCtrlContainerSubclassProc, kTDMainSubclassId, 0);
    // Dark title bar
    wxMSWDarkMode::EnableForTLW(hwndTD);
    HBRUSH nb = CreateSolidBrush(TDDarkCol::kPrimary);
    SetClassLongPtr(hwndTD, GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(nb));
    EnumChildWindows(hwndTD, [](HWND h, LPARAM)->BOOL { SendMessage(h, WM_SYSCOLORCHANGE, 0, 0); return TRUE; }, 0);
    SendMessage(hwndTD, WM_THEMECHANGED, 0, 0);
}

static void TDDetach(HWND hwndTD)
{
    EnumChildWindows(hwndTD, [](HWND hChild, LPARAM)->BOOL
        {
            DWORD_PTR ex = 0;
            if (GetWindowSubclass(hChild, TDPageSubclassProc, kTDPageSubclassId, &ex))
            {
                RemoveWindowSubclass(hChild, TDPageSubclassProc, kTDPageSubclassId); DestroyTDPageState(hChild);
            }
            if (GetWindowSubclass(hChild, TDCtrlContainerSubclassProc, kTDCtrlSubclassId, &ex))
                RemoveWindowSubclass(hChild, TDCtrlContainerSubclassProc, kTDCtrlSubclassId);
            if (GetWindowSubclass(hChild, TDRadioButtonSubclassProc, kTDCtrlSubclassId, &ex))
                RemoveWindowSubclass(hChild, TDRadioButtonSubclassProc, kTDCtrlSubclassId);
            SetWindowTheme(hChild, nullptr, nullptr);
            return TRUE;
        }, 0);

} // anonymous namespace


// ============================================================================
// implementation
// ============================================================================

namespace wxMSWImpl
{

// Global pointers of the functions we use: they're not only undocumented, but
// don't appear in the SDK headers at all.
//
// Note that, not being public, they use C++ bool type and not Win32 BOOL.
bool (WINAPI *ShouldAppsUseDarkMode)() = nullptr;
bool (WINAPI *AllowDarkModeForWindow)(HWND hwnd, bool allow) = nullptr;
DWORD (WINAPI *SetPreferredAppMode)(DWORD) = nullptr;

bool InitDarkMode()
{
    // In theory, dark mode support was added in v1809 (build 17763), so enable
    // it for all later versions, even though in practice this code has been
    // mostly tested under v2004 ("20H1", build number 19041) and later ones.
    if ( !wxCheckOsVersion(10, 0, 17763) )
    {
        wxLogTrace(TRACE_DARKMODE, "Unsupported due to OS version");
        return false;
    }

    wxLoadedDLL dllUxTheme(wxS("uxtheme.dll"));

    // These functions are not only undocumented but are not even exported by
    // name, and have to be resolved using their ordinals.
    return TryLoadByOrd(ShouldAppsUseDarkMode, dllUxTheme, 132) &&
           TryLoadByOrd(AllowDarkModeForWindow, dllUxTheme, 133) &&
           TryLoadByOrd(SetPreferredAppMode, dllUxTheme, 135);
}

// This function is only used in this file as it's more clear than using
// IsActive() without the namespace name -- but in the rest of our code, it's
// IsActive() which is more clear.
bool ShouldUseDarkMode()
{
    switch ( gs_appMode )
    {
        case AppMode_Default:
            // Dark mode support not enabled, don't try using dark mode.
            return false;

        case AppMode_AllowDark:
            // Follow the global setting.
            return wxMSWImpl::ShouldAppsUseDarkMode();

        case AppMode_ForceDark:
            return true;

        case AppMode_ForceLight:
            return false;
    }

    wxFAIL_MSG( "unreachable" );

    return false;
}

} // namespace wxMSWImpl

// ----------------------------------------------------------------------------
// Public API
// ----------------------------------------------------------------------------

bool wxApp::MSWEnableDarkMode(int flags, wxDarkModeSettings* settings)
{
    if ( !wxMSWImpl::InitDarkMode() )
        return false;

    const PreferredAppMode mode = flags & DarkMode_Always ? AppMode_ForceDark
                                                          : AppMode_AllowDark;
    const DWORD rc = wxMSWImpl::SetPreferredAppMode(mode);

    // It's supposed to return the old mode normally.
    if ( rc != static_cast<DWORD>(gs_appMode) )
    {
        wxLogTrace(TRACE_DARKMODE,
                   "SetPreferredAppMode(%d) unexpectedly returned %d",
                   mode, rc);
    }

    gs_appMode = mode;

    // Set up the settings to use, allocating a default one if none specified.
    if ( !settings )
        settings = new wxDarkModeSettings();

    wxDarkModeModule::SetSettings(settings);

    return true;
}

wxApp::AppearanceResult wxApp::SetAppearance(Appearance appearance)
{
    // We currently can't change the appearance of the existing windows because
    // we initialize/create them differently depending on the mode value in a
    // lot of places, so don't even try as we risk finishing with a horrible
    // mix of light and dark mode elements.
    if ( !wxTopLevelWindows.empty() || gs_appMode != AppMode_Default )
        return AppearanceResult::CannotChange;

    int flags = 0;
    switch ( appearance )
    {
        case Appearance::System:
            flags = DarkMode_Auto;
            break;

        case Appearance::Light:
            // Nothing to do, this is the default.
            return AppearanceResult::Ok;

        case Appearance::Dark:
            flags = DarkMode_Always;
            break;
    }

    // Do (try to) change it.
    return MSWEnableDarkMode(flags) ? AppearanceResult::Ok
                                    : AppearanceResult::Failure;
}

// ----------------------------------------------------------------------------
// Default wxDarkModeSettings implementation
// ----------------------------------------------------------------------------

// Implemented here to ensure that it's generated inside the DLL.
wxDarkModeSettings::~wxDarkModeSettings() = default;

wxColour wxDarkModeSettings::GetColour(wxSystemColour index)
{
    // This is not great at all, but better than using light mode colours that
    // are not appropriate for the dark mode.
    //
    // There is also an undocumented GetImmersiveColorFromColorSetEx(), but it
    // doesn't seem to return any colours with the values that are actually
    // used in e.g. Explorer in the dark mode, such as 0x202020 background.
    switch ( index )
    {
        case wxSYS_COLOUR_BTNSHADOW:
            return *wxBLACK;

        case wxSYS_COLOUR_ACTIVECAPTION:
        case wxSYS_COLOUR_APPWORKSPACE:
        case wxSYS_COLOUR_INFOBK:
        case wxSYS_COLOUR_LISTBOX:
        case wxSYS_COLOUR_WINDOW:
            return wxColour(0x202020);

        case wxSYS_COLOUR_BTNTEXT:
        case wxSYS_COLOUR_CAPTIONTEXT:
        case wxSYS_COLOUR_HIGHLIGHTTEXT:
        case wxSYS_COLOUR_INFOTEXT:
        case wxSYS_COLOUR_LISTBOXHIGHLIGHTTEXT:
        case wxSYS_COLOUR_LISTBOXTEXT:
        case wxSYS_COLOUR_MENUTEXT:
        case wxSYS_COLOUR_WINDOWTEXT:
            return wxColour(0xe0e0e0);

        case wxSYS_COLOUR_HOTLIGHT:
            return wxColour(0xe48435);

        case wxSYS_COLOUR_SCROLLBAR:
            return wxColour(0x4d4d4d);

        case wxSYS_COLOUR_INACTIVECAPTION:
        case wxSYS_COLOUR_MENU:
            return wxColour(0x2b2b2b);

        case wxSYS_COLOUR_GRIDLINES:
        case wxSYS_COLOUR_BTNFACE:
            return wxColour(0x333333);

        case wxSYS_COLOUR_MENUBAR:
        case wxSYS_COLOUR_LISTBOXHIGHLIGHT:
            return wxColour(0x626262);

        case wxSYS_COLOUR_HIGHLIGHT:
        case wxSYS_COLOUR_MENUHILIGHT:
            return wxColour(0x9e5315);

        case wxSYS_COLOUR_BTNHIGHLIGHT:
            return wxColour(0x777777);

        case wxSYS_COLOUR_INACTIVECAPTIONTEXT:
            return wxColour(0xaaaaaa);

        case wxSYS_COLOUR_3DDKSHADOW:
        case wxSYS_COLOUR_3DLIGHT:
        case wxSYS_COLOUR_ACTIVEBORDER:
        case wxSYS_COLOUR_DESKTOP:
        case wxSYS_COLOUR_GRADIENTACTIVECAPTION:
        case wxSYS_COLOUR_GRADIENTINACTIVECAPTION:
        case wxSYS_COLOUR_GRAYTEXT:
        case wxSYS_COLOUR_INACTIVEBORDER:
        case wxSYS_COLOUR_WINDOWFRAME:
            return wxColour();

        case wxSYS_COLOUR_MAX:
            break;
    }

    wxFAIL_MSG( "unreachable" );
    return wxColour();
}

wxColour wxDarkModeSettings::GetMenuColour(wxMenuColour which)
{
    switch ( which )
    {
        case wxMenuColour::StandardFg:
            return wxColour(0xffffff);

        case wxMenuColour::StandardBg:
            return GetColour(wxSYS_COLOUR_MENU);

        case wxMenuColour::DisabledFg:
            return wxColour(0x6d6d6d);

        case wxMenuColour::HotBg:
            return wxColour(0x414141);
    }

    wxFAIL_MSG( "unreachable" );
    return wxColour();
}

wxPen wxDarkModeSettings::GetBorderPen()
{
    // Use a darker pen than the default white one by default. There doesn't
    // seem to be any standard colour to use for it, Windows itself uses both
    // 0x666666 and 0x797979 for the borders in the "Colours" control panel
    // window, so it doesn't seem like anybody cares about consistency here.
    return *wxGREY_PEN;
}

// ----------------------------------------------------------------------------
// Supporting functions for the rest of wxMSW code
// ----------------------------------------------------------------------------

namespace wxMSWDarkMode
{

bool IsActive()
{
    return wxMSWImpl::ShouldUseDarkMode();
}
// ----------------------------------------------------------------------------
// TaskDialog dark mode — public entry points
// ----------------------------------------------------------------------------

void AllowForTaskDialog(HWND hwnd, const TASKDIALOGCONFIG* pCfg)
{
    if (!wxMSWImpl::ShouldUseDarkMode())
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
}

void RemoveFromTaskDialog(HWND hwnd)
{
    TDDetach(hwnd);
}
void EnableForTLW(HWND hwnd)
{
    // Nothing to do, dark mode support not enabled or dark mode is not used.
    if ( !wxMSWImpl::ShouldUseDarkMode() )
        return;

    BOOL useDarkMode = TRUE;

    // DWMWA_USE_IMMERSIVE_DARK_MODE is 19 for v1809, but is 20 for later
    // versions, so to set title bar black for both v1809 and later versions,
    // we try to call GetDwmSetWindowAttribute() with the current value first,
    // but if it fails, we also retry with the old one.
    HRESULT hr = wxDarkModeModule::GetDwmSetWindowAttribute()
                 (
                    hwnd,
                    DWMWA_USE_IMMERSIVE_DARK_MODE,
                    &useDarkMode,
                    sizeof(useDarkMode)
                 );
    if ( FAILED(hr) )
    {
        hr = wxDarkModeModule::GetDwmSetWindowAttribute()
             (
                hwnd,
                19,
                &useDarkMode,
                sizeof(useDarkMode)
             );
    }
    if ( FAILED(hr) )
        wxLogApiError("DwmSetWindowAttribute(USE_IMMERSIVE_DARK_MODE)", hr);

    wxMSWImpl::AllowDarkModeForWindow(hwnd, true);
}

void AllowForWindow(HWND hwnd, const wchar_t* themeName, const wchar_t* themeId)
{
    if ( !wxMSWImpl::ShouldUseDarkMode() )
        return;

    if ( wxMSWImpl::AllowDarkModeForWindow(hwnd, true) )
        wxLogTrace(TRACE_DARKMODE, "Allow dark mode for %p failed", hwnd);

    if ( themeName || themeId )
    {
        HRESULT hr = ::SetWindowTheme(hwnd, themeName, themeId);
        if ( FAILED(hr) )
        {
            wxLogApiError(wxString::Format("SetWindowTheme(%p, %s, %s)",
                                           hwnd, themeName, themeId), hr);
        }
    }
}

wxColour GetColour(wxSystemColour index)
{
    return wxDarkModeModule::GetSettings().GetColour(index);
}

wxPen GetBorderPen()
{
    return wxDarkModeModule::GetSettings().GetBorderPen();
}

HBRUSH GetBackgroundBrush()
{
    wxBrush* const brush =
        wxTheBrushList->FindOrCreateBrush(GetColour(wxSYS_COLOUR_WINDOW));

    return brush ? GetHbrushOf(*brush) : 0;
}

#if wxUSE_IMAGE

static void
InvertBitmapPixel(unsigned char& r, unsigned char& g, unsigned char& b,
                  unsigned char& WXUNUSED(a))
{
    wxImage::RGBValue rgb(r, g, b);
    wxImage::HSVValue hsv = wxImage::RGBtoHSV(rgb);

    // There is no really good way to convert normal colours to dark mode,
    // but try to do a bit better than just inverting the value because
    // this results in colours which are much too dark.
    hsv.value = sqrt(1.0 - hsv.value*hsv.value);

    rgb = wxImage::HSVtoRGB(hsv);

    r = rgb.red;
    g = rgb.green;
    b = rgb.blue;
}

#endif // wxUSE_IMAGE

wxBitmap InvertBitmap(const wxBitmap& bmp)
{
#if wxUSE_IMAGE
    return wxMSWImpl::PostPaintEachPixel(bmp, InvertBitmapPixel);
#else // !wxUSE_IMAGE
    return bmp;
#endif // wxUSE_IMAGE/!wxUSE_IMAGE
}

bool PaintIfNecessary(HWND hwnd, WXWNDPROC defWndProc)
{
#if wxUSE_IMAGE
    if ( !wxMSWImpl::ShouldUseDarkMode() )
        return false;

    wxMSWImpl::CustomPaint
    (
        hwnd,
        [defWndProc](HWND hwnd, WPARAM wParam)
        {
            if ( defWndProc )
                ::CallWindowProc(defWndProc, hwnd, WM_PAINT, wParam, 0);
            else
                ::DefWindowProc(hwnd, WM_PAINT, wParam, 0);
        },
        InvertBitmap
    );

    return true;
#else // !wxUSE_IMAGE
    wxUnusedVar(hwnd);
    wxUnusedVar(defWndProc);

    return false;
#endif
}

// ----------------------------------------------------------------------------
// Menu bar custom drawing
// ----------------------------------------------------------------------------

namespace wxMSWMenuImpl
{

using namespace ::wxMSWMenuImpl;

wxColour GetMenuColour(wxMenuColour which)
{
    return wxDarkModeModule::GetSettings().GetMenuColour(which);
}

HBRUSH GetMenuBrush(wxMenuColour which = wxMenuColour::StandardBg)
{
    wxBrush* const brush =
        wxTheBrushList->FindOrCreateBrush(GetMenuColour(which));

    return brush ? GetHbrushOf(*brush) : 0;
}

} // namespace wxMSWMenuImpl

bool
HandleMenuMessage(WXLRESULT* result,
                  wxWindow* w,
                  WXUINT nMsg,
                  WXWPARAM wParam,
                  WXLPARAM lParam)
{
    if ( !wxMSWImpl::ShouldUseDarkMode() )
        return false;

    using namespace wxMSWMenuImpl;

    switch ( nMsg )
    {
        case WM_MENUBAR_INITMENU:
            // Round corners are only supported in Windows 11.
            if ( wxGetWinVersion() < wxWinVersion_11 )
                break;

            // Menu theme is turned off in high contrast mode.
            if ( wxMSWImpl::IsHighContrast() )
                break;

            // Enable rounded corners for UAH menus
            if ( auto* const pUahMenu = (MenuBarDrawMenu*)lParam )
            {
                // This field is not documented but seems to contain flags and
                // these bits are set for the menu initialization.
                if ( pUahMenu->dwReserved & 0x4000001 )
                {
                    if ( HWND hWndMenu = ::WindowFromDC(pUahMenu->hdc) )
                        wxMSWImpl::EnableRoundCorners(hWndMenu);
                }
            }
            break;

        case WM_MENUBAR_DRAWMENU:
            // Erase the menu bar background using custom brush.
            if ( auto* const drawMenu = (MenuBarDrawMenu*)lParam )
            {
                HWND hwnd = GetHwndOf(w);

                WinStruct<MENUBARINFO> mbi;
                if ( !::GetMenuBarInfo(hwnd, OBJID_MENU, 0, &mbi) )
                {
                    wxLogLastError("GetMenuBarInfo");
                    break;
                }

                const RECT rcWindow = wxGetWindowRect(hwnd);

                // rcBar is expressed in screen coordinates.
                ::OffsetRect(&mbi.rcBar, -rcWindow.left, -rcWindow.top);

                ::FillRect(drawMenu->hdc, &mbi.rcBar, GetMenuBrush());
            }

            *result = 0;
            return true;

        case WM_NCPAINT:
        case WM_NCACTIVATE:
            // Drawing the menu bar background in WM_MENUBAR_DRAWMENU somehow
            // leaves a single pixel line unpainted (and increasing the size of
            // the rectangle doesn't help, i.e. drawing is clipped to an area
            // which is one pixel too small), so we have to draw over it here
            // to get rid of it.
            {
                *result = w->MSWDefWindowProc(nMsg, wParam, lParam);

                HWND hwnd = GetHwndOf(w);
                WindowHDC hdc(hwnd);

                // Create a RECT one pixel above the client area: note that we
                // have to use window (and not client) coordinates for this as
                // this is outside of the client area of the window.
                const RECT rcWindow = wxGetWindowRect(hwnd);
                RECT rc = wxGetClientRect(hwnd);

                // Convert client coordinates to window ones.
                wxMapWindowPoints(hwnd, HWND_DESKTOP, &rc);
                ::OffsetRect(&rc, -rcWindow.left, -rcWindow.top);

                rc.bottom = rc.top;
                rc.top--;

                ::FillRect(hdc, &rc, GetMenuBrush());
            }
            return true;

        case WM_MENUBAR_DRAWMENUITEM:
            if ( auto* const drawMenuItem = (MenuBarDrawMenuItem*)lParam )
            {
                const DRAWITEMSTRUCT& dis = drawMenuItem->dis;

                // Just a sanity check.
                if ( dis.CtlType != ODT_MENU )
                    break;

                wchar_t buf[256];
                WinStruct<MENUITEMINFO> mii;
                mii.fMask = MIIM_STRING;
                mii.dwTypeData = buf;
                mii.cch = sizeof(buf) - 1;

                // Note that we need to use the iPosition field of the
                // undocumented struct here because DRAWITEMSTRUCT::itemID is
                // not initialized in the struct passed to us here, so this is
                // the only way to identify the item we're dealing with.
                if ( !::GetMenuItemInfo((HMENU)dis.hwndItem,
                                        drawMenuItem->mbmi.iPosition,
                                        TRUE,
                                        &mii) )
                    break;

                const UINT itemState = dis.itemState;

                HBRUSH hbr = 0;
                int partState = 0;
                wxMenuColour colText = wxMenuColour::StandardFg;
                if ( itemState & ODS_INACTIVE )
                {
                    partState = MBI_DISABLED;
                    colText = wxMenuColour::DisabledFg;
                }
                else if ( (itemState & ODS_GRAYED) && (itemState & ODS_HOTLIGHT) )
                {
                    partState = MBI_DISABLEDHOT;
                }
                else if ( itemState & ODS_GRAYED )
                {
                    partState = MBI_DISABLED;
                    colText = wxMenuColour::DisabledFg;
                }
                else if ( itemState & (ODS_HOTLIGHT | ODS_SELECTED) )
                {
                    partState = MBI_HOT;

                    hbr = GetMenuBrush(wxMenuColour::HotBg);
                }
                else
                {
                    partState = MBI_NORMAL;
                }

                RECT* const rcItem = const_cast<RECT*>(&dis.rcItem);

                // Don't use DrawThemeBackground() here, as it doesn't use the
                // correct colours in the dark mode, at least not when using
                // the "Menu" theme.
                ::FillRect(dis.hDC, &dis.rcItem, hbr ? hbr : GetMenuBrush());

                // We have to specify the text colour explicitly as by default
                // black would be used, making the menu label unreadable on the
                // (almost) black background.
                DTTOPTS textOpts;
                textOpts.dwSize = sizeof(textOpts);
                textOpts.dwFlags = DTT_TEXTCOLOR;
                textOpts.crText = wxColourToRGB(GetMenuColour(colText));

                DWORD drawTextFlags = DT_CENTER | DT_SINGLELINE | DT_VCENTER;
                if ( itemState & ODS_NOACCEL)
                    drawTextFlags |= DT_HIDEPREFIX;

                wxUxThemeHandle menuTheme(w, L"Menu");
                ::DrawThemeTextEx(menuTheme, dis.hDC, MENU_BARITEM, partState,
                                  buf, mii.cch, drawTextFlags, rcItem,
                                  &textOpts);
            }
            return true;
    }

    return false;
}

} // namespace wxMSWDarkMode

#else // !wxUSE_DARK_MODE

bool
wxApp::MSWEnableDarkMode(int WXUNUSED(flags),
                         wxDarkModeSettings* WXUNUSED(settings))
{
    return false;
}

wxApp::AppearanceResult wxApp::SetAppearance(Appearance WXUNUSED(appearance))
{
    return AppearanceResult::Failure;
}

namespace wxMSWDarkMode
{

bool IsActive()
{
    return false;
}

void EnableForTLW(HWND WXUNUSED(hwnd))
{
}

void AllowForWindow(HWND WXUNUSED(hwnd), const wchar_t* WXUNUSED(themeClass))
{
}

wxColour GetColour(wxSystemColour WXUNUSED(index))
{
    return wxColour();
}

wxPen GetBorderPen()
{
    return wxPen{};
}

HBRUSH GetBackgroundBrush()
{
    return 0;
}

wxBitmap InvertBitmap(const wxBitmap& WXUNUSED(bmp))
{
    return wxBitmap();
}

bool PaintIfNecessary(HWND WXUNUSED(hwnd), WXWNDPROC WXUNUSED(defWndProc))
{
    return false;
}

bool
HandleMenuMessage(WXLRESULT* WXUNUSED(result),
                  wxWindow* WXUNUSED(w),
                  WXUINT WXUNUSED(nMsg),
                  WXWPARAM WXUNUSED(wParam),
                  WXLPARAM WXUNUSED(lParam))
{
    return false;
}

} // namespace wxMSWDarkMode

#endif // wxUSE_DARK_MODE/!wxUSE_DARK_MODE

void wxMSWImpl::EnableRoundCorners(HWND hwnd)
{
    const auto dwmSetWinAttr = wxDarkModeModule::GetDwmSetWindowAttribute();
    if ( !dwmSetWinAttr )
        return;

    constexpr DWORD DWMWA_WINDOW_CORNER_PREFERENCE = 33;
    constexpr DWORD DWMWA_BORDER_COLOR = 34;
    constexpr int WindowCornerPreference = 3; // DWMWCP_ROUNDSMALL

    // Apply rounded corners
    HRESULT hr = dwmSetWinAttr(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE,
                               &WindowCornerPreference,
                               sizeof(WindowCornerPreference));

    if ( FAILED(hr) )
        return;

    // Clean up conflicting styles like drop shadows.
    DWORD style = ::GetClassLongPtr(hwnd, GCL_STYLE);
    if ( style & CS_DROPSHADOW )
    {
        style &= ~CS_DROPSHADOW;
        ::SetClassLongPtr(hwnd, GCL_STYLE, style);
    }

    // Determine the appropriate border colour for the current theme.
    auto hTheme = wxUxThemeHandle::NewAtStdDPI
        (
            hwnd,
            L"LightMode_ImmersiveStart::Menu;MENU",
            L"DarkMode_ImmersiveStart::Menu;DarkMode::Menu;MENU"
        );
    wxColour colBorder = hTheme.GetColour(MENU_POPUPBORDERS, TMT_FILLCOLORHINT);
    if ( !colBorder.IsOk() )
        colBorder = wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVEBORDER);

    DWORD color = static_cast<DWORD>(wxColourToRGB(colBorder));
    dwmSetWinAttr(hwnd, DWMWA_BORDER_COLOR, &color, sizeof(color));
}
