///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/taskdlg.cpp
// Purpose:     Dark mode support for native TaskDialog.
// Author:      Mohmed Abdel-Fattah (memoarfaa)
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
    #ifdef __WXUNIVERSAL__
        #define wxUSE_DARK_MODE 0
    #else
        #define wxUSE_DARK_MODE 1
    #endif
#endif

#ifndef WX_PRECOMP
    #include "wx/brush.h"
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
#include <windowsx.h>

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

// Small helper class freeing BSTR automatically if necessary.
class AutoBSTR
{
public:
    AutoBSTR() = default;

    AutoBSTR(const AutoBSTR&) = delete;
    AutoBSTR& operator=(const AutoBSTR&) = delete;

    ~AutoBSTR()
    {
        if ( m_bstr )
            ::SysFreeString(m_bstr);
    }

    operator const wchar_t*() const
    {
        return m_bstr;
    }

    // May be called once to fill in the BSTR, which will be freed in dtor.
    BSTR* Out()
    {
        wxASSERT_MSG( !m_bstr, "Can't reuse same object" );

        return &m_bstr;
    }

private:
    BSTR m_bstr = nullptr;
};

// Helper function create an HBRUSH from a brush stored in wxTheBrushList.
// This allows not to recreate the brushes for the same colour and also ensures
// that the brushes are eventually deleted.
HBRUSH GetSolidBrush(COLORREF rgb)
{
    const wxColour col = wxRGBToColour(rgb);
    wxBrush* const brush = wxTheBrushList->FindOrCreateBrush(col);
    if ( !brush )
        return 0;

    return GetHbrushOf(*brush);
}

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

// Another helper to get the automation ID of an element.
std::wstring GetCurrentAutomationId(IUIAutomationElement* element)
{
    std::wstring result;

    AutoBSTR bstr;
    if ( element->get_CurrentAutomationId(bstr.Out()) == S_OK && bstr )
        result = bstr;

    return result;
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
    wxUxThemeHandle hButton ; // Button (checkbox glyph)
    bool themesOk = false;

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


// Subclass IDs: 0x7778 == "wx"
static constexpr UINT_PTR kTDMainSubclassId = 0x77780010ul;
static constexpr UINT_PTR kTDPageSubclassId = 0x77780011ul;
static constexpr UINT_PTR kTDCtrlSubclassId = 0x77780012ul;

// ============================================================================
// TaskDialog theme helpers
// ============================================================================

// Check if the theme with the "dark" name exists and is different from the
// standard one.
bool
wxHasRealDarkTheme(const wchar_t* stdClass, const wchar_t* darkClass)
{
    const auto darkTheme = wxUxThemeHandle::NewAtStdDPI(darkClass);
    if ( darkTheme )
    {
        const auto stdTheme = wxUxThemeHandle::NewAtStdDPI(stdClass);
        if ( stdTheme && stdTheme != darkTheme )
            return true;
    }

    return false;
}

// Cached probe: does the OS have a native dark TaskDialog theme (Win11+)?
bool TDHasNativeDarkTheme()
{
    static int s_hasDarkTheme = -1;
    if ( s_hasDarkTheme == -1 )
    {
        s_hasDarkTheme = wxHasRealDarkTheme(L"TaskDialog",
                                            L"DarkMode_DarkTheme::TaskDialog");
    }

    return s_hasDarkTheme == 1;
}

void TDRefreshThemes(HWND hwnd, TDPageState& s)
{
    const int dpi = wxGetWindowDPI(hwnd).x;

    if ( TDHasNativeDarkTheme() )
    {
        const wchar_t* mainClass = L"DarkMode_Explorer::TaskDialog";
        const wchar_t* btnClass = L"DarkMode_Explorer::Button";

        s.hTD = wxUxThemeHandle::NewAtDPI(hwnd, mainClass, mainClass, dpi);
        s.hButton = wxUxThemeHandle::NewAtDPI(hwnd, btnClass, btnClass, dpi);
    }
    else // Try the best we can with the themes available on older OS versions.
    {
        s.hTD = wxUxThemeHandle::NewAtDPI(hwnd, L"TaskDialog", dpi);
        s.hButton = wxUxThemeHandle::NewAtDPI(hwnd, L"Button", dpi);
    }

    s.themesOk = true;
}

COLORREF TDGetTextColour(const TDPageState& s, int uiPart)
{
    if ( TDHasNativeDarkTheme() )
    {
        const wxColour col = s.hTD.GetColour(uiPart, TMT_TEXTCOLOR);
        if ( col.IsOk() )
            return wxColourToRGB(col);
    }

    switch ( uiPart )
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

HICON TDLoadStockIcon(const TASKDIALOGCONFIG* cfg, bool isMain)
{
    if ( !cfg )
        return nullptr;

    if ( isMain )
    {
        if ( cfg->dwFlags & TDF_USE_HICON_MAIN )
            return cfg->hMainIcon;
    }
    else
    {
        if ( cfg->dwFlags & TDF_USE_HICON_FOOTER )
            return cfg->hFooterIcon;
    }

    SHSTOCKICONID id;

    const LPCWSTR res = isMain ? cfg->pszMainIcon : cfg->pszFooterIcon;
    if ( res == TD_WARNING_ICON )
        id = SIID_WARNING;
    else if ( res == TD_ERROR_ICON )
        id = SIID_ERROR;
    else if ( res == TD_INFORMATION_ICON )
        id = SIID_INFO;
    else if ( res == TD_SHIELD_ICON )
        id = SIID_SHIELD;
    else
         return nullptr;

    WinStruct<SHSTOCKICONINFO> sii;
    if ( ::SHGetStockIconInfo(id, SHGSI_ICON | SHGSI_LARGEICON, &sii) != S_OK )
        return nullptr;

    return sii.hIcon;
}

// ============================================================================
// UIA layout cache
// ============================================================================

void TDBuildLayoutCache(HWND hwnd, std::vector<TDLayoutElement>& out)
{
    out.clear();
    IUIAutomation* const pAuto = wxTaskDialogDarkModule::GetUIAutomation();
    if ( !pAuto )
        return;

    wxCOMPtr <IUIAutomationElement> pRoot;
    if ( FAILED(pAuto->ElementFromHandle(hwnd, &pRoot)) )
        return;

    wxCOMPtr<IUIAutomationTreeWalker> pWalker;
    pAuto->get_ContentViewWalker(&pWalker);
    if ( !pWalker )
        return;

    wxCOMPtr<IUIAutomationElement> pChild;
    pWalker->GetFirstChildElement(pRoot, &pChild);

    while ( pChild )
    {
        TDLayoutElement info;
        pChild->get_CurrentBoundingRectangle(&info.rect);
        ::ScreenToClient(hwnd, reinterpret_cast<POINT*>(&info.rect.left));
        ::ScreenToClient(hwnd, reinterpret_cast<POINT*>(&info.rect.right));

        info.automationId = GetCurrentAutomationId(pChild);

        AutoBSTR b;
        if ( pChild->get_CurrentName(b.Out()) == S_OK && b )
            info.name = b;

        if ( info.automationId == L"VerificationCheckBox" )
        {
            VARIANT v;
            ::VariantInit(&v);

            if ( SUCCEEDED(pChild->GetCurrentPropertyValue
                                   (
                                    UIA_LegacyIAccessibleStatePropertyId,
                                    &v
                                   )) && v.vt == VT_I4 )
            {
                info.legacyState = v.lVal;
            }

            ::VariantClear(&v);
        }

        if ( !info.automationId.empty() && !::IsRectEmpty(&info.rect) )
        {
            const std::wstring& id = info.automationId;
            if ( id == L"MainIcon" ||
                 id == L"FootnoteIcon" ||
                 id == L"MainInstruction" ||
                 id == L"ContentText" ||
                 id == L"ExpandedFooterText" ||
                 id == L"FootnoteText" ||
                 id == L"ExpandoButton" ||
                 id == L"VerificationCheckBox" ||
                 id.find(L"RadioButton_") == 0 ||
                 id.find(L"CommandLink_") == 0 ||
                 id.find(L"CommandButton_") == 0 )
            {
                out.push_back(std::move(info));
            }
        }

        wxCOMPtr<IUIAutomationElement> pNext;
        pWalker->GetNextSiblingElement(pChild, &pNext);
        pChild = pNext;
    }
}

void TDUpdateLayoutCache(HWND hwnd, TDPageState& s)
{
    if ( !s.elemsOk )
    {
        TDBuildLayoutCache(hwnd, s.elements);
        s.elemsOk = true;
    }

    for ( const auto& el : s.elements )
    {
        if ( el.automationId == L"VerificationCheckBox" )
        {
            s.isChecked = (el.legacyState & STATE_SYSTEM_CHECKED) != 0;
            break;
        }
    }

    if ( s.defChecked )
        s.isChecked = true;
}

int TDHitTest(const std::vector<TDLayoutElement>& els, POINT pt)
{
    for ( int i = 0; i < wxSsize(els); ++i )
    {
        if ( ::PtInRect(&els[i].rect, pt) )
            return i;
    }

    return -1;
}

// ============================================================================
// Paint routines
// ============================================================================

void TDPaintPixelSwap(HPAINTBUFFER hbp, int w, int h)
{
    RGBQUAD* pPx = nullptr;
    int rw = 0;
    if ( FAILED(::GetBufferedPaintBits(hbp, &pPx, &rw)) )
        return;

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

    for ( int y = 0; y < h; ++y )
    {
        RGBQUAD* row = pPx + static_cast<ptrdiff_t>(y) * rw;
        for ( int x = 0; x < w; ++x )
        {
            RGBQUAD& p = row[x];
            for ( const Rule& r : rules )
            {
                if ( p.rgbRed == r.sR && p.rgbGreen == r.sG && p.rgbBlue == r.sB )
                {
                    p.rgbRed = r.dR;
                    p.rgbGreen = r.dG;
                    p.rgbBlue = r.dB;
                    p.rgbReserved = 55;
                    break;
                }
            }
        }
    }
}

void TDPaintIcons(HDC hdc, const TDPageState& s)
{
    if ( !s.pCfg || TDHasNativeDarkTheme() )
        return;

    for ( const auto& el : s.elements )
    {
        if ( ::IsRectEmpty(&el.rect) )
            continue;

        HICON hIcon = nullptr;
        HBRUSH brBg = nullptr;
        if ( el.automationId == L"MainIcon" )
        {
            hIcon = TDLoadStockIcon(s.pCfg, true);
            brBg = s.brPrimary;
        }
        else if ( el.automationId == L"FootnoteIcon" )
        {
            hIcon = TDLoadStockIcon(s.pCfg, false);
            brBg = s.brFootnote;
        }

        if ( !hIcon )
            continue;

        ::FillRect(hdc, &el.rect, brBg);
        ::DrawIconEx
        (
            hdc,
            el.rect.left,
            el.rect.top,
            hIcon,
            el.rect.right - el.rect.left,
            el.rect.bottom - el.rect.top,
            0,
            nullptr,
            DI_NORMAL
        );

        if ( ::DestroyIcon(hIcon) == 0 )
        {
            wxLogLastError("DestroyIcon");
        }
    }
}

void TDPaintGlyphs(HDC hdc, TDPageState& s)
{
    if ( !s.hTD && !s.hButton )
        return;

    if ( TDHasNativeDarkTheme() )
        return;

    for ( int i = 0; i < wxSsize(s.elements); ++i )
    {
        const TDLayoutElement& el = s.elements[i];
        if ( ::IsRectEmpty(&el.rect) )
            continue;

        const bool hot = i == s.hotIdx;
        const bool press = hot && s.pressing;

        if ( el.automationId == L"ExpandoButton" && s.hTD )
        {
            const int width =
                s.hTD.GetTrueSize(TDLG_EXPANDOBUTTON, TDLGEBS_NORMAL, hdc).x;

            RECT rc = el.rect;
            rc.right = el.rect.left + width + 3;

            int state;
            if ( press )
                state = s.isExpanded ? TDLGEBS_EXPANDEDPRESSED : TDLGEBS_PRESSED;
            else if ( hot )
                state = s.isExpanded ? TDLGEBS_EXPANDEDHOVER : TDLGEBS_HOVER;
            else
                state = s.isExpanded ? TDLGEBS_EXPANDEDNORMAL : TDLGEBS_NORMAL;

            ::FillRect(hdc, &rc, s.brSecondary);
            s.hTD.DrawBackground(hdc, rc, TDLG_EXPANDOBUTTON, state, &el.rect);
        }
        else if ( el.automationId == L"VerificationCheckBox" && s.hButton )
        {
            const wxSize size =
                s.hButton.GetDrawSize(BP_CHECKBOX, CBS_UNCHECKEDNORMAL, hdc);

            const int mg = (el.rect.bottom - el.rect.top - size.y) / 3;
            RECT rc = { el.rect.left + mg + 1,el.rect.top + mg + 1,el.rect.left + mg + 1 + size.x,el.rect.bottom };

            int state;
            if ( press )
                state = s.isChecked ? CBS_CHECKEDPRESSED : CBS_UNCHECKEDPRESSED;
            else if ( hot )
                state = s.isChecked ? CBS_CHECKEDHOT : CBS_UNCHECKEDHOT;
            else
                state = s.isChecked ? CBS_CHECKEDNORMAL : CBS_UNCHECKEDNORMAL;

            ::FillRect(hdc, &rc, s.brSecondary);
            s.hButton.DrawBackground(hdc, rc, BP_CHECKBOX, state);
        }
    }
}

void TDPaintText(HDC hdc, const TDPageState& s)
{
    if ( !s.hTD )
        return;

    const bool native = TDHasNativeDarkTheme();

    for ( const auto& el : s.elements )
    {
        if ( ::IsRectEmpty(&el.rect) )
            continue;

        RECT   rcText = el.rect;
        int    part = 0;
        HBRUSH brBg = 0;
        DWORD  dtF = DT_LEFT | DT_VCENTER | DT_WORDBREAK | DT_NOPREFIX;

        if ( el.automationId == L"MainInstruction" )
        {
            part = TDLG_MAININSTRUCTIONPANE;
            brBg = s.brPrimary;
        }
        else if ( el.automationId == L"ContentText" )
        {
            part = TDLG_CONTENTPANE;
            brBg = s.brPrimary;
        }
        else if ( el.automationId == L"ExpandedFooterText" )
        {
            part = TDLG_EXPANDEDFOOTERAREA;
            brBg = s.brFootnote;
        }
        else if ( el.automationId == L"FootnoteText" )
        {
            part = TDLG_FOOTNOTEPANE;
            brBg = s.brFootnote;
        }
        else if ( el.automationId == L"ExpandoButton" && s.hTD )
        {
            const int width =
                s.hTD.GetTrueSize(TDLG_EXPANDOBUTTON, TDLGEBS_NORMAL, hdc).x;

            MARGINS vm = {};
            s.hTD.GetMargins(vm, TDLG_VERIFICATIONTEXT, TMT_CONTENTMARGINS, 0, hdc);
            rcText.left += width + vm.cxLeftWidth - 2;
            rcText.top += 1;

            part = TDLG_EXPANDOTEXT;
            brBg = s.brSecondary;

            dtF = DT_LEFT | DT_VCENTER | DT_NOPREFIX;
        }
        else if ( el.automationId == L"VerificationCheckBox" && s.hButton && s.hTD )
        {
            const int width =
                s.hButton.GetDrawSize(BP_CHECKBOX, CBS_UNCHECKEDNORMAL, hdc).x;

            MARGINS tm = {};
            s.hTD.GetMargins(tm, TDLG_VERIFICATIONTEXT, TMT_CONTENTMARGINS, 0, hdc);
            rcText.left = el.rect.left + width + tm.cxLeftWidth + 3;
            rcText.top += 5;

            part = TDLG_VERIFICATIONTEXT;
            brBg = s.brSecondary;

            dtF = DT_LEFT | DT_VCENTER | DT_NOPREFIX;
        }

        if ( !part )
            continue;

        WinStructWordSize<DTTOPTS> opts;
        if ( native )
        {
            if (part == TDLG_EXPANDEDFOOTERAREA)
            {
                WinStructWordSize<DTBGOPTS> optsBg;
                optsBg.dwFlags = DTBG_OMITBORDER;

                ::DrawThemeBackgroundEx(s.hTD, hdc, TDLG_SECONDARYPANEL, 0, &el.rect, &optsBg);
            }
        }
        else
        {
            opts.dwFlags = DTT_COMPOSITED | DTT_TEXTCOLOR;
            opts.crText = TDGetTextColour(s, part);

            ::FillRect(hdc, &rcText, brBg);
        }

        ::DrawThemeTextEx(s.hTD, hdc, part, 0, el.name.c_str(), -1, dtF, &rcText, &opts);
    }
}

void TDPaintPage(HWND hwnd, HDC hdcWin, TDPageState& s)
{
    if ( !s.themesOk )
        TDRefreshThemes(hwnd, s);

    const RECT rc = wxGetClientRect(hwnd);
    HDC hdcBuf = 0;
    HPAINTBUFFER hbp = ::BeginBufferedPaint(hdcWin, &rc, BPBF_TOPDOWNDIB, nullptr, &hdcBuf);
    if ( !hbp )
    {
        ::DefSubclassProc(hwnd, WM_PRINTCLIENT, reinterpret_cast<WPARAM>(hdcWin), PRF_CLIENT);
        return;
    }

    ::DefSubclassProc(hwnd, WM_PRINTCLIENT, reinterpret_cast<WPARAM>(hdcBuf), PRF_CLIENT);

    if ( !TDHasNativeDarkTheme() )
    {
        RECT rcBuf;
        ::GetBufferedPaintTargetRect(hbp, &rcBuf);
        TDPaintPixelSwap(hbp, rcBuf.right - rcBuf.left, rcBuf.bottom - rcBuf.top);
    }

    TDPaintIcons(hdcBuf, s);
    TDPaintGlyphs(hdcBuf, s);
    TDPaintText(hdcBuf, s);

    ::EndBufferedPaint(hbp, TRUE);
}

// ============================================================================
// Subclass procedures
// ============================================================================

LRESULT CALLBACK
TDPageSubclassProc(HWND hwnd,
                   UINT msg,
                   WPARAM wParam,
                   LPARAM lParam,
                   UINT_PTR uId,
                   DWORD_PTR dwRef)
{
    switch ( msg )
    {
        case WM_ERASEBKGND:
            return TRUE;

        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = ::BeginPaint(hwnd, &ps);
                TDPageState& s = TDPageState::Get(hwnd);
                s.isExpanded = ::GetPropW(GetParent(hwnd), L"IsExpanded");
                s.elemsOk = false;
                TDUpdateLayoutCache(hwnd, s);
                TDPaintPage(hwnd, hdc, s);
                ::EndPaint(hwnd, &ps);
            }
            return 0;

        case WM_MOUSEMOVE:
            {
                TDPageState& s = TDPageState::Get(hwnd);
                if ( !s.tracking )
                {
                    WinStruct<TRACKMOUSEEVENT> tme;
                    tme.dwFlags = TME_LEAVE;
                    tme.hwndTrack = hwnd;
                    ::TrackMouseEvent(&tme);

                    s.tracking = true;
                }
                POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

                const int nh = TDHitTest(s.elements, pt);
                if ( nh != s.hotIdx )
                {
                    s.hotIdx = nh;
                    ::InvalidateRect(hwnd, nullptr, FALSE);
                }
            }
            break;

        case WM_MOUSELEAVE:
            {
                TDPageState& s = TDPageState::Get(hwnd);
                s.tracking = false;
                s.pressing = false;

                if ( s.hotIdx != -1 )
                {
                    s.hotIdx = -1;
                    ::InvalidateRect(hwnd, nullptr, FALSE);
                }
            }
            break;

        case WM_LBUTTONDOWN:
            TDPageState::Get(hwnd).pressing = true;
            TDUpdateLayoutCache(hwnd, TDPageState::Get(hwnd));
            ::InvalidateRect(hwnd, nullptr, FALSE);
            break;

        case WM_LBUTTONUP:
            TDPageState::Get(hwnd).pressing = false;
            TDUpdateLayoutCache(hwnd, TDPageState::Get(hwnd));
            ::InvalidateRect(hwnd, nullptr, FALSE);
            break;

        case WM_THEMECHANGED:
            {
                TDPageState& s = TDPageState::Get(hwnd);
                s.themesOk = false;
                s.elemsOk = false;
                ::InvalidateRect(hwnd, nullptr, FALSE);
            }
            break;

        case WM_DESTROY:
            // Restore the original class brush we had changed.
            ::SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, dwRef);
            TDPageState::Destroy(hwnd);
            ::RemoveWindowSubclass(hwnd, TDPageSubclassProc, uId);
            break;
    }

    return ::DefSubclassProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK
TDCtrlContainerSubclassProc(HWND hwnd,
                            UINT msg,
                            WPARAM wParam,
                            LPARAM lParam,
                            UINT_PTR uId,
                            DWORD_PTR dwRef)
{
    HBRUSH hbr = reinterpret_cast<HBRUSH>(dwRef);

    switch ( msg )
    {
        case WM_ERASEBKGND:
            {
                HDC hdc = reinterpret_cast<HDC>(wParam);
                wxFillRect(hwnd, hdc, hbr);
                return 1;
            }

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
                if ( hbr )
                {
                    LOGBRUSH lb = {};
                    ::GetObject(hbr, sizeof(lb), &lb);
                    if ( lb.lbStyle == BS_SOLID )
                        bg = lb.lbColor;
                }

                ::SetBkColor(hdc, bg);
                ::SetTextColor(hdc, TDDarkCol::kTextNormal);

                if ( !hbr )
                    hbr = GetSolidBrush(TDDarkCol::kSecondary);

                return reinterpret_cast<LRESULT>(hbr);
            }

        case WM_DESTROY:
            ::RemoveWindowSubclass(hwnd, TDCtrlContainerSubclassProc, uId);
            break;
    }

    return ::DefSubclassProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK
TDRadioButtonSubclassProc(HWND hwnd,
                          UINT msg,
                          WPARAM wParam,
                          LPARAM lParam,
                          UINT_PTR uId,
                          DWORD_PTR WXUNUSED(dwRef))
{
    switch ( msg )
    {
        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = ::BeginPaint(hwnd, &ps);

                const int dpi = wxGetWindowDPI(hwnd).x;
                auto hStyle = wxUxThemeHandle::NewAtStdDPI(L"TaskDialogStyle");
                auto hBtn = wxUxThemeHandle::NewAtDPI(nullptr, L"Button", dpi);

                const RECT rcClient = wxGetClientRect(hwnd);
                HDC hdcBuf = 0;
                HPAINTBUFFER hbp = ::BeginBufferedPaint(hdc, &rcClient, BPBF_TOPDOWNDIB, nullptr, &hdcBuf);

                ::DefSubclassProc(hwnd, WM_PRINTCLIENT, reinterpret_cast<WPARAM>(hdcBuf), PRF_CLIENT);

                wchar_t text[512] = {};
                GetWindowTextW(hwnd, text, static_cast<int>(std::size(text)));

                auto gs = hBtn.GetTrueSize(BP_RADIOBUTTON, RBS_UNCHECKEDNORMAL);
                RECT rcText = { gs.x + 2, 0, rcClient.right, rcClient.bottom };

                WinStructWordSize<DTTOPTS> opts;
                opts.dwFlags = DTT_COMPOSITED | DTT_TEXTCOLOR;
                opts.crText = TDDarkCol::kTextNormal;

                LOGFONT lf = {};
                if ( hStyle.GetFont(lf, hdcBuf, TDLG_RADIOBUTTONPANE) )
                {
                    AutoHFONT hFont(lf);
                    SelectInHDC sel(hdcBuf, hFont);

                    ::DrawThemeTextEx(hStyle, hdcBuf, TDLG_RADIOBUTTONPANE, 0,
                                      text, -1,
                                      DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS,
                                      &rcText, &opts);
                }

                if ( hbp )
                    ::EndBufferedPaint(hbp, TRUE);
                ::EndPaint(hwnd, &ps);
            }
            return 0;

        case WM_DESTROY:
            ::RemoveWindowSubclass(hwnd, TDRadioButtonSubclassProc, uId);
            break;
    }

    return ::DefSubclassProc(hwnd, msg, wParam, lParam);
}

// ============================================================================
// Attachment helpers
// ============================================================================

// Helper which calls SetWindowSubclass() only if the subclass is not already
// set.
//
// This overload takes a lambda as the last parameter which is called to create
// the reference data if the subclass needs to be set and to avoid creating any
// resources passed to the subclass procedure unnecessarily.
template <typename InitFunc>
void
SetWindowSubclassIfNeeded(HWND hwnd,
                           SUBCLASSPROC proc,
                           UINT_PTR uId,
                           InitFunc initFunc)
{
    DWORD_PTR dwRef = 0;
    if ( ::GetWindowSubclass(hwnd, proc, uId, &dwRef) )
        return;

    dwRef = static_cast<DWORD_PTR>(initFunc());
    if ( !::SetWindowSubclass(hwnd, proc, uId, dwRef) )
    {
        wxLogLastError("SetWindowSubclass");
    }
}

// This is a simple overload when we don't need to pass any reference data to
// the subclass procedure.
void
SetWindowSubclassIfNeeded(HWND hwnd,
                           SUBCLASSPROC proc,
                           UINT_PTR uId)
{
    return SetWindowSubclassIfNeeded(hwnd, proc, uId, []() { return 0; });
}

// Remove the subclass if it was installed.
//
// Return false if it wasn't.
bool RemoveWindowSubclassIfNeeded(HWND hwnd, SUBCLASSPROC proc, UINT_PTR uId)
{
    DWORD_PTR dwRef = 0;
    if ( !::GetWindowSubclass(hwnd, proc, uId, &dwRef) )
        return false;

    if ( !::RemoveWindowSubclass(hwnd, proc, uId) )
    {
        wxLogLastError("RemoveWindowSubclass");
    }

    return true;
}

void TDSubclassContainer(HWND hwndParent, COLORREF bg)
{
    if ( !hwndParent )
        return;

    SetWindowSubclassIfNeeded
    (
        hwndParent,
        TDCtrlContainerSubclassProc,
        kTDCtrlSubclassId,
        [bg]() { return reinterpret_cast<DWORD_PTR>(GetSolidBrush(bg)); }
    );
}

void TDApplyToChildren(IUIAutomationElement* pEl)
{
    IUIAutomation* const uiAuto = wxTaskDialogDarkModule::GetUIAutomation();
    if ( !uiAuto )
        return;

    const bool native = TDHasNativeDarkTheme();

    wxCOMPtr<IUIAutomationTreeWalker> pWalker;
    uiAuto->get_ContentViewWalker(&pWalker);
    if ( !pWalker )
        return;

    wxCOMPtr<IUIAutomationElement> pChild;
    pWalker->GetFirstChildElement(pEl, &pChild);

    while ( pChild )
    {
        CONTROLTYPEID ct = 0;
        pChild->get_CurrentControlType(&ct);

        if ( ct == UIA_ButtonControlTypeId ||
             ct == UIA_RadioButtonControlTypeId ||
             ct == UIA_ProgressBarControlTypeId ||
             ct == UIA_HyperlinkControlTypeId ||
             ct == UIA_ScrollBarControlTypeId ||
             ct == UIA_PaneControlTypeId)
        {
            if ( const HWND hBtn = GetHWNDFromElement(pChild) )
            {
                const std::wstring id = GetCurrentAutomationId(pChild);

                HWND hwndParent = ::GetParent(hBtn);

                if ( ct == UIA_ProgressBarControlTypeId )
                {
                    wxMSWDarkMode::SetTheme
                    (
                        hBtn,
                        wxHasRealDarkTheme(L"Progress", L"DarkMode_CopyEngine::Progress")
                            ? L"DarkMode_CopyEngine"
                            : L"DarkMode_Explorer"
                    );
                }
                else if ( ct == UIA_RadioButtonControlTypeId ||
                            id.find(L"RadioButton_") == 0 ||
                                ct == UIA_HyperlinkControlTypeId )
                {
                    if ( native )
                    {
                        wxMSWDarkMode::SetTheme(hBtn, L"DarkMode_DarkTheme");
                    }
                    else
                    {
                        SetWindowSubclassIfNeeded(hBtn, TDRadioButtonSubclassProc, kTDCtrlSubclassId);
                    }

                    TDSubclassContainer(hwndParent, native ? TDDarkCol::kSecondary : TDDarkCol::kPrimary);
                }
                else if ( id.find(L"CommandLink_") == 0 || id.find(L"CommandButton_") == 0 )
                {
                    wxMSWDarkMode::SetTheme(hBtn, L"DarkMode_Explorer");
                    TDSubclassContainer(hwndParent, TDDarkCol::kSecondary);
                }
                else
                {
                    wxMSWDarkMode::SetTheme(hBtn, L"DarkMode_Explorer");
                }
            }
        }

        wxCOMPtr<IUIAutomationElement> pNext;
        pWalker->GetNextSiblingElement(pChild, &pNext);
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

BOOL CALLBACK TDEnumAttachProc(HWND hwndChild, LPARAM lparam)
{
    IUIAutomation* const uiAuto = wxTaskDialogDarkModule::GetUIAutomation();
    if ( !uiAuto )
        return FALSE;

    wxCOMPtr<IUIAutomationElement> pEl;
    if ( FAILED(uiAuto->ElementFromHandle(hwndChild, &pEl)) )
        return TRUE;

    AutoBSTR cls;
    pEl->get_CurrentClassName(cls.Out());

    if ( !cls )
        return TRUE;

    // SysLink controls (footnote / content hyperlinks)
    if ( wcscmp(cls, L"CCSysLink") == 0 )
    {
        if ( const HWND hLink = GetHWNDFromElement(pEl) )
        {
            const std::wstring id = GetCurrentAutomationId(pEl);
            bool isFn = id.find(L"Footnote") != std::wstring::npos ||
                            id.find(L"ExpandedFooter") != std::wstring::npos;

            TDSubclassContainer
            (
                GetParent(hLink),
                isFn && !TDHasNativeDarkTheme()
                    ? TDDarkCol::kFootnote
                    : TDDarkCol::kPrimary
            );
        }

        return TRUE;
    }

    if ( wcscmp(cls, L"CCVScrollBar") == 0 || wcscmp(cls, L"CCHScrollBar") == 0 )
    {
        if ( const HWND hScroll = GetHWNDFromElement(pEl) )
        {
            wxMSWDarkMode::SetTheme(hScroll, L"DarkMode_Explorer");
        }

        return TRUE;
    }

    // Main TaskPage (DirectUI "TaskDialog" class)
    if ( wcscmp(cls, L"TaskDialog") != 0 )
        return TRUE;

    const HWND hDUI = GetHWNDFromElement(pEl);
    if ( !hDUI )
        return TRUE;

    TDApplyToChildren(pEl);
    wxMSWDarkMode::SetTheme(hDUI, L"DarkMode_Explorer");

    // Initialise per-page state
    TDEnumData* const d = reinterpret_cast<TDEnumData*>(lparam);
    TDPageState& s = TDPageState::Get(hDUI);
    s.pCfg = d->pCfg;
    s.defExpanded = d->pCfg && (d->pCfg->dwFlags & TDF_EXPANDED_BY_DEFAULT);
    s.defChecked = d->pCfg && (d->pCfg->dwFlags & TDF_VERIFICATION_FLAG_CHECKED);
    s.isExpanded = ::GetPropW(d->hwndTD, L"IsExpanded");
    s.isChecked = s.defChecked || ::GetPropW(d->hwndTD, L"IsChecked");
    s.elemsOk = false;
    TDUpdateLayoutCache(hDUI, s);

    SetWindowSubclassIfNeeded
    (
        hDUI,
        TDPageSubclassProc,
        kTDPageSubclassId,
        [hDUI]()
        {
            // Change class background brush when subclassing.
            const auto newBrush = reinterpret_cast<LONG_PTR>(
                GetSolidBrush(TDDarkCol::kSecondary)
            );
            return ::SetClassLongPtr(hDUI, GCLP_HBRBACKGROUND, newBrush);
        }
    );

    d->found = true;
    return TRUE;
}

BOOL CALLBACK TDEnumSysColorProc(HWND h, LPARAM)
{
    ::SendMessage(h, WM_SYSCOLORCHANGE, 0, 0);
    return TRUE;
}

BOOL CALLBACK TDEnumDetachProc(HWND hChild, LPARAM)
{
    if ( RemoveWindowSubclassIfNeeded(hChild, TDPageSubclassProc, kTDPageSubclassId) )
        TDPageState::Destroy(hChild);

    RemoveWindowSubclassIfNeeded(hChild, TDCtrlContainerSubclassProc, kTDCtrlSubclassId);

    RemoveWindowSubclassIfNeeded(hChild, TDRadioButtonSubclassProc, kTDCtrlSubclassId);

    ::SetWindowTheme(hChild, nullptr, nullptr);

    return TRUE;
}

void TDAttach(HWND hwndTD, const TASKDIALOGCONFIG* pCfg)
{
    const bool native = TDHasNativeDarkTheme();

    TDEnumData data;
    data.hwndTD = hwndTD;
    data.pCfg = pCfg;

    ::EnumChildWindows(hwndTD, TDEnumAttachProc, reinterpret_cast<LPARAM>(&data));

    if ( !data.found )
        return;

    if ( native )
        wxMSWDarkMode::AllowForWindow(hwndTD, L"DarkMode_Explorer", nullptr);

    HBRUSH nb = GetSolidBrush(TDDarkCol::kPrimary);

    SetWindowSubclassIfNeeded
    (
        hwndTD,
        TDCtrlContainerSubclassProc,
        kTDMainSubclassId,
        [nb]() { return reinterpret_cast<DWORD_PTR>(nb); }
    );

    // Dark title bar
    wxMSWDarkMode::ConfigureTLW(hwndTD);
    ::EnumChildWindows(hwndTD, TDEnumSysColorProc, 0);
    ::SendMessage(hwndTD, WM_THEMECHANGED, 0, 0);
}

void TDDetach(HWND hwndTD)
{
    ::EnumChildWindows(hwndTD, TDEnumDetachProc, 0);
}

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
    const HRESULT hr = CoInitializeEx
                       (
                        nullptr,
                        COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE
                       );
    if ( FAILED(hr) && hr != S_FALSE )
        return;

    TDAttach(hwnd, pCfg);

    CoUninitialize();
#else // !wxUSE_DARK_MODE
    wxUnusedVar(hwnd);
    wxUnusedVar(pCfg);
#endif // wxUSE_DARK_MODE
}

void wxMSWDarkMode::RemoveFromTaskDialog(HWND hwnd)
{
#if wxUSE_DARK_MODE
    TDDetach(hwnd);
#else // !wxUSE_DARK_MODE
    wxUnusedVar(hwnd);
#endif // wxUSE_DARK_MODE
}
