/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/textctrl.cpp
// Purpose:     wxWinUI wxTextCtrl implementation
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef WXWINUI_TEST_SUPPORT
    #include "textctrl-test-access.h"
#endif

#if wxUSE_TEXTCTRL

#include "wx/textctrl.h"
#include "wx/winui/winui.h"

#ifndef WX_PRECOMP
    #include "wx/font.h"
#endif

#include "wx/arrstr.h"
#include "wx/clipbrd.h"
#include "wx/dataobj.h"
#include "wx/dcclient.h"
#include "wx/dialog.h"
#include "wx/dir.h"
#include "wx/filename.h"
#include "wx/scopeguard.h"
#include "wx/textcompleter.h"
#include "wx/weakref.h"
#include "wx/winui/private/inputtest.h"
#include "wx/winui/private/textpeer.h"
#include "wx/winui/private/tlwhost.h"

#include "private.h"

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

#if wxUSE_RICHEDIT
    #include "wx/menu.h"
#endif

#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Text.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Text.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Text.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <vector>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXCP = winrt::Microsoft::UI::Xaml::Controls::Primitives;
namespace MUXI = winrt::Microsoft::UI::Xaml::Input;
namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
namespace MUXAPR = winrt::Microsoft::UI::Xaml::Automation::Provider;
namespace MUXAT = winrt::Microsoft::UI::Xaml::Automation::Text;
namespace MUXD = winrt::Microsoft::UI::Dispatching;
namespace WU = winrt::Windows::UI;
namespace WUT = winrt::Microsoft::UI::Text;
namespace WUTF = winrt::Windows::UI::Text;

namespace
{

class wxWinUIPasswordAutomationPeer
    : public MUXAP::RichEditBoxAutomationPeerT<
          wxWinUIPasswordAutomationPeer>
{
public:
    explicit wxWinUIPasswordAutomationPeer(
        const MUXC::RichEditBox& owner)
        : RichEditBoxAutomationPeerT(owner)
    {
    }

    winrt::Windows::Foundation::IInspectable GetPatternCore(
        MUXAP::PatternInterface) const
    {
        // This is the documented PasswordBox automation contract: no Value
        // or Text pattern is exposed, so neither the secret model nor even
        // the masked backing document can be harvested by UI Automation.
        return nullptr;
    }

    winrt::hstring GetClassNameCore() const
    {
        return L"PasswordBox";
    }

    MUXAP::AutomationControlType GetAutomationControlTypeCore() const
    {
        return MUXAP::AutomationControlType::Edit;
    }

    bool IsPasswordCore() const
    {
        return true;
    }
};

class wxWinUIPasswordRichEditBox
    : public MUXC::RichEditBoxT<wxWinUIPasswordRichEditBox>
{
public:
    MUXC::RichEditBox Projection() const
    {
        return this->m_inner.as<MUXC::RichEditBox>();
    }

    MUXAP::AutomationPeer OnCreateAutomationPeer()
    {
        return winrt::make<wxWinUIPasswordAutomationPeer>(
            this->m_inner.as<MUXC::RichEditBox>());
    }
};

wxString wxWinUIMaskPassword(const wxString& value)
{
    // Keep one glyph per UTF-16 code unit. TOM, wxTextEntry positions and the
    // Windows edit APIs all use the same coordinate system, including for a
    // surrogate pair, so selection/caret geometry remains lossless.
    wxString masked;
    masked.reserve(value.length());
    for ( std::size_t n = 0; n < value.length(); ++n )
        masked += static_cast<wxChar>(0x25cf);
    return masked;
}

struct wxWinUIUTF16CodePoint
{
    unsigned int value;
    std::size_t length;
};

wxWinUIUTF16CodePoint wxWinUIReadUTF16CodePoint(const wxString& value,
                                                 std::size_t offset)
{
    const unsigned int first = static_cast<unsigned int>(value[offset]);
    if ( first >= 0xd800 && first <= 0xdbff &&
         offset + 1 < value.length() )
    {
        const unsigned int second =
            static_cast<unsigned int>(value[offset + 1]);
        if ( second >= 0xdc00 && second <= 0xdfff )
        {
            return {
                0x10000 + ((first - 0xd800) << 10) + (second - 0xdc00),
                2
            };
        }
    }

    return { first, 1 };
}

wxWinUIUTF16CodePoint wxWinUIReadPreviousUTF16CodePoint(
    const wxString& value,
    std::size_t offset)
{
    std::size_t start = offset - 1;
    const unsigned int last = static_cast<unsigned int>(value[start]);
    if ( last >= 0xdc00 && last <= 0xdfff && start > 0 )
    {
        const unsigned int first =
            static_cast<unsigned int>(value[start - 1]);
        if ( first >= 0xd800 && first <= 0xdbff )
            --start;
    }

    return wxWinUIReadUTF16CodePoint(value, start);
}

bool wxWinUIIsGraphemeExtension(unsigned int codePoint)
{
    if ( codePoint == 0x200c || codePoint == 0x200d ||
         (codePoint >= 0xfe00 && codePoint <= 0xfe0f) ||
         (codePoint >= 0x1f3fb && codePoint <= 0x1f3ff) ||
         (codePoint >= 0xe0020 && codePoint <= 0xe007f) ||
         (codePoint >= 0xe0100 && codePoint <= 0xe01ef) )
    {
        return true;
    }

    if ( codePoint <= 0xffff )
    {
        const wchar_t character = static_cast<wchar_t>(codePoint);
        WORD type = 0;
        if ( ::GetStringTypeW(CT_CTYPE3, &character, 1, &type) )
        {
            return (type & (C3_NONSPACING | C3_DIACRITIC |
                            C3_VOWELMARK)) != 0;
        }
    }

    return false;
}

enum class wxWinUIHangulSyllableType
{
    None,
    L,
    V,
    T,
    LV,
    LVT
};

wxWinUIHangulSyllableType wxWinUIGetHangulSyllableType(
    unsigned int codePoint)
{
    if ( (codePoint >= 0x1100 && codePoint <= 0x115f) ||
         (codePoint >= 0xa960 && codePoint <= 0xa97c) )
    {
        return wxWinUIHangulSyllableType::L;
    }
    if ( (codePoint >= 0x1160 && codePoint <= 0x11a7) ||
         (codePoint >= 0xd7b0 && codePoint <= 0xd7c6) )
    {
        return wxWinUIHangulSyllableType::V;
    }
    if ( (codePoint >= 0x11a8 && codePoint <= 0x11ff) ||
         (codePoint >= 0xd7cb && codePoint <= 0xd7fb) )
    {
        return wxWinUIHangulSyllableType::T;
    }
    if ( codePoint >= 0xac00 && codePoint <= 0xd7a3 )
    {
        return (codePoint - 0xac00) % 28
                   ? wxWinUIHangulSyllableType::LVT
                   : wxWinUIHangulSyllableType::LV;
    }

    return wxWinUIHangulSyllableType::None;
}

bool wxWinUIIsSafeTextBoundary(const wxString& value, std::size_t offset)
{
    if ( offset == 0 || offset >= value.length() )
        return true;

    const unsigned int current = static_cast<unsigned int>(value[offset]);
    const unsigned int previous =
        static_cast<unsigned int>(value[offset - 1]);
    if ( current >= 0xdc00 && current <= 0xdfff &&
         previous >= 0xd800 && previous <= 0xdbff )
    {
        return false;
    }

    const wxWinUIUTF16CodePoint before =
        wxWinUIReadPreviousUTF16CodePoint(value, offset);
    const wxWinUIUTF16CodePoint after =
        wxWinUIReadUTF16CodePoint(value, offset);
    if ( (before.value == '\r' && after.value == '\n') ||
         wxWinUIIsGraphemeExtension(after.value) ||
         before.value == 0x200d )
    {
        return false;
    }

    const wxWinUIHangulSyllableType beforeHangul =
        wxWinUIGetHangulSyllableType(before.value);
    const wxWinUIHangulSyllableType afterHangul =
        wxWinUIGetHangulSyllableType(after.value);
    if ( (beforeHangul == wxWinUIHangulSyllableType::L &&
          (afterHangul == wxWinUIHangulSyllableType::L ||
           afterHangul == wxWinUIHangulSyllableType::V ||
           afterHangul == wxWinUIHangulSyllableType::LV ||
           afterHangul == wxWinUIHangulSyllableType::LVT)) ||
         ((beforeHangul == wxWinUIHangulSyllableType::LV ||
           beforeHangul == wxWinUIHangulSyllableType::V) &&
          (afterHangul == wxWinUIHangulSyllableType::V ||
           afterHangul == wxWinUIHangulSyllableType::T)) ||
         ((beforeHangul == wxWinUIHangulSyllableType::LVT ||
           beforeHangul == wxWinUIHangulSyllableType::T) &&
          afterHangul == wxWinUIHangulSyllableType::T) )
    {
        return false;
    }

    if ( before.value >= 0x1f1e6 && before.value <= 0x1f1ff &&
         after.value >= 0x1f1e6 && after.value <= 0x1f1ff )
    {
        std::size_t regionalIndicatorCount = 0;
        std::size_t scan = offset;
        while ( scan > 0 )
        {
            const wxWinUIUTF16CodePoint codePoint =
                wxWinUIReadPreviousUTF16CodePoint(value, scan);
            if ( codePoint.value < 0x1f1e6 || codePoint.value > 0x1f1ff )
                break;
            ++regionalIndicatorCount;
            scan -= codePoint.length;
        }
        if ( regionalIndicatorCount % 2 )
            return false;
    }

    return true;
}

void wxWinUIFindSafeCommonTextEdges(const wxString& before,
                                    const wxString& proposed,
                                    std::size_t *prefixLength,
                                    std::size_t *suffixLength)
{
    *prefixLength = 0;
    const std::size_t commonLength =
        wxMin(before.length(), proposed.length());
    while ( *prefixLength < commonLength &&
            before[*prefixLength] == proposed[*prefixLength] )
    {
        ++*prefixLength;
    }

    while ( *prefixLength > 0 &&
            (!wxWinUIIsSafeTextBoundary(before, *prefixLength) ||
             !wxWinUIIsSafeTextBoundary(proposed, *prefixLength)) )
    {
        --*prefixLength;
    }

    *suffixLength = 0;
    while ( *suffixLength < before.length() - *prefixLength &&
            *suffixLength < proposed.length() - *prefixLength &&
            before[before.length() - *suffixLength - 1] ==
                proposed[proposed.length() - *suffixLength - 1] )
    {
        ++*suffixLength;
    }

    while ( *suffixLength > 0 &&
            (!wxWinUIIsSafeTextBoundary(
                 before, before.length() - *suffixLength) ||
             !wxWinUIIsSafeTextBoundary(
                 proposed, proposed.length() - *suffixLength)) )
    {
        --*suffixLength;
    }
}

wxString wxWinUIFitTextPrefix(const wxString& value,
                              std::size_t available,
                              bool twoCharacterNewlines)
{
    std::size_t end = 0;
    std::size_t used = 0;
    while ( end < value.length() )
    {
        std::size_t clusterEnd =
            end + wxWinUIReadUTF16CodePoint(value, end).length;
        while ( clusterEnd < value.length() &&
                !wxWinUIIsSafeTextBoundary(value, clusterEnd) )
        {
            clusterEnd +=
                wxWinUIReadUTF16CodePoint(value, clusterEnd).length;
        }

        const std::size_t remaining = available - used;
        std::size_t clusterCost = 0;
        bool fits = true;
        for ( std::size_t offset = end; offset < clusterEnd; )
        {
            const wxWinUIUTF16CodePoint codePoint =
                wxWinUIReadUTF16CodePoint(value, offset);
            const std::size_t cost =
                twoCharacterNewlines && codePoint.value == '\n'
                    ? 2
                    : codePoint.length;
            if ( cost > remaining - clusterCost )
            {
                fits = false;
                break;
            }
            clusterCost += cost;
            offset += codePoint.length;
        }
        if ( !fits )
            break;
        used += clusterCost;
        end = clusterEnd;
    }
    return value.substr(0, end);
}

bool wxWinUIDecodePasswordEdit(const wxString& before,
                               long oldSelectionStart,
                               long oldSelectionEnd,
                               const wxString& rawPeerValue,
                               long newSelectionStart,
                               long newSelectionEnd,
                               int deleteDirection,
                               std::size_t *replaceStart,
                               std::size_t *replaceEnd,
                               wxString *inserted,
                               long *mappedSelectionStart,
                               long *mappedSelectionEnd)
{
    if ( !replaceStart || !replaceEnd || !inserted ||
         !mappedSelectionStart || !mappedSelectionEnd )
    {
        return false;
    }

    const std::size_t oldLength = before.length();
    const std::size_t newLength = rawPeerValue.length();
    const auto clampIndex = [](long value, std::size_t length)
    {
        if ( value <= 0 )
            return std::size_t{0};
        return wxMin(static_cast<std::size_t>(value), length);
    };
    std::size_t oldStart = clampIndex(oldSelectionStart, oldLength);
    std::size_t oldEnd = clampIndex(oldSelectionEnd, oldLength);
    if ( oldStart > oldEnd )
        std::swap(oldStart, oldEnd);

    const std::size_t newCaret = clampIndex(
        wxMin(newSelectionStart, newSelectionEnd), newLength);
    std::size_t start = oldStart;
    std::size_t end = oldEnd;
    std::size_t insertionLength = 0;
    bool haveSelectionBasedEdit = false;

    if ( oldEnd > oldStart )
    {
        const std::size_t preserved =
            oldLength - (oldEnd - oldStart);
        if ( newLength >= preserved )
        {
            insertionLength = newLength - preserved;
            haveSelectionBasedEdit =
                start + insertionLength <= newLength;
        }
    }
    else if ( newLength > oldLength )
    {
        insertionLength = newLength - oldLength;
        start = newCaret >= insertionLength
                    ? newCaret - insertionLength
                    : oldStart;
        start = wxMin(start, oldLength);
        end = start;
        haveSelectionBasedEdit =
            start + insertionLength <= newLength;
    }
    else if ( newLength < oldLength )
    {
        const std::size_t removed = oldLength - newLength;
        // A backward deletion moves the caret left; Delete leaves it at the
        // old insertion point. This disambiguates identical mask glyphs,
        // for which a plain longest-prefix diff would always pick the tail.
        if ( deleteDirection < 0 )
            start = oldStart >= removed ? oldStart - removed : 0;
        else if ( deleteDirection > 0 )
            start = oldStart;
        else
            start = newCaret < oldStart ? newCaret : oldStart;
        start = wxMin(start, oldLength);
        end = wxMin(oldLength, start + removed);
        insertionLength = 0;
        haveSelectionBasedEdit = true;
    }

    const auto isMaskedSpan = [&rawPeerValue](std::size_t from,
                                               std::size_t to)
    {
        to = wxMin(to, rawPeerValue.length());
        for ( std::size_t n = from; n < to; ++n )
        {
            if ( rawPeerValue[n] != static_cast<wxChar>(0x25cf) )
                return false;
        }
        return true;
    };

    if ( haveSelectionBasedEdit )
    {
        const std::size_t expectedSuffix = oldLength - end;
        const std::size_t rawSuffixStart = start + insertionLength;
        if ( rawSuffixStart + expectedSuffix != newLength ||
             !isMaskedSpan(0, start) ||
             !isMaskedSpan(rawSuffixStart, newLength) )
        {
            haveSelectionBasedEdit = false;
        }
    }

    if ( !haveSelectionBasedEdit )
    {
        const wxString oldMask = wxWinUIMaskPassword(before);
        start = 0;
        const std::size_t common = wxMin(oldLength, newLength);
        while ( start < common && oldMask[start] == rawPeerValue[start] )
            ++start;

        std::size_t suffix = 0;
        while ( suffix < oldLength - start &&
                suffix < newLength - start &&
                oldMask[oldLength - suffix - 1] ==
                    rawPeerValue[newLength - suffix - 1] )
        {
            ++suffix;
        }
        end = oldLength - suffix;
        insertionLength = newLength - start - suffix;
    }

    if ( start > oldLength || end < start || end > oldLength ||
         start + insertionLength > newLength )
    {
        return false;
    }

    *replaceStart = start;
    *replaceEnd = end;
    *inserted = rawPeerValue.substr(start, insertionLength);
    *mappedSelectionStart = static_cast<long>(
        clampIndex(newSelectionStart, newLength));
    *mappedSelectionEnd = static_cast<long>(
        clampIndex(newSelectionEnd, newLength));
    return true;
}

class wxWinUIFileNameCompleter final : public wxTextCompleterSimple
{
public:
    explicit wxWinUIFileNameCompleter(int flags)
        : m_flags(flags)
    {
    }

    void GetCompletions(const wxString& prefix,
                        wxArrayString& completions) override
    {
        wxFileName name(prefix);
        wxString directory = name.GetPath();
        const wxString leaf = name.GetFullName();
        if ( directory.empty() )
            directory = wxS(".");

        wxDir dir(directory);
        if ( !dir.IsOpened() )
            return;

        const int dirFlags = m_flags == wxDIR
            ? wxDIR_DIRS
            : wxDIR_DIRS | wxDIR_FILES;
        wxString entry;
        bool found = dir.GetFirst(&entry, leaf + wxS("*"), dirFlags);
        while ( found )
        {
            wxFileName completion(directory, entry);
            wxString value = completion.GetFullPath();
            if ( name.GetPath().empty() )
                value = entry;
            if ( completion.DirExists() )
                value += wxFileName::GetPathSeparator();
            completions.Add(value);
            found = dir.GetNext(&entry);
        }
        completions.Sort();
    }

private:
    const int m_flags;
};

struct wxWinUIAutoUrlRange
{
    long start = 0;
    long end = 0;
    wxString target;
};

std::vector<wxWinUIAutoUrlRange> wxWinUIFindAutoUrls(
    const wxString& value)
{
    struct Scheme
    {
        const wxChar *prefix;
        const wxChar *targetPrefix;
    };

    // This is the default RichEdit scheme set documented for
    // EM_AUTOURLDETECT, plus the long-standing wxGTK/MSW-compatible shorthand
    // host forms. Match the scheme name itself (rather than requiring //):
    // e.g. mailto:, tel: and news: are all valid detected links.
    static const Scheme schemes[] =
    {
        { wxS("callto:"), nullptr },
        { wxS("file:"), nullptr },
        { wxS("ftp:"), nullptr },
        { wxS("gopher:"), nullptr },
        { wxS("http:"), nullptr },
        { wxS("https:"), nullptr },
        { wxS("mailto:"), nullptr },
        { wxS("news:"), nullptr },
        { wxS("notes:"), nullptr },
        { wxS("nntp:"), nullptr },
        { wxS("onenote:"), nullptr },
        { wxS("outlook:"), nullptr },
        { wxS("prospero:"), nullptr },
        { wxS("tel:"), nullptr },
        { wxS("telnet:"), nullptr },
        { wxS("wais:"), nullptr },
        { wxS("webcal:"), nullptr },
        { wxS("www."), wxS("http://") },
        { wxS("ftp."), wxS("ftp://") },
        { wxS("\\\\"), nullptr }
    };

    const auto isBoundary = [](wxUniChar ch)
    {
        // A scheme embedded in an identifier or host name isn't a new URL.
        // All other punctuation is a legal word boundary, matching RichEdit's
        // ability to detect links after e.g. '(' or ':'.
        return !wxIsalnum(static_cast<wxChar>(ch)) &&
               ch != '_' && ch != '-' && ch != '.' && ch != '+';
    };
    const auto isTerminator = [](wxUniChar ch)
    {
        return ch <= 0x20 || ch == '"' || ch == '\'' ||
               ch == '<' || ch == '>' || ch == '`';
    };
    const auto isAlwaysTrailingPunctuation = [](wxUniChar ch)
    {
        return ch == '.' || ch == ',' || ch == ';' || ch == ':' ||
               ch == '!' || ch == '?';
    };
    const auto matchingOpen = [](wxUniChar ch) -> wxUniChar
    {
        switch ( static_cast<wxChar>(ch) )
        {
            case ')': return '(';
            case ']': return '[';
            case '}': return '{';
            default: return 0;
        }
    };

    std::vector<wxWinUIAutoUrlRange> ranges;
    const std::size_t maximumPosition =
        static_cast<std::size_t>((std::numeric_limits<long>::max)());
    const std::size_t searchableLength =
        wxMin(value.length(), maximumPosition);
    for ( std::size_t i = 0; i < searchableLength; ++i )
    {
        if ( i != 0 && !isBoundary(value[i - 1]) )
            continue;

        const Scheme *matched = nullptr;
        std::size_t prefixLength = 0;
        for ( const Scheme& scheme : schemes )
        {
            const wxString prefix(scheme.prefix);
            if ( i + prefix.length() <= searchableLength &&
                 value.Mid(i, prefix.length()).CmpNoCase(prefix) == 0 )
            {
                matched = &scheme;
                prefixLength = prefix.length();
                break;
            }
        }
        if ( !matched )
            continue;

        std::size_t end = i + prefixLength;
        while ( end < searchableLength && !isTerminator(value[end]) )
            ++end;
        while ( end > i + prefixLength &&
                isAlwaysTrailingPunctuation(value[end - 1]) )
        {
            --end;
        }

        // Keep balanced parentheses/brackets inside a URL, but exclude an
        // unmatched closing delimiter belonging to surrounding prose.
        for ( ;; )
        {
            if ( end <= i + prefixLength )
                break;
            const wxUniChar open = matchingOpen(value[end - 1]);
            if ( !open )
                break;

            unsigned opens = 0;
            unsigned closes = 0;
            for ( std::size_t n = i + prefixLength; n < end; ++n )
            {
                if ( value[n] == open )
                    ++opens;
                else if ( value[n] == value[end - 1] )
                    ++closes;
            }
            if ( closes <= opens )
                break;
            --end;
        }
        if ( end == i + prefixLength )
            continue;

        wxString target = value.Mid(i, end - i);
        if ( matched->targetPrefix )
            target.Prepend(matched->targetPrefix);
        ranges.push_back(
            { static_cast<long>(i), static_cast<long>(end), target });
        i = end - 1;
    }
    return ranges;
}

bool wxWinUIGetTOMLinkTarget(const winrt::hstring& link,
                             wxString *target)
{
    if ( !target )
        return false;

    wxString value = wxWinUIFromHString(link);

    // RichEdit is allowed to prefix a friendly-name hyperlink with the TOM
    // link sentinel. The setter accepts both forms and the getter is free to
    // return either one, so compare the semantic URL rather than one of its
    // two wire representations.
    if ( !value.empty() && value[0] == static_cast<wxChar>(0xfddf) )
        value.erase(0, 1);

    if ( value.empty() )
    {
        return false;
    }

    // The setter requires the quoted TOM wire form. Depending on the RichEdit
    // generation, the getter can expose either that form or the decoded URL.
    // Both prove the same active link as long as the exact payload matches.
    if ( value.length() >= 2 && value[0] == '"' &&
         value[value.length() - 1] == '"' )
    {
        value = value.Mid(1, value.length() - 2);
    }

    if ( value.empty() )
        return false;

    *target = value;
    return true;
}

long wxWinUIClampTextPos(long pos, long len)
{
    if ( pos < 0 )
        return len;
    if ( pos > len )
        return len;
    return pos;
}

bool wxWinUIUsesTwoCharacterNewlines(const wxTextCtrl& control)
{
#if defined(__WXMSW__)
    return control.IsMultiLine() &&
           !control.HasFlag(wxTE_RICH | wxTE_RICH2);
#else
    wxUnusedVar(control);
    return false;
#endif
}

wxWinUITextPositionMap wxWinUIMakeTextPositionMap(
    const wxTextCtrl& control,
    const wxString& value)
{
    return wxWinUITextPositionMap(
        value,
        control.IsMultiLine(),
        wxWinUIUsesTwoCharacterNewlines(control));
}

MUXC::ScrollViewer wxWinUIGetCurrentTextScrollViewer(
    const MUXC::TextBox& textBox)
{
    if ( !textBox )
        return nullptr;

    try
    {
        const auto control =
            textBox.try_as<MUXC::IControlProtected>();
        return control
                 ? control.GetTemplateChild(L"ContentElement").
                       try_as<MUXC::ScrollViewer>()
                 : nullptr;
    }
    catch ( const winrt::hresult_error& )
    {
        return nullptr;
    }
}

MUXC::ScrollViewer wxWinUIGetCurrentTextScrollViewer(
    const MUXC::RichEditBox& richEditBox)
{
    if ( !richEditBox )
        return nullptr;

    try
    {
        const auto control =
            richEditBox.try_as<MUXC::IControlProtected>();
        return control
                 ? control.GetTemplateChild(L"ContentElement").
                       try_as<MUXC::ScrollViewer>()
                 : nullptr;
    }
    catch ( const winrt::hresult_error& )
    {
        return nullptr;
    }
}

bool wxWinUIIsCurrentTextScrollView(
    const MUXC::TextBox& textBox,
    const MUXC::ScrollViewer& scroll,
    const MUX::UIElement& view = nullptr)
{
    if ( !textBox || !scroll )
        return false;

    try
    {
        const MUXC::ScrollViewer current =
            wxWinUIGetCurrentTextScrollViewer(textBox);
        if ( !current || current != scroll )
            return false;

        return !view ||
               current.Content().try_as<MUX::UIElement>() == view;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxWinUIIsCurrentTextScrollView(
    const MUXC::RichEditBox& richEditBox,
    const MUXC::ScrollViewer& scroll,
    const MUX::UIElement& view = nullptr)
{
    if ( !richEditBox || !scroll )
        return false;

    try
    {
        const MUXC::ScrollViewer current =
            wxWinUIGetCurrentTextScrollViewer(richEditBox);
        if ( !current || current != scroll )
            return false;

        return !view ||
               current.Content().try_as<MUX::UIElement>() == view;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxWinUIIsFiniteRect(
    const winrt::Windows::Foundation::Rect& rect);
bool wxWinUIIsFinitePoint(
    const winrt::Windows::Foundation::Point& point);

enum class wxWinUITextViewportState
{
    Unavailable,
    Visible,
    Outside
};

enum : unsigned
{
    wxWinUIRichVisibilityReflow = 1u << 0,
    wxWinUIRichVisibilityUnavailable = 1u << 1,
    wxWinUIRichVisibilityExactRequested = 1u << 2,
    wxWinUIRichVisibilityExactAccepted = 1u << 3,
    wxWinUIRichVisibilityCoarseRequested = 1u << 4,
    wxWinUIRichVisibilityCoarseAccepted = 1u << 5,
    wxWinUIRichVisibilityVisible = 1u << 6,
    wxWinUIRichVisibilityMeasured = 1u << 7
};

constexpr unsigned wxWinUIRichVisibilityMaxPasses = 8;

template <typename TextPeer>
wxWinUITextViewportState wxWinUIProjectTextViewTargetToViewport(
    const TextPeer& peer,
    const MUXC::ScrollViewer& scroll,
    const MUX::UIElement& view,
    const winrt::Windows::Foundation::Rect& documentTarget)
{
    if ( !wxWinUIIsFiniteRect(documentTarget) ||
         !wxWinUIIsCurrentTextScrollView(peer, scroll, view) )
    {
        return wxWinUITextViewportState::Unavailable;
    }

    try
    {
        const auto viewToScroll = view.TransformToVisual(scroll);
        if ( !viewToScroll ||
             !wxWinUIIsCurrentTextScrollView(peer, scroll, view) )
        {
            return wxWinUITextViewportState::Unavailable;
        }

        const winrt::Windows::Foundation::Rect viewportTarget =
            viewToScroll.TransformBounds(documentTarget);
        if ( !wxWinUIIsFiniteRect(viewportTarget) ||
             !wxWinUIIsCurrentTextScrollView(peer, scroll, view) )
        {
            return wxWinUITextViewportState::Unavailable;
        }

        const double viewportWidth =
            scroll.ViewportWidth() > 0.0
                ? scroll.ViewportWidth()
                : scroll.ActualWidth();
        if ( !wxWinUIIsCurrentTextScrollView(peer, scroll, view) )
            return wxWinUITextViewportState::Unavailable;
        const double viewportHeight =
            scroll.ViewportHeight() > 0.0
                ? scroll.ViewportHeight()
                : scroll.ActualHeight();
        if ( !wxWinUIIsCurrentTextScrollView(peer, scroll, view) ||
             !(viewportWidth > 0.0) || !(viewportHeight > 0.0) ||
             !std::isfinite(viewportWidth) ||
             !std::isfinite(viewportHeight) )
        {
            return wxWinUITextViewportState::Unavailable;
        }

        constexpr double Tolerance = 0.75;
        const double right = viewportTarget.X + viewportTarget.Width;
        const double bottom = viewportTarget.Y + viewportTarget.Height;
        return viewportTarget.X >= -Tolerance &&
                       viewportTarget.Y >= -Tolerance &&
                       right <= viewportWidth + Tolerance &&
                       bottom <= viewportHeight + Tolerance
                   ? wxWinUITextViewportState::Visible
                   : wxWinUITextViewportState::Outside;
    }
    catch ( const winrt::hresult_error& )
    {
        return wxWinUITextViewportState::Unavailable;
    }
}

template <typename TextPeer>
bool wxWinUIStartTextViewBringIntoView(
    const TextPeer& peer,
    const MUXC::ScrollViewer& scroll,
    const MUX::UIElement& view,
    const winrt::Windows::Foundation::Rect& documentTarget)
{
    if ( !wxWinUIIsFiniteRect(documentTarget) ||
         !wxWinUIIsCurrentTextScrollView(peer, scroll, view) )
    {
        return false;
    }

    try
    {
        MUX::BringIntoViewOptions options;
        options.AnimationDesired(false);
        options.TargetRect(
            winrt::box_value(documentTarget).
                as<winrt::Windows::Foundation::IReference<
                    winrt::Windows::Foundation::Rect>>());
        if ( !wxWinUIIsCurrentTextScrollView(peer, scroll, view) )
            return false;
        view.StartBringIntoView(options);
        return wxWinUIIsCurrentTextScrollView(peer, scroll, view);
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

template <typename TextPeer>
bool wxWinUIChangeTextViewToTarget(
    const TextPeer& peer,
    const MUXC::ScrollViewer& scroll,
    const MUX::UIElement& view,
    const winrt::Windows::Foundation::Rect& documentTarget)
{
    if ( !wxWinUIIsFiniteRect(documentTarget) ||
         !wxWinUIIsCurrentTextScrollView(peer, scroll, view) )
    {
        return false;
    }

    try
    {
        const auto viewToScroll = view.TransformToVisual(scroll);
        if ( !viewToScroll ||
             !wxWinUIIsCurrentTextScrollView(peer, scroll, view) )
        {
            return false;
        }

        const winrt::Windows::Foundation::Rect viewportTarget =
            viewToScroll.TransformBounds(documentTarget);
        const auto origin =
            viewToScroll.TransformPoint({ 0.0f, 0.0f });
        const auto unitX =
            viewToScroll.TransformPoint({ 1.0f, 0.0f });
        const auto unitY =
            viewToScroll.TransformPoint({ 0.0f, 1.0f });
        if ( !wxWinUIIsFiniteRect(viewportTarget) ||
             !wxWinUIIsFinitePoint(origin) ||
             !wxWinUIIsFinitePoint(unitX) ||
             !wxWinUIIsFinitePoint(unitY) ||
             !wxWinUIIsCurrentTextScrollView(peer, scroll, view) )
        {
            return false;
        }

        const double unitXScale = std::hypot(
            static_cast<double>(unitX.X - origin.X),
            static_cast<double>(unitX.Y - origin.Y));
        const double unitYScale = std::hypot(
            static_cast<double>(unitY.X - origin.X),
            static_cast<double>(unitY.Y - origin.Y));
        const double viewportWidth =
            scroll.ViewportWidth() > 0.0
                ? scroll.ViewportWidth()
                : scroll.ActualWidth();
        const double viewportHeight =
            scroll.ViewportHeight() > 0.0
                ? scroll.ViewportHeight()
                : scroll.ActualHeight();
        if ( !(unitXScale > 0.0) || !(unitYScale > 0.0) ||
             !(viewportWidth > 0.0) || !(viewportHeight > 0.0) ||
             !std::isfinite(unitXScale) || !std::isfinite(unitYScale) ||
             !std::isfinite(viewportWidth) ||
             !std::isfinite(viewportHeight) ||
             !wxWinUIIsCurrentTextScrollView(peer, scroll, view) )
        {
            return false;
        }

        constexpr double Tolerance = 0.75;
        double horizontalDelta = 0.0;
        if ( viewportTarget.X < -Tolerance )
            horizontalDelta = viewportTarget.X;
        else if ( viewportTarget.X + viewportTarget.Width >
                      viewportWidth + Tolerance )
        {
            horizontalDelta =
                viewportTarget.X + viewportTarget.Width - viewportWidth;
        }

        double verticalDelta = 0.0;
        if ( viewportTarget.Y < -Tolerance )
            verticalDelta = viewportTarget.Y;
        else if ( viewportTarget.Y + viewportTarget.Height >
                      viewportHeight + Tolerance )
        {
            verticalDelta =
                viewportTarget.Y + viewportTarget.Height - viewportHeight;
        }

        winrt::Windows::Foundation::IReference<double> horizontal{ nullptr };
        winrt::Windows::Foundation::IReference<double> vertical{ nullptr };
        if ( horizontalDelta != 0.0 )
        {
            const double offset = wxMax(
                0.0,
                wxMin(scroll.ScrollableWidth(),
                      scroll.HorizontalOffset() +
                          horizontalDelta / unitXScale));
            horizontal = winrt::box_value(offset).
                as<winrt::Windows::Foundation::IReference<double>>();
        }
        if ( verticalDelta != 0.0 )
        {
            const double offset = wxMax(
                0.0,
                wxMin(scroll.ScrollableHeight(),
                      scroll.VerticalOffset() +
                          verticalDelta / unitYScale));
            vertical = winrt::box_value(offset).
                as<winrt::Windows::Foundation::IReference<double>>();
        }

        if ( !horizontal && !vertical )
            return true;
        if ( !wxWinUIIsCurrentTextScrollView(peer, scroll, view) )
            return false;

        return scroll.ChangeView(horizontal, vertical, nullptr, true) &&
               wxWinUIIsCurrentTextScrollView(peer, scroll, view);
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxWinUIProjectRichTextTarget(
    const MUXC::RichEditBox& richEditBox,
    const MUXC::ScrollViewer& scroll,
    const MUX::UIElement& view,
    const winrt::Windows::Foundation::Rect& documentTarget,
    winrt::Windows::Foundation::Rect *viewportTarget,
    double *viewportWidthOut = nullptr,
    double *viewportHeightOut = nullptr)
{
    if ( !viewportTarget || !wxWinUIIsFiniteRect(documentTarget) ||
         !wxWinUIIsCurrentTextScrollView(
             richEditBox, scroll, view) )
    {
        return false;
    }

    try
    {
        // With ClientCoordinates|IncludeInset|AllowOffClient, RichEdit's TOM
        // point is in the complete document client: it already owns the text
        // inset, but it is deliberately not clipped or shifted by the XAML
        // ScrollViewer. Its exact viewport projection is therefore the TOM
        // point minus the published ScrollViewer offsets. Do not add the
        // ContentElement origin: doing so counts IncludeInset twice, while
        // TransformToVisual can also lag ChangeView's synchronous offset.
        const double horizontalOffset = scroll.HorizontalOffset();
        const double verticalOffset = scroll.VerticalOffset();
        const double viewportWidth =
            scroll.ViewportWidth() > 0.0
                ? scroll.ViewportWidth()
                : scroll.ActualWidth();
        const double viewportHeight =
            scroll.ViewportHeight() > 0.0
                ? scroll.ViewportHeight()
                : scroll.ActualHeight();
        if ( horizontalOffset < 0.0 || verticalOffset < 0.0 ||
             !(viewportWidth > 0.0) || !(viewportHeight > 0.0) ||
             !std::isfinite(horizontalOffset) ||
             !std::isfinite(verticalOffset) ||
             !std::isfinite(viewportWidth) ||
             !std::isfinite(viewportHeight) ||
             !wxWinUIIsCurrentTextScrollView(
                 richEditBox, scroll, view) )
        {
            return false;
        }

        winrt::Windows::Foundation::Rect projected = documentTarget;
        projected.X = static_cast<float>(
            projected.X - horizontalOffset);
        projected.Y = static_cast<float>(
            projected.Y - verticalOffset);
        if ( !wxWinUIIsFiniteRect(projected) )
            return false;

        if ( viewportWidthOut )
            *viewportWidthOut = viewportWidth;
        if ( viewportHeightOut )
            *viewportHeightOut = viewportHeight;
        *viewportTarget = projected;
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

wxWinUITextViewportState wxWinUIProjectTextViewTargetToViewport(
    const MUXC::RichEditBox& richEditBox,
    const MUXC::ScrollViewer& scroll,
    const MUX::UIElement& view,
    const winrt::Windows::Foundation::Rect& documentTarget)
{
    winrt::Windows::Foundation::Rect viewportTarget{};
    double viewportWidth = 0.0;
    double viewportHeight = 0.0;
    if ( !wxWinUIProjectRichTextTarget(
             richEditBox, scroll, view, documentTarget,
             &viewportTarget, &viewportWidth, &viewportHeight) )
    {
        return wxWinUITextViewportState::Unavailable;
    }

    constexpr double Tolerance = 0.75;
    const double right = viewportTarget.X + viewportTarget.Width;
    const double bottom = viewportTarget.Y + viewportTarget.Height;
    return viewportTarget.X >= -Tolerance &&
                   viewportTarget.Y >= -Tolerance &&
                   right <= viewportWidth + Tolerance &&
                   bottom <= viewportHeight + Tolerance
               ? wxWinUITextViewportState::Visible
               : wxWinUITextViewportState::Outside;
}

bool wxWinUIChangeTextViewToTarget(
    const MUXC::RichEditBox& richEditBox,
    const MUXC::ScrollViewer& scroll,
    const MUX::UIElement& view,
    const winrt::Windows::Foundation::Rect& documentTarget)
{
    winrt::Windows::Foundation::Rect viewportTarget{};
    double viewportWidth = 0.0;
    double viewportHeight = 0.0;
    if ( !wxWinUIProjectRichTextTarget(
             richEditBox, scroll, view, documentTarget,
             &viewportTarget, &viewportWidth, &viewportHeight) )
    {
        return false;
    }

    constexpr double Tolerance = 0.75;
    double horizontalDelta = 0.0;
    if ( viewportTarget.X < -Tolerance )
        horizontalDelta = viewportTarget.X;
    else if ( viewportTarget.X + viewportTarget.Width >
                  viewportWidth + Tolerance )
    {
        horizontalDelta =
            viewportTarget.X + viewportTarget.Width - viewportWidth;
    }

    double verticalDelta = 0.0;
    if ( viewportTarget.Y < -Tolerance )
        verticalDelta = viewportTarget.Y;
    else if ( viewportTarget.Y + viewportTarget.Height >
                  viewportHeight + Tolerance )
    {
        verticalDelta =
            viewportTarget.Y + viewportTarget.Height - viewportHeight;
    }

    try
    {
        winrt::Windows::Foundation::IReference<double> horizontal{ nullptr };
        winrt::Windows::Foundation::IReference<double> vertical{ nullptr };
        if ( horizontalDelta != 0.0 )
        {
            horizontal = winrt::box_value(wxMax(
                0.0,
                wxMin(scroll.ScrollableWidth(),
                      scroll.HorizontalOffset() + horizontalDelta))).
                as<winrt::Windows::Foundation::IReference<double>>();
        }
        if ( verticalDelta != 0.0 )
        {
            vertical = winrt::box_value(wxMax(
                0.0,
                wxMin(scroll.ScrollableHeight(),
                      scroll.VerticalOffset() + verticalDelta))).
                as<winrt::Windows::Foundation::IReference<double>>();
        }

        if ( !horizontal && !vertical )
            return true;
        if ( !wxWinUIIsCurrentTextScrollView(
                 richEditBox, scroll, view) )
        {
            return false;
        }
        return scroll.ChangeView(horizontal, vertical, nullptr, true) &&
               wxWinUIIsCurrentTextScrollView(
                   richEditBox, scroll, view);
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

wxWinUIKeyboardModifiers wxWinUITextCtrlGetModifiers(WXMSG *msg)
{
    wxWinUIKeyboardModifiers modifiers = {};
#ifdef WXWINUI_TEST_SUPPORT
    if ( wxWinUI3GetKeyboardModifiersOverrideForTesting(&modifiers) )
        return modifiers;
#endif

    modifiers.shiftDown = wxIsShiftDown();
    modifiers.controlDown = wxIsCtrlDown();
    modifiers.leftAltDown =
        msg && (HIWORD(msg->lParam) & KF_ALTDOWN) != 0;
    modifiers.rightAltDown = false;
    return modifiers;
}

template <typename TextPeer>
MUXC::ScrollViewer wxWinUIResolveTextScrollViewer(
    const wxTextCtrl& owner,
    const TextPeer& textBox)
{
    if ( !textBox )
        return nullptr;

    try
    {
        // A control can be added after its TLW is already visible. Its slot
        // is then only scheduled for the next coalesced host flush, and
        // ApplyTemplate() alone can't realize a peer which isn't in the
        // visual tree yet. Publish pending slot geometry first, then force
        // the template/layout pass needed by ContentElement.
        const wxWeakRef<wxWindow> ownerRef(
            const_cast<wxTextCtrl *>(&owner));
        wxWinUITopLevelHost * const initialHost =
            wxWinUITopLevelHost::FindSlotOwner(&owner);
        wxWinUISlot * const initialSlot =
            initialHost ? initialHost->FindSlot(&owner) : nullptr;
        const auto isCurrentTextBox = [&]()
        {
            wxWindow *liveOwner = ownerRef.get();
            if ( !liveOwner )
                return false;
            if ( !initialHost )
                return true;

            wxWinUITopLevelHost * const liveHost =
                wxWinUITopLevelHost::FindSlotOwner(liveOwner);
            wxWinUISlot * const liveSlot =
                liveHost == initialHost
                    ? liveHost->FindSlot(liveOwner)
                    : nullptr;
            if ( !liveSlot || liveSlot != initialSlot )
                return false;

            const MUX::UIElement content = liveSlot->GetContent();
            liveOwner = ownerRef.get();
            wxWinUITopLevelHost * const finalHost =
                liveOwner
                    ? wxWinUITopLevelHost::FindSlotOwner(liveOwner)
                    : nullptr;
            wxWinUISlot * const finalSlot =
                finalHost == liveHost
                    ? finalHost->FindSlot(liveOwner)
                    : nullptr;
            return liveOwner && finalSlot == liveSlot &&
                   content == textBox;
        };
        if ( initialHost )
        {
            initialHost->FlushSync();
            if ( !isCurrentTextBox() )
                return nullptr;

            // FlushSync publishes the slot's Width/Height, but it doesn't
            // synchronously arrange the shared XAML root. A descendant-only
            // UpdateLayout() can consequently retain the previous viewport
            // (or even a 0x0 one) after wxWindow::SetSize(). Arrange from the
            // slot container before resolving/querying the template part.
            wxWindow *liveOwner = ownerRef.get();
            wxWinUITopLevelHost * const liveHost =
                liveOwner
                    ? wxWinUITopLevelHost::FindSlotOwner(liveOwner)
                    : nullptr;
            wxWinUISlot * const slot =
                liveHost ? liveHost->FindSlot(liveOwner) : nullptr;
            if ( !slot )
                return nullptr;

            const MUXC::Canvas root = liveHost->Root();
            const MUXC::Grid container = slot->GetContainer();
            if ( root )
                root.UpdateLayout();

            // UpdateLayout() is a synchronous XAML re-entrancy boundary.
            // Never retain a raw host/slot across it without proving that the
            // owner is still registered in the same slot.
            liveOwner = ownerRef.get();
            wxWinUITopLevelHost * const hostAfterRoot =
                liveOwner
                    ? wxWinUITopLevelHost::FindSlotOwner(liveOwner)
                    : nullptr;
            wxWinUISlot * const slotAfterRoot =
                hostAfterRoot == liveHost
                    ? hostAfterRoot->FindSlot(liveOwner)
                    : nullptr;
            if ( slotAfterRoot != slot ||
                 !slotAfterRoot ||
                 slotAfterRoot->GetContainer() != container )
            {
                return nullptr;
            }

            container.UpdateLayout();

            liveOwner = ownerRef.get();
            wxWinUITopLevelHost * const hostAfterContainer =
                liveOwner
                    ? wxWinUITopLevelHost::FindSlotOwner(liveOwner)
                    : nullptr;
            wxWinUISlot * const slotAfterContainer =
                hostAfterContainer == liveHost
                    ? hostAfterContainer->FindSlot(liveOwner)
                    : nullptr;
            if ( slotAfterContainer != slot ||
                 !slotAfterContainer ||
                 slotAfterContainer->GetContainer() != container )
            {
                return nullptr;
            }
        }

        textBox.ApplyTemplate();
        if ( !isCurrentTextBox() )
            return nullptr;
        const auto control =
            textBox.try_as<MUXC::IControlProtected>();
        if ( !isCurrentTextBox() )
            return nullptr;
        if ( control )
        {
            if ( const auto scroll =
                    control.GetTemplateChild(L"ContentElement").
                        try_as<MUXC::ScrollViewer>() )
            {
                return isCurrentTextBox() ? scroll : nullptr;
            }

            // The first ApplyTemplate() can succeed while its named parts
            // are still pending their initial layout. Only pay for a forced
            // layout on this unresolved path; geometry queries on an already
            // realized control stay cheap.
            textBox.UpdateLayout();
            if ( !isCurrentTextBox() )
                return nullptr;
            textBox.ApplyTemplate();
            if ( !isCurrentTextBox() )
                return nullptr;
            if ( const auto scroll =
                    control.GetTemplateChild(L"ContentElement").
                        try_as<MUXC::ScrollViewer>() )
            {
                return isCurrentTextBox() ? scroll : nullptr;
            }
        }
    }
    catch ( const winrt::hresult_error& )
    {
    }

    return nullptr;
}

struct wxWinUITextEdge
{
    winrt::Windows::Foundation::Point point{};
    double height = 0.0;
};

bool wxWinUIIsUsableTextRect(
    const winrt::Windows::Foundation::Rect& rect)
{
    return std::isfinite(rect.X) &&
           std::isfinite(rect.Y) &&
           std::isfinite(rect.Height) &&
           rect.Height > 0.0;
}

bool wxWinUIIsUsableTextEdge(const wxWinUITextEdge& edge)
{
    // Scrolling legitimately translates realized TextBoxView edges into
    // negative coordinates. RichEdit's not-realized sentinel is instead
    // distinguished by its zero height; never manufacture a positive height
    // for it below.
    return std::isfinite(edge.point.X) &&
           std::isfinite(edge.point.Y) &&
           std::isfinite(edge.height) &&
           edge.height > 0.0;
}

bool wxWinUIIsSimpleDirectionalRun(const wxString& text,
                                   MUX::FlowDirection direction)
{
    // Measuring a prefix with a standalone TextBlock only preserves the
    // caret edge when the run has one visual direction. In mixed bidi text,
    // the logical end of the prefix need not be at either measured outer
    // edge, so prefer no answer over publishing a wrong caret position.
    constexpr std::size_t MaxMeasuredRunLength = 4096;
    if ( text.empty() || text.length() > MaxMeasuredRunLength )
        return false;

    const wxWCharBuffer wide = text.wc_str();
    std::vector<WORD> classes(text.length());
    if ( !::GetStringTypeW(CT_CTYPE2,
                           wide.data(),
                           static_cast<int>(text.length()),
                           classes.data()) )
    {
        return false;
    }

    const bool rtl = direction == MUX::FlowDirection::RightToLeft;
    for ( std::size_t i = 0; i < classes.size(); ++i )
    {
        const wchar_t ch = wide.data()[i];
        if ( ch < 0x20 || ch == 0x7f ||
             (ch >= 0xd800 && ch <= 0xdfff) ||
             ch == 0x061c || ch == 0x200e || ch == 0x200f ||
             (ch >= 0x202a && ch <= 0x202e) ||
             (ch >= 0x2066 && ch <= 0x2069) )
        {
            return false;
        }

        const WORD cls = classes[i];
        if ( cls == C2_LEFTTORIGHT )
        {
            if ( rtl )
                return false;
        }
        else if ( cls == C2_RIGHTTOLEFT )
        {
            if ( !rtl )
                return false;
        }
        else if ( cls == C2_WHITESPACE )
        {
            if ( ch != L' ' )
                return false;
        }
        else if ( !rtl &&
                  (cls == C2_EUROPENUMBER ||
                   cls == C2_EUROPESEPARATOR ||
                   cls == C2_EUROPETERMINATOR ||
                   cls == C2_COMMONSEPARATOR ||
                   (cls == C2_OTHERNEUTRAL &&
                    ch >= 0x21 && ch <= 0x7e)) )
        {
            // These classes retain their natural left-to-right order in an
            // otherwise LTR run. They are deliberately rejected for RTL,
            // where they introduce an embedded directional segment.
            continue;
        }
        else
        {
            return false;
        }
    }

    return true;
}

bool wxWinUISynthesizeWrappedTrailingRect(
    const MUXC::TextBox& textBox,
    const wxString& value,
    long logical,
    const winrt::Windows::Foundation::Rect& leading,
    winrt::Windows::Foundation::Rect *trailing)
{
    if ( !trailing || logical < 0 ||
         static_cast<std::size_t>(logical) >= value.length() ||
         value[static_cast<std::size_t>(logical)] == '\n' ||
         !wxWinUIIsUsableTextRect(leading) )
    {
        return false;
    }

    // WinUI 3's RichEdit-backed TextBox can return the trailing edge in the
    // unwrapped first-line coordinate space while the leading edge is in the
    // realized wrapped line. Reconstruct only this demonstrably incoherent
    // case. Walk only the adjacent, realized physical line: a global binary
    // search can probe a virtualized middle range and mistake RichEdit's
    // zero-height sentinel for layout. The walk is strictly bounded, and an
    // answer is published only after proving the line boundary. XAML then
    // measures the complete shaped run so proportional fonts, kerning and
    // character spacing are preserved.
    const double tolerance = wxMax(1.0, leading.Height * 0.20);
    constexpr std::size_t MaxInspectedPhysicalLineUnits = 4096;
    std::size_t inspected = 0;
    long lineStart = logical;
    bool lineBoundaryProved = lineStart == 0;
    while ( lineStart > 0 )
    {
        const std::size_t previousIndex =
            static_cast<std::size_t>(lineStart - 1);
        if ( value[previousIndex] == '\n' )
        {
            lineBoundaryProved = true;
            break;
        }
        if ( inspected == MaxInspectedPhysicalLineUnits )
            return false;

        const auto previous = textBox.GetRectFromCharacterIndex(
            static_cast<int32_t>(lineStart - 1), false);
        ++inspected;
        if ( !wxWinUIIsUsableTextRect(previous) )
            return false;
        if ( std::fabs(previous.Y - leading.Y) > tolerance )
        {
            lineBoundaryProved = true;
            break;
        }

        --lineStart;
        if ( lineStart == 0 )
            lineBoundaryProved = true;
    }

    if ( !lineBoundaryProved )
        return false;

    const auto lineLeading = textBox.GetRectFromCharacterIndex(
        static_cast<int32_t>(lineStart), false);
    if ( !wxWinUIIsUsableTextRect(lineLeading) ||
         std::fabs(lineLeading.Y - leading.Y) > tolerance )
    {
        return false;
    }

    if ( textBox.TextAlignment() == MUX::TextAlignment::Justify ||
         textBox.TextAlignment() == MUX::TextAlignment::DetectFromContent )
    {
        return false;
    }

    const wxString run =
        value.Mid(
            static_cast<std::size_t>(lineStart),
            static_cast<std::size_t>(logical - lineStart + 1));
    if ( !wxWinUIIsSimpleDirectionalRun(run, textBox.FlowDirection()) )
        return false;

    const auto measureWidth =
        [&textBox](const wxString& measured, double *width)
        {
            if ( measured.empty() )
            {
                *width = 0.0;
                return true;
            }

            MUXC::TextBlock measure;
            measure.Text(wxWinUIToHString(measured));
            measure.TextWrapping(MUX::TextWrapping::NoWrap);
            measure.FontFamily(textBox.FontFamily());
            measure.FontSize(textBox.FontSize());
            measure.FontStyle(textBox.FontStyle());
            measure.FontWeight(textBox.FontWeight());
            measure.FontStretch(textBox.FontStretch());
            measure.CharacterSpacing(textBox.CharacterSpacing());
            measure.FlowDirection(textBox.FlowDirection());
            measure.Language(textBox.Language());
            const float infinity =
                (std::numeric_limits<float>::infinity)();
            measure.Measure({ infinity, infinity });
            *width = measure.DesiredSize().Width;
            return std::isfinite(*width) && *width >= 0.0;
        };

    double prefixWidth = 0.0;
    double width = 0.0;
    if ( !measureWidth(run.Left(run.length() - 1), &prefixWidth) ||
         !measureWidth(run, &width) )
    {
        return false;
    }

    const double directionSign =
        textBox.FlowDirection() == MUX::FlowDirection::RightToLeft
            ? -1.0
            : 1.0;
    const double advance = width - prefixWidth;
    if ( !std::isfinite(advance) || advance <= 0.25 )
        return false;

    // RichEdit's leading edge is authoritative for the realized wrapped line.
    // The standalone TextBlock can have a different absolute prefix origin at
    // fractional/raster scales, even though its local shaped glyph advance is
    // valid. Anchor only that incremental advance to the native leading edge;
    // anchoring the complete prefix to lineLeading falsely rejects the final
    // character and makes HitTest fall back to an unwrapped model answer.
    *trailing = leading;
    trailing->X = static_cast<float>(
        leading.X + directionSign * advance);
    return wxWinUIIsUsableTextRect(*trailing);
}

bool wxWinUIGetTextEdge(const MUXC::TextBox& textBox,
                        const wxString& value,
                        long logical,
                        wxWinUITextEdge *edge)
{
    if ( !textBox || !edge || logical < 0 ||
         static_cast<std::size_t>(logical) > value.length() )
    {
        return false;
    }

    try
    {
        winrt::Windows::Foundation::Rect rect;
        if ( logical < static_cast<long>(value.length()) )
        {
            rect = textBox.GetRectFromCharacterIndex(
                static_cast<int32_t>(logical), false);
        }
        else if ( logical > 0 )
        {
            const int32_t lastIndex =
                static_cast<int32_t>(logical - 1);
            rect = textBox.GetRectFromCharacterIndex(lastIndex, true);

            if ( value[logical - 1] == '\n' )
            {
                // WinUI reports both edges of a terminal LF on the preceding
                // physical line. wx positions the end caret at the start of
                // the following empty line. Derive the empty-line X from the
                // preceding physical line's aligned span: left uses its
                // leading edge, right its trailing edge and centre their
                // midpoint.
                const auto leading =
                    textBox.GetRectFromCharacterIndex(lastIndex, false);
                int32_t lineStart = lastIndex;
                while ( lineStart > 0 &&
                        value[static_cast<std::size_t>(lineStart - 1)] != '\n' )
                {
                    --lineStart;
                }
                const auto lineLeading =
                    textBox.GetRectFromCharacterIndex(lineStart, false);
                if ( !wxWinUIIsUsableTextRect(rect) ||
                     !wxWinUIIsUsableTextRect(leading) ||
                     !wxWinUIIsUsableTextRect(lineLeading) )
                {
                    return false;
                }
                switch ( textBox.TextAlignment() )
                {
                    case MUX::TextAlignment::Right:
                        rect.X = leading.X;
                        break;

                    case MUX::TextAlignment::Center:
                        rect.X =
                            static_cast<float>(
                                (lineLeading.X + leading.X) / 2.0);
                        break;

                    case MUX::TextAlignment::Left:
                    case MUX::TextAlignment::Justify:
                    case MUX::TextAlignment::DetectFromContent:
                    default:
                        rect.X = lineLeading.X;
                        break;
                }
                const double lineHeight =
                    wxMax<double>(
                        wxMax<double>(leading.Height, rect.Height),
                        textBox.FontSize());
                rect.Y = static_cast<float>(leading.Y + lineHeight);
                rect.Height = static_cast<float>(lineHeight);
            }
            else
            {
                const auto leading =
                    textBox.GetRectFromCharacterIndex(lastIndex, false);
                const double tolerance =
                    wxWinUIIsUsableTextRect(leading)
                        ? wxMax(1.0, leading.Height * 0.20)
                        : 0.0;
                if ( !wxWinUIIsUsableTextRect(rect) ||
                     !wxWinUIIsUsableTextRect(leading) ||
                     std::fabs(rect.Y - leading.Y) > tolerance )
                {
                    if ( !wxWinUISynthesizeWrappedTrailingRect(
                             textBox, value, logical - 1, leading, &rect) )
                    {
                        return false;
                    }
                }
            }
        }
        else
        {
            // This point is local to the TextBoxView. Its transform to the
            // root already contains the TextBox padding.
            edge->point = { 0.0f, 0.0f };
            edge->height = textBox.FontSize();
            return wxWinUIIsUsableTextEdge(*edge);
        }

        if ( !wxWinUIIsUsableTextRect(rect) )
            return false;

        edge->point = { rect.X, rect.Y };
        edge->height = rect.Height;
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxWinUIIsPlausibleWrappedTextEdge(
    const MUXC::TextBox& textBox,
    const wxWinUITextEdge& edge)
{
    if ( !wxWinUIIsUsableTextEdge(edge) )
        return false;

    try
    {
        if ( textBox.TextWrapping() != MUX::TextWrapping::Wrap )
            return true;

        // RichEdit can expose a finite, positive but stale unwrapped edge for
        // the final character of a virtualized wrapped range. It is not a
        // usable layout answer: in Wrap mode a caret in TextBoxView coordinates
        // cannot be more than one line height outside the arranged TextBox.
        // Treat this exact geometric contradiction like the negative
        // virtualization sentinel so EnsurePositionVisible() realizes the
        // range through its bounded, selection-restoring transaction.
        const double arrangedWidth = textBox.ActualWidth();
        if ( !(arrangedWidth > 0.0) || !std::isfinite(arrangedWidth) )
            return true;

        const double tolerance = wxMax(1.0, edge.height);
        return edge.point.X >= -tolerance &&
               edge.point.X <= arrangedWidth + tolerance;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

struct wxWinUITextPhysicalLine
{
    long start = 0;
    long end = 0;
};

template <typename IsCurrent>
bool wxWinUIGetAutomationTextBoxLines(
    const MUXC::TextBox& textBox,
    const wxString& value,
    IsCurrent&& isCurrent,
    std::vector<wxWinUITextPhysicalLine> *lines)
{
    if ( !lines || !textBox || !isCurrent() )
        return false;

    // TextBox doesn't expose TOM, but its automation peer exposes the same
    // physical Line unit used by accessibility clients. Unlike
    // GetRectFromCharacterIndex(), TextPattern isn't limited to the currently
    // realized viewport, so prefer it for large virtualized documents. The
    // reconstruction check below makes this a strictly exact path: if a future
    // WinAppSDK changes line-range newline semantics, callers fall back to the
    // insertion-edge implementation instead of accepting shifted positions.
    MUXAP::AutomationPeer peer =
        MUXAP::FrameworkElementAutomationPeer::FromElement(textBox);
    if ( !isCurrent() )
        return false;
    if ( !peer )
    {
        peer = MUXAP::FrameworkElementAutomationPeer::CreatePeerForElement(
            textBox);
        if ( !peer || !isCurrent() )
            return false;
    }

    const winrt::Windows::Foundation::IInspectable pattern =
        peer.GetPattern(MUXAP::PatternInterface::Text);
    if ( !isCurrent() )
        return false;
    const MUXAPR::ITextProvider provider =
        pattern.try_as<MUXAPR::ITextProvider>();
    if ( !provider || !isCurrent() )
        return false;

    const MUXAPR::ITextRangeProvider document = provider.DocumentRange();
    if ( !document || !isCurrent() )
        return false;
    const MUXAPR::ITextRangeProvider cursor = document.Clone();
    if ( !cursor || !isCurrent() )
        return false;
    cursor.MoveEndpointByRange(
        MUXAT::TextPatternRangeEndpoint::End,
        cursor,
        MUXAT::TextPatternRangeEndpoint::Start);
    if ( !isCurrent() )
        return false;

    std::vector<wxWinUITextPhysicalLine> physicalLines;
    wxString reconstructed;
    long logical = 0;
    const std::size_t maximumLines = value.length() + 1;
    for ( std::size_t lineIndex = 0;
          lineIndex < maximumLines;
          ++lineIndex )
    {
        const MUXAPR::ITextRangeProvider line = cursor.Clone();
        if ( !line || !isCurrent() )
            return false;
        line.ExpandToEnclosingUnit(MUXAT::TextUnit::Line);
        if ( !isCurrent() )
            return false;

        const wxString raw =
            wxWinUITextPositionMap::NormalizeNewlines(
                wxWinUIFromHString(line.GetText(-1)));
        if ( !isCurrent() )
            return false;

        wxString content = raw;
        if ( content.EndsWith(wxS("\n")) )
            content.RemoveLast();
        if ( content.length() >
                 static_cast<std::size_t>(
                     (std::numeric_limits<long>::max)() - logical) )
        {
            return false;
        }

        const long end = logical + static_cast<long>(content.length());
        physicalLines.push_back({ logical, end });
        reconstructed += raw;
        if ( reconstructed.length() > value.length() )
            return false;
        logical += static_cast<long>(raw.length());

        const int moved = cursor.Move(MUXAT::TextUnit::Line, 1);
        if ( !isCurrent() )
            return false;
        if ( moved == 0 )
            break;
    }

    if ( reconstructed != value || physicalLines.empty() || !isCurrent() )
        return false;

    if ( value.EndsWith(wxS("\n")) &&
         physicalLines.back().start != static_cast<long>(value.length()) )
    {
        const long length = static_cast<long>(value.length());
        physicalLines.push_back({ length, length });
    }

    *lines = std::move(physicalLines);
    return true;
}

template <typename IsCurrent>
bool wxWinUIGetWrappedTextBoxLines(
    const wxTextCtrl& owner,
    const MUXC::TextBox& textBox,
    const wxString& value,
    IsCurrent&& isCurrent,
    std::vector<wxWinUITextPhysicalLine> *lines)
{
    if ( !lines || !textBox ||
         value.length() > static_cast<std::size_t>(
                              (std::numeric_limits<int32_t>::max)()) )
    {
        return false;
    }

    lines->clear();
    lines->push_back({ 0, 0 });
    if ( value.empty() )
        return true;

    try
    {
        MUXC::ScrollViewer scroll =
            wxWinUIResolveTextScrollViewer(owner, textBox);
        if ( !scroll || !isCurrent() )
            return false;
        textBox.UpdateLayout();
        if ( !isCurrent() )
            return false;
        scroll = wxWinUIGetCurrentTextScrollViewer(textBox);
        if ( !scroll || !isCurrent() )
            return false;
        scroll.UpdateLayout();
        if ( !isCurrent() ||
             wxWinUIGetCurrentTextScrollViewer(textBox) != scroll )
        {
            return false;
        }

        if ( wxWinUIGetAutomationTextBoxLines(
                 textBox, value, isCurrent, lines) )
        {
            return true;
        }
        if ( !isCurrent() )
            return false;
        lines->clear();
        lines->push_back({ 0, 0 });

        wxWinUITextEdge previous;
        if ( !wxWinUIGetTextEdge(textBox, value, 0, &previous) ||
             !wxWinUIIsPlausibleWrappedTextEdge(textBox, previous) ||
             !isCurrent() )
        {
            return false;
        }

        const long length = static_cast<long>(value.length());
        for ( long logical = 1; logical <= length; ++logical )
        {
            wxWinUITextEdge current;
            if ( !wxWinUIGetTextEdge(
                     textBox, value, logical, &current) ||
                 !wxWinUIIsPlausibleWrappedTextEdge(textBox, current) ||
                 !isCurrent() )
            {
                return false;
            }

            const double tolerance = wxMax(
                0.5,
                wxMin(previous.height, current.height) * 0.20);
            const bool explicitBreak =
                value[static_cast<std::size_t>(logical - 1)] == '\n';
            if ( explicitBreak ||
                 std::fabs(current.point.Y - previous.point.Y) > tolerance )
            {
                lines->back().end = explicitBreak ? logical - 1 : logical;
                lines->push_back({ logical, logical });
            }
            previous = current;
        }

        lines->back().end = length;
        return isCurrent();
    }
    catch ( const winrt::hresult_error& )
    {
        lines->clear();
        return false;
    }
}

bool wxWinUIGetTextCharacterSpan(
    const MUXC::TextBox& textBox,
    const wxString& value,
    long logical,
    wxWinUITextEdge *trailing)
{
    if ( !textBox || !trailing || logical < 0 ||
         static_cast<std::size_t>(logical) >= value.length() )
    {
        return false;
    }

    try
    {
        auto rect = textBox.GetRectFromCharacterIndex(
            static_cast<int32_t>(logical), true);
        const auto leading = textBox.GetRectFromCharacterIndex(
            static_cast<int32_t>(logical), false);
        const double tolerance =
            wxWinUIIsUsableTextRect(leading)
                ? wxMax(1.0, leading.Height * 0.20)
                : 0.0;
        if ( value[static_cast<std::size_t>(logical)] != '\n' &&
             (!wxWinUIIsUsableTextRect(rect) ||
              !wxWinUIIsUsableTextRect(leading) ||
              std::fabs(rect.Y - leading.Y) > tolerance) )
        {
            if ( !wxWinUISynthesizeWrappedTrailingRect(
                     textBox, value, logical, leading, &rect) )
            {
                return false;
            }
        }
        if ( !wxWinUIIsUsableTextRect(rect) )
            return false;

        trailing->point = { rect.X, rect.Y };
        trailing->height = rect.Height;
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

wxTextCtrl *wxWinUIResolveLiveTextOwner(
    const wxWeakRef<wxWindow>& self,
    const MUXC::TextBox& textBox,
    wxWinUITopLevelHost **hostOut = nullptr)
{
    wxTextCtrl *live = wxDynamicCast(self.get(), wxTextCtrl);
    wxWinUITopLevelHost *host =
        live ? wxWinUITopLevelHost::FindSlotOwner(live) : nullptr;
    wxWinUISlot *slot = host && live ? host->FindSlot(live) : nullptr;
    if ( !slot )
        return nullptr;

    const MUX::UIElement content = slot->GetContent();
    const MUXC::TextBox currentTextBox =
        content.try_as<MUXC::TextBox>();

    // Both projection calls above are COM boundaries. Re-resolve the weak wx
    // owner and exact slot before publishing either the owner or host.
    live = wxDynamicCast(self.get(), wxTextCtrl);
    wxWinUITopLevelHost * const currentHost =
        live ? wxWinUITopLevelHost::FindSlotOwner(live) : nullptr;
    wxWinUISlot * const currentSlot =
        currentHost ? currentHost->FindSlot(live) : nullptr;
    if ( !live || !currentHost || currentHost != host ||
         !currentSlot || currentSlot != slot ||
         currentTextBox != textBox )
    {
        return nullptr;
    }

    const MUX::UIElement currentContent = currentSlot->GetContent();
    live = wxDynamicCast(self.get(), wxTextCtrl);
    wxWinUITopLevelHost * const finalHost =
        live ? wxWinUITopLevelHost::FindSlotOwner(live) : nullptr;
    wxWinUISlot * const finalSlot =
        finalHost ? finalHost->FindSlot(live) : nullptr;
    if ( !live || finalHost != currentHost ||
         !finalSlot || finalSlot != currentSlot ||
         currentContent != content )
    {
        return nullptr;
    }

    if ( hostOut )
        *hostOut = finalHost;
    return live;
}

bool wxWinUITextViewPointToClient(
    const wxWeakRef<wxWindow>& self,
    const MUXC::TextBox& textBox,
    const MUXC::ScrollViewer& scroll,
    const winrt::Windows::Foundation::Point& point,
    wxPoint *clientPoint)
{
    if ( !clientPoint || !scroll )
        return false;

    wxTextCtrl *live =
        wxWinUIResolveLiveTextOwner(self, textBox);
    if ( !scroll || !live )
        return false;

    try
    {
        const MUX::UIElement view =
            scroll.Content().try_as<MUX::UIElement>();
        if ( !view ||
             !wxWinUIIsCurrentTextScrollView(textBox, scroll, view) )
            return false;

        winrt::Windows::Foundation::Point mapped{};
        if ( wxWinUIVisualCoordinates::ElementPointToClient(
                 live, view, point, &mapped) !=
             wxWinUICoordinateResult::Mapped )
        {
            return false;
        }

        live = wxWinUIResolveLiveTextOwner(self, textBox);
        if ( !live ||
             !wxWinUIIsCurrentTextScrollView(textBox, scroll, view) )
            return false;
        *clientPoint = wxPoint(
            static_cast<int>(std::lround(mapped.X)),
            static_cast<int>(std::lround(mapped.Y)));
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxWinUIClientPointToTextBox(
    const wxTextCtrl& owner,
    const MUXC::TextBox& textBox,
    const wxPoint& clientPoint,
    winrt::Windows::Foundation::Point *textPoint)
{
    if ( !textPoint )
        return false;

    const wxWeakRef<wxWindow> self(
        const_cast<wxTextCtrl *>(&owner));
    const MUXC::ScrollViewer scroll =
        wxWinUIResolveTextScrollViewer(owner, textBox);
    wxTextCtrl *live =
        wxWinUIResolveLiveTextOwner(self, textBox);
    if ( !scroll || !live )
        return false;

    try
    {
        const MUX::UIElement view =
            scroll.Content().try_as<MUX::UIElement>();
        if ( !view ||
             !wxWinUIIsCurrentTextScrollView(textBox, scroll, view) )
        {
            return false;
        }

        const winrt::Windows::Foundation::Point physicalClient
        {
            static_cast<float>(clientPoint.x),
            static_cast<float>(clientPoint.y)
        };
        if ( wxWinUIVisualCoordinates::ClientPointToElement(
                 live, physicalClient, view, textPoint) !=
             wxWinUICoordinateResult::Mapped )
        {
            return false;
        }

        return wxWinUIIsCurrentTextScrollView(textBox, scroll, view) &&
               wxWinUIResolveLiveTextOwner(self, textBox) != nullptr;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxWinUIHitTestTextContent(
    const MUXC::TextBox& textBox,
    const wxString& value,
    const winrt::Windows::Foundation::Point& textPoint,
    wxTextCtrlHitTestResult *result,
    long *logicalPosition)
{
    if ( !result || !logicalPosition ||
         value.length() >
            static_cast<std::size_t>(
                (std::numeric_limits<int32_t>::max)()) )
    {
        return false;
    }

    // GetRectFromCharacterIndex() is a synchronous COM call. Cache the
    // handful of edges touched by the binary searches and scan only the
    // selected physical line; a HitTest() on a large multiline value must not
    // enumerate the entire document on the UI thread.
    std::map<long, wxWinUITextEdge> edgeCache;
    const auto getEdge =
        [&textBox, &value, &edgeCache](
            long logical, wxWinUITextEdge *edge)
        {
            const auto found = edgeCache.find(logical);
            if ( found != edgeCache.end() )
            {
                *edge = found->second;
                return true;
            }

            wxWinUITextEdge resolved;
            if ( !wxWinUIGetTextEdge(
                     textBox, value, logical, &resolved) )
            {
                return false;
            }

            edgeCache.emplace(logical, resolved);
            *edge = resolved;
            return true;
        };
    const auto getUsableEdge =
        [&getEdge](long logical, wxWinUITextEdge *edge)
        {
            return getEdge(logical, edge) &&
                   wxWinUIIsUsableTextEdge(*edge);
        };

    const long length = static_cast<long>(value.length());
    wxWinUITextEdge first;
    wxWinUITextEdge last;
    if ( !getUsableEdge(0, &first) )
        return false;

    // Wrapped TextBox ranges are virtualized. Before a far range has been
    // realized, GetRectFromCharacterIndex() returns a finite negative
    // sentinel instead of failing. Never interpret that sentinel as the
    // document's last physical line: constrain the search to the realized
    // prefix. ShowPosition() realizes the requested far range before a hit
    // test can target it, so a point in the live viewport still falls inside
    // this bounded range.
    long searchableEnd = length;
    const bool haveDocumentEnd =
        getUsableEdge(length, &last);
    if ( !haveDocumentEnd )
    {
        long valid = 0;
        long invalid = length;
        while ( valid + 1 < invalid )
        {
            const long middle =
                valid + (invalid - valid) / 2;
            wxWinUITextEdge edge;
            if ( getUsableEdge(middle, &edge) )
                valid = middle;
            else
                invalid = middle;
        }

        searchableEnd = valid;
        if ( !getUsableEdge(searchableEnd, &last) )
            return false;
    }

    if ( textPoint.Y < first.point.Y )
    {
        *logicalPosition = 0;
        *result = wxTE_HT_BEFORE;
        return true;
    }

    if ( haveDocumentEnd &&
         textPoint.Y > last.point.Y + wxMax(1.0, last.height) )
    {
        *logicalPosition = length;
        *result = wxTE_HT_BELOW;
        return true;
    }

    // A point below the currently realized prefix cannot be classified
    // honestly as document end. Let the caller use its conservative fallback
    // instead of returning the virtualized range's sentinel as a hit.
    if ( !haveDocumentEnd &&
         textPoint.Y > last.point.Y + wxMax(1.0, last.height) )
    {
        return false;
    }

    // Character-leading edges are monotonic vertically even for bidi text:
    // logical order can move in either X direction, but it only advances to
    // the next physical line. Find the last edge not below the requested Y.
    long lower = 0;
    long upper = searchableEnd;
    while ( lower < upper )
    {
        const long middle =
            lower + (upper - lower + 1) / 2;
        wxWinUITextEdge edge;
        if ( !getUsableEdge(middle, &edge) )
            return false;

        if ( edge.point.Y <= textPoint.Y )
            lower = middle;
        else
            upper = middle - 1;
    }

    const long anchor = lower;
    wxWinUITextEdge anchorEdge;
    if ( !getUsableEdge(anchor, &anchorEdge) )
        return false;

    const double bandY = anchorEdge.point.Y;
    const double bandTolerance =
        wxMax(1.0, wxMax(1.0, anchorEdge.height) * 0.20);

    // Bound the one physical line containing the anchor. The remaining scan
    // is line-local, which also keeps bidi and proportional text correct
    // without assuming monotonic X coordinates.
    lower = 0;
    upper = anchor;
    while ( lower < upper )
    {
        const long middle = lower + (upper - lower) / 2;
        wxWinUITextEdge edge;
        if ( !getUsableEdge(middle, &edge) )
            return false;

        if ( edge.point.Y < bandY - bandTolerance )
            lower = middle + 1;
        else
            upper = middle;
    }
    const long bandStart = lower;

    lower = anchor;
    upper = searchableEnd;
    while ( lower < upper )
    {
        const long middle =
            lower + (upper - lower + 1) / 2;
        wxWinUITextEdge edge;
        if ( !getUsableEdge(middle, &edge) )
            return false;

        if ( edge.point.Y <= bandY + bandTolerance )
            lower = middle;
        else
            upper = middle - 1;
    }
    const long bandEnd = lower;

    // The exact fallback below intentionally walks a physical line to retain
    // bidi/proportional hit semantics. Bound its synchronous COM work: an
    // application can create a single unwrapped line with millions of code
    // units, in which case the already-computed model answer is preferable to
    // freezing the UI thread.
    constexpr unsigned long long MaxExactPhysicalLineLength = 4096;
    if ( bandEnd < bandStart ||
         static_cast<unsigned long long>(bandEnd) -
             static_cast<unsigned long long>(bandStart) >=
                 MaxExactPhysicalLineLength )
    {
        return false;
    }

    double bandMinX = (std::numeric_limits<double>::max)();
    double bandMaxX = -(std::numeric_limits<double>::max)();
    long leftLogical = bandStart;
    long rightLogical = bandStart;
    double nearestEdgeDistance =
        (std::numeric_limits<double>::max)();
    long nearestEdgeLogical = bandStart;

    const auto includePoint =
        [&](double x, long logical)
        {
            if ( x < bandMinX )
            {
                bandMinX = x;
                leftLogical = logical;
            }
            if ( x > bandMaxX )
            {
                bandMaxX = x;
                rightLogical = logical;
            }

            const double distance = std::fabs(textPoint.X - x);
            if ( distance < nearestEdgeDistance )
            {
                nearestEdgeDistance = distance;
                nearestEdgeLogical = logical;
            }
        };

    for ( long logical = bandStart; logical <= bandEnd; ++logical )
    {
        wxWinUITextEdge edge;
        if ( !getUsableEdge(logical, &edge) )
            return false;
        if ( std::fabs(edge.point.Y - bandY) <= bandTolerance )
            includePoint(edge.point.X, logical);
    }

    bool haveCharacter = false;
    long nearestCharacter = bandStart;
    double nearestCharacterDistance =
        (std::numeric_limits<double>::max)();
    bool haveContainingCharacter = false;
    long containingCharacter = bandStart;
    double containingLeadingDistance =
        (std::numeric_limits<double>::max)();

    for ( long logical = bandStart;
          logical <= bandEnd && logical < length;
          ++logical )
    {
        wxWinUITextEdge leading;
        wxWinUITextEdge trailing;
        if ( !getUsableEdge(logical, &leading) ||
             !wxWinUIGetTextCharacterSpan(
                 textBox, value, logical, &trailing) ||
             !wxWinUIIsUsableTextEdge(trailing) )
        {
            return false;
        }

        // A newline has its leading and trailing carets on different lines.
        // It has no visual glyph rectangle on either one.
        if ( std::fabs(leading.point.Y - bandY) > bandTolerance ||
             std::fabs(trailing.point.Y - bandY) > bandTolerance )
        {
            continue;
        }

        includePoint(leading.point.X, logical);
        includePoint(trailing.point.X, logical + 1);

        const double left =
            wxMin<double>(leading.point.X, trailing.point.X);
        const double right =
            wxMax<double>(leading.point.X, trailing.point.X);
        if ( right - left <= 0.25 )
            continue;

        haveCharacter = true;
        const double distance =
            textPoint.X < left
                ? left - textPoint.X
                : textPoint.X > right
                    ? textPoint.X - right
                    : 0.0;
        if ( distance < nearestCharacterDistance )
        {
            nearestCharacterDistance = distance;
            nearestCharacter = logical;
        }

        if ( textPoint.X >= left && textPoint.X <= right )
        {
            // ElementPointToClient() publishes integral physical pixels while
            // this inverse query is in XAML DIPs. At fractional/raster scales,
            // a point one physical pixel inside a character can therefore be
            // shared by its exact leading edge and the preceding synthesized
            // trailing edge. Do not let logical scan order choose the previous
            // character: the closest real leading edge is the stable inverse
            // of PositionToCoords() in this small overlap.
            const double leadingDistance =
                std::fabs(textPoint.X - leading.point.X);
            if ( !haveContainingCharacter ||
                 leadingDistance < containingLeadingDistance )
            {
                haveContainingCharacter = true;
                containingCharacter = logical;
                containingLeadingDistance = leadingDistance;
            }
        }
    }

    if ( bandMinX > bandMaxX )
        return false;

    if ( haveContainingCharacter )
    {
        *logicalPosition = containingCharacter;
        *result = wxTE_HT_ON_TEXT;
        return true;
    }

    // BEFORE/BEYOND are visual classifications. They cannot be derived from
    // the document index: the first index of a wrapped line is non-zero, and
    // the only caret in an empty value still has index zero.
    if ( textPoint.X < bandMinX )
    {
        *logicalPosition = leftLogical;
        *result = wxTE_HT_BEFORE;
    }
    else if ( textPoint.X > bandMaxX )
    {
        *logicalPosition = rightLogical;
        *result = wxTE_HT_BEYOND;
    }
    else if ( haveCharacter )
    {
        // This can occur in a visual gap inside a bidi line. Return the
        // closest real character, not an arbitrary logical edge.
        *logicalPosition = nearestCharacter;
        *result = wxTE_HT_ON_TEXT;
    }
    else
    {
        *logicalPosition = nearestEdgeLogical;
        *result = textPoint.X <= bandMinX
                    ? wxTE_HT_BEFORE
                    : wxTE_HT_BEYOND;
    }

    return true;
}

// RichEditBox exposes the complete post-edit value, while PasswordBox doesn't
// expose its selection or caret at all. Derive the inserted/replaced span from
// the before/after values for both peers. The limit is expressed in public wx
// coordinates: in particular, a newline in a plain multiline MSW-compatible
// control occupies two positions even though the RichEdit document uses LF.
// Keeping cluster-safe common prefix and suffix preserves all pre-existing text
// and truncates only the newly inserted part, which is the wx max-length
// contract. The public-coordinate intervals let the caller restore a native
// caret or selection without moving it across the preserved suffix.
struct wxWinUIConstrainedPeerEdit
{
    wxString value;
    bool wasTruncated{false};
    long proposedStart{0};
    long proposedEnd{0};
    long acceptedStart{0};
    long acceptedEnd{0};

    long RemapProposedPosition(long position) const
    {
        if ( !wasTruncated || position < 0 )
            return position;

        if ( position <= proposedStart )
        {
            return wxMax(0L,
                         acceptedStart - (proposedStart - position));
        }
        if ( position >= proposedEnd )
            return acceptedEnd + (position - proposedEnd);

        return acceptedStart +
               wxMin(position - proposedStart,
                     acceptedEnd - acceptedStart);
    }
};

wxWinUIConstrainedPeerEdit wxWinUIConstrainPeerEdit(
    const wxTextCtrl& control,
    const wxString& before,
    const wxString& proposed,
    unsigned long configuredLimit)
{
    wxWinUIConstrainedPeerEdit result;
    result.value = proposed;

    if ( !configuredLimit )
        return result;

    const long beforePublicLength =
        wxWinUIMakeTextPositionMap(control, before).GetLastPosition();
    const unsigned long effectiveLimit = wxMax<unsigned long>(
        configuredLimit,
        beforePublicLength > 0
            ? static_cast<unsigned long>(beforePublicLength)
            : 0);
    const long proposedPublicLength =
        wxWinUIMakeTextPositionMap(control, proposed).GetLastPosition();
    if ( proposedPublicLength >= 0 &&
         static_cast<unsigned long>(proposedPublicLength) <= effectiveLimit )
    {
        return result;
    }

    std::size_t prefixLength = 0;
    std::size_t suffixLength = 0;
    wxWinUIFindSafeCommonTextEdges(
        before, proposed, &prefixLength, &suffixLength);

    const std::size_t proposedInsertionLength =
        proposed.length() - prefixLength - suffixLength;
    const wxString preserved =
        before.substr(0, prefixLength) +
        (suffixLength
            ? before.substr(before.length() - suffixLength)
            : wxString());
    const long preservedPublicLength =
        wxWinUIMakeTextPositionMap(control, preserved).GetLastPosition();
    const std::size_t available =
        preservedPublicLength >= 0 &&
        static_cast<unsigned long>(preservedPublicLength) < effectiveLimit
            ? static_cast<std::size_t>(
                  effectiveLimit -
                  static_cast<unsigned long>(preservedPublicLength))
            : 0;
    const wxString acceptedInsertion = wxWinUIFitTextPrefix(
        proposed.substr(prefixLength, proposedInsertionLength),
        available,
        wxWinUIUsesTwoCharacterNewlines(control));

    result.value = before.substr(0, prefixLength);
    result.value += acceptedInsertion;
    if ( suffixLength )
        result.value += before.substr(before.length() - suffixLength);

    const wxWinUITextPositionMap proposedMap =
        wxWinUIMakeTextPositionMap(control, proposed);
    const wxWinUITextPositionMap acceptedMap =
        wxWinUIMakeTextPositionMap(control, result.value);
    result.proposedStart = proposedMap.LogicalToPublic(
        static_cast<long>(prefixLength));
    result.proposedEnd = proposedMap.LogicalToPublic(
        static_cast<long>(prefixLength + proposedInsertionLength));
    result.acceptedStart = acceptedMap.LogicalToPublic(
        static_cast<long>(prefixLength));
    result.acceptedEnd = acceptedMap.LogicalToPublic(
        static_cast<long>(prefixLength + acceptedInsertion.length()));
    result.wasTruncated = true;
    return result;
}

// Apply a wxFont to a WinUI Control; no-op for an invalid font.
void wxWinUIApplyControlFont(const MUXC::Control& control, const wxFont& font)
{
    if ( !control || !font.IsOk() )
        return;

    const wxString face = font.GetFaceName();
    if ( !face.empty() )
    {
        control.FontFamily(
            winrt::Microsoft::UI::Xaml::Media::FontFamily(wxWinUIToHString(face)));
    }
    else
    {
        control.ClearValue(MUXC::Control::FontFamilyProperty());
    }

    const double pt = font.GetFractionalPointSize();
    control.FontSize(pt > 0.0 ? pt * 96.0 / 72.0 : 14.0);
    control.FontWeight(font.GetNumericWeight() >= wxFONTWEIGHT_BOLD
        ? winrt::Microsoft::UI::Text::FontWeights::Bold()
        : winrt::Microsoft::UI::Text::FontWeights::Normal());
    control.FontStyle(font.GetStyle() == wxFONTSTYLE_NORMAL
        ? WUTF::FontStyle::Normal
        : WUTF::FontStyle::Italic);
}

WU::Color wxWinUITextColour(const wxColour& colour)
{
    return WU::Color
    {
        colour.Alpha(),
        colour.Red(),
        colour.Green(),
        colour.Blue()
    };
}

wxColour wxWinUITextColour(const WU::Color& colour)
{
    return wxColour(colour.R, colour.G, colour.B, colour.A);
}

constexpr float wxWinUITextTenthsMillimetreToPoints(double value)
{
    return static_cast<float>(value * 72.0 / 254.0);
}

int wxWinUITextPointsToTenthsMillimetre(float value)
{
    return static_cast<int>(std::lround(value * 254.0 / 72.0));
}

WUT::FormatEffect wxWinUITextFormatEffect(bool enabled)
{
    return enabled ? WUT::FormatEffect::On : WUT::FormatEffect::Off;
}

wxString wxWinUIGetRichPlainText(const MUXC::RichEditBox& richEditBox)
{
    if ( !richEditBox )
        return wxString();

    winrt::hstring text;
    const WUT::TextGetOptions options = static_cast<WUT::TextGetOptions>(
        static_cast<uint32_t>(WUT::TextGetOptions::UseLf) |
        static_cast<uint32_t>(WUT::TextGetOptions::AllowFinalEop));
    richEditBox.Document().GetText(options, text);
    wxString value = wxWinUIFromHString(text);

    // The TOM document always owns a final end-of-paragraph marker. It is
    // returned even for an empty RichEditBox and is distinct from an actual
    // trailing newline (which produces a second marker), so remove exactly
    // one marker before exposing the wx value.
    if ( !value.empty() &&
         (value.Last() == '\r' || value.Last() == '\n') )
    {
        value.RemoveLast();
    }

    return wxWinUITextPositionMap::NormalizeNewlines(value);
}

struct wxWinUIPasswordScrubResult
{
    bool isMasked = false;
    bool selectionRestored = false;
    bool isEmpty = false;
    HRESULT failure = S_OK;
};

wxWinUIPasswordScrubResult wxWinUIScrubPasswordDocument(
    const MUXC::RichEditBox& richEditBox,
    const wxString& mask,
    long selectionStart,
    long selectionEnd,
    HRESULT forcedMaskFailure = S_OK,
    bool forcePartialMask = false)
{
    wxWinUIPasswordScrubResult result;
    const auto rememberFailure = [&result](HRESULT failure)
    {
        if ( SUCCEEDED(result.failure) )
            result.failure = FAILED(failure) ? failure : E_UNEXPECTED;
    };
    const auto documentEquals = [&](const wxString& expected)
    {
        try
        {
            return wxWinUIGetRichPlainText(richEditBox) == expected;
        }
        catch ( const winrt::hresult_error& e )
        {
            rememberFailure(e.code());
            return false;
        }
    };
    const auto setDocumentText = [&](const wxString& value)
    {
        try
        {
            const WUT::ITextDocument document = richEditBox.Document();
            if ( !document )
            {
                rememberFailure(E_POINTER);
                return false;
            }
            document.SetText(WUT::TextSetOptions::None,
                             wxWinUIToHString(value));
            return documentEquals(value);
        }
        catch ( const winrt::hresult_error& e )
        {
            rememberFailure(e.code());
            return false;
        }
    };
    const auto setStoryText = [&](const wxString& value)
    {
        try
        {
            const WUT::ITextDocument document = richEditBox.Document();
            if ( !document )
            {
                rememberFailure(E_POINTER);
                return false;
            }
            const WUT::ITextRange story = document.GetRange(0, 0);
            if ( !story )
            {
                rememberFailure(E_POINTER);
                return false;
            }
            story.Expand(WUT::TextRangeUnit::Story);
            story.SetText(WUT::TextSetOptions::None,
                          wxWinUIToHString(value));
            return documentEquals(value);
        }
        catch ( const winrt::hresult_error& e )
        {
            rememberFailure(e.code());
            return false;
        }
    };

    if ( forcePartialMask )
    {
        // Model a provider accepting SetText() but retaining only a prefix.
        // The read-back proof, rather than the HRESULT, must reject it.
        wxString partialMask = mask;
        if ( !partialMask.empty() )
            partialMask.RemoveLast();
        else
            partialMask = wxS("x");
        setDocumentText(partialMask);
        rememberFailure(E_UNEXPECTED);
    }
    else if ( FAILED(forcedMaskFailure) )
    {
        // This deterministic seam represents failure of every operation that
        // could leave the requested mask in the document. The independent
        // empty-document fallbacks below still execute normally.
        rememberFailure(forcedMaskFailure);
    }
    else
    {
        result.isMasked = setDocumentText(mask) || setStoryText(mask);
    }

    if ( result.isMasked )
    {
        // Selection restoration is not part of the secrecy proof: once the
        // complete document has been read back as mask glyphs, failure here
        // must not cause a destructive fallback that exposes more data.
        try
        {
            const WUT::ITextSelection selection =
                richEditBox.Document().Selection();
            if ( selection )
            {
                const long last = static_cast<long>(mask.length());
                selection.SetRange(
                    static_cast<int32_t>(
                        wxWinUIClampTextPos(selectionStart, last)),
                    static_cast<int32_t>(
                        wxWinUIClampTextPos(selectionEnd, last)));
                result.selectionRestored = true;
            }
            else
                rememberFailure(E_POINTER);
        }
        catch ( const winrt::hresult_error& e )
        {
            rememberFailure(e.code());
        }
        return result;
    }

    // Never return an unproved document to the renderer. Use both the
    // document-wide setter and an independently acquired TOM Story range: a
    // failure in either projection must not strand clear input in RichEdit.
    result.isEmpty = setDocumentText(wxString()) || setStoryText(wxString());
    if ( result.isEmpty )
    {
        try
        {
            const WUT::ITextDocument document = richEditBox.Document();
            if ( !document )
            {
                rememberFailure(E_POINTER);
            }
            else
            {
                // The terminal peer must not be able to resurrect a clear
                // transient edit through RichEdit's private undo history.
                document.UndoLimit(0);
                document.CaretType(WUT::CaretType::Null);
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            rememberFailure(e.code());
        }
    }
    if ( !result.isEmpty && SUCCEEDED(result.failure) )
        result.failure = E_UNEXPECTED;
    return result;
}

wxString wxWinUITextFaceForFamily(wxFontFamily family)
{
    switch ( family )
    {
        case wxFONTFAMILY_ROMAN:
            return wxS("Times New Roman");

        case wxFONTFAMILY_SCRIPT:
            return wxS("Segoe Script");

        case wxFONTFAMILY_MODERN:
        case wxFONTFAMILY_TELETYPE:
            return wxS("Consolas");

        case wxFONTFAMILY_DECORATIVE:
            return wxS("Segoe Print");

        case wxFONTFAMILY_SWISS:
        case wxFONTFAMILY_DEFAULT:
            return wxS("Segoe UI");

        default:
            return wxString();
    }
}

bool wxWinUIApplyTextCharacterFormat(
    const WUT::ITextCharacterFormat& format,
    const wxTextAttr& style)
{
    bool fullySupported = true;

    if ( style.HasTextColour() )
        format.ForegroundColor(wxWinUITextColour(style.GetTextColour()));
    if ( style.HasBackgroundColour() )
        format.BackgroundColor(wxWinUITextColour(style.GetBackgroundColour()));

    if ( style.HasFontFaceName() && !style.GetFontFaceName().empty() )
    {
        format.Name(wxWinUIToHString(style.GetFontFaceName()));
    }
    else if ( style.HasFontFamily() )
    {
        const wxString face =
            wxWinUITextFaceForFamily(style.GetFontFamily());
        if ( face.empty() )
            fullySupported = false;
        else
            format.Name(wxWinUIToHString(face));
    }

    if ( style.HasFontPointSize() )
    {
        format.Size(static_cast<float>(style.GetFontSize()));
    }
    else if ( style.HasFontPixelSize() )
    {
        // XAML uses 96-DPI device-independent pixels and TOM uses points.
        format.Size(static_cast<float>(style.GetFontSize() * 72.0 / 96.0));
    }

    if ( style.HasFontWeight() )
        format.Weight(static_cast<int32_t>(style.GetFontWeight()));
    if ( style.HasFontItalic() )
    {
        format.FontStyle(
            style.GetFontStyle() == wxFONTSTYLE_NORMAL
                ? WUTF::FontStyle::Normal
                : style.GetFontStyle() == wxFONTSTYLE_SLANT
                    ? WUTF::FontStyle::Oblique
                    : WUTF::FontStyle::Italic);
    }
    if ( style.HasFontUnderlined() )
    {
        WUT::UnderlineType type = WUT::UnderlineType::None;
        switch ( style.GetUnderlineType() )
        {
            case wxTEXT_ATTR_UNDERLINE_SOLID:
                type = WUT::UnderlineType::Single;
                break;

            case wxTEXT_ATTR_UNDERLINE_DOUBLE:
                type = WUT::UnderlineType::Double;
                break;

            case wxTEXT_ATTR_UNDERLINE_SPECIAL:
                type = WUT::UnderlineType::Wave;
                break;

            default:
                break;
        }
        format.Underline(type);

        // ITextCharacterFormat exposes the underline shape but not a separate
        // underline colour. Never silently claim this optional extension.
        if ( style.GetUnderlineColour().IsOk() )
            fullySupported = false;
    }
    if ( style.HasFontStrikethrough() )
    {
        format.Strikethrough(
            wxWinUITextFormatEffect(style.GetFontStrikethrough()));
    }

    if ( style.HasTextEffects() )
    {
        const int mask = style.GetTextEffectFlags();
        const int effects = style.GetTextEffects();
        const auto applyEffect = [&](int bit, const auto& setter)
        {
            if ( mask & bit )
                setter(wxWinUITextFormatEffect((effects & bit) != 0));
        };

        applyEffect(wxTEXT_ATTR_EFFECT_CAPITALS,
                    [&](WUT::FormatEffect value) { format.AllCaps(value); });
        applyEffect(wxTEXT_ATTR_EFFECT_SMALL_CAPITALS,
                    [&](WUT::FormatEffect value) { format.SmallCaps(value); });
        applyEffect(wxTEXT_ATTR_EFFECT_STRIKETHROUGH,
                    [&](WUT::FormatEffect value) { format.Strikethrough(value); });
        applyEffect(wxTEXT_ATTR_EFFECT_OUTLINE,
                    [&](WUT::FormatEffect value) { format.Outline(value); });
        applyEffect(wxTEXT_ATTR_EFFECT_SUPERSCRIPT,
                    [&](WUT::FormatEffect value) { format.Superscript(value); });
        applyEffect(wxTEXT_ATTR_EFFECT_SUBSCRIPT,
                    [&](WUT::FormatEffect value) { format.Subscript(value); });

        constexpr int supportedEffects =
            wxTEXT_ATTR_EFFECT_CAPITALS |
            wxTEXT_ATTR_EFFECT_SMALL_CAPITALS |
            wxTEXT_ATTR_EFFECT_STRIKETHROUGH |
            wxTEXT_ATTR_EFFECT_OUTLINE |
            wxTEXT_ATTR_EFFECT_SUPERSCRIPT |
            wxTEXT_ATTR_EFFECT_SUBSCRIPT;
        if ( mask & ~supportedEffects )
            fullySupported = false;
    }

    return fullySupported;
}

bool wxWinUIApplyTextParagraphFormat(
    const WUT::ITextParagraphFormat& format,
    const wxTextAttr& style)
{
    bool fullySupported = true;

    if ( style.HasAlignment() )
    {
        WUT::ParagraphAlignment alignment = WUT::ParagraphAlignment::Left;
        switch ( style.GetAlignment() )
        {
            case wxTEXT_ALIGNMENT_CENTRE:
                alignment = WUT::ParagraphAlignment::Center;
                break;

            case wxTEXT_ALIGNMENT_RIGHT:
                alignment = WUT::ParagraphAlignment::Right;
                break;

            case wxTEXT_ALIGNMENT_JUSTIFIED:
                alignment = WUT::ParagraphAlignment::Justify;
                break;

            default:
                break;
        }
        format.Alignment(alignment);
    }

    if ( style.HasLeftIndent() || style.HasRightIndent() )
    {
        float first = format.FirstLineIndent();
        float left = format.LeftIndent();
        float right = format.RightIndent();
        if ( style.HasLeftIndent() )
        {
            first = wxWinUITextTenthsMillimetreToPoints(
                style.GetLeftIndent());
            left = wxWinUITextTenthsMillimetreToPoints(
                style.GetLeftIndent() + style.GetLeftSubIndent());
        }
        if ( style.HasRightIndent() )
        {
            right = wxWinUITextTenthsMillimetreToPoints(
                style.GetRightIndent());
        }
        format.SetIndents(first, left, right);
    }

    if ( style.HasTabs() )
    {
        format.ClearAllTabs();
        for ( const int tab : style.GetTabs() )
        {
            format.AddTab(
                wxWinUITextTenthsMillimetreToPoints(tab),
                WUT::TabAlignment::Left,
                WUT::TabLeader::Spaces);
        }
    }

    if ( style.HasParagraphSpacingBefore() )
    {
        format.SpaceBefore(wxWinUITextTenthsMillimetreToPoints(
            style.GetParagraphSpacingBefore()));
    }
    if ( style.HasParagraphSpacingAfter() )
    {
        format.SpaceAfter(wxWinUITextTenthsMillimetreToPoints(
            style.GetParagraphSpacingAfter()));
    }
    if ( style.HasLineSpacing() )
    {
        WUT::LineSpacingRule rule = WUT::LineSpacingRule::Single;
        switch ( style.GetLineSpacing() )
        {
            case wxTEXT_ATTR_LINE_SPACING_HALF:
                rule = WUT::LineSpacingRule::OneAndHalf;
                break;

            case wxTEXT_ATTR_LINE_SPACING_TWICE:
                rule = WUT::LineSpacingRule::Double;
                break;

            case wxTEXT_ATTR_LINE_SPACING_NORMAL:
                break;

            default:
                fullySupported = false;
                break;
        }
        format.SetLineSpacing(rule, 0.0f);
    }

    if ( style.HasBulletStyle() )
    {
        const int bullet = style.GetBulletStyle();
        WUT::MarkerType type = WUT::MarkerType::None;
        if ( bullet & wxTEXT_ATTR_BULLET_STYLE_STANDARD )
            type = WUT::MarkerType::Bullet;
        else if ( bullet & wxTEXT_ATTR_BULLET_STYLE_ARABIC )
            type = WUT::MarkerType::Arabic;
        else if ( bullet & wxTEXT_ATTR_BULLET_STYLE_LETTERS_LOWER )
            type = WUT::MarkerType::LowercaseEnglishLetter;
        else if ( bullet & wxTEXT_ATTR_BULLET_STYLE_LETTERS_UPPER )
            type = WUT::MarkerType::UppercaseEnglishLetter;
        else if ( bullet & wxTEXT_ATTR_BULLET_STYLE_ROMAN_LOWER )
            type = WUT::MarkerType::LowercaseRoman;
        else if ( bullet & wxTEXT_ATTR_BULLET_STYLE_ROMAN_UPPER )
            type = WUT::MarkerType::UppercaseRoman;
        else if ( bullet != wxTEXT_ATTR_BULLET_STYLE_NONE )
            fullySupported = false;
        format.ListType(type);

        if ( bullet & wxTEXT_ATTR_BULLET_STYLE_PARENTHESES )
            format.ListStyle(WUT::MarkerStyle::Parentheses);
        else if ( bullet & wxTEXT_ATTR_BULLET_STYLE_RIGHT_PARENTHESIS )
            format.ListStyle(WUT::MarkerStyle::Parenthesis);
        else if ( bullet & wxTEXT_ATTR_BULLET_STYLE_PERIOD )
            format.ListStyle(WUT::MarkerStyle::Period);
        else
            format.ListStyle(WUT::MarkerStyle::Plain);

        if ( bullet & wxTEXT_ATTR_BULLET_STYLE_ALIGN_RIGHT )
            format.ListAlignment(WUT::MarkerAlignment::Right);
        else if ( bullet & wxTEXT_ATTR_BULLET_STYLE_ALIGN_CENTRE )
            format.ListAlignment(WUT::MarkerAlignment::Center);
        else
            format.ListAlignment(WUT::MarkerAlignment::Left);
    }
    if ( style.HasBulletNumber() )
        format.ListStart(style.GetBulletNumber());

    // RichEditBox/TOM has no wxTextAttr analogue for custom bitmap/symbol
    // marker payloads. Signal this explicitly to the caller.
    if ( style.HasBulletText() || style.HasBulletName() )
        fullySupported = false;

    return fullySupported;
}

bool wxWinUIApplyTextStyle(const WUT::ITextRange& range,
                           const wxTextAttr& style)
{
    const bool characterOk =
        wxWinUIApplyTextCharacterFormat(range.CharacterFormat(), style);
    const bool paragraphOk =
        wxWinUIApplyTextParagraphFormat(range.ParagraphFormat(), style);
    return characterOk && paragraphOk;
}

void wxWinUIReadTextStyle(const WUT::ITextRange& range,
                          wxTextAttr *style)
{
    if ( !style )
        return;

    *style = wxTextAttr();
    const WUT::ITextCharacterFormat character = range.CharacterFormat();
    const WUT::ITextParagraphFormat paragraph = range.ParagraphFormat();

    const wxString face = wxWinUIFromHString(character.Name());
    if ( !face.empty() )
        style->SetFontFaceName(face);

    const float size = character.Size();
    if ( std::isfinite(size) && size > 0.0f )
        style->SetFontPointSize(static_cast<int>(std::lround(size)));

    const int32_t weight = character.Weight();
    if ( weight > 0 )
        style->SetFontWeight(static_cast<wxFontWeight>(weight));

    const WUTF::FontStyle fontStyle = character.FontStyle();
    style->SetFontStyle(
        fontStyle == WUTF::FontStyle::Normal
            ? wxFONTSTYLE_NORMAL
            : fontStyle == WUTF::FontStyle::Oblique
                ? wxFONTSTYLE_SLANT
                : wxFONTSTYLE_ITALIC);

    const WUT::UnderlineType underline = character.Underline();
    if ( underline != WUT::UnderlineType::Undefined )
    {
        wxTextAttrUnderlineType type = wxTEXT_ATTR_UNDERLINE_NONE;
        if ( underline == WUT::UnderlineType::Double )
            type = wxTEXT_ATTR_UNDERLINE_DOUBLE;
        else if ( underline == WUT::UnderlineType::Wave ||
                  underline == WUT::UnderlineType::DoubleWave ||
                  underline == WUT::UnderlineType::HeavyWave )
            type = wxTEXT_ATTR_UNDERLINE_SPECIAL;
        else if ( underline != WUT::UnderlineType::None )
            type = wxTEXT_ATTR_UNDERLINE_SOLID;
        style->SetFontUnderlined(type);
    }

    const WUT::FormatEffect strike = character.Strikethrough();
    if ( strike != WUT::FormatEffect::Undefined )
        style->SetFontStrikethrough(strike == WUT::FormatEffect::On);

    style->SetTextColour(wxWinUITextColour(character.ForegroundColor()));
    style->SetBackgroundColour(wxWinUITextColour(character.BackgroundColor()));

    switch ( paragraph.Alignment() )
    {
        case WUT::ParagraphAlignment::Center:
            style->SetAlignment(wxTEXT_ALIGNMENT_CENTRE);
            break;

        case WUT::ParagraphAlignment::Right:
            style->SetAlignment(wxTEXT_ALIGNMENT_RIGHT);
            break;

        case WUT::ParagraphAlignment::Justify:
            style->SetAlignment(wxTEXT_ALIGNMENT_JUSTIFIED);
            break;

        case WUT::ParagraphAlignment::Left:
            style->SetAlignment(wxTEXT_ALIGNMENT_LEFT);
            break;

        default:
            break;
    }

    const int first =
        wxWinUITextPointsToTenthsMillimetre(paragraph.FirstLineIndent());
    const int left =
        wxWinUITextPointsToTenthsMillimetre(paragraph.LeftIndent());
    const int right =
        wxWinUITextPointsToTenthsMillimetre(paragraph.RightIndent());
    style->SetLeftIndent(first, left - first);
    style->SetRightIndent(right);

    wxArrayInt tabs;
    const int32_t tabCount = paragraph.TabCount();
    for ( int32_t i = 0; i < tabCount; ++i )
    {
        float position = 0.0f;
        WUT::TabAlignment alignment = WUT::TabAlignment::Left;
        WUT::TabLeader leader = WUT::TabLeader::Spaces;
        paragraph.GetTab(i, position, alignment, leader);
        tabs.Add(wxWinUITextPointsToTenthsMillimetre(position));
    }
    if ( tabCount >= 0 )
        style->SetTabs(tabs);

    style->SetParagraphSpacingBefore(
        wxWinUITextPointsToTenthsMillimetre(paragraph.SpaceBefore()));
    style->SetParagraphSpacingAfter(
        wxWinUITextPointsToTenthsMillimetre(paragraph.SpaceAfter()));
    switch ( paragraph.LineSpacingRule() )
    {
        case WUT::LineSpacingRule::OneAndHalf:
            style->SetLineSpacing(wxTEXT_ATTR_LINE_SPACING_HALF);
            break;

        case WUT::LineSpacingRule::Double:
            style->SetLineSpacing(wxTEXT_ATTR_LINE_SPACING_TWICE);
            break;

        case WUT::LineSpacingRule::Single:
            style->SetLineSpacing(wxTEXT_ATTR_LINE_SPACING_NORMAL);
            break;

        default:
            break;
    }
}

WUT::PointOptions wxWinUIRichClientPointOptions(bool useStart = false)
{
    WUT::PointOptions options =
        WUT::PointOptions::ClientCoordinates |
        WUT::PointOptions::IncludeInset |
        WUT::PointOptions::AllowOffClient;
    if ( useStart )
        options |= WUT::PointOptions::Start;
    return options;
}

bool wxWinUIIsFinitePoint(
    const winrt::Windows::Foundation::Point& point)
{
    return std::isfinite(point.X) && std::isfinite(point.Y);
}

bool wxWinUIIsFiniteRect(
    const winrt::Windows::Foundation::Rect& rect)
{
    return std::isfinite(rect.X) && std::isfinite(rect.Y) &&
           std::isfinite(rect.Width) && std::isfinite(rect.Height) &&
           rect.Width >= 0.0f && rect.Height >= 0.0f;
}

wxTextCtrl *wxWinUIResolveLiveRichTextOwner(
    const wxWeakRef<wxWindow>& self,
    const MUXC::RichEditBox& richEditBox)
{
    wxTextCtrl *live = wxDynamicCast(self.get(), wxTextCtrl);
    wxWinUITopLevelHost *host =
        live ? wxWinUITopLevelHost::FindSlotOwner(live) : nullptr;
    wxWinUISlot *slot = host && live ? host->FindSlot(live) : nullptr;
    if ( !slot )
        return nullptr;

    const MUX::UIElement content = slot->GetContent();
    const MUXC::RichEditBox current =
        content.try_as<MUXC::RichEditBox>();

    // Every projection above may run application code. Reacquire the complete
    // wx/slot identity before exposing the owner to a caller.
    live = wxDynamicCast(self.get(), wxTextCtrl);
    wxWinUITopLevelHost * const currentHost =
        live ? wxWinUITopLevelHost::FindSlotOwner(live) : nullptr;
    wxWinUISlot * const currentSlot =
        currentHost && live ? currentHost->FindSlot(live) : nullptr;
    if ( !live || currentHost != host || !currentSlot ||
         currentSlot != slot || current != richEditBox )
    {
        return nullptr;
    }

    const MUX::UIElement currentContent = currentSlot->GetContent();
    live = wxDynamicCast(self.get(), wxTextCtrl);
    wxWinUITopLevelHost * const finalHost =
        live ? wxWinUITopLevelHost::FindSlotOwner(live) : nullptr;
    wxWinUISlot * const finalSlot =
        finalHost && live ? finalHost->FindSlot(live) : nullptr;
    if ( !live || finalHost != currentHost || finalSlot != currentSlot ||
         currentContent != content )
    {
        return nullptr;
    }

    return live;
}

bool wxWinUIRichElementPointToClient(
    const wxWeakRef<wxWindow>& self,
    const MUXC::RichEditBox& richEditBox,
    const winrt::Windows::Foundation::Point& point,
    wxPoint *clientPoint)
{
    if ( !clientPoint || !wxWinUIIsFinitePoint(point) )
        return false;

    wxTextCtrl *live =
        wxWinUIResolveLiveRichTextOwner(self, richEditBox);
    if ( !live )
        return false;

    const MUXC::ScrollViewer scroll =
        wxWinUIResolveTextScrollViewer(*live, richEditBox);
    live = wxWinUIResolveLiveRichTextOwner(self, richEditBox);
    if ( !scroll || !live )
        return false;

    try
    {
        // Microsoft.UI.Text's ClientCoordinates are relative to the internal
        // RichEdit document client. Project that document point into the
        // ScrollViewer viewport first: immediately after ChangeView() the
        // published offset can be newer than the composition transform, and
        // mapping through ContentElement alone would then omit the scroll.
        const MUX::UIElement view =
            scroll.Content().try_as<MUX::UIElement>();
        if ( !view ||
             !wxWinUIIsCurrentTextScrollView(
                 richEditBox, scroll, view) )
        {
            return false;
        }

        winrt::Windows::Foundation::Rect viewportTarget{};
        if ( !wxWinUIProjectRichTextTarget(
                 richEditBox,
                 scroll,
                 view,
                 { point.X, point.Y, 0.0f, 0.0f },
                 &viewportTarget) )
        {
            return false;
        }

        live = wxWinUIResolveLiveRichTextOwner(self, richEditBox);
        if ( !live ||
             !wxWinUIIsCurrentTextScrollView(
                 richEditBox, scroll, view) )
        {
            return false;
        }

        winrt::Windows::Foundation::Point mapped{};
        if ( wxWinUIVisualCoordinates::ElementPointToClient(
                 live,
                 scroll,
                 { viewportTarget.X, viewportTarget.Y },
                 &mapped) !=
             wxWinUICoordinateResult::Mapped )
        {
            return false;
        }

        live = wxWinUIResolveLiveRichTextOwner(self, richEditBox);
        if ( !live || !wxWinUIIsFinitePoint(mapped) ||
             !wxWinUIIsCurrentTextScrollView(
                 richEditBox, scroll, view) )
        {
            return false;
        }

        *clientPoint = wxPoint(
            static_cast<int>(std::lround(mapped.X)),
            static_cast<int>(std::lround(mapped.Y)));
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxWinUIClientPointToRichElement(
    const wxTextCtrl& owner,
    const MUXC::RichEditBox& richEditBox,
    const wxPoint& clientPoint,
    winrt::Windows::Foundation::Point *richPoint)
{
    if ( !richPoint )
        return false;

    const wxWeakRef<wxWindow> self(
        const_cast<wxTextCtrl *>(&owner));
    wxTextCtrl *live =
        wxWinUIResolveLiveRichTextOwner(self, richEditBox);
    if ( !live )
        return false;

    const MUXC::ScrollViewer scroll =
        wxWinUIResolveTextScrollViewer(*live, richEditBox);
    live = wxWinUIResolveLiveRichTextOwner(self, richEditBox);
    if ( !scroll || !live )
        return false;

    try
    {
        const MUX::UIElement view =
            scroll.Content().try_as<MUX::UIElement>();
        if ( !view ||
             !wxWinUIIsCurrentTextScrollView(
                 richEditBox, scroll, view) )
        {
            return false;
        }

        const winrt::Windows::Foundation::Point physicalClient
        {
            static_cast<float>(clientPoint.x),
            static_cast<float>(clientPoint.y)
        };
        winrt::Windows::Foundation::Point viewportPoint{};
        if ( wxWinUIVisualCoordinates::ClientPointToElement(
                 live, physicalClient, scroll, &viewportPoint) !=
             wxWinUICoordinateResult::Mapped )
        {
            return false;
        }

        live = wxWinUIResolveLiveRichTextOwner(self, richEditBox);
        if ( !live || !wxWinUIIsFinitePoint(viewportPoint) ||
             !wxWinUIIsCurrentTextScrollView(
                 richEditBox, scroll, view) )
        {
            return false;
        }

        const double horizontalOffset = scroll.HorizontalOffset();
        const double verticalOffset = scroll.VerticalOffset();
        if ( horizontalOffset < 0.0 || verticalOffset < 0.0 ||
             !std::isfinite(horizontalOffset) ||
             !std::isfinite(verticalOffset) ||
             !wxWinUIIsCurrentTextScrollView(
                 richEditBox, scroll, view) )
        {
            return false;
        }
        const winrt::Windows::Foundation::Point documentPoint
        {
            static_cast<float>(viewportPoint.X + horizontalOffset),
            static_cast<float>(viewportPoint.Y + verticalOffset)
        };
        live = wxWinUIResolveLiveRichTextOwner(self, richEditBox);
        if ( !live || !wxWinUIIsFinitePoint(documentPoint) ||
             !wxWinUIIsCurrentTextScrollView(
                 richEditBox, scroll, view) )
        {
            return false;
        }

        *richPoint = documentPoint;
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxWinUIGetRichTextRangePoint(
    const WUT::ITextRange& range,
    WUT::HorizontalCharacterAlignment horizontalAlignment,
    bool useStart,
    winrt::Windows::Foundation::Point *point)
{
    if ( !range || !point )
        return false;

    try
    {
        const auto abi =
            static_cast<winrt::impl::abi_t<WUT::ITextRange> *>(
                winrt::get_abi(range));
        if ( !abi )
            return false;

        winrt::Windows::Foundation::Point candidate
        {
            std::numeric_limits<float>::quiet_NaN(),
            std::numeric_limits<float>::quiet_NaN()
        };
        const int32_t result = abi->GetPoint(
            static_cast<int32_t>(horizontalAlignment),
            static_cast<int32_t>(
                WUT::VerticalCharacterAlignment::Top),
            static_cast<uint32_t>(
                wxWinUIRichClientPointOptions(useStart)),
            &candidate);
        if ( result < 0 )
            winrt::check_hresult(result);

        // TOM uses S_FALSE for an unavailable/virtualized point. The normal
        // C++/WinRT projection only checks FAILED(hr), which turns S_FALSE
        // into an apparent success and can expose an untouched (0, 0) output.
        if ( result != 0 || !wxWinUIIsFinitePoint(candidate) )
            return false;

        *point = candidate;
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        // A freshly replaced or virtualized story can reject an off-client
        // point until ScrollIntoView realizes it. This is an unavailable
        // geometry answer, not a failure of the caller's scroll transaction.
        return false;
    }
}

bool wxWinUIGetRichTextPoint(
    const MUXC::RichEditBox& richEditBox,
    long logical,
    winrt::Windows::Foundation::Point *point)
{
    if ( !richEditBox || !point || logical < 0 ||
         logical > (std::numeric_limits<int32_t>::max)() )
    {
        return false;
    }

    try
    {
        const WUT::ITextRange range = richEditBox.Document().GetRange(
            static_cast<int32_t>(logical), static_cast<int32_t>(logical));
        return wxWinUIGetRichTextRangePoint(
            range,
            WUT::HorizontalCharacterAlignment::Left,
            true,
            point);
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

template <typename IsCurrent>
bool wxWinUIHitTestBoundedRichTextGeometry(
    const MUXC::RichEditBox& richEditBox,
    const wxString& value,
    const winrt::Windows::Foundation::Point& point,
    IsCurrent&& isCurrent,
    wxTextCtrlHitTestResult *result,
    long *logicalPosition)
{
    if ( !richEditBox || !result || !logicalPosition || !isCurrent() )
        return false;

    // GetRangeFromPoint() depends on the HWND/screen projection owned by the
    // XAML island and can reject an otherwise valid RichEdit client point.
    // For a bounded story, derive the exact physical line from TOM's own
    // character rectangles instead. Long documents retain the native
    // point-to-range path below, so HitTest never becomes an unbounded scan.
    constexpr std::size_t MaxInspectedTextUnits = 4096;
    if ( value.length() > MaxInspectedTextUnits )
        return false;

    struct CharacterRect
    {
        long logical = 0;
        winrt::Windows::Foundation::Rect rect{};
        winrt::Windows::Foundation::Point leading{};
        double left = 0.0;
        double right = 0.0;
    };

    try
    {
        const WUT::ITextDocument document = richEditBox.Document();
        if ( !document || !isCurrent() )
            return false;

        constexpr double CoordinateTolerance = 0.75;
        if ( value.empty() )
        {
            winrt::Windows::Foundation::Point caret{};
            if ( !wxWinUIGetRichTextPoint(richEditBox, 0, &caret) ||
                 !isCurrent() )
            {
                return false;
            }

            const double lineHeight = wxMax(1.0, richEditBox.FontSize());
            if ( !isCurrent() ||
                 point.Y < caret.Y - CoordinateTolerance ||
                 point.Y > caret.Y + lineHeight + CoordinateTolerance )
            {
                return false;
            }

            *logicalPosition = 0;
            *result = point.X < caret.X
                        ? wxTE_HT_BEFORE
                        : wxTE_HT_BEYOND;
            return true;
        }

        const WUT::PointOptions options = wxWinUIRichClientPointOptions();
        std::vector<CharacterRect> lineCharacters;
        lineCharacters.reserve(value.length());
        for ( std::size_t offset = 0; offset < value.length(); ++offset )
        {
            if ( value[offset] == '\n' )
                continue;

            WUT::ITextRange character{ nullptr };
            winrt::Windows::Foundation::Rect rect{};
            try
            {
                character = document.GetRange(
                    static_cast<int32_t>(offset),
                    static_cast<int32_t>(offset + 1));
                int32_t hit = 0;
                character.GetRect(options, rect, hit);
                wxUnusedVar(hit);
            }
            catch ( const winrt::hresult_error& )
            {
                // RichEdit may decline a rectangle for an unrealized range
                // even with AllowOffClient. Keep the bounded scan looking for
                // the physical line that is realized under the input point.
                if ( !isCurrent() )
                    return false;
                continue;
            }
            if ( !isCurrent() )
                return false;
            if ( !wxWinUIIsFiniteRect(rect) ||
                 rect.Width <= 0.0f || rect.Height <= 0.0f )
            {
                continue;
            }

            if ( point.Y >= rect.Y - CoordinateTolerance &&
                 point.Y <= rect.Y + rect.Height + CoordinateTolerance )
            {
                winrt::Windows::Foundation::Point leading{};
                if ( !wxWinUIGetRichTextPoint(
                         richEditBox,
                         static_cast<long>(offset),
                         &leading) ||
                     !isCurrent() )
                {
                    return false;
                }

                // GetRect() remains authoritative for the physical line and
                // vertical cell. At fractional raster scales neighbouring
                // glyph rectangles can both include the same rounded pixel,
                // so obtain the native TOM left/right points for this exact
                // character range as well. If an endpoint is virtualized,
                // retaining the native rectangle is still an exact fallback.
                winrt::Windows::Foundation::Point rangeLeft{};
                winrt::Windows::Foundation::Point rangeRight{};
                const bool hasRangeEdges =
                    wxWinUIGetRichTextRangePoint(
                        character,
                        WUT::HorizontalCharacterAlignment::Left,
                        true,
                        &rangeLeft) &&
                    isCurrent() &&
                    wxWinUIGetRichTextRangePoint(
                        character,
                        WUT::HorizontalCharacterAlignment::Right,
                        true,
                        &rangeRight) &&
                    isCurrent();
                if ( !isCurrent() )
                    return false;

                double left = rect.X;
                double right = rect.X + rect.Width;
                if ( hasRangeEdges )
                {
                    const double nativeLeft =
                        wxMin<double>(rangeLeft.X, rangeRight.X);
                    const double nativeRight =
                        wxMax<double>(rangeLeft.X, rangeRight.X);
                    if ( nativeRight > nativeLeft )
                    {
                        left = nativeLeft;
                        right = nativeRight;
                    }
                }

                lineCharacters.push_back(
                    { static_cast<long>(offset), rect, leading, left, right });
            }
        }

        if ( !isCurrent() || lineCharacters.empty() )
            return false;

        double minX = (std::numeric_limits<double>::max)();
        double maxX = -(std::numeric_limits<double>::max)();
        long leftLogical = lineCharacters.front().logical;
        long rightLogical = lineCharacters.front().logical + 1;
        long nearestLogical = lineCharacters.front().logical;
        double nearestDistance = (std::numeric_limits<double>::max)();
        bool hasContainingCharacter = false;
        long containingLogical = lineCharacters.front().logical;
        double containingLeadingDistance =
            (std::numeric_limits<double>::max)();
        for ( const CharacterRect& character : lineCharacters )
        {
            const double left = character.left;
            const double right = character.right;
            if ( left < minX )
            {
                minX = left;
                leftLogical = character.logical;
            }
            if ( right > maxX )
            {
                maxX = right;
                rightLogical = character.logical + 1;
            }

            const double distance =
                point.X < left
                    ? left - point.X
                    : point.X > right
                        ? point.X - right
                        : 0.0;
            if ( distance < nearestDistance )
            {
                nearestDistance = distance;
                nearestLogical = character.logical;
            }

            if ( point.X >= left - CoordinateTolerance &&
                 point.X <= right + CoordinateTolerance )
            {
                const double deltaX = point.X - character.leading.X;
                const double deltaY = point.Y - character.leading.Y;
                const double leadingDistance =
                    deltaX * deltaX + deltaY * deltaY;
                if ( !hasContainingCharacter ||
                     leadingDistance < containingLeadingDistance )
                {
                    hasContainingCharacter = true;
                    containingLogical = character.logical;
                    containingLeadingDistance = leadingDistance;
                }
            }
        }

        if ( hasContainingCharacter )
        {
            // Do not let logical scan order decide between two native cells
            // whose fractional-DIP tolerance overlaps. PositionToCoords() is
            // defined by this same TOM leading point, so the closest exact
            // leading is the stable inverse for both visual directions.
            *logicalPosition = containingLogical;
            *result = wxTE_HT_ON_TEXT;
            return true;
        }

        if ( point.X < minX )
        {
            *logicalPosition = leftLogical;
            *result = wxTE_HT_BEFORE;
        }
        else if ( point.X > maxX )
        {
            *logicalPosition = rightLogical;
            *result = wxTE_HT_BEYOND;
        }
        else
        {
            *logicalPosition = nearestLogical;
            *result = wxTE_HT_ON_TEXT;
        }
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

template <typename IsCurrent>
bool wxWinUIHitTestRichTextLine(
    const MUXC::RichEditBox& richEditBox,
    const wxString& value,
    const winrt::Windows::Foundation::Point& point,
    IsCurrent&& isCurrent,
    wxTextCtrlHitTestResult *result,
    long *logicalPosition)
{
    if ( !richEditBox || !result || !logicalPosition || !isCurrent() ||
         value.length() > static_cast<std::size_t>(
                              (std::numeric_limits<int32_t>::max)()) )
    {
        return false;
    }

    struct CharacterRect
    {
        long logical = 0;
        winrt::Windows::Foundation::Rect rect{};
        winrt::Windows::Foundation::Point leading{};
        double left = 0.0;
        double right = 0.0;
    };

    try
    {
        const WUT::PointOptions options = wxWinUIRichClientPointOptions();
        const WUT::ITextDocument document = richEditBox.Document();
        if ( !document || !isCurrent() )
            return false;

        const WUT::ITextRange nearest =
            document.GetRangeFromPoint(point, options);
        if ( !nearest || !isCurrent() )
            return false;

        const WUT::ITextRange line = nearest.GetClone();
        if ( !line || !isCurrent() )
            return false;
        line.Expand(WUT::TextRangeUnit::Line);
        if ( !isCurrent() )
            return false;

        const long length = static_cast<long>(value.length());
        const long lineStart = wxMax<long>(
            0, wxMin<long>(line.StartPosition(), length));
        long lineEnd = wxMax<long>(
            lineStart, wxMin<long>(line.EndPosition(), length));
        if ( !isCurrent() )
            return false;

        // The paragraph delimiter belongs to the TOM Line unit but has no
        // glyph rectangle on that visual line. Its following caret is the
        // leading edge of the next physical line.
        if ( lineEnd > lineStart &&
             value[static_cast<std::size_t>(lineEnd - 1)] == '\n' )
        {
            --lineEnd;
        }

        constexpr unsigned long long MaxExactPhysicalLineLength = 4096;
        if ( static_cast<unsigned long long>(lineEnd - lineStart) >
                 MaxExactPhysicalLineLength )
        {
            return false;
        }

        constexpr double CoordinateTolerance = 0.75;
        double minX = (std::numeric_limits<double>::max)();
        double maxX = -(std::numeric_limits<double>::max)();
        long leftLogical = lineStart;
        long rightLogical = lineStart;
        double top = (std::numeric_limits<double>::max)();
        double bottom = -(std::numeric_limits<double>::max)();
        std::vector<CharacterRect> characters;
        characters.reserve(static_cast<std::size_t>(lineEnd - lineStart));

        for ( long logical = lineStart; logical <= lineEnd; ++logical )
        {
            winrt::Windows::Foundation::Point edge{};
            if ( !wxWinUIGetRichTextPoint(
                     richEditBox, logical, &edge) ||
                 !isCurrent() )
            {
                return false;
            }

            if ( edge.X < minX )
            {
                minX = edge.X;
                leftLogical = logical;
            }
            if ( edge.X > maxX )
            {
                maxX = edge.X;
                rightLogical = logical;
            }

            if ( logical == lineEnd )
                continue;

            const WUT::ITextRange character = document.GetRange(
                static_cast<int32_t>(logical),
                static_cast<int32_t>(logical + 1));
            winrt::Windows::Foundation::Rect rect{};
            int32_t hit = 0;
            character.GetRect(options, rect, hit);
            wxUnusedVar(hit);
            if ( !isCurrent() || !wxWinUIIsFiniteRect(rect) )
                return false;

            if ( rect.Width <= 0.0f || rect.Height <= 0.0f )
                continue;

            minX = wxMin<double>(minX, rect.X);
            maxX = wxMax<double>(maxX, rect.X + rect.Width);
            top = wxMin<double>(top, rect.Y);
            bottom = wxMax<double>(bottom, rect.Y + rect.Height);

            winrt::Windows::Foundation::Point rangeLeft{};
            winrt::Windows::Foundation::Point rangeRight{};
            const bool hasRangeEdges =
                wxWinUIGetRichTextRangePoint(
                    character,
                    WUT::HorizontalCharacterAlignment::Left,
                    true,
                    &rangeLeft) &&
                isCurrent() &&
                wxWinUIGetRichTextRangePoint(
                    character,
                    WUT::HorizontalCharacterAlignment::Right,
                    true,
                    &rangeRight) &&
                isCurrent();
            if ( !isCurrent() )
                return false;

            double left = rect.X;
            double right = rect.X + rect.Width;
            if ( hasRangeEdges )
            {
                const double nativeLeft =
                    wxMin<double>(rangeLeft.X, rangeRight.X);
                const double nativeRight =
                    wxMax<double>(rangeLeft.X, rangeRight.X);
                if ( nativeRight > nativeLeft )
                {
                    left = nativeLeft;
                    right = nativeRight;
                    minX = wxMin<double>(minX, nativeLeft);
                    maxX = wxMax<double>(maxX, nativeRight);
                }
            }
            characters.push_back({ logical, rect, edge, left, right });
        }

        if ( !isCurrent() || minX > maxX )
            return false;

        if ( !characters.empty() )
        {
            if ( point.Y < top - CoordinateTolerance ||
                 point.Y > bottom + CoordinateTolerance )
            {
                return false;
            }

            long nearestCharacter = characters.front().logical;
            double nearestDistance =
                (std::numeric_limits<double>::max)();
            bool hasContainingCharacter = false;
            long containingCharacter = characters.front().logical;
            double containingLeadingDistance =
                (std::numeric_limits<double>::max)();
            for ( const CharacterRect& character : characters )
            {
                const double left = character.left;
                const double right = character.right;
                const double distance =
                    point.X < left
                        ? left - point.X
                        : point.X > right
                            ? point.X - right
                            : 0.0;
                if ( distance < nearestDistance )
                {
                    nearestDistance = distance;
                    nearestCharacter = character.logical;
                }

                // A formatted run can have a smaller glyph rectangle than the
                // physical line box. Once Y is proved to be on this line, the
                // character's horizontal cell is the exact wx ON_TEXT test.
                if ( point.X >= left - CoordinateTolerance &&
                     point.X <= right + CoordinateTolerance )
                {
                    const double deltaX = point.X - character.leading.X;
                    const double deltaY = point.Y - character.leading.Y;
                    const double leadingDistance =
                        deltaX * deltaX + deltaY * deltaY;
                    if ( !hasContainingCharacter ||
                         leadingDistance < containingLeadingDistance )
                    {
                        hasContainingCharacter = true;
                        containingCharacter = character.logical;
                        containingLeadingDistance = leadingDistance;
                    }
                }
            }

            if ( hasContainingCharacter )
            {
                *logicalPosition = containingCharacter;
                *result = wxTE_HT_ON_TEXT;
                return true;
            }

            if ( point.X >= minX - CoordinateTolerance &&
                 point.X <= maxX + CoordinateTolerance )
            {
                *logicalPosition = nearestCharacter;
                *result = wxTE_HT_ON_TEXT;
                return true;
            }
        }

        if ( point.X < minX )
        {
            *logicalPosition = leftLogical;
            *result = wxTE_HT_BEFORE;
        }
        else
        {
            *logicalPosition = rightLogical;
            *result = wxTE_HT_BEYOND;
        }
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

} // namespace

struct wxWinUISecurePasswordSnapshot
{
    wxString value;
    long insertionPoint = 0;
    long selectionStart = 0;
    long selectionEnd = 0;
};

constexpr std::size_t wxWinUIMaxSecurePasswordUndoSnapshots = 100;

// A non-password wxTextCtrl is backed by RichEditBox even when the public wx
// style requests a plain editor. This is an implementation detail: plain wx
// semantics (CRLF positions, no RTF/style API and plain clipboard payloads)
// remain style-gated, while TOM gives both kinds an exact caret and geometry
// surface. Password fields use a RichEdit/TOM adapter whose document contains
// mask glyphs only and whose automation peer exactly follows PasswordBox.
class wxWinUITextCtrlImpl
{
public:
    ~wxWinUITextCtrlImpl()
    {
        Close();
    }

    void RestoreRichPositionVisibilityWrapping()
    {
        if ( !richPositionVisibilityNeedsWrapRestore )
            return;

        if ( !richEditBox )
        {
            richPositionVisibilityNeedsWrapRestore = false;
            return;
        }

        try
        {
            richEditBox.TextWrapping(MUX::TextWrapping::Wrap);
            richEditBox.InvalidateMeasure();
            richEditBox.InvalidateArrange();
            richPositionVisibilityNeedsWrapRestore = false;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI RichEditBox wrapping restoration", e);
        }
    }

    void InvalidatePositionVisibilityRequest()
    {
        RestoreRichPositionVisibilityWrapping();
        ++positionVisibilityGeneration;
        positionVisibilityContentGeneration = textContentGeneration;
        positionVisibilityPending = false;
        positionVisibilityHasEdge = false;
        positionVisibilityScroll = nullptr;
        positionVisibilityView = nullptr;
        positionVisibilityRetryCount = 0;
        richPositionVisibilityTrace = 0;
        richPositionVisibilityPassCount = 0;
    }

    void Close()
    {
        // Invalidate before attempting any revocation: detaching a focused
        // text peer can synchronously flush a pending TextChanged callback,
        // and individual WinRT revocations are allowed to fail.
        if ( callbackState )
            callbackState->Invalidate();

        ++autoCompleteGeneration;

        // Detach application-owned state now, but destroy it only after all
        // member access below has finished. MenuFlyout::Hide() can synchronously
        // run focus/application callbacks and a custom wxTextCompleter can have
        // an arbitrary destructor, including one re-entering this control.
        MUXC::MenuFlyout detachedAutoCompleteFlyout =
            std::move(autoCompleteFlyout);
        autoCompleteFlyout = nullptr;
        std::shared_ptr<wxTextCompleter> detachedAutoCompleter =
            std::move(autoCompleter);

        if ( textBox )
        {
            if ( changedToken.value )
            {
                try
                {
                    textBox.TextChanged(changedToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI TextBox TextChanged removal", e);
                }
            }

            if ( selectionChangedToken.value )
            {
                try
                {
                    textBox.SelectionChanged(selectionChangedToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI TextBox SelectionChanged removal", e);
                }
            }

            if ( beforeTextChangingToken.value )
            {
                try
                {
                    textBox.BeforeTextChanging(beforeTextChangingToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI TextBox BeforeTextChanging removal", e);
                }
            }
        }
        else if ( richEditBox )
        {
            if ( changedToken.value )
            {
                try
                {
                    richEditBox.TextChanged(changedToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI RichEditBox TextChanged removal", e);
                }
            }

            if ( selectionChangedToken.value )
            {
                try
                {
                    richEditBox.SelectionChanged(selectionChangedToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI RichEditBox SelectionChanged removal", e);
                }
            }

            if ( selectionChangingToken.value )
            {
                try
                {
                    richEditBox.SelectionChanging(selectionChangingToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI RichEditBox SelectionChanging removal", e);
                }
            }

            if ( textChangingToken.value )
            {
                try
                {
                    richEditBox.TextChanging(textChangingToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI RichEditBox TextChanging removal", e);
                }
            }

            if ( textCompositionStartedToken.value )
            {
                try
                {
                    richEditBox.TextCompositionStarted(
                        textCompositionStartedToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI RichEditBox composition-start removal", e);
                }
            }

            if ( textCompositionEndedToken.value )
            {
                try
                {
                    richEditBox.TextCompositionEnded(
                        textCompositionEndedToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI RichEditBox composition-end removal", e);
                }
            }

#if wxUSE_RICHEDIT && wxUSE_MENUS
            if ( contextRequestedToken.value )
            {
                try
                {
                    richEditBox.ContextRequested(contextRequestedToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI RichEditBox ContextRequested removal", e);
                }
            }
#endif
        }
        else if ( passwordBox && changedToken.value )
        {
            try
            {
                passwordBox.PasswordChanged(changedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI PasswordBox PasswordChanged removal", e);
            }
        }
        changedToken = {};
        selectionChangingToken = {};
        selectionChangedToken = {};
        beforeTextChangingToken = {};
        textChangingToken = {};
        textCompositionStartedToken = {};
        textCompositionEndedToken = {};
        contextRequestedToken = {};

        if ( copyingToken.value )
        {
            try
            {
                if ( textBox )
                    textBox.CopyingToClipboard(copyingToken);
                else if ( richEditBox )
                    richEditBox.CopyingToClipboard(copyingToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI text CopyingToClipboard removal", e);
            }
        }
        if ( cuttingToken.value )
        {
            try
            {
                if ( textBox )
                    textBox.CuttingToClipboard(cuttingToken);
                else if ( richEditBox )
                    richEditBox.CuttingToClipboard(cuttingToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI text CuttingToClipboard removal", e);
            }
        }
        copyingToken = {};
        cuttingToken = {};

        MUX::UIElement const peer = element();
        if ( peer )
        {
            if ( keyDownToken.value )
            {
                try
                {
                    peer.KeyDown(keyDownToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI text KeyDown removal", e);
                }
            }

            if ( pasteToken.value )
            {
                try
                {
                    if ( textBox )
                        textBox.Paste(pasteToken);
                    else if ( richEditBox )
                        richEditBox.Paste(pasteToken);
                    else if ( passwordBox )
                        passwordBox.Paste(pasteToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI text Paste removal", e);
                }
            }

            if ( passwordChangingToken.value && passwordBox )
            {
                try
                {
                    passwordBox.PasswordChanging(passwordChangingToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI PasswordBox PasswordChanging removal", e);
                }
            }
        }
        keyDownToken = {};
        pasteToken = {};
        passwordChangingToken = {};

        host.Close();
        textBox = nullptr;
        richEditBox = nullptr;
        passwordBox = nullptr;
        callbackState.reset();
        hasPendingPeerValue = false;
        pendingPeerValue.clear();
        passwordContentChangePending = false;
        securePasswordMasking = false;
        securePasswordDispatchPending = false;
        securePasswordDispatchText = false;
        securePasswordDispatchMaxLength = false;
        securePasswordFailClosed = false;
        ++securePasswordFailClosedGeneration;
        securePasswordCompositionActive = false;
        securePasswordCompositionHasSnapshot = false;
        securePasswordDeleteDirection = 0;
        securePasswordEditSelectionArmed = false;
#ifdef WXWINUI_TEST_SUPPORT
        securePasswordExplicitInsertionArmedForTesting = false;
        securePasswordExplicitInsertionForTesting.clear();
#endif
        ++securePasswordEditSelectionGeneration;
        securePasswordUndo.clear();
        securePasswordRedo.clear();
        passwordClipboardExportAttemptCount = 0;
        clipboardOperationInProgress = false;
        InvalidatePositionVisibilityRequest();

        // These are deliberately the final operations in Close(): callbacks
        // reached from either boundary only observe the already-invalidated,
        // fully detached implementation and we never dereference this again.
        if ( detachedAutoCompleteFlyout )
        {
            try
            {
                detachedAutoCompleteFlyout.Hide();
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI text auto-complete flyout removal", e);
            }
        }
        detachedAutoCompleteFlyout = nullptr;
        detachedAutoCompleter.reset();
    }

    wxWinUIControlHost host;
    std::shared_ptr<wxWinUITextCallbackState> callbackState;
    MUXC::TextBox textBox{ nullptr };
    MUXC::RichEditBox richEditBox{ nullptr };
    MUXC::PasswordBox passwordBox{ nullptr };
    winrt::event_token changedToken{};
    winrt::event_token keyDownToken{};
    winrt::event_token selectionChangingToken{};
    winrt::event_token selectionChangedToken{};
    winrt::event_token beforeTextChangingToken{};
    winrt::event_token textChangingToken{};
    winrt::event_token textCompositionStartedToken{};
    winrt::event_token textCompositionEndedToken{};
    winrt::event_token pasteToken{};
    winrt::event_token passwordChangingToken{};
    winrt::event_token copyingToken{};
    winrt::event_token cuttingToken{};
    winrt::event_token contextRequestedToken{};
    std::shared_ptr<wxTextCompleter> autoCompleter;
    MUXC::MenuFlyout autoCompleteFlyout{ nullptr };
    std::uint64_t autoCompleteGeneration = 0;
    std::vector<wxWinUIAutoUrlRange> autoUrlRanges;
    std::uint64_t autoUrlContentGeneration =
        (std::numeric_limits<std::uint64_t>::max)();
    wxString pendingPeerValue;
    bool hasPendingPeerValue = false;
    bool passwordContentChangePending = false;
    bool securePasswordPeer = false;
    bool securePasswordMasking = false;
    bool securePasswordDispatchPending = false;
    bool securePasswordDispatchText = false;
    bool securePasswordDispatchMaxLength = false;
    bool securePasswordFailClosed = false;
    std::uint64_t securePasswordFailClosedGeneration = 0;
    bool securePasswordCompositionActive = false;
    bool securePasswordCompositionHasSnapshot = false;
    int securePasswordDeleteDirection = 0;
    bool securePasswordEditSelectionArmed = false;
    long securePasswordEditSelectionStart = 0;
    long securePasswordEditSelectionEnd = 0;
#ifdef WXWINUI_TEST_SUPPORT
    bool securePasswordExplicitInsertionArmedForTesting = false;
    wxString securePasswordExplicitInsertionForTesting;
#endif
    std::uint64_t securePasswordEditSelectionGeneration = 0;
    std::vector<wxWinUISecurePasswordSnapshot> securePasswordUndo;
    std::vector<wxWinUISecurePasswordSnapshot> securePasswordRedo;
    unsigned passwordClipboardExportAttemptCount = 0;
    bool clipboardOperationInProgress = false;
    std::uint64_t textContentGeneration = 0;
    std::uint64_t positionVisibilityGeneration = 0;
    std::uint64_t positionVisibilityContentGeneration = 0;
    long positionVisibilityLogical = 0;
    wxWinUITextEdge positionVisibilityEdge;
    MUXC::ScrollViewer positionVisibilityScroll{ nullptr };
    MUX::UIElement positionVisibilityView{ nullptr };
    bool positionVisibilityHasEdge = false;
    bool positionVisibilityPending = false;
    unsigned positionVisibilityRetryCount = 0;
    unsigned richPositionVisibilityTrace = 0;
    unsigned richPositionVisibilityPassCount = 0;
    bool richPositionVisibilityNeedsWrapRestore = false;
#ifdef WXWINUI_TEST_SUPPORT
    bool forceNextRichPositionUnavailableForTesting = false;
    bool forceNextPositionVisibilityRetryForTesting = false;
    bool forceTemporarySelectionForTesting = false;
#endif

    MUXC::Control control() const
    {
        if ( passwordBox )
            return passwordBox;
        if ( richEditBox )
            return richEditBox;
        return textBox;
    }

    MUX::UIElement element() const
    {
        if ( passwordBox )
            return passwordBox;
        if ( richEditBox )
            return richEditBox;
        return textBox;
    }

    wxString GetText() const
    {
        if ( passwordBox )
            return wxWinUIFromHString(passwordBox.Password());
        if ( richEditBox )
            return wxWinUIGetRichPlainText(richEditBox);
        if ( textBox )
            return wxWinUIFromHString(textBox.Text());
        return wxString();
    }

    void SetText(const wxString& s,
                 long *insertedStart = nullptr,
                 long *insertedEnd = nullptr)
    {
        if ( insertedStart )
            *insertedStart = wxNOT_FOUND;
        if ( insertedEnd )
            *insertedEnd = wxNOT_FOUND;

        if ( passwordBox )
            passwordBox.Password(wxWinUIToHString(s));
        else if ( richEditBox )
        {
            const wxString oldText = GetText();
            if ( oldText == s )
                return;

            std::size_t prefix = 0;
            const std::size_t common = wxMin(oldText.length(), s.length());
            while ( prefix < common && oldText[prefix] == s[prefix] )
                ++prefix;

            std::size_t suffix = 0;
            while ( suffix < oldText.length() - prefix &&
                    suffix < s.length() - prefix &&
                    oldText[oldText.length() - suffix - 1] ==
                        s[s.length() - suffix - 1] )
            {
                ++suffix;
            }

            const std::size_t oldEnd = oldText.length() - suffix;
            const std::size_t newEnd = s.length() - suffix;
            const WUT::ITextRange range = richEditBox.Document().GetRange(
                static_cast<int32_t>(prefix),
                static_cast<int32_t>(oldEnd));
            range.SetText(
                WUT::TextSetOptions::None,
                wxWinUIToHString(s.Mid(prefix, newEnd - prefix)));
            if ( insertedStart )
                *insertedStart = static_cast<long>(prefix);
            if ( insertedEnd )
                *insertedEnd = static_cast<long>(newEnd);
        }
        else if ( textBox )
            textBox.Text(wxWinUIToHString(s));
    }
};

wxTextCtrl::wxTextCtrl()
    : m_insertionPoint(0),
      m_selectionStart(0),
      m_selectionEnd(0),
      m_maxLength(0),
      m_modified(false),
      m_editable(true),
      m_updatingPeer(false)
{
    // The handler is installed once per C++ object so wxTE_AUTO_URL can also
    // be toggled on an already-rich control without duplicating bindings.
    Bind(wxEVT_MOTION, &wxTextCtrl::OnAutoUrlMouse, this);
    Bind(wxEVT_LEFT_DOWN, &wxTextCtrl::OnAutoUrlMouse, this);
    Bind(wxEVT_LEFT_UP, &wxTextCtrl::OnAutoUrlMouse, this);
    Bind(wxEVT_LEFT_DCLICK, &wxTextCtrl::OnAutoUrlMouse, this);
    Bind(wxEVT_RIGHT_DOWN, &wxTextCtrl::OnAutoUrlMouse, this);
    Bind(wxEVT_RIGHT_UP, &wxTextCtrl::OnAutoUrlMouse, this);
    Bind(wxEVT_RIGHT_DCLICK, &wxTextCtrl::OnAutoUrlMouse, this);
    Bind(wxEVT_SET_FOCUS, &wxTextCtrl::OnSetFocus, this);
#if wxUSE_MENUS
    for ( const int id : { wxID_UNDO, wxID_REDO, wxID_CUT, wxID_COPY,
                           wxID_PASTE, wxID_CLEAR, wxID_SELECTALL } )
    {
        Bind(wxEVT_MENU,
             &wxTextCtrl::OnRichContextMenuCommand,
             this,
             id);
        Bind(wxEVT_UPDATE_UI,
             &wxTextCtrl::OnUpdateRichContextMenuCommand,
             this,
             id);
    }
#endif
#if wxUSE_DRAG_AND_DROP
    Bind(wxEVT_DROP_FILES, &wxTextCtrl::OnDropFiles, this);
#endif
}

wxTextCtrl::wxTextCtrl(wxWindow *parent,
                       wxWindowID id,
                       const wxString& value,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
    : wxTextCtrl()
{
    Create(parent, id, value, pos, size, style, validator, name);
}

wxTextCtrl::~wxTextCtrl()
{
    // The impl is declared before all logical text state and would otherwise
    // be destroyed after it. Disconnect while the complete wx object is
    // still alive, then make the impl destructor a harmless second Close().
    if ( m_winui )
    {
        m_winui->Close();
        m_winui.reset();
    }
}

bool wxTextCtrl::MSWShouldPreProcessMessage(WXMSG* msg)
{
    // A plain Return belongs to a multiline editor. Sending it through the
    // parent accelerator table first lets an "Enter" command steal the key
    // before ContentPreTranslateMessage can deliver it to the XAML TextBox.
    // This is the same rule as the native MSW text control, using the WinUI
    // modifier override so the production and deterministic pipelines agree.
    if ( msg && msg->message == WM_KEYDOWN && msg->wParam == VK_RETURN )
    {
        const wxWinUIKeyboardModifiers modifiers =
            wxWinUITextCtrlGetModifiers(msg);
        if ( !modifiers.shiftDown && !modifiers.controlDown &&
             !modifiers.leftAltDown && !modifiers.rightAltDown &&
             IsMultiLine() )
        {
            return false;
        }
    }

    // Keep standard editing shortcuts in the XAML text peer instead of
    // allowing a parent accelerator to consume them.
    return wxTextEntry::MSWShouldPreProcessMessage(msg) &&
           wxControl::MSWShouldPreProcessMessage(msg);
}

bool wxTextCtrl::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxValidator& validator,
                        const wxString& name)
{
    // wxTE_AUTO_URL is a RichEdit/TOM feature on Windows. Match wxMSW by
    // upgrading a plain request before the peer kind and the public position
    // mapping are fixed for the lifetime of the control.
    if ( style & wxTE_AUTO_URL )
        style |= wxTE_RICH2;
    if ( style & wxTE_RICH2 )
        style |= wxTE_RICH;

    // The WinUI control draws its own border, so suppress the native one.
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    m_value = wxWinUITextPositionMap::NormalizeNewlines(value);
    m_passwordPeerFailClosedHresult = 0;
    m_passwordPeerWasEmptyOnFailClosed = false;
    m_insertionPoint =
        wxWinUIMakeTextPositionMap(*this, m_value).GetLastPosition();
    m_selectionStart = m_selectionEnd = m_insertionPoint;
    m_editable = (style & wxTE_READONLY) == 0;

    m_winui.reset(new wxWinUITextCtrlImpl);
    m_winui->callbackState =
        std::make_shared<wxWinUITextCallbackState>(this);
    wxWinUITextCtrlImpl * const createImpl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> createState =
        createImpl->callbackState;
    const std::uint64_t createGeneration =
        createState->GetGeneration();
    const auto getLiveOwner =
        [createState, createGeneration,
         createImpl]() -> wxTextCtrl *
        {
            if ( createState->GetGeneration() != createGeneration )
                return nullptr;

            wxTextCtrl * const owner =
                createState->GetOwner<wxTextCtrl>();
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != createImpl ||
                 owner->m_winui->callbackState != createState )
            {
                return nullptr;
            }
            return owner;
        };
    const auto closeLiveOwner =
        [&getLiveOwner]()
        {
            if ( wxTextCtrl * const owner = getLiveOwner() )
            {
                owner->m_winui->Close();
                owner->m_winui.reset();
            }
        };

    if ( !createImpl->host.Initialize(this) )
    {
        closeLiveOwner();
        return false;
    }

    wxTextCtrl *liveOwner = getLiveOwner();
    if ( !liveOwner )
        return false;

#ifdef WXWINUI_TEST_SUPPORT
    const auto loadedHook =
        liveOwner->m_nextCreateLoadedHook;
    void * const loadedHookContext =
        liveOwner->m_nextCreateLoadedContext;
    liveOwner->m_nextCreateLoadedHook = nullptr;
    liveOwner->m_nextCreateLoadedContext = nullptr;
    if ( loadedHook )
    {
        createImpl->host.SetNextContentLoadedHookForTesting(
            [createState, createGeneration, createImpl,
             loadedHook, loadedHookContext]()
            {
                if ( createState->GetGeneration() != createGeneration )
                    return;

                wxTextCtrl *owner =
                    createState->GetOwner<wxTextCtrl>();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != createImpl ||
                     owner->m_winui->callbackState != createState )
                {
                    return;
                }
                loadedHook(owner, loadedHookContext);
            });
    }
#endif // WXWINUI_TEST_SUPPORT

    const bool password = (style & wxTE_PASSWORD) != 0;
    const bool multiline = (style & wxTE_MULTILINE) != 0;
    const bool rich = !password && (style & (wxTE_RICH | wxTE_RICH2));
    // Production non-password controls always use RichEditBox. Retain a
    // pre-Create deterministic seam for exercising the bounded legacy
    // TextBox continuation while it remains as a defensive fallback.
    const bool useRichPeer = !password
#ifdef WXWINUI_TEST_SUPPORT
        && !m_useTextBoxPeer
#endif
        ;

    try
    {
        if ( password )
        {
            const winrt::com_ptr<wxWinUIPasswordRichEditBox>
                passwordImplementation =
                    winrt::make_self<wxWinUIPasswordRichEditBox>();
            const MUXC::RichEditBox richEditBox =
                passwordImplementation->Projection();
            richEditBox.AcceptsReturn(false);
            richEditBox.TextWrapping(MUX::TextWrapping::NoWrap);
            richEditBox.IsReadOnly(!m_editable);
            richEditBox.IsSpellCheckEnabled(false);
            richEditBox.IsTextPredictionEnabled(false);
            richEditBox.MaxLength(0);
            richEditBox.ClipboardCopyFormat(
                MUXC::RichEditClipboardFormat::PlainText);
            MUXI::InputScope passwordScope;
            passwordScope.Names().Append(
                MUXI::InputScopeName(
                    MUXI::InputScopeNameValue::Password));
            richEditBox.InputScope(passwordScope);
            // wxTextCtrl owns a separate, bounded secure undo model. Keeping
            // RichEdit undo disabled from the first edit prevents transient
            // clear input from ever entering a native undo record.
            richEditBox.Document().UndoLimit(0);

            m_winui->securePasswordPeer = true;
            m_winui->richEditBox = richEditBox;
            const std::shared_ptr<wxWinUITextCallbackState> callbackState =
                m_winui->callbackState;
            m_winui->textChangingToken = richEditBox.TextChanging(
                [callbackState](
                    MUXC::RichEditBox const&,
                    MUXC::RichEditBoxTextChangingEventArgs const& args)
                {
                    if ( !args.IsContentChanging() )
                        return;

                    wxTextCtrl * const owner =
                        callbackState->GetOwner<wxTextCtrl>();
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui->callbackState != callbackState ||
                         !owner->m_winui->securePasswordPeer ||
                         owner->m_updatingPeer ||
                         owner->m_winui->securePasswordMasking )
                    {
                        return;
                    }
                    owner->ProcessSecurePasswordTextChanging();
                });

            m_winui->changedToken = richEditBox.TextChanged(
                [callbackState](
                    winrt::Windows::Foundation::IInspectable const&,
                    MUX::RoutedEventArgs const&)
                {
                    wxTextCtrl *owner =
                        callbackState->GetOwner<wxTextCtrl>();
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui->callbackState != callbackState )
                    {
                        return;
                    }
                    owner->DispatchSecurePasswordChange();
                });

            m_winui->selectionChangingToken =
                richEditBox.SelectionChanging(
                    [callbackState](
                        MUXC::RichEditBox const&,
                        MUXC::RichEditBoxSelectionChangingEventArgs const&)
                    {
                        wxTextCtrl *owner =
                            callbackState->GetOwner<wxTextCtrl>();
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui->callbackState != callbackState ||
                             !owner->m_winui->securePasswordPeer ||
                             owner->m_updatingPeer ||
                             owner->m_winui->securePasswordMasking )
                        {
                            return;
                        }

                        // Preserve the selection that the edit is replacing.
                        // SelectionChanged is allowed to precede TextChanging
                        // for an input/IME transaction, but decoding identical
                        // mask glyphs requires the pre-edit range.
                        owner->m_winui->securePasswordEditSelectionArmed =
                            true;
                        owner->m_winui->securePasswordEditSelectionStart =
                            owner->m_selectionStart;
                        owner->m_winui->securePasswordEditSelectionEnd =
                            owner->m_selectionEnd;
                        ++owner->m_winui->
                            securePasswordEditSelectionGeneration;
                    });

            m_winui->selectionChangedToken =
                richEditBox.SelectionChanged(
                    [callbackState](
                        winrt::Windows::Foundation::IInspectable const&,
                        MUX::RoutedEventArgs const&)
                    {
                        wxTextCtrl *owner =
                            callbackState->GetOwner<wxTextCtrl>();
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui->callbackState != callbackState ||
                             owner->m_updatingPeer ||
                             owner->m_winui->securePasswordMasking )
                        {
                            return;
                        }
                        owner->ReadSelectionFromPeer();

                        owner = callbackState->GetOwner<wxTextCtrl>();
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui->callbackState != callbackState ||
                             !owner->m_winui->securePasswordPeer ||
                             !owner->m_winui->
                                 securePasswordEditSelectionArmed )
                        {
                            return;
                        }

                        // A pointer/arrow-only selection change must not arm
                        // a later edit. Deferring the reset keeps the snapshot
                        // alive through the current synchronous input or IME
                        // transaction if TextChanging follows this event.
                        wxWinUITextCtrlImpl * const impl =
                            owner->m_winui.get();
                        const std::uint64_t callbackGeneration =
                            callbackState->GetGeneration();
                        const std::uint64_t selectionGeneration =
                            impl->securePasswordEditSelectionGeneration;
                        const MUXD::DispatcherQueue queue =
                            impl->richEditBox.DispatcherQueue();
                        if ( queue )
                        {
                            queue.TryEnqueue(
                                [callbackState, callbackGeneration, impl,
                                 selectionGeneration]()
                                {
                                    if ( callbackState->GetGeneration() !=
                                             callbackGeneration )
                                    {
                                        return;
                                    }
                                    wxTextCtrl * const live =
                                        callbackState->GetOwner<wxTextCtrl>();
                                    if ( live && live->m_winui &&
                                         live->m_winui.get() == impl &&
                                         live->m_winui->callbackState ==
                                             callbackState &&
                                         live->m_winui->
                                             securePasswordEditSelectionGeneration ==
                                             selectionGeneration )
                                    {
                                        live->m_winui->
                                            securePasswordEditSelectionArmed =
                                                false;
                                    }
                                });
                        }
                    });

            m_winui->textCompositionStartedToken =
                richEditBox.TextCompositionStarted(
                    [callbackState](
                        MUXC::RichEditBox const&,
                        MUXC::TextCompositionStartedEventArgs const&)
                    {
                        wxTextCtrl * const owner =
                            callbackState->GetOwner<wxTextCtrl>();
                        if ( owner && owner->m_winui &&
                             owner->m_winui->callbackState == callbackState &&
                             owner->m_winui->securePasswordPeer )
                        {
                            owner->m_winui->
                                securePasswordEditSelectionArmed = true;
                            owner->m_winui->
                                securePasswordEditSelectionStart =
                                    owner->m_selectionStart;
                            owner->m_winui->
                                securePasswordEditSelectionEnd =
                                    owner->m_selectionEnd;
                            ++owner->m_winui->
                                securePasswordEditSelectionGeneration;
                            owner->m_winui->securePasswordCompositionActive =
                                true;
                            owner->m_winui->
                                securePasswordCompositionHasSnapshot = false;
                        }
                    });

            m_winui->textCompositionEndedToken =
                richEditBox.TextCompositionEnded(
                    [callbackState](
                        MUXC::RichEditBox const&,
                        MUXC::TextCompositionEndedEventArgs const&)
                    {
                        wxTextCtrl * const owner =
                            callbackState->GetOwner<wxTextCtrl>();
                        if ( owner && owner->m_winui &&
                             owner->m_winui->callbackState == callbackState &&
                             owner->m_winui->securePasswordPeer )
                        {
                            owner->m_winui->securePasswordCompositionActive =
                                false;
                            owner->m_winui->
                                securePasswordCompositionHasSnapshot = false;
                        }
                    });
        }
        else if ( useRichPeer )
        {
            MUXC::RichEditBox richEditBox;
            richEditBox.AcceptsReturn(multiline);
            richEditBox.TextWrapping(
                (multiline && !(style & wxTE_DONTWRAP))
                    ? MUX::TextWrapping::Wrap
                    : MUX::TextWrapping::NoWrap);

            if ( style & wxTE_RIGHT )
                richEditBox.TextAlignment(MUX::TextAlignment::Right);
            else if ( style & wxTE_CENTRE )
                richEditBox.TextAlignment(MUX::TextAlignment::Center);
            else
                richEditBox.TextAlignment(MUX::TextAlignment::Left);

            richEditBox.IsReadOnly(!m_editable);
            richEditBox.IsSpellCheckEnabled(false);
            richEditBox.MaxLength(0);
            // Preserve rich formatting only for a public rich editor. A plain
            // wxTextCtrl happens to use the same peer for TOM/caret/geometry,
            // but must still put only plain text on the clipboard.
            richEditBox.ClipboardCopyFormat(
                rich ? MUXC::RichEditClipboardFormat::AllFormats
                     : MUXC::RichEditClipboardFormat::PlainText);
            m_winui->richEditBox = richEditBox;

            const std::shared_ptr<wxWinUITextCallbackState> callbackState =
                m_winui->callbackState;
            m_winui->changedToken = richEditBox.TextChanged(
                [callbackState](
                    winrt::Windows::Foundation::IInspectable const&,
                    MUX::RoutedEventArgs const&)
                {
                    wxTextCtrl *owner =
                        callbackState->GetOwner<wxTextCtrl>();
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui->callbackState != callbackState )
                    {
                        return;
                    }

                    wxWinUITextCtrlImpl * const impl = owner->m_winui.get();
                    const wxString peerValue = impl->GetText();
                    if ( impl->hasPendingPeerValue &&
                         impl->pendingPeerValue == peerValue )
                    {
                        impl->hasPendingPeerValue = false;
                        impl->pendingPeerValue.clear();
                        return;
                    }
                    impl->hasPendingPeerValue = false;
                    impl->pendingPeerValue.clear();

                    if ( owner->m_updatingPeer )
                        return;
                    if ( !owner->m_editable )
                    {
                        owner->ApplyValueToPeer();
                        return;
                    }

                    const wxString previousValue = owner->m_value;
                    const wxWinUIConstrainedPeerEdit constrainedEdit =
                        wxWinUIConstrainPeerEdit(
                            *owner,
                            previousValue,
                            peerValue,
                            owner->m_maxLength);
                    const wxString& acceptedValue = constrainedEdit.value;

                    ++impl->textContentGeneration;
                    impl->InvalidatePositionVisibilityRequest();

                    // Capture the native caret before correcting an overflow,
                    // then map it around the rejected part of the proposed edit
                    // before projecting it into the accepted document.
                    owner->m_value = peerValue;
                    owner->ReadSelectionFromPeer();
                    owner->m_selectionStart =
                        constrainedEdit.RemapProposedPosition(
                            owner->m_selectionStart);
                    owner->m_selectionEnd =
                        constrainedEdit.RemapProposedPosition(
                            owner->m_selectionEnd);
                    owner->m_insertionPoint =
                        constrainedEdit.RemapProposedPosition(
                            owner->m_insertionPoint);
                    owner->m_value = acceptedValue;
                    owner->ClampInsertionPoint();
                    const long last = owner->GetLastPosition();
                    owner->m_selectionStart = wxWinUIClampTextPos(
                        owner->m_selectionStart, last);
                    owner->m_selectionEnd = wxWinUIClampTextPos(
                        owner->m_selectionEnd, last);

                    const wxWeakRef<wxWindow> self(owner);
                    if ( constrainedEdit.wasTruncated )
                    {
                        owner->ApplyValueToPeer();
                        owner = wxDynamicCast(self.get(), wxTextCtrl);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui.get() != impl ||
                             owner->m_winui->callbackState != callbackState )
                        {
                            return;
                        }
                    }

                    if ( owner->HasFlag(wxTE_AUTO_URL) &&
                         owner->m_winui->autoUrlContentGeneration !=
                             owner->m_winui->textContentGeneration )
                    {
                        owner->UpdateAutoUrlRanges();
                        owner = wxDynamicCast(self.get(), wxTextCtrl);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui.get() != impl ||
                             owner->m_winui->callbackState != callbackState )
                        {
                            return;
                        }
                    }

                    const bool valueChanged = acceptedValue != previousValue;
                    if ( valueChanged )
                    {
                        owner->m_modified = true;
                        owner->SendTextEvent();
                        owner = wxDynamicCast(self.get(), wxTextCtrl);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui.get() != impl ||
                             owner->m_winui->callbackState != callbackState )
                            return;
                    }

                    if ( constrainedEdit.wasTruncated )
                    {
                        owner->SendMaxLengthEvent();
                        owner = wxDynamicCast(self.get(), wxTextCtrl);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui.get() != impl ||
                             owner->m_winui->callbackState != callbackState )
                        {
                            return;
                        }
                    }

                    if ( valueChanged )
                        owner->RefreshAutoComplete();
                });

            m_winui->selectionChangedToken =
                richEditBox.SelectionChanged(
                    [callbackState](
                        winrt::Windows::Foundation::IInspectable const&,
                        MUX::RoutedEventArgs const&)
                    {
                        wxTextCtrl * const owner =
                            callbackState->GetOwner<wxTextCtrl>();
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui->callbackState != callbackState ||
                             owner->m_updatingPeer )
                        {
                            return;
                        }
                        owner->ReadSelectionFromPeer();
                    });
        }
        else
        {
            MUXC::TextBox textBox;
            textBox.AcceptsReturn(multiline);

            // Wrap style (only meaningful for multiline).  Note: a TextBox only
            // supports NoWrap and Wrap -- WrapWholeWords is TextBlock-only and
            // throws E_INVALIDARG here, so all wx wrap styles map to Wrap.
            textBox.TextWrapping(
                (multiline && !(style & wxTE_DONTWRAP))
                    ? MUX::TextWrapping::Wrap
                    : MUX::TextWrapping::NoWrap);

            // Alignment.
            if ( style & wxTE_RIGHT )
                textBox.TextAlignment(MUX::TextAlignment::Right);
            else if ( style & wxTE_CENTRE )
                textBox.TextAlignment(MUX::TextAlignment::Center);
            else
                textBox.TextAlignment(MUX::TextAlignment::Left);

            textBox.IsReadOnly(!m_editable);

            // Never set the ScrollViewer attached property on TextBox itself:
            // doing so on a multiline TextBox hosted in a XAML island can
            // deadlock the island. wxTE_NO_VSCROLL is applied to the realized
            // ContentElement ScrollViewer after SetContent() below.

            m_winui->textBox = textBox;
            const std::shared_ptr<wxWinUITextCallbackState> callbackState =
                m_winui->callbackState;
            m_winui->changedToken = textBox.TextChanged(
                [callbackState](
                    winrt::Windows::Foundation::IInspectable const&,
                    MUXC::TextChangedEventArgs const&)
                {
                    wxTextCtrl *owner =
                        callbackState->GetOwner<wxTextCtrl>();
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui->callbackState != callbackState )
                    {
                        return;
                    }

                    wxWinUITextCtrlImpl * const impl = owner->m_winui.get();
                    const wxString peerValue =
                        wxWinUITextPositionMap::NormalizeNewlines(
                            impl->GetText());
                    if ( impl->hasPendingPeerValue &&
                         impl->pendingPeerValue == peerValue )
                    {
                        impl->hasPendingPeerValue = false;
                        impl->pendingPeerValue.clear();
                        return;
                    }
                    impl->hasPendingPeerValue = false;
                    impl->pendingPeerValue.clear();

                    if ( owner->m_updatingPeer )
                        return;

                    if ( !owner->m_editable )
                    {
                        owner->ApplyValueToPeer();
                        return;
                    }

                    // Any real peer edit invalidates a deferred geometry
                    // request even if a later edit happens to restore the
                    // same string: the content/layout transaction itself is
                    // part of the request identity.
                    ++impl->textContentGeneration;
                    impl->InvalidatePositionVisibilityRequest();
                    owner->m_value = peerValue;
                    owner->ReadSelectionFromPeer();
                    owner->m_modified = true;
                    const wxWeakRef<wxWindow> self(owner);
                    owner->SendTextEvent();
                    owner = wxDynamicCast(self.get(), wxTextCtrl);
                    if ( owner && owner->m_winui &&
                         owner->m_winui.get() == impl &&
                         owner->m_winui->callbackState == callbackState )
                    {
                        owner->RefreshAutoComplete();
                    }
                });

            // Keep the wx-side caret/selection in sync with the real one so
            // that GetInsertionPoint()/GetSelection() reflect user clicks.
            m_winui->selectionChangedToken = textBox.SelectionChanged(
                [callbackState](
                    winrt::Windows::Foundation::IInspectable const&,
                    MUX::RoutedEventArgs const&)
                {
                    wxTextCtrl * const owner =
                        callbackState->GetOwner<wxTextCtrl>();
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui->callbackState != callbackState ||
                         owner->m_updatingPeer )
                    {
                        return;
                    }
                    owner->ReadSelectionFromPeer();
                });

            // Enforce the wx length contract in public (CRLF-aware)
            // coordinates. XAML's MaxLength counts its normalized LF string
            // and would otherwise accept too much text in a plain multiline
            // control. Programmatic setters are excluded and remain allowed
            // to exceed the user-input limit.
            m_winui->beforeTextChangingToken =
                textBox.BeforeTextChanging(
                    [callbackState](
                        MUXC::TextBox const&,
                        MUXC::TextBoxBeforeTextChangingEventArgs const& args)
                    {
                        wxTextCtrl * const owner =
                            callbackState->GetOwner<wxTextCtrl>();
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui->callbackState != callbackState ||
                             owner->m_updatingPeer || !owner->m_editable ||
                             !owner->m_maxLength )
                        {
                            return;
                        }

                        const wxString proposed =
                            wxWinUITextPositionMap::NormalizeNewlines(
                                wxWinUIFromHString(args.NewText()));
                        const long proposedLength =
                            wxWinUIMakeTextPositionMap(
                                *owner, proposed).GetLastPosition();
                        const long currentLength =
                            owner->GetLastPosition();
                        const unsigned long effectiveLimit =
                            wxMax<unsigned long>(
                                owner->m_maxLength,
                                currentLength > 0
                                    ? static_cast<unsigned long>(
                                          currentLength)
                                    : 0);
                        if ( proposedLength <= 0 ||
                             static_cast<unsigned long>(proposedLength) <=
                                 effectiveLimit )
                        {
                            return;
                        }

                        // Paste is intercepted separately and applies the
                        // fitting prefix. Other over-limit edits (including
                        // IME commits) are rejected atomically.
                        args.Cancel(true);
                        owner->SendMaxLengthEvent();
                    });
        }

        // All peers keep their native limit disabled. TextBox is governed by
        // BeforeTextChanging. RichEditBox and PasswordBox expose the complete
        // proposed value so the shared edit-diff policy can keep the fitting
        // insertion and emit wxEVT_TEXT_MAXLEN exactly.
        if ( m_winui->textBox )
            m_winui->textBox.MaxLength(0);
        else if ( m_winui->richEditBox )
            m_winui->richEditBox.MaxLength(0);
        else if ( m_winui->passwordBox )
            m_winui->passwordBox.MaxLength(0);

        // Project the style bits whose WinUI representation is a dependency
        // property. This also initializes wxTE_NOHIDESEL; subsequent calls use
        // the same path, so creation-time and hot style changes cannot drift.
        SetWindowStyleFlag(style);

        // wxTE_PROCESS_ENTER reports Enter as wxEVT_TEXT_ENTER, while an
        // editable wxTE_PROCESS_TAB control owns Tab and inserts it. The mixed
        // host arbiter deliberately lets the latter key reach this peer.
        // Register this unconditionally because these style bits are mutable.
        // The delegate consults the current flags, which makes adding or
        // removing wxTE_PROCESS_ENTER/TAB after Create() behave exactly like
        // creating the control with the final style.
        {
            const std::shared_ptr<wxWinUITextCallbackState> callbackState =
                m_winui->callbackState;
            m_winui->keyDownToken = m_winui->element().KeyDown(
                [callbackState](
                    winrt::Windows::Foundation::IInspectable const&,
                    MUX::Input::KeyRoutedEventArgs const& args)
                {
                    wxTextCtrl * const owner =
                        callbackState->GetOwner<wxTextCtrl>();
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui->callbackState != callbackState )
                    {
                        return;
                    }

                    if ( owner->m_winui->securePasswordPeer )
                    {
                        owner->m_winui->securePasswordEditSelectionArmed =
                            true;
                        owner->m_winui->securePasswordEditSelectionStart =
                            owner->m_selectionStart;
                        owner->m_winui->securePasswordEditSelectionEnd =
                            owner->m_selectionEnd;
                        ++owner->m_winui->
                            securePasswordEditSelectionGeneration;

                        if ( args.Key() ==
                                 winrt::Windows::System::VirtualKey::Back )
                        {
                            owner->m_winui->securePasswordDeleteDirection =
                                -1;
                        }
                        else if ( args.Key() ==
                                      winrt::Windows::System::VirtualKey::Delete )
                        {
                            owner->m_winui->securePasswordDeleteDirection =
                                1;
                        }
                        else
                        {
                            owner->m_winui->securePasswordDeleteDirection =
                                0;
                        }

                        const int armedDeleteDirection =
                            owner->m_winui->
                                securePasswordDeleteDirection;
                        if ( armedDeleteDirection )
                        {
                            wxWinUITextCtrlImpl * const impl =
                                owner->m_winui.get();
                            const std::uint64_t generation =
                                callbackState->GetGeneration();
                            const MUXD::DispatcherQueue queue =
                                impl->element().DispatcherQueue();
                            if ( queue )
                            {
                                queue.TryEnqueue(
                                    [callbackState, generation, impl,
                                     armedDeleteDirection]()
                                    {
                                        if ( callbackState->GetGeneration() !=
                                                 generation )
                                        {
                                            return;
                                        }
                                        wxTextCtrl * const live =
                                            callbackState->
                                                GetOwner<wxTextCtrl>();
                                        if ( live && live->m_winui &&
                                             live->m_winui.get() == impl &&
                                             live->m_winui->callbackState ==
                                                 callbackState &&
                                             live->m_winui->
                                                 securePasswordDeleteDirection ==
                                                 armedDeleteDirection )
                                        {
                                            live->m_winui->
                                                securePasswordDeleteDirection =
                                                    0;
                                        }
                                    });
                            }
                            if ( callbackState->GetOwner<wxTextCtrl>() !=
                                     owner )
                            {
                                return;
                            }
                        }

                        const wxWinUIKeyboardModifiers modifiers =
                            wxWinUITextCtrlGetModifiers(nullptr);
                        if ( modifiers.controlDown &&
                             !modifiers.leftAltDown &&
                             !modifiers.rightAltDown )
                        {
                            if ( args.Key() ==
                                     winrt::Windows::System::VirtualKey::Z )
                            {
                                args.Handled(true);
                                if ( modifiers.shiftDown )
                                    owner->Redo();
                                else
                                    owner->Undo();
                                return;
                            }
                            if ( args.Key() ==
                                     winrt::Windows::System::VirtualKey::Y )
                            {
                                args.Handled(true);
                                owner->Redo();
                                return;
                            }
                        }
                    }

                    if ( args.Key() ==
                             winrt::Windows::System::VirtualKey::Tab )
                    {
                        if ( owner->HasFlag(wxTE_PROCESS_TAB) &&
                             owner->ProcessTab() )
                        {
                            args.Handled(true);
                        }
                        return;
                    }

                    if ( args.Key() ==
                             winrt::Windows::System::VirtualKey::Enter &&
                         owner->ProcessEnter() )
                    {
                        args.Handled(true);
                    }
                });
        }

        // Project native copy/cut commands (keyboard, context menu and
        // accessibility commands) through the wx clipboard events. A handled
        // event vetoes the XAML default operation just like a handled WM_COPY
        // or WM_CUT does in native wxMSW.
        const std::shared_ptr<wxWinUITextCallbackState> callbackState =
            m_winui->callbackState;
        const auto copyingHandler =
            [callbackState](
                winrt::Windows::Foundation::IInspectable const&,
                MUXC::TextControlCopyingToClipboardEventArgs const& args)
            {
                wxTextCtrl *owner =
                    callbackState->GetOwner<wxTextCtrl>();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState )
                {
                    args.Handled(true);
                    return;
                }

                // Programmatic Copy()/Cut() already emitted the wx event and
                // is now asking TOM to perform the native rich operation.
                if ( owner->m_winui->clipboardOperationInProgress )
                    return;

                const bool protectedContent =
                    owner->HasFlag(wxTE_PASSWORD);
                const bool handled =
                    owner->SendClipboardEvent(wxEVT_TEXT_COPY);
                owner = callbackState->GetOwner<wxTextCtrl>();
                if ( protectedContent || handled ||
                     !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState )
                {
                    args.Handled(true);
                }

                owner = callbackState->GetOwner<wxTextCtrl>();
                if ( owner && owner->m_winui &&
                     owner->m_winui->callbackState == callbackState &&
                     owner->HasFlag(wxTE_PASSWORD) && !args.Handled() )
                {
                    ++owner->m_winui->
                        passwordClipboardExportAttemptCount;
                }
            };
        const auto cuttingHandler =
            [callbackState](
                winrt::Windows::Foundation::IInspectable const&,
                MUXC::TextControlCuttingToClipboardEventArgs const& args)
            {
                wxTextCtrl *owner =
                    callbackState->GetOwner<wxTextCtrl>();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState )
                {
                    args.Handled(true);
                    return;
                }

                if ( owner->m_winui->clipboardOperationInProgress )
                    return;

                const bool protectedContent =
                    owner->HasFlag(wxTE_PASSWORD);
                const bool handled =
                    owner->SendClipboardEvent(wxEVT_TEXT_CUT);
                owner = callbackState->GetOwner<wxTextCtrl>();
                if ( protectedContent || handled ||
                     !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState )
                {
                    args.Handled(true);
                }

                owner = callbackState->GetOwner<wxTextCtrl>();
                if ( owner && owner->m_winui &&
                     owner->m_winui->callbackState == callbackState &&
                     owner->HasFlag(wxTE_PASSWORD) && !args.Handled() )
                {
                    ++owner->m_winui->
                        passwordClipboardExportAttemptCount;
                }
            };
        if ( m_winui->textBox )
        {
            m_winui->copyingToken =
                m_winui->textBox.CopyingToClipboard(copyingHandler);
            m_winui->cuttingToken =
                m_winui->textBox.CuttingToClipboard(cuttingHandler);
        }
        else if ( m_winui->richEditBox )
        {
            m_winui->copyingToken =
                m_winui->richEditBox.CopyingToClipboard(copyingHandler);
            m_winui->cuttingToken =
                m_winui->richEditBox.CuttingToClipboard(cuttingHandler);
        }

        // Own all paste entry points (keyboard and context menu) so validators,
        // partial insertion and wxEVT_TEXT_MAXLEN use one deterministic policy.
        const auto pasteHandler =
            [callbackState](
                winrt::Windows::Foundation::IInspectable const&,
                MUXC::TextControlPasteEventArgs const& args)
            {
                wxTextCtrl *owner =
                    callbackState->GetOwner<wxTextCtrl>();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState )
                {
                    args.Handled(true);
                    return;
                }

                // A programmatic rich Paste() is already inside the wx event
                // transaction. Leaving this nested XAML notification alone
                // lets TOM preserve the native AllFormats payload exactly.
                if ( owner->m_winui->clipboardOperationInProgress )
                    return;

#if wxUSE_CLIPBOARD
                args.Handled(true);
                owner->Paste();
#else
                // Without wxClipboard support there is no synchronous data
                // API with which to apply the partial-insertion policy. Still
                // emit the wx veto event, but otherwise leave XAML's native
                // clipboard command untouched instead of disabling paste.
                if ( owner->m_winui->securePasswordPeer )
                {
                    owner->m_winui->securePasswordEditSelectionArmed = true;
                    owner->m_winui->securePasswordEditSelectionStart =
                        owner->m_selectionStart;
                    owner->m_winui->securePasswordEditSelectionEnd =
                        owner->m_selectionEnd;
                    ++owner->m_winui->
                        securePasswordEditSelectionGeneration;
                }
                const bool handled =
                    owner->SendClipboardEvent(wxEVT_TEXT_PASTE);
                owner = callbackState->GetOwner<wxTextCtrl>();
                if ( handled || !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState )
                {
                    args.Handled(true);
                }
#endif // wxUSE_CLIPBOARD
            };
        if ( m_winui->textBox )
            m_winui->pasteToken = m_winui->textBox.Paste(pasteHandler);
        else if ( m_winui->richEditBox )
            m_winui->pasteToken = m_winui->richEditBox.Paste(pasteHandler);
        else if ( m_winui->passwordBox )
        {
            // PasswordBox intentionally exposes no selection/caret API. Emit
            // the wx event solely as a cancellable gate and leave the actual
            // operation to XAML so it edits the real native selection.
            m_winui->pasteToken = m_winui->passwordBox.Paste(
                [callbackState](
                    winrt::Windows::Foundation::IInspectable const&,
                    MUXC::TextControlPasteEventArgs const& args)
                {
                    wxTextCtrl *owner =
                        callbackState->GetOwner<wxTextCtrl>();
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui->callbackState != callbackState )
                    {
                        args.Handled(true);
                        return;
                    }
                    if ( owner->m_winui->clipboardOperationInProgress )
                        return;

                    const bool handled =
                        owner->SendClipboardEvent(wxEVT_TEXT_PASTE);
                    owner = callbackState->GetOwner<wxTextCtrl>();
                    if ( handled || !owner || !owner->m_winui ||
                         owner->m_winui->callbackState != callbackState )
                    {
                        args.Handled(true);
                    }
                });
        }

#if wxUSE_RICHEDIT && wxUSE_MENUS
        if ( m_winui->richEditBox )
        {
            m_winui->contextRequestedToken =
                m_winui->richEditBox.ContextRequested(
                    [callbackState](
                        winrt::Windows::Foundation::IInspectable const&,
                        MUX::Input::ContextRequestedEventArgs const& args)
                    {
                        // Claim the XAML request before invoking user code so
                        // the built-in flyout can't appear after destruction.
                        args.Handled(true);
                        wxTextCtrl * const owner =
                            callbackState->GetOwner<wxTextCtrl>();
                        if ( owner && owner->m_winui &&
                             owner->m_winui->callbackState == callbackState )
                        {
                            owner->ShowRichContextMenu();
                        }
                    });
        }
#endif

        ApplyValueToPeer();
        UpdateWinUIAppearance();
        const MUX::UIElement element = createImpl->element();
        if ( !createImpl->host.SetContent(element) )
        {
            closeLiveOwner();
            return false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TextBox creation", e);
        closeLiveOwner();
        return false;
    }

    liveOwner = getLiveOwner();
    if ( !liveOwner )
        return false;

#ifdef WXWINUI_TEST_SUPPORT
    if ( loadedHook )
    {
        // The production Loaded path remains naturally asynchronous. The
        // implementation-only hook asks us to cross it synchronously so
        // destruction and re-entry during Create() are deterministic tests.
        createImpl->host.ForceRender();
        liveOwner = getLiveOwner();
        if ( !liveOwner )
            return false;

        // Final host access until getLiveOwner() proves that the callback did
        // not destroy or replace this exact implementation.
        createImpl->host.DispatchPendingContentLoadedHookForTesting();
        liveOwner = getLiveOwner();
        if ( !liveOwner )
            return false;
    }
#endif // WXWINUI_TEST_SUPPORT

    liveOwner->SetInitialSize(size);

    liveOwner = getLiveOwner();
    if ( !liveOwner )
        return false;

    if ( multiline && !password )
    {
        // Initial attempt plus at most three dispatcher turns. The callback
        // state and exact implementation identity make every deferred attempt
        // inert after destroy or re-entrant Create().
        liveOwner->ApplyVerticalScrollBarPolicy(3);
    }

    liveOwner = getLiveOwner();
    if ( !liveOwner )
        return false;

    // ForceRender() may synchronously dispatch application callbacks and
    // destroy the control. It is deliberately the final operation touching
    // the implementation; only independently-owned callback state is read
    // afterwards.
    createImpl->host.ForceRender();
    return getLiveOwner() != nullptr;
}

bool wxTextCtrl::ProcessEnter()
{
    if ( !HasFlag(wxTE_PROCESS_ENTER) )
        return false;

    const wxWeakRef<wxWindow> self(this);
    wxCommandEvent event(wxEVT_TEXT_ENTER, GetId());
    event.SetEventObject(this);
    event.SetString(m_value);
    if ( HandleWindowEvent(event) )
        return true;

    wxWindow * const liveSelf = self.get();
    if ( !liveSelf )
        return true;

    // A multiline editor owns Return even when its TEXT_ENTER event is
    // skipped. For a single-line editor, match the native MSW contract:
    // an unhandled wxTE_PROCESS_ENTER activates the dialog's effective
    // default button.
    if ( HasFlag(wxTE_MULTILINE) )
        return false;

    wxDialog * const dialog =
        wxDynamicCast(wxGetTopLevelParent(liveSelf), wxDialog);
    if ( !dialog || dialog->IsBeingDeleted() )
        return false;

    return dialog->WinUIActivateDefaultButton();
}

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITextCtrlTestAccess::ProcessEnter(wxTextCtrl& textCtrl)
{
    return textCtrl.ProcessEnter();
}
#endif // WXWINUI_TEST_SUPPORT

bool wxTextCtrl::ProcessTab()
{
    if ( !HasFlag(wxTE_PROCESS_TAB) || !IsEditable() )
        return false;

    WriteTextWithPolicy(wxS("\t"), true);
    return true;
}

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITextCtrlTestAccess::ProcessTab(wxTextCtrl& textCtrl)
{
    return textCtrl.ProcessTab();
}
#endif // WXWINUI_TEST_SUPPORT

wxString wxTextCtrl::GetRange(long from, long to) const
{
    const wxWinUITextPositionMap map =
        wxWinUIMakeTextPositionMap(*this, m_value);
    const long len = map.GetLastPosition();
    from = wxWinUIClampTextPos(from, len);
    to = wxWinUIClampTextPos(to, len);

    if ( to < from )
        return wxString();

    return map.GetRange(from, to);
}

wxString wxTextCtrl::GetRTFValue() const
{
    wxCHECK_MSG(IsRich() && m_winui && m_winui->richEditBox,
                wxString(),
                "RTF support is only available for rich controls");

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const wxWeakRef<wxWindow> self(const_cast<wxTextCtrl *>(this));
    const auto isCurrent = [&]()
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return false;
        const wxTextCtrl * const live =
            wxDynamicCast(self.get(), wxTextCtrl);
        return live && live->m_winui && live->m_winui.get() == impl &&
               live->m_winui->callbackState == callbackState &&
               live->m_winui->richEditBox == richEditBox;
    };

    try
    {
        winrt::hstring rtf;
        const WUT::ITextDocument document = richEditBox.Document();
        if ( !document || !isCurrent() )
            return wxString();
        document.GetText(WUT::TextGetOptions::FormatRtf, rtf);
        return isCurrent() ? wxWinUIFromHString(rtf) : wxString();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI RichEditBox RTF query", e);
        return wxString();
    }
}

void wxTextCtrl::SetRTFValue(const wxString& val)
{
    wxCHECK_RET(IsRTFSupported() && m_winui && m_winui->richEditBox,
                "RTF support is only available for rich controls");

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const wxWeakRef<wxWindow> self(this);
    const auto getLiveOwner = [&]() -> wxTextCtrl *
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return nullptr;
        wxTextCtrl * const owner = wxDynamicCast(self.get(), wxTextCtrl);
        return owner && owner->m_winui &&
                       owner->m_winui.get() == impl &&
                       owner->m_winui->callbackState == callbackState &&
                       owner->m_winui->richEditBox == richEditBox
                 ? owner
                 : nullptr;
    };

    const wxString oldValue = m_value;
    const bool forceUpper = m_forceUpper;
    const bool wasUpdatingPeer = m_updatingPeer;
    bool wasReadOnly = false;
    try
    {
        m_updatingPeer = true;
        wxScopeGuard updatingGuard = wxMakeGuard(
            [getLiveOwner, wasUpdatingPeer]()
            {
                if ( wxTextCtrl * const owner = getLiveOwner() )
                    owner->m_updatingPeer = wasUpdatingPeer;
            });
        wxUnusedVar(updatingGuard);

        wasReadOnly = richEditBox.IsReadOnly();
        if ( !getLiveOwner() )
            return;
        if ( wasReadOnly )
        {
            richEditBox.IsReadOnly(false);
            if ( !getLiveOwner() )
                return;
        }

        const WUT::ITextDocument document = richEditBox.Document();
        if ( !document || !getLiveOwner() )
            return;
        document.SetText(
            WUT::TextSetOptions::FormatRtf, wxWinUIToHString(val));
        if ( !getLiveOwner() )
            return;

        if ( forceUpper )
        {
            const WUT::ITextRange story =
                document.GetRange(0, 0);
            if ( !story || !getLiveOwner() )
                return;
            story.Expand(WUT::TextRangeUnit::Story);
            if ( !getLiveOwner() )
                return;
            story.ChangeCase(WUT::LetterCase::Upper);
            if ( !getLiveOwner() )
                return;
        }

        if ( wasReadOnly )
            richEditBox.IsReadOnly(true);
        if ( !getLiveOwner() )
            return;

        const wxString newValue = wxWinUIGetRichPlainText(richEditBox);
        wxTextCtrl * const live = getLiveOwner();
        if ( !live )
            return;
        live->m_value = newValue;
        live->m_insertionPoint = 0;
        live->m_selectionStart = live->m_selectionEnd = 0;
        live->m_modified = false;
        ++live->m_winui->textContentGeneration;
        live->m_winui->InvalidatePositionVisibilityRequest();
        document.Selection().SetRange(0, 0);
    }
    catch ( const winrt::hresult_error& e )
    {
        if ( wasReadOnly )
        {
            try
            {
                richEditBox.IsReadOnly(true);
            }
            catch ( const winrt::hresult_error& )
            {
            }
        }
        wxWinUILogException("WinUI RichEditBox RTF assignment", e);
        return;
    }

    wxTextCtrl *live = getLiveOwner();
    if ( live && live->HasFlag(wxTE_AUTO_URL) )
    {
        live->UpdateAutoUrlRanges();
        live = getLiveOwner();
    }
    if ( live && live->m_value != oldValue )
        live->SendTextEvent();
}

wxTextSearchResult wxTextCtrl::SearchText(
    const wxTextSearch& search) const
{
    if ( !IsRich() || !m_winui || !m_winui->richEditBox ||
         search.m_searchValue.empty() )
    {
        return wxTextSearchResult();
    }

    const long last = GetLastPosition();
    const long start = search.m_startingPosition != -1
                         ? search.m_startingPosition
                         : search.m_direction == wxTextSearch::Direction::Down
                             ? 0
                             : last;
    if ( start < 0 || start > last ||
         last > (std::numeric_limits<int32_t>::max)() )
    {
        return wxTextSearchResult();
    }

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const wxWeakRef<wxWindow> self(const_cast<wxTextCtrl *>(this));
    const auto isCurrent = [&]()
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return false;
        const wxTextCtrl * const live =
            wxDynamicCast(self.get(), wxTextCtrl);
        return live && live->m_winui && live->m_winui.get() == impl &&
               live->m_winui->callbackState == callbackState &&
               live->m_winui->richEditBox == richEditBox;
    };

    try
    {
        WUT::FindOptions options = WUT::FindOptions::None;
        if ( search.m_matchCase )
        {
            options = static_cast<WUT::FindOptions>(
                static_cast<uint32_t>(options) |
                static_cast<uint32_t>(WUT::FindOptions::Case));
        }
        if ( search.m_wholeWord )
        {
            options = static_cast<WUT::FindOptions>(
                static_cast<uint32_t>(options) |
                static_cast<uint32_t>(WUT::FindOptions::Word));
        }

        const WUT::ITextDocument document = richEditBox.Document();
        if ( !document || !isCurrent() )
            return wxTextSearchResult();
        const WUT::ITextRange range = document.GetRange(
            static_cast<int32_t>(start),
            static_cast<int32_t>(start));
        if ( !range || !isCurrent() )
            return wxTextSearchResult();
        const int32_t scanLength =
            search.m_direction == wxTextSearch::Direction::Down
                ? static_cast<int32_t>(last - start)
                : -static_cast<int32_t>(start);
        if ( range.FindText(
                 wxWinUIToHString(search.m_searchValue),
                 scanLength,
                 options) == 0 || !isCurrent() )
        {
            return wxTextSearchResult();
        }

        const long foundStart = range.StartPosition();
        if ( !isCurrent() )
            return wxTextSearchResult();
        const long foundEnd = range.EndPosition();
        return isCurrent()
                 ? wxTextSearchResult{foundStart, foundEnd}
                 : wxTextSearchResult();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI RichEditBox text search", e);
        return wxTextSearchResult();
    }
}

bool wxTextCtrl::SetStyle(long start,
                          long end,
                          const wxTextAttr& style)
{
    if ( !IsRich() || !m_winui || !m_winui->richEditBox )
        return false;

    const long last = GetLastPosition();
    if ( start == -1 && end == -1 )
    {
        start = 0;
        end = last;
    }
    else
    {
        start = wxWinUIClampTextPos(start, last);
        end = wxWinUIClampTextPos(end, last);
        if ( start > end )
            wxSwap(start, end);
    }
    if ( end > (std::numeric_limits<int32_t>::max)() )
        return false;

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const wxWeakRef<wxWindow> self(this);
    const auto getLiveOwner = [&]() -> wxTextCtrl *
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return nullptr;
        wxTextCtrl * const live = wxDynamicCast(self.get(), wxTextCtrl);
        return live && live->m_winui && live->m_winui.get() == impl &&
                       live->m_winui->callbackState == callbackState &&
                       live->m_winui->richEditBox == richEditBox
                 ? live
                 : nullptr;
    };

    try
    {
        const WUT::ITextDocument document = richEditBox.Document();
        if ( !document || !getLiveOwner() )
            return false;
        const WUT::ITextRange range = document.GetRange(
            static_cast<int32_t>(start), static_cast<int32_t>(end));
        if ( !range || !getLiveOwner() )
            return false;
        const bool fullySupported = wxWinUIApplyTextStyle(range, style);
        wxTextCtrl * const live = getLiveOwner();
        if ( !live )
            return false;
        // Final access: rendering can synchronously dispatch Loaded/layout
        // application callbacks and destroy the control.
        live->m_winui->host.ForceRender();
        return fullySupported;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI RichEditBox style assignment", e);
        return false;
    }
}

bool wxTextCtrl::SetDefaultStyle(const wxTextAttr& style)
{
    if ( !wxTextCtrlBase::SetDefaultStyle(style) )
        return false;
    if ( !IsRich() || !m_winui || !m_winui->richEditBox )
        return false;

    // Match wxMSW: formatting the final insertion range makes subsequent
    // typing and AppendText() use the merged default without rewriting any
    // existing run.
    return !IsEditable() ||
           SetStyle(GetLastPosition(), GetLastPosition(), m_defaultStyle);
}

bool wxTextCtrl::GetStyle(long position, wxTextAttr& style)
{
    if ( !IsRich() || !m_winui || !m_winui->richEditBox )
        return false;

    const long last = GetLastPosition();
    if ( position < 0 || position > last ||
         last > (std::numeric_limits<int32_t>::max)() )
    {
        return false;
    }

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const wxWeakRef<wxWindow> self(this);
    const auto isCurrent = [&]()
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return false;
        const wxTextCtrl * const live =
            wxDynamicCast(self.get(), wxTextCtrl);
        return live && live->m_winui && live->m_winui.get() == impl &&
               live->m_winui->callbackState == callbackState &&
               live->m_winui->richEditBox == richEditBox;
    };

    try
    {
        const long end = position < last ? position + 1 : position;
        const WUT::ITextDocument document = richEditBox.Document();
        if ( !document || !isCurrent() )
            return false;
        const WUT::ITextRange range = document.GetRange(
            static_cast<int32_t>(position), static_cast<int32_t>(end));
        if ( !range || !isCurrent() )
            return false;
        wxWinUIReadTextStyle(range, &style);
        return isCurrent();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI RichEditBox style query", e);
        return false;
    }
}

#if wxUSE_SPELLCHECK

bool wxTextCtrl::EnableProofCheck(const wxTextProofOptions& options)
{
    wxCHECK_MSG(HasFlag(wxTE_RICH2) && IsRich() &&
                    m_winui && m_winui->richEditBox,
                false,
                "Proof checking requires wxTE_RICH2");

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const wxWeakRef<wxWindow> self(this);
    const auto isCurrent = [&]()
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return false;
        const wxTextCtrl * const live =
            wxDynamicCast(self.get(), wxTextCtrl);
        return live && live->m_winui && live->m_winui.get() == impl &&
               live->m_winui->callbackState == callbackState &&
               live->m_winui->richEditBox == richEditBox;
    };

    try
    {
        if ( !options.GetLang().empty() )
        {
            richEditBox.Language(wxWinUIToHString(options.GetLang()));
            if ( !isCurrent() )
                return false;
        }
        richEditBox.IsSpellCheckEnabled(options.IsSpellCheckEnabled());
        if ( !isCurrent() )
            return false;

        // This is also the wxMSW contract: RichEdit exposes spelling but no
        // independently controllable grammar switch through wxTextCtrl.
        const bool enabled = richEditBox.IsSpellCheckEnabled();
        return isCurrent() &&
               enabled == options.IsSpellCheckEnabled() &&
               !options.IsGrammarCheckEnabled();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI RichEditBox proof checking", e);
        return false;
    }
}

wxTextProofOptions wxTextCtrl::GetProofCheckOptions() const
{
    wxTextProofOptions options = wxTextProofOptions::Disable();
    if ( !HasFlag(wxTE_RICH2) || !IsRich() ||
         !m_winui || !m_winui->richEditBox )
    {
        return options;
    }

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const wxWeakRef<wxWindow> self(const_cast<wxTextCtrl *>(this));
    const auto isCurrent = [&]()
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return false;
        const wxTextCtrl * const live =
            wxDynamicCast(self.get(), wxTextCtrl);
        return live && live->m_winui && live->m_winui.get() == impl &&
               live->m_winui->callbackState == callbackState &&
               live->m_winui->richEditBox == richEditBox;
    };

    try
    {
        const bool spellCheckEnabled =
            richEditBox.IsSpellCheckEnabled();
        if ( !isCurrent() )
            return wxTextProofOptions::Disable();
        if ( spellCheckEnabled )
            options.SpellCheck();
        const wxString language =
            wxWinUIFromHString(richEditBox.Language());
        if ( !isCurrent() )
            return wxTextProofOptions::Disable();
        if ( !language.empty() )
            options.Language(language);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI RichEditBox proof state query", e);
    }
    return options;
}

#endif // wxUSE_SPELLCHECK

bool wxTextCtrl::IsEmpty() const
{
    return m_value.empty();
}

void wxTextCtrl::WriteText(const wxString& text)
{
    WriteTextWithPolicy(text, false);
}

void wxTextCtrl::WriteTextWithPolicy(const wxString& text,
                                     bool enforceMaxLength)
{
    long from = m_selectionStart;
    long to = m_selectionEnd;
    if ( from > to )
        wxSwap(from, to);

    if ( from == to )
        from = to = m_insertionPoint;

    wxString accepted =
        wxWinUITextPositionMap::NormalizeNewlines(text);
    bool truncated = false;
    if ( enforceMaxLength && m_maxLength )
    {
        const wxWinUITextPositionMap map =
            wxWinUIMakeTextPositionMap(*this, m_value);
        wxString baseValue;
        if ( map.ReplaceRange(
                 wxWinUIClampTextPos(from, map.GetLastPosition()),
                 wxWinUIClampTextPos(to, map.GetLastPosition()),
                 wxString(),
                 &baseValue) )
        {
            const long baseLength =
                wxWinUIMakeTextPositionMap(
                    *this, baseValue).GetLastPosition();
            const unsigned long available =
                baseLength < 0 ||
                static_cast<unsigned long>(baseLength) >= m_maxLength
                    ? 0
                    : m_maxLength -
                          static_cast<unsigned long>(baseLength);
            const long acceptedLength =
                wxWinUIMakeTextPositionMap(
                    *this, accepted).GetLastPosition();
            if ( acceptedLength < 0 ||
                 static_cast<unsigned long>(acceptedLength) > available )
            {
                accepted = wxWinUIFitTextPrefix(
                    accepted,
                    available,
                    IsMultiLine() &&
                        wxWinUIUsesTwoCharacterNewlines(*this));
                truncated = true;
            }
        }
    }

    const wxWeakRef<wxWindow> self(this);
    if ( !accepted.empty() || from != to )
        Replace(from, to, accepted);

    if ( truncated )
    {
        wxTextCtrl * const live =
            wxDynamicCast(self.get(), wxTextCtrl);
        if ( live )
            live->SendMaxLengthEvent();
    }
}

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUITextCtrlTestAccess::PasteText(
    wxTextCtrl& textCtrl,
    const wxString& text)
{
    textCtrl.WriteTextWithPolicy(text, true);
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITextCtrlTestAccess::InjectPasswordContentChange(
    wxTextCtrl& textCtrl,
    const wxString& proposed)
{
    if ( !textCtrl.m_winui )
        return false;

    if ( textCtrl.m_winui->securePasswordPeer && textCtrl.m_winui->richEditBox )
    {
        textCtrl.ProcessPasswordPeerChange(
            wxWinUITextPositionMap::NormalizeNewlines(proposed), true);
        return true;
    }

    if ( !textCtrl.m_winui->passwordBox )
        return false;

    // Setting Password is not a deterministic surrogate for user input:
    // PasswordChanging/PasswordChanged may be omitted or deferred by XAML.
    // This seam explicitly injects a complete content-change proposal into
    // the same transaction used by the real PasswordChanged callback. It
    // first updates the peer under the ordinary programmatic-update guard so
    // the model and peer have the same post-edit starting point.
    const wxString normalized =
        wxWinUITextPositionMap::NormalizeNewlines(proposed);
    const MUXC::PasswordBox passwordBox = textCtrl.m_winui->passwordBox;
    wxWinUITextCtrlImpl * const impl = textCtrl.m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const wxWeakRef<wxWindow> self(&textCtrl);
    const auto getExactOwner = [&]() -> wxTextCtrl *
    {
        wxTextCtrl * const live =
            wxDynamicCast(self.get(), wxTextCtrl);
        return live && live->m_winui &&
                       live->m_winui.get() == impl &&
                       live->m_winui->callbackState == callbackState &&
                       live->m_winui->passwordBox == passwordBox
                 ? live
                 : nullptr;
    };
    const bool wasUpdatingPeer = textCtrl.m_updatingPeer;
    try
    {
        {
            textCtrl.m_updatingPeer = true;
            wxScopeGuard updatingGuard = wxMakeGuard(
                [getExactOwner, wasUpdatingPeer]()
            {
                if ( wxTextCtrl * const live = getExactOwner() )
                    live->m_updatingPeer = wasUpdatingPeer;
            });
            wxUnusedVar(updatingGuard);
            passwordBox.Password(wxWinUIToHString(normalized));
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI PasswordBox content-change test injection", e);
        return false;
    }

    wxTextCtrl * const live = getExactOwner();
    if ( !live )
        return false;
    live->ProcessPasswordPeerChange(normalized, true);
    return true;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITextCtrlTestAccess::SetTextBoxPeerText(
    wxTextCtrl& textCtrl,
    const wxString& text)
{
    if ( !textCtrl.m_winui ||
         (!textCtrl.m_winui->textBox && !textCtrl.m_winui->richEditBox) )
        return false;

    wxWinUITextCtrlImpl * const impl = textCtrl.m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const MUXC::TextBox textBox = impl->textBox;
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const bool unlockReadOnly = !textCtrl.m_editable;
    const auto getExactOwner = [&]() -> wxTextCtrl *
    {
        wxTextCtrl * const owner =
            callbackState->GetOwner<wxTextCtrl>();
        return owner && owner->m_winui &&
                       owner->m_winui.get() == impl &&
                       owner->m_winui->callbackState == callbackState &&
                       owner->m_winui->textBox == textBox &&
                       owner->m_winui->richEditBox == richEditBox
                 ? owner
                 : nullptr;
    };

    impl->hasPendingPeerValue = false;
    impl->pendingPeerValue.clear();
    try
    {
        wxScopeGuard readOnlyGuard = wxMakeGuard(
            [getExactOwner, textBox, richEditBox, unlockReadOnly]()
            {
                if ( !unlockReadOnly || !getExactOwner() )
                    return;
                try
                {
                    if ( textBox )
                        textBox.IsReadOnly(true);
                    else
                        richEditBox.IsReadOnly(true);
                }
                catch ( const winrt::hresult_error& )
                {
                }
            });
        wxUnusedVar(readOnlyGuard);

        if ( unlockReadOnly )
        {
            if ( textBox )
                textBox.IsReadOnly(false);
            else
                richEditBox.IsReadOnly(false);
            if ( !getExactOwner() )
                return false;
        }

        const wxString normalized =
            wxWinUITextPositionMap::NormalizeNewlines(text);
        if ( textBox )
            textBox.Text(wxWinUIToHString(normalized));
        else
            impl->SetText(normalized);
        return getExactOwner() != nullptr;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI text peer test mutation", e);
        return false;
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITextCtrlTestAccess::GetPeerText(
    const wxTextCtrl& textCtrl,
    wxString *text)
{
    if ( !text || !textCtrl.m_winui ||
         (!textCtrl.m_winui->textBox && !textCtrl.m_winui->richEditBox &&
          !textCtrl.m_winui->passwordBox) )
    {
        return false;
    }

    wxWinUITextCtrlImpl * const impl = textCtrl.m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const MUXC::TextBox textBox = impl->textBox;
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const MUXC::PasswordBox passwordBox = impl->passwordBox;
    const wxWeakRef<wxWindow> self(
        const_cast<wxTextCtrl *>(&textCtrl));
    const auto isCurrent = [&]()
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return false;
        const wxTextCtrl * const owner =
            wxDynamicCast(self.get(), wxTextCtrl);
        return owner && owner->m_winui &&
               owner->m_winui.get() == impl &&
               owner->m_winui->callbackState == callbackState &&
               owner->m_winui->textBox == textBox &&
               owner->m_winui->richEditBox == richEditBox &&
               owner->m_winui->passwordBox == passwordBox;
    };

    try
    {
        wxString peerValue;
        if ( passwordBox )
            peerValue = wxWinUIFromHString(passwordBox.Password());
        else if ( richEditBox )
            peerValue = wxWinUIGetRichPlainText(richEditBox);
        else
            peerValue = wxWinUIFromHString(textBox.Text());

        if ( !isCurrent() )
            return false;
        *text = wxWinUITextPositionMap::NormalizeNewlines(peerValue);
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI text peer test query", e);
        return false;
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITextCtrlTestAccess::ReplacePeerSelection(
    wxTextCtrl& textCtrl,
    const wxString& text)
{
    if ( !textCtrl.m_winui ||
         (!textCtrl.m_winui->textBox && !textCtrl.m_winui->richEditBox) )
        return false;

    wxWinUITextCtrlImpl * const impl = textCtrl.m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const bool securePasswordPeer = impl->securePasswordPeer;
    const bool unlockReadOnlyPasswordPeer =
        securePasswordPeer && !textCtrl.m_editable;
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const wxWeakRef<wxWindow> self(&textCtrl);
    try
    {
        wxScopeGuard readOnlyGuard = wxMakeGuard(
            [callbackState, impl, richEditBox,
             unlockReadOnlyPasswordPeer]()
            {
                if ( !unlockReadOnlyPasswordPeer )
                    return;
                wxTextCtrl * const owner =
                    callbackState->GetOwner<wxTextCtrl>();
                if ( owner && owner->m_winui &&
                     owner->m_winui.get() == impl &&
                     owner->m_winui->callbackState == callbackState &&
                     owner->m_winui->richEditBox == richEditBox )
                {
                    try
                    {
                        richEditBox.IsReadOnly(true);
                    }
                    catch ( const winrt::hresult_error& )
                    {
                    }
                }
            });
        wxUnusedVar(readOnlyGuard);
        if ( unlockReadOnlyPasswordPeer )
            richEditBox.IsReadOnly(false);

        // SelectedText goes through RichEdit's real edit/undo transaction and
        // the production TextChanged callback. Unlike assigning Text, &textCtrl
        // therefore provides a deterministic, non-input test seam for the
        // native undo history.
        const wxString normalized =
            wxWinUITextPositionMap::NormalizeNewlines(text);
        wxString peerReplacement = normalized;
        bool explicitInsertionArmed = false;
        if ( securePasswordPeer && !normalized.empty() )
        {
            winrt::hstring selectedText;
            richEditBox.Document().Selection().GetText(
                WUT::TextGetOptions::None, selectedText);
            const wxString selected = wxWinUIFromHString(selectedText);
            if ( selected == normalized )
            {
                // Replacing a mask glyph with the same literal glyph is a TOM
                // no-op and emits no TextChanging event. The seam must preserve
                // the caller's explicit insertion intent without pretending a
                // native event occurred: write an equal-length non-secret
                // sentinel so the real synchronous production transaction runs,
                // and let ProcessSecurePasswordTextChanging() substitute the
                // explicitly armed input while decoding that transaction.
                impl->securePasswordExplicitInsertionArmedForTesting = true;
                impl->securePasswordExplicitInsertionForTesting = normalized;
                explicitInsertionArmed = true;
                peerReplacement.clear();
                peerReplacement.reserve(normalized.length());
                for ( std::size_t n = 0; n < normalized.length(); ++n )
                    peerReplacement += static_cast<wxChar>(0xfffd);
            }
        }
        wxScopeGuard explicitInsertionGuard = wxMakeGuard(
            [callbackState, impl, richEditBox, explicitInsertionArmed]()
            {
                if ( !explicitInsertionArmed )
                    return;
                wxTextCtrl * const owner =
                    callbackState->GetOwner<wxTextCtrl>();
                if ( owner && owner->m_winui &&
                     owner->m_winui.get() == impl &&
                     owner->m_winui->callbackState == callbackState &&
                     owner->m_winui->richEditBox == richEditBox )
                {
                    impl->securePasswordExplicitInsertionArmedForTesting =
                        false;
                    impl->securePasswordExplicitInsertionForTesting.clear();
                }
            });
        wxUnusedVar(explicitInsertionGuard);

        const winrt::hstring replacement =
            wxWinUIToHString(peerReplacement);
        if ( impl->textBox )
            impl->textBox.SelectedText(replacement);
        else
            richEditBox.Document().Selection().SetText(
                WUT::TextSetOptions::None, replacement);

        if ( securePasswordPeer )
        {
            wxTextCtrl * const live =
                wxDynamicCast(self.get(), wxTextCtrl);
            if ( !live || !live->m_winui ||
                 live->m_winui.get() != impl ||
                 live->m_winui->callbackState != callbackState )
            {
                return false;
            }
            live->DispatchSecurePasswordChange();

            wxTextCtrl * const finalOwner =
                wxDynamicCast(self.get(), wxTextCtrl);
            if ( !finalOwner ||
                 finalOwner->m_passwordPeerFailClosedHresult ||
                 (explicitInsertionArmed && finalOwner->m_winui &&
                  finalOwner->m_winui.get() == impl &&
                  finalOwner->m_winui->
                      securePasswordExplicitInsertionArmedForTesting) )
            {
                return false;
            }
        }
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI TextBox selected-text test mutation", e);
        return false;
    }
}
#endif // WXWINUI_TEST_SUPPORT

void wxTextCtrl::AppendText(const wxString& text)
{
    SetInsertionPointEnd();
    WriteText(text);
}

void wxTextCtrl::Replace(long from, long to, const wxString& value)
{
    const wxWinUITextPositionMap oldMap =
        wxWinUIMakeTextPositionMap(*this, m_value);
    const long len = oldMap.GetLastPosition();
    from = wxWinUIClampTextPos(from, len);
    to = to < 0 ? len : wxWinUIClampTextPos(to, len);
    if ( to < from )
        wxSwap(from, to);

    wxString newValue;
    long newInsertionPoint = 0;
    const wxString replacement = m_forceUpper ? value.Upper() : value;
    if ( !oldMap.ReplaceRange(
             from, to, replacement, &newValue, &newInsertionPoint) )
    {
        return;
    }

    if ( newValue != m_value && m_winui )
    {
        if ( m_winui->securePasswordPeer )
            PushSecurePasswordUndoSnapshot();
        ++m_winui->textContentGeneration;
        m_winui->InvalidatePositionVisibilityRequest();
    }
    m_value = newValue;
    m_insertionPoint = newInsertionPoint;
    m_selectionStart = m_selectionEnd = m_insertionPoint;
    m_modified = true;

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl ? impl->callbackState : nullptr;
    const std::uint64_t contentGeneration =
        impl ? impl->textContentGeneration : 0;
    const wxWeakRef<wxWindow> self(this);
    const auto getLiveOwner = [&]() -> wxTextCtrl *
    {
        wxTextCtrl * const live = wxDynamicCast(self.get(), wxTextCtrl);
        if ( !live || live->m_winui.get() != impl ||
             live->m_value != newValue ||
             live->m_insertionPoint != newInsertionPoint )
        {
            return nullptr;
        }
        if ( impl &&
             (live->m_winui->callbackState != callbackState ||
              live->m_winui->textContentGeneration != contentGeneration) )
        {
            return nullptr;
        }
        return live;
    };

    ApplyValueToPeer();
    wxTextCtrl *live = getLiveOwner();
    if ( !live )
        return;
    live->EnsurePositionVisible(newInsertionPoint);
    live = getLiveOwner();
    if ( live )
        live->SendTextEvent();
}

void wxTextCtrl::Remove(long from, long to)
{
    const wxWinUITextPositionMap oldMap =
        wxWinUIMakeTextPositionMap(*this, m_value);
    const long len = oldMap.GetLastPosition();
    from = wxWinUIClampTextPos(from, len);
    to = to < 0 ? len : wxWinUIClampTextPos(to, len);
    if ( to < from )
        wxSwap(from, to);

    wxString newValue;
    long newInsertionPoint = 0;
    if ( !oldMap.ReplaceRange(
             from, to, wxString(), &newValue, &newInsertionPoint) )
    {
        return;
    }

    if ( newValue != m_value && m_winui )
    {
        if ( m_winui->securePasswordPeer )
            PushSecurePasswordUndoSnapshot();
        ++m_winui->textContentGeneration;
        m_winui->InvalidatePositionVisibilityRequest();
    }
    m_value = newValue;
    m_insertionPoint = newInsertionPoint;
    m_selectionStart = m_selectionEnd = m_insertionPoint;
    m_modified = true;

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl ? impl->callbackState : nullptr;
    const std::uint64_t contentGeneration =
        impl ? impl->textContentGeneration : 0;
    const wxWeakRef<wxWindow> self(this);
    const auto getLiveOwner = [&]() -> wxTextCtrl *
    {
        wxTextCtrl * const live = wxDynamicCast(self.get(), wxTextCtrl);
        if ( !live || live->m_winui.get() != impl ||
             live->m_value != newValue ||
             live->m_insertionPoint != newInsertionPoint )
        {
            return nullptr;
        }
        if ( impl &&
             (live->m_winui->callbackState != callbackState ||
              live->m_winui->textContentGeneration != contentGeneration) )
        {
            return nullptr;
        }
        return live;
    };

    ApplyValueToPeer();
    wxTextCtrl *live = getLiveOwner();
    if ( !live )
        return;
    live->EnsurePositionVisible(newInsertionPoint);
    live = getLiveOwner();
    if ( live )
        live->SendTextEvent();
}

void wxTextCtrl::Clear()
{
    Remove(0, -1);
}

bool wxTextCtrl::SendClipboardEvent(wxEventType type)
{
    wxClipboardTextEvent event(type, GetId());
    event.SetEventObject(this);
    return HandleWindowEvent(event);
}

bool wxTextCtrl::CanCopy() const
{
    // Match the native password edit contract: its value remains available to
    // the owning application through GetValue(), but it must never be exposed
    // through the system clipboard.
    return !HasFlag(wxTE_PASSWORD) && wxTextCtrlBase::CanCopy();
}

bool wxTextCtrl::CanCut() const
{
    return !HasFlag(wxTE_PASSWORD) && wxTextCtrlBase::CanCut();
}

void wxTextCtrl::Copy()
{
    if ( !HasSelection() )
        return;

    const wxWeakRef<wxWindow> self(this);
    if ( SendClipboardEvent(wxEVT_TEXT_COPY) )
        return;

    wxTextCtrl *live = wxDynamicCast(self.get(), wxTextCtrl);
    if ( !live )
        return;

    // PasswordBox intentionally doesn't provide CopySelectionToClipboard().
    // Do not let it fall through to the generic wxClipboard path: doing so
    // would turn a programmatic model selection into a secret exfiltration.
    if ( live->HasFlag(wxTE_PASSWORD) )
        return;

    if ( live->m_winui && live->m_winui->richEditBox )
    {
        wxWinUITextCtrlImpl * const impl = live->m_winui.get();
        const std::shared_ptr<wxWinUITextCallbackState> callbackState =
            impl->callbackState;
        const MUXC::RichEditBox richEditBox = impl->richEditBox;
        const bool wasInProgress = impl->clipboardOperationInProgress;
        impl->clipboardOperationInProgress = true;
        wxScopeGuard operationGuard = wxMakeGuard(
            [callbackState, impl, richEditBox, wasInProgress]()
            {
                wxTextCtrl * const owner =
                    callbackState->GetOwner<wxTextCtrl>();
                if ( owner && owner->m_winui &&
                     owner->m_winui.get() == impl &&
                     owner->m_winui->callbackState == callbackState &&
                     owner->m_winui->richEditBox == richEditBox )
                {
                    owner->m_winui->clipboardOperationInProgress =
                        wasInProgress;
                }
            });
        wxUnusedVar(operationGuard);

        try
        {
            richEditBox.ClipboardCopyFormat(
                live->IsRich()
                    ? MUXC::RichEditClipboardFormat::AllFormats
                    : MUXC::RichEditClipboardFormat::PlainText);
            if ( live->HasFlag(wxTE_PASSWORD) )
                ++impl->passwordClipboardExportAttemptCount;
            richEditBox.Document().Selection().Copy();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI RichEditBox copy", e);
        }
        return;
    }

    if ( live->m_winui && live->m_winui->textBox )
    {
        wxWinUITextCtrlImpl * const impl = live->m_winui.get();
        const std::shared_ptr<wxWinUITextCallbackState> callbackState =
            impl->callbackState;
        const MUXC::TextBox textBox = impl->textBox;
        const bool wasInProgress = impl->clipboardOperationInProgress;
        impl->clipboardOperationInProgress = true;
        wxScopeGuard operationGuard = wxMakeGuard(
            [callbackState, impl, textBox, wasInProgress]()
            {
                wxTextCtrl * const owner =
                    callbackState->GetOwner<wxTextCtrl>();
                if ( owner && owner->m_winui &&
                     owner->m_winui.get() == impl &&
                     owner->m_winui->callbackState == callbackState &&
                     owner->m_winui->textBox == textBox )
                {
                    owner->m_winui->clipboardOperationInProgress =
                        wasInProgress;
                }
            });
        wxUnusedVar(operationGuard);

        try
        {
            if ( live->HasFlag(wxTE_PASSWORD) )
                ++impl->passwordClipboardExportAttemptCount;
            textBox.CopySelectionToClipboard();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI TextBox copy", e);
        }
        return;
    }

#if wxUSE_CLIPBOARD
    if ( !wxTheClipboard )
        return;

    // Materialize all control-owned data before opening the OLE clipboard.
    // Delayed-rendering IDataObject implementations may call application code
    // synchronously from Open()/SetData(). No wxTextCtrl pointer is used after
    // either boundary.
    const wxString selected = live->GetStringSelection();
    if ( wxTheClipboard->Open() )
    {
        wxTheClipboard->SetData(new wxTextDataObject(selected));
        wxTheClipboard->Close();
    }
#endif
}

void wxTextCtrl::Cut()
{
    if ( !m_editable || !HasSelection() )
        return;

    const wxWeakRef<wxWindow> self(this);
    if ( SendClipboardEvent(wxEVT_TEXT_CUT) )
        return;

    wxTextCtrl *live = wxDynamicCast(self.get(), wxTextCtrl);
    if ( !live )
        return;

    if ( live->HasFlag(wxTE_PASSWORD) )
        return;

    if ( live->m_winui && live->m_winui->richEditBox )
    {
        wxWinUITextCtrlImpl * const impl = live->m_winui.get();
        const std::shared_ptr<wxWinUITextCallbackState> callbackState =
            impl->callbackState;
        const MUXC::RichEditBox richEditBox = impl->richEditBox;
        const bool wasInProgress = impl->clipboardOperationInProgress;
        impl->clipboardOperationInProgress = true;
        wxScopeGuard operationGuard = wxMakeGuard(
            [callbackState, impl, richEditBox, wasInProgress]()
            {
                wxTextCtrl * const owner =
                    callbackState->GetOwner<wxTextCtrl>();
                if ( owner && owner->m_winui &&
                     owner->m_winui.get() == impl &&
                     owner->m_winui->callbackState == callbackState &&
                     owner->m_winui->richEditBox == richEditBox )
                {
                    owner->m_winui->clipboardOperationInProgress =
                        wasInProgress;
                }
            });
        wxUnusedVar(operationGuard);

        try
        {
            richEditBox.ClipboardCopyFormat(
                live->IsRich()
                    ? MUXC::RichEditClipboardFormat::AllFormats
                    : MUXC::RichEditClipboardFormat::PlainText);
            if ( live->HasFlag(wxTE_PASSWORD) )
                ++impl->passwordClipboardExportAttemptCount;
            richEditBox.Document().Selection().Cut();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI RichEditBox cut", e);
        }
        return;
    }

    if ( live->m_winui && live->m_winui->textBox )
    {
        wxWinUITextCtrlImpl * const impl = live->m_winui.get();
        const std::shared_ptr<wxWinUITextCallbackState> callbackState =
            impl->callbackState;
        const MUXC::TextBox textBox = impl->textBox;
        const bool wasInProgress = impl->clipboardOperationInProgress;
        impl->clipboardOperationInProgress = true;
        wxScopeGuard operationGuard = wxMakeGuard(
            [callbackState, impl, textBox, wasInProgress]()
            {
                wxTextCtrl * const owner =
                    callbackState->GetOwner<wxTextCtrl>();
                if ( owner && owner->m_winui &&
                     owner->m_winui.get() == impl &&
                     owner->m_winui->callbackState == callbackState &&
                     owner->m_winui->textBox == textBox )
                {
                    owner->m_winui->clipboardOperationInProgress =
                        wasInProgress;
                }
            });
        wxUnusedVar(operationGuard);

        try
        {
            if ( live->HasFlag(wxTE_PASSWORD) )
                ++impl->passwordClipboardExportAttemptCount;
            textBox.CutSelectionToClipboard();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI TextBox cut", e);
        }
        return;
    }

#if wxUSE_CLIPBOARD
    if ( !wxTheClipboard )
        return;

    long from = 0;
    long to = 0;
    live->GetSelection(&from, &to);
    const wxString selected = live->GetRange(from, to);
    wxWinUITextCtrlImpl * const impl =
        live->m_winui ? live->m_winui.get() : nullptr;
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl ? impl->callbackState : nullptr;
    const std::uint64_t callbackGeneration =
        callbackState ? callbackState->GetGeneration() : 0;
    const std::uint64_t contentGeneration =
        impl ? impl->textContentGeneration : 0;
    const wxString value = live->m_value;
    const long selectionStart = live->m_selectionStart;
    const long selectionEnd = live->m_selectionEnd;
    const auto getExactOwner = [&]() -> wxTextCtrl *
    {
        wxTextCtrl * const owner = wxDynamicCast(self.get(), wxTextCtrl);
        if ( !owner || owner->m_winui.get() != impl ||
             owner->m_value != value ||
             owner->m_selectionStart != selectionStart ||
             owner->m_selectionEnd != selectionEnd )
        {
            return nullptr;
        }
        if ( callbackState &&
             (callbackState->GetGeneration() != callbackGeneration ||
              owner->m_winui->callbackState != callbackState ||
              owner->m_winui->textContentGeneration != contentGeneration) )
        {
            return nullptr;
        }
        return owner;
    };
    if ( wxTheClipboard->Open() )
    {
        const bool copied =
            wxTheClipboard->SetData(new wxTextDataObject(selected));
        wxTheClipboard->Close();

        live = getExactOwner();
        if ( copied && live )
            live->Remove(from, to);
    }
#endif // wxUSE_CLIPBOARD
}

void wxTextCtrl::Paste()
{
    if ( !m_editable )
        return;

    const wxWeakRef<wxWindow> self(this);
    if ( SendClipboardEvent(wxEVT_TEXT_PASTE) )
        return;

    wxTextCtrl *live = wxDynamicCast(self.get(), wxTextCtrl);
    if ( !live )
        return;

    if ( live->IsRich() && live->m_winui &&
         live->m_winui->richEditBox )
    {
        wxWinUITextCtrlImpl * const impl = live->m_winui.get();
        const std::shared_ptr<wxWinUITextCallbackState> callbackState =
            impl->callbackState;
        const std::uint64_t callbackGeneration =
            callbackState->GetGeneration();
        const std::uint64_t contentGeneration =
            impl->textContentGeneration;
        const MUXC::RichEditBox richEditBox = impl->richEditBox;
        const wxString value = live->m_value;
        const long selectionStart = live->m_selectionStart;
        const long selectionEnd = live->m_selectionEnd;
        const auto getExactOwner = [&]() -> wxTextCtrl *
        {
            if ( callbackState->GetGeneration() != callbackGeneration )
                return nullptr;
            wxTextCtrl * const owner =
                wxDynamicCast(self.get(), wxTextCtrl);
            return owner && owner->m_winui &&
                           owner->m_winui.get() == impl &&
                           owner->m_winui->callbackState == callbackState &&
                           owner->m_winui->richEditBox == richEditBox &&
                           owner->m_winui->textContentGeneration ==
                               contentGeneration &&
                           owner->m_value == value &&
                           owner->m_selectionStart == selectionStart &&
                           owner->m_selectionEnd == selectionEnd
                     ? owner
                     : nullptr;
        };
        const bool wasInProgress = impl->clipboardOperationInProgress;
        impl->clipboardOperationInProgress = true;
        wxScopeGuard operationGuard = wxMakeGuard(
            [callbackState, impl, richEditBox, wasInProgress]()
            {
                wxTextCtrl * const owner =
                    callbackState->GetOwner<wxTextCtrl>();
                if ( owner && owner->m_winui &&
                     owner->m_winui.get() == impl &&
                     owner->m_winui->callbackState == callbackState &&
                     owner->m_winui->richEditBox == richEditBox )
                {
                    owner->m_winui->clipboardOperationInProgress =
                        wasInProgress;
                }
            });
        wxUnusedVar(operationGuard);

        try
        {
            const WUT::ITextDocument document = richEditBox.Document();
            if ( !document || !getExactOwner() )
                return;
            const bool canPaste = document.CanPaste();
            if ( !canPaste || !getExactOwner() )
                return;
            const WUT::ITextSelection selection = document.Selection();
            if ( !selection || !getExactOwner() )
                return;
            selection.Paste(0);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI RichEditBox paste", e);
        }
        return;
    }

    if ( live->m_winui && live->m_winui->passwordBox )
    {
        wxWinUITextCtrlImpl * const impl = live->m_winui.get();
        const std::shared_ptr<wxWinUITextCallbackState> callbackState =
            impl->callbackState;
        const MUXC::PasswordBox passwordBox = impl->passwordBox;
        const bool wasInProgress = impl->clipboardOperationInProgress;
        impl->clipboardOperationInProgress = true;
        wxScopeGuard operationGuard = wxMakeGuard(
            [callbackState, impl, passwordBox, wasInProgress]()
            {
                wxTextCtrl * const owner =
                    callbackState->GetOwner<wxTextCtrl>();
                if ( owner && owner->m_winui &&
                     owner->m_winui.get() == impl &&
                     owner->m_winui->callbackState == callbackState &&
                     owner->m_winui->passwordBox == passwordBox )
                {
                    owner->m_winui->clipboardOperationInProgress =
                        wasInProgress;
                }
            });
        wxUnusedVar(operationGuard);

        try
        {
            passwordBox.PasteFromClipboard();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI PasswordBox paste", e);
        }
        return;
    }

#if wxUSE_CLIPBOARD
    if ( !wxTheClipboard )
        return;

    wxWinUITextCtrlImpl * const impl =
        live->m_winui ? live->m_winui.get() : nullptr;
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl ? impl->callbackState : nullptr;
    const std::uint64_t callbackGeneration =
        callbackState ? callbackState->GetGeneration() : 0;
    const std::uint64_t contentGeneration =
        impl ? impl->textContentGeneration : 0;
    const wxString value = live->m_value;
    const long selectionStart = live->m_selectionStart;
    const long selectionEnd = live->m_selectionEnd;
    const auto getExactOwner = [&]() -> wxTextCtrl *
    {
        wxTextCtrl * const owner = wxDynamicCast(self.get(), wxTextCtrl);
        if ( !owner || owner->m_winui.get() != impl ||
             owner->m_value != value ||
             owner->m_selectionStart != selectionStart ||
             owner->m_selectionEnd != selectionEnd )
        {
            return nullptr;
        }
        if ( callbackState &&
             (callbackState->GetGeneration() != callbackGeneration ||
              owner->m_winui->callbackState != callbackState ||
              owner->m_winui->textContentGeneration != contentGeneration) )
        {
            return nullptr;
        }
        return owner;
    };

    if ( wxTheClipboard->Open() )
    {
        bool haveText = false;
        wxString clipboardText;
        if ( getExactOwner() &&
             wxTheClipboard->IsSupported(wxDF_TEXT) &&
             getExactOwner() )
        {
            wxTextDataObject data;
            if ( wxTheClipboard->GetData(data) )
            {
                clipboardText = data.GetText();
                haveText = true;
            }
        }
        wxTheClipboard->Close();

        live = getExactOwner();
        if ( live && haveText )
            live->WriteTextWithPolicy(clipboardText, true);
    }
#else
    if ( !live->m_winui )
        return;

    wxWinUITextCtrlImpl * const impl = live->m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const bool wasInProgress = impl->clipboardOperationInProgress;
    impl->clipboardOperationInProgress = true;
    wxScopeGuard operationGuard = wxMakeGuard(
        [callbackState, impl, wasInProgress]()
        {
            wxTextCtrl * const owner =
                callbackState->GetOwner<wxTextCtrl>();
            if ( owner && owner->m_winui &&
                 owner->m_winui.get() == impl &&
                 owner->m_winui->callbackState == callbackState )
            {
                owner->m_winui->clipboardOperationInProgress = wasInProgress;
            }
        });
    wxUnusedVar(operationGuard);

    try
    {
        if ( impl->textBox )
            impl->textBox.PasteFromClipboard();
        else if ( impl->richEditBox )
        {
            // API-plain and secure-password controls use RichEditBox only as
            // an implementation detail. Force Unicode text so an RTF/object
            // payload can never cross either public or security boundary.
            const WUT::ITextDocument document =
                impl->richEditBox.Document();
            if ( document && document.CanPaste() )
            {
                document.Selection().Paste(CF_UNICODETEXT);
            }
        }
        else if ( impl->passwordBox )
            impl->passwordBox.PasteFromClipboard();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI text paste", e);
    }
#endif
}

void wxTextCtrl::Undo()
{
    if ( m_winui && m_winui->securePasswordPeer )
    {
        RestoreSecurePasswordSnapshot(false);
        return;
    }

    if ( m_winui && (m_winui->textBox || m_winui->richEditBox) )
    {
        const MUXC::TextBox textBox = m_winui->textBox;
        const MUXC::RichEditBox richEditBox = m_winui->richEditBox;
        try
        {
            if ( textBox )
                textBox.Undo();
            else
                richEditBox.Document().Undo();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI TextBox undo", e);
        }
    }
}

void wxTextCtrl::Redo()
{
    if ( m_winui && m_winui->securePasswordPeer )
    {
        RestoreSecurePasswordSnapshot(true);
        return;
    }

    if ( m_winui && (m_winui->textBox || m_winui->richEditBox) )
    {
        const MUXC::TextBox textBox = m_winui->textBox;
        const MUXC::RichEditBox richEditBox = m_winui->richEditBox;
        try
        {
            if ( textBox )
                textBox.Redo();
            else
                richEditBox.Document().Redo();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI TextBox redo", e);
        }
    }
}

bool wxTextCtrl::CanUndo() const
{
    if ( m_winui && m_winui->securePasswordPeer )
        return !m_winui->securePasswordUndo.empty();

    if ( !m_winui ||
         (!m_winui->textBox && !m_winui->richEditBox) )
        return false;
    const MUXC::TextBox textBox = m_winui->textBox;
    const MUXC::RichEditBox richEditBox = m_winui->richEditBox;
    try
    {
        return textBox ? textBox.CanUndo()
                       : richEditBox.Document().CanUndo();
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxTextCtrl::CanRedo() const
{
    if ( m_winui && m_winui->securePasswordPeer )
        return !m_winui->securePasswordRedo.empty();

    if ( !m_winui ||
         (!m_winui->textBox && !m_winui->richEditBox) )
        return false;
    const MUXC::TextBox textBox = m_winui->textBox;
    const MUXC::RichEditBox richEditBox = m_winui->richEditBox;
    try
    {
        return textBox ? textBox.CanRedo()
                       : richEditBox.Document().CanRedo();
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

void wxTextCtrl::EmptyUndoBuffer()
{
    if ( m_winui && m_winui->securePasswordPeer )
    {
        m_winui->securePasswordUndo.clear();
        m_winui->securePasswordRedo.clear();
    }

    if ( !m_winui ||
         (!m_winui->textBox && !m_winui->richEditBox) )
        return;

    const MUXC::TextBox textBox = m_winui->textBox;
    const MUXC::RichEditBox richEditBox = m_winui->richEditBox;
    try
    {
        // This is a native TextBox primitive: unlike a Text reassignment or
        // peer replacement it changes neither content, selection, dirty
        // state, focus nor event flow, and clears both stacks atomically.
        if ( textBox )
        {
            textBox.ClearUndoRedoHistory();
        }
        else
        {
            const WUT::ITextDocument document =
                richEditBox.Document();
            const uint32_t oldLimit = document.UndoLimit();
            document.UndoLimit(0);
            document.UndoLimit(oldLimit);
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TextBox clear undo history", e);
    }
}

void wxTextCtrl::SetInsertionPoint(long pos)
{
    const wxWinUITextPositionMap map =
        wxWinUIMakeTextPositionMap(*this, m_value);
    m_insertionPoint =
        wxWinUIClampTextPos(pos, map.GetLastPosition());
    m_selectionStart = m_selectionEnd = m_insertionPoint;
    const long insertionPoint = m_insertionPoint;
    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl ? impl->callbackState : nullptr;
    const wxWeakRef<wxWindow> self(this);
    ApplyValueToPeer();
    wxTextCtrl * const live = wxDynamicCast(self.get(), wxTextCtrl);
    if ( live && live->m_winui.get() == impl &&
         live->m_insertionPoint == insertionPoint &&
         (!impl || live->m_winui->callbackState == callbackState) )
    {
        live->EnsurePositionVisible(insertionPoint);
    }
}

void wxTextCtrl::SetInsertionPointEnd()
{
    SetInsertionPoint(-1);
}

long wxTextCtrl::GetInsertionPoint() const
{
    return m_insertionPoint;
}

wxTextPos wxTextCtrl::GetLastPosition() const
{
    return wxWinUIMakeTextPositionMap(*this, m_value).GetLastPosition();
}

void wxTextCtrl::SetSelection(long from, long to)
{
    const long len = GetLastPosition();
    if ( from == -1 && to == -1 )
    {
        from = 0;
        to = len;
    }
    else
    {
        from = wxWinUIClampTextPos(from, len);
        to = wxWinUIClampTextPos(to, len);
    }

    m_selectionStart = from;
    m_selectionEnd = to;
    // wxTextEntry's public contract exposes the beginning of the selection as
    // its insertion point (including after SelectAll()).
    m_insertionPoint = from;
    const long insertionPoint = m_insertionPoint;
    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl ? impl->callbackState : nullptr;
    const wxWeakRef<wxWindow> self(this);
    ApplyValueToPeer();
    wxTextCtrl * const live = wxDynamicCast(self.get(), wxTextCtrl);
    if ( live && live->m_winui.get() == impl &&
         live->m_selectionStart == from && live->m_selectionEnd == to &&
         (!impl || live->m_winui->callbackState == callbackState) )
    {
        live->EnsurePositionVisible(insertionPoint);
    }
}

void wxTextCtrl::GetSelection(long *from, long *to) const
{
    if ( from )
        *from = m_selectionStart;
    if ( to )
        *to = m_selectionEnd;
}

bool wxTextCtrl::IsEditable() const
{
    return m_editable;
}

void wxTextCtrl::SetEditable(bool editable)
{
    m_editable = editable;
    if ( m_winui &&
         (m_winui->textBox || m_winui->richEditBox) )
    {
        const MUXC::TextBox textBox = m_winui->textBox;
        const MUXC::RichEditBox richEditBox = m_winui->richEditBox;
        try
        {
            if ( textBox )
                textBox.IsReadOnly(!editable);
            else
                richEditBox.IsReadOnly(!editable);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI TextBox editable state", e);
        }
    }
}

void wxTextCtrl::SetMaxLength(unsigned long len)
{
    m_maxLength = wxMin<unsigned long>(
        len, static_cast<unsigned long>(std::numeric_limits<int32_t>::max()));
    if ( !m_winui )
        return;

    const MUXC::TextBox textBox = m_winui->textBox;
    const MUXC::RichEditBox richEditBox = m_winui->richEditBox;
    const MUXC::PasswordBox passwordBox = m_winui->passwordBox;
    try
    {
        if ( textBox )
        {
            // Enforced by BeforeTextChanging in wx public coordinates.
            textBox.MaxLength(0);
        }
        else if ( richEditBox )
        {
            // Enforced from the complete TextChanged proposal so the fitting
            // part of an insertion and wxEVT_TEXT_MAXLEN remain observable.
            richEditBox.MaxLength(0);
        }
        else if ( passwordBox )
        {
            passwordBox.MaxLength(0);
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI text maximum length", e);
    }
}

void wxTextCtrl::ForceUpper()
{
    m_forceUpper = true;

    // Convert the model first, preserving the public selection in the same way
    // as wxTextEntryBase. CharacterCasing then covers all future native edits,
    // including IME commits, without routing them through the HWND shell.
    const wxWeakRef<wxWindow> self(this);
    ConvertToUpperCase();
    wxTextCtrl * const live = wxDynamicCast(self.get(), wxTextCtrl);
    if ( !live || !live->m_winui )
        return;

    const MUXC::TextBox textBox = live->m_winui->textBox;
    const MUXC::RichEditBox richEditBox = live->m_winui->richEditBox;
    try
    {
        if ( textBox )
        {
            textBox.CharacterCasing(MUXC::CharacterCasing::Upper);
        }
        else if ( richEditBox )
        {
            richEditBox.CharacterCasing(MUXC::CharacterCasing::Upper);
        }
        // PasswordBox has no CharacterCasing property. Its complete proposed
        // value is upper-cased in ProcessPasswordPeerChange() below.
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI text character casing", e);
    }
}

bool wxTextCtrl::DoSetMargins(const wxPoint& margins)
{
    const bool fullySupported = margins.y == -1;
    if ( margins.x == -1 )
        return fullySupported;
    if ( margins.x < 0 || !m_winui || !m_winui->control() )
        return false;

    const double horizontal =
        static_cast<double>(ToDIP(wxSize(margins.x, 0)).x);
    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const MUXC::Control control = impl->control();
    const wxWeakRef<wxWindow> self(this);
    const auto getLiveOwner = [&]() -> wxTextCtrl *
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return nullptr;
        wxTextCtrl * const live = wxDynamicCast(self.get(), wxTextCtrl);
        return live && live->m_winui && live->m_winui.get() == impl &&
                       live->m_winui->callbackState == callbackState &&
                       live->m_winui->control() == control
                 ? live
                 : nullptr;
    };
    try
    {
        MUX::Thickness padding = control.Padding();
        if ( !getLiveOwner() )
            return false;
        padding.Left = horizontal;
        padding.Right = horizontal;
        control.Padding(padding);
        wxTextCtrl * const live = getLiveOwner();
        if ( !live )
            return false;
        live->InvalidateBestSize();
        return fullySupported;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI text margins", e);
        return false;
    }
}

wxPoint wxTextCtrl::DoGetMargins() const
{
    if ( !m_winui || !m_winui->control() )
        return wxPoint(-1, -1);

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const MUXC::Control control = impl->control();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const wxSize dipScale = FromDIP(wxSize(96, 96));
    const wxWeakRef<wxWindow> self(const_cast<wxTextCtrl *>(this));
    const auto isCurrent = [&]()
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return false;
        const wxTextCtrl * const live =
            wxDynamicCast(self.get(), wxTextCtrl);
        return live && live->m_winui && live->m_winui.get() == impl &&
               live->m_winui->callbackState == callbackState &&
               live->m_winui->control() == control;
    };
    try
    {
        const double left = control.Padding().Left;
        if ( !isCurrent() || !std::isfinite(left) )
            return wxPoint(-1, -1);
        return wxPoint(
            wxRound(left * dipScale.x / 96.0), -1);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI text margins query", e);
        return wxPoint(-1, -1);
    }
}

void wxTextCtrl::SetWindowStyleFlag(long style)
{
    const long oldStyle = GetWindowStyle();
    const wxWeakRef<wxWindow> self(this);

    // Password/rich/multiline are creation semantics in wxMSW as well. Keep
    // those public bits stable when callers mutate unrelated styles later.
    // AUTO_URL is the one deliberate upgrade: wxMSW promotes such a control
    // to RICH2, and our retained RichEditBox can perform the same transition
    // without recreating the peer (removing AUTO_URL doesn't downgrade it).
    if ( m_winui )
    {
        const long peerMask =
            wxTE_PASSWORD | wxTE_RICH | wxTE_RICH2 | wxTE_MULTILINE;
        long peerStyle = oldStyle & peerMask;
        if ( (style & wxTE_AUTO_URL) && !(peerStyle & wxTE_PASSWORD) )
            peerStyle |= wxTE_RICH | wxTE_RICH2;
        style = (style & ~peerMask) | peerStyle;
    }

    wxControl::SetWindowStyleFlag(style);
    wxTextCtrl * const current =
        wxDynamicCast(self.get(), wxTextCtrl);
    if ( !current || !current->m_winui )
        return;

    current->m_editable = (style & wxTE_READONLY) == 0;
    const MUX::TextAlignment alignment =
        style & wxTE_RIGHT
            ? MUX::TextAlignment::Right
            : style & wxTE_CENTRE
                ? MUX::TextAlignment::Center
                : MUX::TextAlignment::Left;
    const bool wrap = (style & wxTE_MULTILINE) != 0 &&
                      (style & wxTE_DONTWRAP) == 0;
    const bool keepSelection = (style & wxTE_NOHIDESEL) != 0;
    const bool editable = current->m_editable;
    const bool forceUpper = current->m_forceUpper;

    wxWinUITextCtrlImpl * const impl = current->m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const MUXC::TextBox textBox = impl->textBox;
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const MUXC::PasswordBox passwordBox = impl->passwordBox;
    const auto getLiveOwner = [&]() -> wxTextCtrl *
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return nullptr;
        wxTextCtrl * const live = wxDynamicCast(self.get(), wxTextCtrl);
        return live && live->m_winui && live->m_winui.get() == impl &&
                       live->m_winui->callbackState == callbackState &&
                       live->m_winui->textBox == textBox &&
                       live->m_winui->richEditBox == richEditBox &&
                       live->m_winui->passwordBox == passwordBox
                 ? live
                 : nullptr;
    };
    const bool updateScrollPolicy =
        (style & wxTE_MULTILINE) != 0 &&
        ((oldStyle ^ style) & wxTE_NO_VSCROLL) != 0;
    const bool updateAutoUrl =
        ((oldStyle ^ style) & wxTE_AUTO_URL) != 0;

    try
    {
        if ( textBox )
        {
            textBox.TextAlignment(alignment);
            if ( !getLiveOwner() )
                return;
            textBox.TextWrapping(
                wrap ? MUX::TextWrapping::Wrap
                     : MUX::TextWrapping::NoWrap);
            if ( !getLiveOwner() )
                return;
            textBox.IsReadOnly(!editable);
            if ( !getLiveOwner() )
                return;
            if ( keepSelection )
            {
                const auto brush = textBox.SelectionHighlightColor();
                if ( !getLiveOwner() )
                    return;
                textBox.SelectionHighlightColorWhenNotFocused(brush);
            }
            else
            {
                textBox.ClearValue(
                    MUXC::TextBox::
                        SelectionHighlightColorWhenNotFocusedProperty());
            }
            if ( !getLiveOwner() )
                return;
            if ( forceUpper )
                textBox.CharacterCasing(MUXC::CharacterCasing::Upper);
        }
        else if ( richEditBox )
        {
            richEditBox.TextAlignment(alignment);
            if ( !getLiveOwner() )
                return;
            richEditBox.TextWrapping(
                wrap ? MUX::TextWrapping::Wrap
                     : MUX::TextWrapping::NoWrap);
            if ( !getLiveOwner() )
                return;
            richEditBox.IsReadOnly(!editable);
            if ( !getLiveOwner() )
                return;
            if ( keepSelection )
            {
                const auto brush = richEditBox.SelectionHighlightColor();
                if ( !getLiveOwner() )
                    return;
                richEditBox.SelectionHighlightColorWhenNotFocused(brush);
            }
            else
            {
                richEditBox.ClearValue(
                    MUXC::RichEditBox::
                        SelectionHighlightColorWhenNotFocusedProperty());
            }
            if ( !getLiveOwner() )
                return;
            if ( forceUpper )
                richEditBox.CharacterCasing(MUXC::CharacterCasing::Upper);
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI text style projection", e);
        return;
    }

    if ( updateAutoUrl )
    {
        if ( wxTextCtrl * const live = getLiveOwner() )
            live->UpdateAutoUrlRanges();
    }

    if ( updateScrollPolicy )
    {
        if ( wxTextCtrl * const live = getLiveOwner() )
            live->ApplyVerticalScrollBarPolicy(3);
    }
}

bool wxTextCtrl::ShowNativeCaret(bool show)
{
    if ( !m_winui || !m_winui->richEditBox )
    {
        // PasswordBox exposes neither caret visibility nor TOM, and operating
        // on the wx HWND shell would not affect its windowless XAML edit peer.
        return false;
    }
    if ( show && m_isNativeCaretShown )
        return true;

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const wxWeakRef<wxWindow> self(this);
    const auto getLiveOwner = [&]() -> wxTextCtrl *
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return nullptr;
        wxTextCtrl * const live = wxDynamicCast(self.get(), wxTextCtrl);
        return live && live->m_winui && live->m_winui.get() == impl &&
                       live->m_winui->callbackState == callbackState &&
                       live->m_winui->richEditBox == richEditBox
                 ? live
                 : nullptr;
    };

    try
    {
        const WUT::ITextDocument document = richEditBox.Document();
        if ( !document || !getLiveOwner() )
            return false;
        document.CaretType(
            show ? WUT::CaretType::Normal : WUT::CaretType::Null);
        wxTextCtrl * const live = getLiveOwner();
        if ( !live )
            return false;
        live->m_isNativeCaretShown = show;
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI RichEditBox caret visibility", e);
        return false;
    }
}

void wxTextCtrl::OnSetFocus(wxFocusEvent& event)
{
    // Focusing an editor may create a fresh native caret. Reapply the hidden
    // TOM caret state just as wxMSW reissues HideCaret() from its focus hook.
    if ( !m_isNativeCaretShown )
        ShowNativeCaret(false);
    event.Skip();
}

#if wxUSE_RICHEDIT
wxMenu *wxTextCtrl::MSWCreateContextMenu()
{
    wxMenu * const menu = new wxMenu;
    menu->Append(wxID_UNDO, _("&Undo"));
    menu->Append(wxID_REDO, _("&Redo"));
    menu->AppendSeparator();
    menu->Append(wxID_CUT, _("Cu&t"));
    menu->Append(wxID_COPY, _("&Copy"));
    menu->Append(wxID_PASTE, _("&Paste"));
    menu->Append(wxID_CLEAR, _("&Delete"));
    menu->AppendSeparator();
    menu->Append(wxID_SELECTALL, _("Select &All"));
    return menu;
}
#endif // wxUSE_RICHEDIT

#if wxUSE_RICHEDIT && wxUSE_MENUS
void wxTextCtrl::ShowRichContextMenu()
{
    if ( !m_winui || !m_winui->richEditBox )
        return;

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t generation = callbackState->GetGeneration();
    const wxWeakRef<wxWindow> self(this);

    // This is virtual application code. Take ownership immediately and only
    // re-enter the surviving exact control instance afterwards.
    std::unique_ptr<wxMenu> menu(MSWCreateContextMenu());
    if ( !menu || callbackState->GetGeneration() != generation )
        return;
    wxTextCtrl * const live = wxDynamicCast(self.get(), wxTextCtrl);
    if ( !live || !live->m_winui || live->m_winui.get() != impl ||
         live->m_winui->callbackState != callbackState ||
         !live->m_winui->richEditBox )
    {
        return;
    }

    // PopupMenu() is a reentrancy boundary and intentionally the final access
    // to the control. The local menu remains valid until the native popup has
    // returned even if a command destroys its owner.
    live->PopupMenu(menu.get());
}
#endif // wxUSE_RICHEDIT && wxUSE_MENUS

#if wxUSE_MENUS
void wxTextCtrl::OnRichContextMenuCommand(wxCommandEvent& event)
{
    switch ( event.GetId() )
    {
        case wxID_UNDO:
            Undo();
            return;
        case wxID_REDO:
            Redo();
            return;
        case wxID_CUT:
            Cut();
            return;
        case wxID_COPY:
            Copy();
            return;
        case wxID_PASTE:
            Paste();
            return;
        case wxID_CLEAR:
            RemoveSelection();
            return;
        case wxID_SELECTALL:
            SelectAll();
            return;
        default:
            event.Skip();
            return;
    }
}

void wxTextCtrl::OnUpdateRichContextMenuCommand(wxUpdateUIEvent& event)
{
    switch ( event.GetId() )
    {
        case wxID_UNDO:
            event.Enable(CanUndo());
            return;
        case wxID_REDO:
            event.Enable(CanRedo());
            return;
        case wxID_CUT:
            event.Enable(CanCut());
            return;
        case wxID_COPY:
            event.Enable(CanCopy());
            return;
        case wxID_PASTE:
            event.Enable(IsEditable());
            return;
        case wxID_CLEAR:
            event.Enable(IsEditable() && HasSelection());
            return;
        case wxID_SELECTALL:
            event.Enable(!IsEmpty());
            return;
        default:
            event.Skip();
            return;
    }
}
#endif // wxUSE_MENUS

bool wxTextCtrl::DoAutoCompleteStrings(const wxArrayString& choices)
{
    wxTextCompleterFixed * const completer = new wxTextCompleterFixed;
    completer->SetCompletions(choices);
    return DoAutoCompleteCustom(completer);
}

bool wxTextCtrl::DoAutoCompleteFileNames(int flags)
{
    if ( flags != wxFILE && flags != wxDIR )
        return false;
    return DoAutoCompleteCustom(new wxWinUIFileNameCompleter(flags));
}

bool wxTextCtrl::DoAutoCompleteCustom(wxTextCompleter *completer)
{
    std::shared_ptr<wxTextCompleter> owned(completer);
    if ( !m_winui || IsMultiLine() || HasFlag(wxTE_PASSWORD) ||
         (!m_winui->textBox && !m_winui->richEditBox) )
    {
        return false;
    }

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const wxWeakRef<wxWindow> self(this);
    const auto getLiveOwner = [&]() -> wxTextCtrl *
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return nullptr;
        wxTextCtrl * const live = wxDynamicCast(self.get(), wxTextCtrl);
        return live && live->m_winui && live->m_winui.get() == impl &&
                       live->m_winui->callbackState == callbackState
                 ? live
                 : nullptr;
    };

    // Publish the replacement before destroying the previous application
    // completer. Its destructor is an application-code boundary and may delete
    // this control, so no member access is permitted until exact revalidation.
    std::shared_ptr<wxTextCompleter> previous =
        std::move(impl->autoCompleter);
    impl->autoCompleter = std::move(owned);
    ++impl->autoCompleteGeneration;
    previous.reset();

    wxTextCtrl * const live = getLiveOwner();
    if ( !live )
        return true;
    live->RefreshAutoComplete();
    return true;
}

void wxTextCtrl::RefreshAutoComplete()
{
    if ( !m_winui )
        return;

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const std::shared_ptr<wxTextCompleter> completer = impl->autoCompleter;
    const std::uint64_t generation = ++impl->autoCompleteGeneration;
    const wxString prefix = m_value;
    const wxWeakRef<wxWindow> self(this);
    const auto getLiveOwner = [&]() -> wxTextCtrl *
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return nullptr;
        wxTextCtrl * const live = wxDynamicCast(self.get(), wxTextCtrl);
        return live && live->m_winui && live->m_winui.get() == impl &&
                       live->m_winui->callbackState == callbackState &&
                       live->m_winui->autoCompleter == completer &&
                       live->m_winui->autoCompleteGeneration == generation &&
                       live->m_value == prefix
                 ? live
                 : nullptr;
    };

    // Detach the old flyout before crossing Hide(): its Closed/focus work may
    // synchronously dispatch application events and start a newer refresh.
    MUXC::MenuFlyout oldFlyout = impl->autoCompleteFlyout;
    impl->autoCompleteFlyout = nullptr;
    if ( oldFlyout )
    {
        try
        {
            oldFlyout.Hide();
        }
        catch ( const winrt::hresult_error& )
        {
        }
        if ( !getLiveOwner() )
            return;
    }

    // Native auto-completion is an editing aid for a non-empty prefix. Keep a
    // local shared owner while invoking user completers: they are application
    // callbacks and may destroy the wx control or replace the completer.
    if ( !completer || prefix.empty() ||
         m_selectionStart != m_selectionEnd ||
         m_insertionPoint != GetLastPosition() )
    {
        return;
    }

    if ( !completer->Start(prefix) || !getLiveOwner() )
        return;

    std::vector<wxString> suggestions;
    const wxString foldedPrefix = prefix.Lower();
    constexpr std::size_t MaximumVisibleSuggestions = 256;
    while ( suggestions.size() < MaximumVisibleSuggestions )
    {
        const wxString candidate = completer->GetNext();
        if ( !getLiveOwner() || candidate.empty() )
            break;
        if ( !candidate.Lower().StartsWith(foldedPrefix) )
            continue;
        if ( std::find(suggestions.begin(), suggestions.end(), candidate) ==
             suggestions.end() )
        {
            suggestions.push_back(candidate);
        }
    }
    if ( !getLiveOwner() || suggestions.empty() )
        return;

    try
    {
        const MUX::FrameworkElement target =
            impl->element().try_as<MUX::FrameworkElement>();
        if ( !target || !target.XamlRoot() || !getLiveOwner() )
            return;

        MUXC::MenuFlyout flyout;
        for ( const wxString& suggestion : suggestions )
        {
            MUXC::MenuFlyoutItem item;
            item.Text(wxWinUIToHString(suggestion));
            item.Click(
                [callbackState, callbackGeneration, impl, generation,
                 suggestion](
                    winrt::Windows::Foundation::IInspectable const&,
                    MUX::RoutedEventArgs const&)
                {
                    if ( callbackState->GetGeneration() !=
                         callbackGeneration )
                    {
                        return;
                    }
                    wxTextCtrl *owner =
                        callbackState->GetOwner<wxTextCtrl>();
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui.get() != impl ||
                         owner->m_winui->callbackState != callbackState ||
                         owner->m_winui->autoCompleteGeneration != generation )
                    {
                        return;
                    }

                    // Detach this exact transient before SetValue() delivers
                    // wxEVT_TEXT: application code may install a replacement
                    // completion session, which must not be hidden below.
                    MUXC::MenuFlyout completedFlyout =
                        std::move(owner->m_winui->autoCompleteFlyout);
                    owner->m_winui->autoCompleteFlyout = nullptr;
                    ++owner->m_winui->autoCompleteGeneration;

                    const wxWeakRef<wxWindow> ownerRef(owner);
                    owner->SetValue(suggestion);
                    owner = wxDynamicCast(ownerRef.get(), wxTextCtrl);
                    if ( owner && owner->m_winui &&
                         owner->m_winui.get() == impl &&
                         owner->m_winui->callbackState == callbackState )
                    {
                        owner->SetInsertionPointEnd();
                    }

                    // Hide() may synchronously dispatch focus/application
                    // callbacks, so it is intentionally the final operation.
                    if ( completedFlyout )
                    {
                        try
                        {
                            completedFlyout.Hide();
                        }
                        catch ( const winrt::hresult_error& )
                        {
                        }
                    }
                });
            flyout.Items().Append(item);
            if ( !getLiveOwner() )
                return;
        }

        impl->autoCompleteFlyout = flyout;
        MUXCP::FlyoutShowOptions options;
        options.Placement(MUXCP::FlyoutPlacementMode::Bottom);
        options.ShowMode(MUXCP::FlyoutShowMode::Transient);
        flyout.ShowAt(target, options);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI text auto-completion", e);
        if ( wxTextCtrl * const live = getLiveOwner() )
            live->m_winui->autoCompleteFlyout = nullptr;
    }
}

void wxTextCtrl::UpdateAutoUrlRanges()
{
    if ( !m_winui || !m_winui->richEditBox )
        return;

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const std::uint64_t contentGeneration = impl->textContentGeneration;
    const wxString value = m_value;
    const std::vector<wxWinUIAutoUrlRange> oldRanges = impl->autoUrlRanges;
    const std::vector<wxWinUIAutoUrlRange> newRanges =
        HasFlag(wxTE_AUTO_URL)
            ? wxWinUIFindAutoUrls(value)
            : std::vector<wxWinUIAutoUrlRange>();
    const wxWeakRef<wxWindow> self(this);
    const auto getLiveOwner = [&]() -> wxTextCtrl *
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return nullptr;
        wxTextCtrl * const live = wxDynamicCast(self.get(), wxTextCtrl);
        return live && live->m_winui && live->m_winui.get() == impl &&
                       live->m_winui->callbackState == callbackState &&
                       live->m_winui->richEditBox == richEditBox &&
                       live->m_winui->textContentGeneration ==
                           contentGeneration &&
                       live->m_value == value
                 ? live
                 : nullptr;
    };
    const auto getExactPeerOwner = [&]() -> wxTextCtrl *
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return nullptr;
        wxTextCtrl * const live = wxDynamicCast(self.get(), wxTextCtrl);
        return live && live->m_winui && live->m_winui.get() == impl &&
                       live->m_winui->callbackState == callbackState &&
                       live->m_winui->richEditBox == richEditBox
                 ? live
                 : nullptr;
    };

    const bool wasUpdatingPeer = m_updatingPeer;
    m_updatingPeer = true;
    wxScopeGuard updatingGuard = wxMakeGuard(
        [getExactPeerOwner, wasUpdatingPeer]()
        {
            // Formatting can synchronously re-enter application code and
            // replace the value/content generation. Restore the flag on the
            // same native peer even in that case; never leave it latched true.
            if ( wxTextCtrl * const live = getExactPeerOwner() )
                live->m_updatingPeer = wasUpdatingPeer;
        });
    wxUnusedVar(updatingGuard);

    bool applied = false;
    WUT::ITextDocument document{ nullptr };
    std::vector<WUT::ITextRange> appliedRanges;
    appliedRanges.reserve(newRanges.size());
    const auto rollback = [&]()
    {
        if ( !document || !getLiveOwner() )
            return;

        // First remove any new links whose exact adjusted ranges survived.
        // Keep attempting the remaining cleanup if one native range rejects
        // the rollback, but stop immediately if re-entrancy changed ownership.
        for ( const WUT::ITextRange& range : appliedRanges )
        {
            try
            {
                range.Link(winrt::hstring());
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI text automatic URL rollback", e);
            }
            if ( !getLiveOwner() )
                return;
        }

        // The previous links were removed before the replacement set was
        // applied. Rebuild them from the end of the story so hidden TOM link
        // payloads cannot invalidate an earlier public text offset.
        for ( auto it = oldRanges.rbegin();
              it != oldRanges.rend();
              ++it )
        {
            if ( !getLiveOwner() )
                return;
            try
            {
                const wxString tomLink =
                    wxS("\"") + it->target + wxS("\"");
                document.GetRange(
                    static_cast<int32_t>(it->start),
                    static_cast<int32_t>(it->end)).Link(
                        wxWinUIToHString(tomLink));
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI text automatic URL rollback", e);
            }
        }
    };
    try
    {
        const long length = static_cast<long>(wxMin<std::size_t>(
            value.length(),
            static_cast<std::size_t>(
                (std::numeric_limits<int32_t>::max)())));
        document = richEditBox.Document();
        if ( !getLiveOwner() )
            return;

        for ( auto it = oldRanges.rbegin();
              it != oldRanges.rend();
              ++it )
        {
            const wxWinUIAutoUrlRange& old = *it;
            const long start = wxMax<long>(0, wxMin(old.start, length));
            const long end = wxMax<long>(start, wxMin(old.end, length));
            if ( start != end )
            {
                document.GetRange(static_cast<int32_t>(start),
                                  static_cast<int32_t>(end)).Link(
                    winrt::hstring());
                if ( !getLiveOwner() )
                    return;
            }
        }

        applied = true;
        for ( auto it = newRanges.rbegin();
              it != newRanges.rend();
              ++it )
        {
            const wxWinUIAutoUrlRange& url = *it;
            if ( url.start < 0 || url.end <= url.start || url.end > length )
            {
                applied = false;
                break;
            }
            // TOM requires the URL payload itself to be quoted. The friendly
            // name remains the existing document text selected by this range.
            const wxString tomLink = wxS("\"") + url.target + wxS("\"");
            const WUT::ITextRange range = document.GetRange(
                static_cast<int32_t>(url.start),
                static_cast<int32_t>(url.end));
            // Track the exact range before the native setter: if it throws
            // after partially changing TOM, rollback can still clear it.
            appliedRanges.push_back(range);
            range.Link(wxWinUIToHString(tomLink));
            if ( !getLiveOwner() )
                return;

            // Link() is documented to adjust this exact range to encompass
            // the complete hyperlink. Re-reading a fresh range at the old
            // offsets can therefore miss the link even though TOM applied it.
            // Verify the adjusted native range immediately, in reverse order
            // so earlier friendly names cannot invalidate later offsets.
            wxString projectedTarget;
            const bool hasProjectedTarget =
                wxWinUIGetTOMLinkTarget(range.Link(), &projectedTarget);
            if ( !getLiveOwner() || !hasProjectedTarget ||
                 projectedTarget != url.target )
            {
                applied = false;
                break;
            }
        }

        if ( !applied )
            rollback();
    }
    catch ( const winrt::hresult_error& e )
    {
        applied = false;
        wxWinUILogException("WinUI text automatic URL detection", e);
        rollback();
    }

    if ( applied )
    {
        if ( wxTextCtrl * const live = getLiveOwner() )
        {
            live->m_winui->autoUrlRanges = newRanges;
            live->m_winui->autoUrlContentGeneration = contentGeneration;
        }
    }
}

void wxTextCtrl::OnAutoUrlMouse(wxMouseEvent& event)
{
    event.Skip();
    if ( !HasFlag(wxTE_AUTO_URL) || !m_winui ||
         m_winui->autoUrlRanges.empty() )
    {
        return;
    }

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const std::uint64_t contentGeneration =
        impl->textContentGeneration;
    const wxString value = m_value;
    const std::vector<wxWinUIAutoUrlRange> ranges =
        impl->autoUrlRanges;
    const wxWeakRef<wxWindow> self(this);
    const auto getLiveOwner = [&]() -> wxTextCtrl *
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return nullptr;
        wxTextCtrl * const live = wxDynamicCast(self.get(), wxTextCtrl);
        return live && live->m_winui && live->m_winui.get() == impl &&
                       live->m_winui->callbackState == callbackState &&
                       live->m_winui->textContentGeneration ==
                           contentGeneration &&
                       live->m_value == value &&
                       live->HasFlag(wxTE_AUTO_URL)
                 ? live
                 : nullptr;
    };
    long position = 0;
    if ( HitTest(event.GetPosition(), &position) != wxTE_HT_ON_TEXT )
        return;

    wxTextCtrl *live = getLiveOwner();
    if ( !live )
        return;

    // Stored ranges are TOM/LF positions. A public plain multiline wxTextCtrl
    // still uses MSW-compatible CRLF positions, so normalize the hit before
    // comparing and convert the notification range back afterwards.
    const wxWinUITextPositionMap map =
        wxWinUIMakeTextPositionMap(*live, value);
    long logicalPosition = 0;
    if ( !map.PublicToLogical(position, &logicalPosition) )
        return;

    wxWinUIAutoUrlRange matched;
    bool found = false;
    for ( const wxWinUIAutoUrlRange& url : ranges )
    {
        if ( logicalPosition >= url.start && logicalPosition < url.end )
        {
            matched = url;
            found = true;
            break;
        }
    }
    if ( !found )
        return;

    live = getLiveOwner();
    if ( !live )
        return;
    wxTextUrlEvent urlEvent(
        live->GetId(),
        event,
        map.LogicalToPublic(matched.start),
        map.LogicalToPublic(matched.end));
    live->InitCommandEvent(urlEvent);
    if ( live->ProcessCommand(urlEvent) )
        event.Skip(false);
}

bool wxTextCtrl::SetHint(const wxString& hint)
{
    // wxTextEntry explicitly doesn't support hints for password controls:
    // displaying prompt text through a masked field is ambiguous and the
    // generic implementation rejects it too.
    if ( HasFlag(wxTE_PASSWORD) || !m_winui ||
         (!m_winui->textBox && !m_winui->richEditBox) )
        return false;

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const MUXC::TextBox textBox = impl->textBox;
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t generation = callbackState->GetGeneration();
    const wxWeakRef<wxWindow> self(this);
    const auto getLiveOwner = [&]() -> wxTextCtrl *
    {
        if ( callbackState->GetGeneration() != generation )
            return nullptr;
        wxTextCtrl * const owner = wxDynamicCast(self.get(), wxTextCtrl);
        return owner && owner->m_winui &&
                       owner->m_winui.get() == impl &&
                       owner->m_winui->callbackState == callbackState &&
                       owner->m_winui->textBox == textBox &&
                       owner->m_winui->richEditBox == richEditBox
                 ? owner
                 : nullptr;
    };

    try
    {
        if ( textBox )
            textBox.PlaceholderText(wxWinUIToHString(hint));
        else
            richEditBox.PlaceholderText(wxWinUIToHString(hint));
        wxTextCtrl * const live = getLiveOwner();
        if ( !live )
            return false;
        // Final boundary: rendering can dispatch application callbacks.
        live->m_winui->host.ForceRender();
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TextBox hint", e);
        return false;
    }
}

wxString wxTextCtrl::GetHint() const
{
    if ( HasFlag(wxTE_PASSWORD) || !m_winui ||
         (!m_winui->textBox && !m_winui->richEditBox) )
        return wxString();

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const MUXC::TextBox textBox = impl->textBox;
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t generation = callbackState->GetGeneration();
    const wxWeakRef<wxWindow> self(const_cast<wxTextCtrl *>(this));
    const auto isCurrent = [&]()
    {
        if ( callbackState->GetGeneration() != generation )
            return false;
        const wxTextCtrl * const owner =
            wxDynamicCast(self.get(), wxTextCtrl);
        return owner && owner->m_winui &&
               owner->m_winui.get() == impl &&
               owner->m_winui->callbackState == callbackState &&
               owner->m_winui->textBox == textBox &&
               owner->m_winui->richEditBox == richEditBox;
    };

    try
    {
        const wxString hint = wxWinUIFromHString(
            textBox ? textBox.PlaceholderText()
                    : richEditBox.PlaceholderText());
        return isCurrent() ? hint : wxString();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TextBox hint query", e);
        return wxString();
    }
}

int wxTextCtrl::GetLineLength(long lineNo) const
{
    return static_cast<int>(GetLineText(lineNo).length());
}

wxString wxTextCtrl::GetLineText(long lineNo) const
{
    const wxString value = m_value;
    const auto logicalFallback = [&]() -> wxString
    {
        wxArrayString logicalLines = wxSplit(value, '\n', '\0');
        if ( logicalLines.empty() )
            logicalLines.Add(wxString());
        return lineNo >= 0 &&
               static_cast<std::size_t>(lineNo) < logicalLines.size()
                 ? logicalLines[static_cast<std::size_t>(lineNo)]
                 : wxString();
    };

    if ( lineNo >= 0 && IsMultiLine() && !HasFlag(wxTE_DONTWRAP) &&
         m_winui && m_winui->textBox )
    {
        wxWinUITextCtrlImpl * const impl = m_winui.get();
        const std::shared_ptr<wxWinUITextCallbackState> callbackState =
            impl->callbackState;
        const std::uint64_t callbackGeneration =
            callbackState->GetGeneration();
        const std::uint64_t contentGeneration =
            impl->textContentGeneration;
        const MUXC::TextBox textBox = impl->textBox;
        const wxWeakRef<wxWindow> self(
            const_cast<wxTextCtrl *>(this));
        const auto isCurrent = [&]()
        {
            if ( callbackState->GetGeneration() != callbackGeneration )
                return false;
            const wxTextCtrl * const live =
                wxDynamicCast(self.get(), wxTextCtrl);
            return live && live->m_winui &&
                   live->m_winui.get() == impl &&
                   live->m_winui->callbackState == callbackState &&
                   live->m_winui->textBox == textBox &&
                   live->m_winui->textContentGeneration ==
                       contentGeneration &&
                   live->m_value == value;
        };
        std::vector<wxWinUITextPhysicalLine> physicalLines;
        if ( wxWinUIGetWrappedTextBoxLines(
                 *this, textBox, value, isCurrent, &physicalLines) )
        {
            if ( static_cast<std::size_t>(lineNo) >= physicalLines.size() )
                return wxString();
            const wxWinUITextPhysicalLine& line =
                physicalLines[static_cast<std::size_t>(lineNo)];
            return value.Mid(
                static_cast<std::size_t>(line.start),
                static_cast<std::size_t>(line.end - line.start));
        }
        return logicalFallback();
    }

    if ( lineNo < 0 || HasFlag(wxTE_DONTWRAP) ||
         !m_winui || !m_winui->richEditBox ||
         value.length() >
             static_cast<std::size_t>(
                 (std::numeric_limits<int32_t>::max)()) )
    {
        return logicalFallback();
    }

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const std::uint64_t contentGeneration = impl->textContentGeneration;
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const wxWeakRef<wxWindow> self(
        const_cast<wxTextCtrl *>(this));
    const auto isCurrent = [&]() -> bool
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return false;
        const wxTextCtrl * const live =
            wxDynamicCast(self.get(), wxTextCtrl);
        return live && live->m_winui &&
               live->m_winui.get() == impl &&
               live->m_winui->callbackState == callbackState &&
               live->m_winui->richEditBox == richEditBox &&
               live->m_winui->textContentGeneration == contentGeneration &&
               live->m_value == value;
    };

    try
    {
        richEditBox.UpdateLayout();
        if ( !isCurrent() )
            return logicalFallback();

        const WUT::ITextDocument document = richEditBox.Document();
        const WUT::ITextRange storyEnd = document.GetRange(
            static_cast<int32_t>(value.length()),
            static_cast<int32_t>(value.length()));
        const int physicalLineCount =
            storyEnd.GetIndex(WUT::TextRangeUnit::Line);
        if ( !isCurrent() || lineNo >= physicalLineCount )
            return wxString();

        const WUT::ITextRange line = document.GetRange(0, 0);
        line.SetIndex(WUT::TextRangeUnit::Line,
                      static_cast<int32_t>(lineNo + 1),
                      false);
        line.Expand(WUT::TextRangeUnit::Line);
        if ( !isCurrent() )
            return logicalFallback();

        const long valueLength = static_cast<long>(value.length());
        const long start = wxMax<long>(
            0, wxMin<long>(line.StartPosition(), valueLength));
        const long end = wxMax<long>(
            start, wxMin<long>(line.EndPosition(), valueLength));
        wxString text = value.Mid(
            static_cast<std::size_t>(start),
            static_cast<std::size_t>(end - start));

        // TOM's Line unit owns the explicit paragraph delimiter. wxTextCtrl's
        // GetLineText()/GetLineLength() contract excludes it.
        if ( !text.empty() && text.Last() == '\n' )
            text.RemoveLast();
        return text;
    }
    catch ( const winrt::hresult_error& )
    {
        return logicalFallback();
    }
}

int wxTextCtrl::GetNumberOfLines() const
{
    if ( !IsMultiLine() )
        return 1;

    const wxString value = m_value;
    const auto getLogicalLineCount = [&]() -> int
    {
        int count = 1;
        for ( const wxUniChar character : value )
        {
            if ( character == '\n' )
            {
                if ( count == (std::numeric_limits<int>::max)() )
                    break;
                ++count;
            }
        }
        return count;
    };
    if ( HasFlag(wxTE_DONTWRAP) )
        return getLogicalLineCount();

    // RichEditBox exposes TOM's physical-line index directly. At the end of a
    // story GetIndex(Line) is the total number of laid-out lines, including
    // wrapping and a final empty line, so this is both exact and O(1).
    if ( m_winui && m_winui->richEditBox &&
         value.length() <=
            static_cast<std::size_t>(
                (std::numeric_limits<int32_t>::max)()) )
    {
        wxWinUITextCtrlImpl * const impl = m_winui.get();
        const std::shared_ptr<wxWinUITextCallbackState> callbackState =
            impl->callbackState;
        const std::uint64_t callbackGeneration =
            callbackState->GetGeneration();
        const std::uint64_t contentGeneration =
            impl->textContentGeneration;
        const MUXC::RichEditBox richEditBox = impl->richEditBox;
        const wxWeakRef<wxWindow> self(
            const_cast<wxTextCtrl *>(this));
        const auto isCurrent = [&]() -> bool
        {
            if ( callbackState->GetGeneration() != callbackGeneration )
                return false;
            const wxTextCtrl * const live =
                wxDynamicCast(self.get(), wxTextCtrl);
            return live && live->m_winui &&
                   live->m_winui.get() == impl &&
                   live->m_winui->callbackState == callbackState &&
                   live->m_winui->richEditBox == richEditBox &&
                   live->m_winui->textContentGeneration ==
                       contentGeneration &&
                   live->m_value == value;
        };

        try
        {
            richEditBox.UpdateLayout();
            if ( !isCurrent() )
                return getLogicalLineCount();

            const WUT::ITextRange end = richEditBox.Document().GetRange(
                static_cast<int32_t>(value.length()),
                static_cast<int32_t>(value.length()));
            const int physicalLines =
                end.GetIndex(WUT::TextRangeUnit::Line);
            if ( isCurrent() && physicalLines > 0 )
                return physicalLines;
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }

    // TextBox doesn't expose TOM's Line unit publicly. Enumerate its actual
    // insertion edges instead of estimating with GDI: the latter uses a
    // different shaping/wrapping engine and produces false line numbers for
    // proportional fonts, DPI changes, bidi runs and soft wraps. There is no
    // arbitrary document-size cutoff; if WinUI virtualizes an unavailable
    // range we return the safe logical count rather than a fabricated answer.
    if ( !m_winui || !m_winui->textBox ||
         value.length() > static_cast<std::size_t>(
                              (std::numeric_limits<int32_t>::max)()) )
    {
        return getLogicalLineCount();
    }

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const std::uint64_t contentGeneration =
        impl->textContentGeneration;
    const MUXC::TextBox textBox = impl->textBox;
    const wxWeakRef<wxWindow> self(
        const_cast<wxTextCtrl *>(this));
    const auto isCurrent = [&]() -> bool
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return false;
        wxTextCtrl * const live =
            wxDynamicCast(self.get(), wxTextCtrl);
        return live && live->m_winui &&
               live->m_winui.get() == impl &&
               live->m_winui->callbackState == callbackState &&
               live->m_winui->textBox == textBox &&
               live->m_winui->textContentGeneration == contentGeneration &&
               live->m_value == value;
    };

    try
    {
        MUXC::ScrollViewer scroll =
            wxWinUIResolveTextScrollViewer(*this, textBox);
        if ( !scroll || !isCurrent() )
            return getLogicalLineCount();

        textBox.UpdateLayout();
        if ( !isCurrent() )
            return getLogicalLineCount();
        scroll = wxWinUIGetCurrentTextScrollViewer(textBox);
        if ( !scroll || !isCurrent() )
            return getLogicalLineCount();
        scroll.UpdateLayout();
        if ( !isCurrent() )
            return getLogicalLineCount();
        if ( wxWinUIGetCurrentTextScrollViewer(textBox) != scroll ||
             !isCurrent() )
        {
            return getLogicalLineCount();
        }

        std::vector<wxWinUITextPhysicalLine> physicalLines;
        if ( wxWinUIGetWrappedTextBoxLines(
                 *this, textBox, value, isCurrent, &physicalLines) &&
             physicalLines.size() <=
                 static_cast<std::size_t>(
                     (std::numeric_limits<int>::max)()) )
        {
            return static_cast<int>(physicalLines.size());
        }
    }
    catch ( const winrt::hresult_error& )
    {
    }

    return getLogicalLineCount();
}

bool wxTextCtrl::IsModified() const
{
    return m_modified;
}

void wxTextCtrl::MarkDirty()
{
    m_modified = true;
}

void wxTextCtrl::DiscardEdits()
{
    m_modified = false;
}

bool wxTextCtrl::EmulateKeyPress(const wxKeyEvent& event)
{
    const wxWeakRef<wxWindow> self(this);
    SetFocus();
    wxTextCtrl *live = wxDynamicCast(self.get(), wxTextCtrl);
    if ( !live )
        return false;

    // wxMSW reports whether the synthesized key changed the text length (not
    // merely whether it moved the caret). Preserve that observable contract
    // while applying the edit through the same max-length/event policy as a
    // real XAML input transaction.
    const std::size_t oldLength = live->m_value.length();
    const auto changedLength = [&]()
    {
        const wxTextCtrl * const current =
            wxDynamicCast(self.get(), wxTextCtrl);
        return current && current->m_value.length() != oldLength;
    };

    const int keycode = event.GetKeyCode();
    if ( event.ControlDown() && !event.AltDown() )
    {
        const int commandKey =
            keycode >= 'a' && keycode <= 'z'
                ? keycode - 'a' + 'A'
                : keycode;
        switch ( commandKey )
        {
            case 'A': live->SelectAll(); break;
            case 'C': live->Copy(); break;
            case 'V': live->Paste(); break;
            case 'X': live->Cut(); break;
            case 'Y': live->Redo(); break;
            case 'Z': live->Undo(); break;
            default: return false;
        }
        return changedLength();
    }

    const wxWinUITextPositionMap map =
        wxWinUIMakeTextPositionMap(*live, live->m_value);
    const long selectionStart =
        wxMin(live->m_selectionStart, live->m_selectionEnd);
    const long selectionEnd =
        wxMax(live->m_selectionStart, live->m_selectionEnd);
    const bool hasSelection = selectionStart != selectionEnd;
    long logical = 0;
    map.PublicToLogical(live->m_insertionPoint, &logical);

    switch ( keycode )
    {
        case WXK_LEFT:
        {
            const long next = hasSelection && !event.ShiftDown()
                ? selectionStart
                : map.LogicalToPublic(wxMax<long>(0, logical - 1));
            if ( event.ShiftDown() )
                live->SetSelection(next, selectionEnd);
            else
                live->SetInsertionPoint(next);
            return false;
        }

        case WXK_RIGHT:
        {
            const long next = hasSelection && !event.ShiftDown()
                ? selectionEnd
                : map.LogicalToPublic(wxMin<long>(
                      static_cast<long>(live->m_value.length()), logical + 1));
            if ( event.ShiftDown() )
                live->SetSelection(selectionStart, next);
            else
                live->SetInsertionPoint(next);
            return false;
        }

        case WXK_HOME:
            live->SetInsertionPoint(0);
            return false;

        case WXK_END:
            live->SetInsertionPointEnd();
            return false;

        case WXK_DELETE:
        case WXK_NUMPAD_DELETE:
            if ( !live->IsEditable() )
                return false;
            if ( hasSelection )
                live->Remove(selectionStart, selectionEnd);
            else if ( logical < static_cast<long>(live->m_value.length()) )
                live->Remove(map.LogicalToPublic(logical),
                             map.LogicalToPublic(logical + 1));
            return changedLength();

        case WXK_BACK:
            if ( !live->IsEditable() )
                return false;
            if ( hasSelection )
                live->Remove(selectionStart, selectionEnd);
            else if ( logical > 0 )
                live->Remove(map.LogicalToPublic(logical - 1),
                             map.LogicalToPublic(logical));
            return changedLength();

        case WXK_RETURN:
        case WXK_NUMPAD_ENTER:
            if ( live->IsMultiLine() && live->IsEditable() )
                live->WriteTextWithPolicy(wxS("\n"), true);
            else if ( live->HasFlag(wxTE_PROCESS_ENTER) )
                live->ProcessEnter();
            return changedLength();

        case WXK_TAB:
            if ( live->HasFlag(wxTE_PROCESS_TAB) && live->IsEditable() )
                live->WriteTextWithPolicy(wxS("\t"), true);
            return changedLength();

        default:
            break;
    }

    wxUniChar character;
    switch ( keycode )
    {
        case WXK_NUMPAD0: character = '0'; break;
        case WXK_NUMPAD1: character = '1'; break;
        case WXK_NUMPAD2: character = '2'; break;
        case WXK_NUMPAD3: character = '3'; break;
        case WXK_NUMPAD4: character = '4'; break;
        case WXK_NUMPAD5: character = '5'; break;
        case WXK_NUMPAD6: character = '6'; break;
        case WXK_NUMPAD7: character = '7'; break;
        case WXK_NUMPAD8: character = '8'; break;
        case WXK_NUMPAD9: character = '9'; break;
        case WXK_MULTIPLY:
        case WXK_NUMPAD_MULTIPLY: character = '*'; break;
        case WXK_ADD:
        case WXK_NUMPAD_ADD: character = '+'; break;
        case WXK_SUBTRACT:
        case WXK_NUMPAD_SUBTRACT: character = '-'; break;
        case WXK_DECIMAL:
        case WXK_NUMPAD_DECIMAL: character = '.'; break;
        case WXK_DIVIDE:
        case WXK_NUMPAD_DIVIDE: character = '/'; break;
        default:
        {
            const int unicode = event.GetUnicodeKey();
            if ( unicode == WXK_NONE || unicode < 0x20 || event.AltDown() )
                return false;
            character = static_cast<wxUniChar>(unicode);
            break;
        }
    }

    if ( !live->IsEditable() )
        return false;
    live->WriteTextWithPolicy(wxString(character), true);
    return changedLength();
}

void wxTextCtrl::Command(wxCommandEvent& event)
{
    const wxWeakRef<wxWindow> self(this);
    SetValue(event.GetString());

    wxTextCtrl * const live = wxDynamicCast(self.get(), wxTextCtrl);
    if ( live )
        live->ProcessCommand(event);
}

#if wxUSE_DRAG_AND_DROP
void wxTextCtrl::OnDropFiles(wxDropFilesEvent& event)
{
    // Match wxMSW/wxOSX: a file drop on the control loads the first file by
    // default. LoadFile() is the final boundary because its file and text
    // events are allowed to destroy this control.
    if ( event.GetNumberOfFiles() > 0 )
        LoadFile(event.GetFiles()[0]);
}
#endif

long wxTextCtrl::XYToPosition(long x, long y) const
{
    const wxString value = m_value;
    const wxWinUITextPositionMap map =
        wxWinUIMakeTextPositionMap(*this, value);
    const long fallback = map.XYToPosition(x, y);

    if ( x >= 0 && y >= 0 && IsMultiLine() &&
         !HasFlag(wxTE_DONTWRAP) && m_winui && m_winui->textBox )
    {
        wxWinUITextCtrlImpl * const impl = m_winui.get();
        const std::shared_ptr<wxWinUITextCallbackState> callbackState =
            impl->callbackState;
        const std::uint64_t callbackGeneration =
            callbackState->GetGeneration();
        const std::uint64_t contentGeneration =
            impl->textContentGeneration;
        const MUXC::TextBox textBox = impl->textBox;
        const wxWeakRef<wxWindow> self(
            const_cast<wxTextCtrl *>(this));
        const auto isCurrent = [&]()
        {
            if ( callbackState->GetGeneration() != callbackGeneration )
                return false;
            const wxTextCtrl * const live =
                wxDynamicCast(self.get(), wxTextCtrl);
            return live && live->m_winui &&
                   live->m_winui.get() == impl &&
                   live->m_winui->callbackState == callbackState &&
                   live->m_winui->textBox == textBox &&
                   live->m_winui->textContentGeneration ==
                       contentGeneration &&
                   live->m_value == value;
        };
        std::vector<wxWinUITextPhysicalLine> physicalLines;
        if ( wxWinUIGetWrappedTextBoxLines(
                 *this, textBox, value, isCurrent, &physicalLines) )
        {
            if ( static_cast<std::size_t>(y) >= physicalLines.size() )
                return -1;
            const wxWinUITextPhysicalLine& line =
                physicalLines[static_cast<std::size_t>(y)];
            if ( x > line.end - line.start )
                return -1;
            return map.LogicalToPublic(line.start + x);
        }
        return fallback;
    }

    if ( x < 0 || y < 0 || !IsMultiLine() || HasFlag(wxTE_DONTWRAP) ||
         !m_winui || !m_winui->richEditBox ||
         value.length() >
             static_cast<std::size_t>(
                 (std::numeric_limits<int32_t>::max)()) ||
         y >= (std::numeric_limits<int32_t>::max)() )
    {
        return fallback;
    }

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const std::uint64_t contentGeneration = impl->textContentGeneration;
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const wxWeakRef<wxWindow> self(
        const_cast<wxTextCtrl *>(this));
    const auto isCurrent = [&]() -> bool
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return false;
        const wxTextCtrl * const live =
            wxDynamicCast(self.get(), wxTextCtrl);
        return live && live->m_winui &&
               live->m_winui.get() == impl &&
               live->m_winui->callbackState == callbackState &&
               live->m_winui->richEditBox == richEditBox &&
               live->m_winui->textContentGeneration == contentGeneration &&
               live->m_value == value;
    };

    try
    {
        richEditBox.UpdateLayout();
        if ( !isCurrent() )
            return fallback;

        const WUT::ITextDocument document = richEditBox.Document();
        const WUT::ITextRange storyEnd = document.GetRange(
            static_cast<int32_t>(value.length()),
            static_cast<int32_t>(value.length()));
        const int physicalLineCount =
            storyEnd.GetIndex(WUT::TextRangeUnit::Line);
        if ( !isCurrent() || y >= physicalLineCount )
            return -1;

        const WUT::ITextRange line = document.GetRange(0, 0);
        line.SetIndex(WUT::TextRangeUnit::Line,
                      static_cast<int32_t>(y + 1),
                      false);
        line.Expand(WUT::TextRangeUnit::Line);
        if ( !isCurrent() )
            return fallback;

        const long valueLength = static_cast<long>(value.length());
        const long start = wxMax<long>(
            0, wxMin<long>(line.StartPosition(), valueLength));
        long end = wxMax<long>(
            start, wxMin<long>(line.EndPosition(), valueLength));
        if ( end > start &&
             value[static_cast<std::size_t>(end - 1)] == '\n' )
        {
            --end;
        }

        if ( x > end - start )
            return -1;
        return map.LogicalToPublic(start + x);
    }
    catch ( const winrt::hresult_error& )
    {
        return fallback;
    }
}

bool wxTextCtrl::PositionToXY(long pos, long *x, long *y) const
{
    const wxString value = m_value;
    const wxWinUITextPositionMap map =
        wxWinUIMakeTextPositionMap(*this, value);
    long fallbackX = 0;
    long fallbackY = 0;
    if ( !map.PositionToXY(pos, &fallbackX, &fallbackY) )
        return false;

    long logical = 0;
    if ( !map.PublicToLogical(pos, &logical) )
        return false;

    if ( IsMultiLine() && !HasFlag(wxTE_DONTWRAP) &&
         m_winui && m_winui->textBox )
    {
        wxWinUITextCtrlImpl * const impl = m_winui.get();
        const std::shared_ptr<wxWinUITextCallbackState> callbackState =
            impl->callbackState;
        const std::uint64_t callbackGeneration =
            callbackState->GetGeneration();
        const std::uint64_t contentGeneration =
            impl->textContentGeneration;
        const MUXC::TextBox textBox = impl->textBox;
        const wxWeakRef<wxWindow> self(
            const_cast<wxTextCtrl *>(this));
        const auto isCurrent = [&]()
        {
            if ( callbackState->GetGeneration() != callbackGeneration )
                return false;
            const wxTextCtrl * const live =
                wxDynamicCast(self.get(), wxTextCtrl);
            return live && live->m_winui &&
                   live->m_winui.get() == impl &&
                   live->m_winui->callbackState == callbackState &&
                   live->m_winui->textBox == textBox &&
                   live->m_winui->textContentGeneration ==
                       contentGeneration &&
                   live->m_value == value;
        };
        std::vector<wxWinUITextPhysicalLine> physicalLines;
        if ( wxWinUIGetWrappedTextBoxLines(
                 *this, textBox, value, isCurrent, &physicalLines) )
        {
            std::size_t lineIndex = 0;
            for ( std::size_t i = 1; i < physicalLines.size(); ++i )
            {
                if ( logical < physicalLines[i].start )
                    break;
                lineIndex = i;
            }
            if ( x )
                *x = logical - physicalLines[lineIndex].start;
            if ( y )
                *y = static_cast<long>(lineIndex);
            return true;
        }
        if ( x )
            *x = fallbackX;
        if ( y )
            *y = fallbackY;
        return true;
    }

    if ( !IsMultiLine() || HasFlag(wxTE_DONTWRAP) ||
         !m_winui || !m_winui->richEditBox ||
         value.length() >
             static_cast<std::size_t>(
                 (std::numeric_limits<int32_t>::max)()) )
    {
        if ( x )
            *x = fallbackX;
        if ( y )
            *y = fallbackY;
        return true;
    }

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const std::uint64_t contentGeneration = impl->textContentGeneration;
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const wxWeakRef<wxWindow> self(
        const_cast<wxTextCtrl *>(this));
    const auto isCurrent = [&]() -> bool
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return false;
        const wxTextCtrl * const live =
            wxDynamicCast(self.get(), wxTextCtrl);
        return live && live->m_winui &&
               live->m_winui.get() == impl &&
               live->m_winui->callbackState == callbackState &&
               live->m_winui->richEditBox == richEditBox &&
               live->m_winui->textContentGeneration == contentGeneration &&
               live->m_value == value;
    };

    try
    {
        richEditBox.UpdateLayout();
        if ( !isCurrent() )
            return false;

        const WUT::ITextRange position =
            richEditBox.Document().GetRange(
                static_cast<int32_t>(logical),
                static_cast<int32_t>(logical));
        const int lineIndex =
            position.GetIndex(WUT::TextRangeUnit::Line);
        const WUT::ITextRange lineStart = position.GetClone();
        lineStart.StartOf(WUT::TextRangeUnit::Line, false);
        if ( !isCurrent() || lineIndex <= 0 )
            return false;

        if ( x )
            *x = logical - lineStart.StartPosition();
        if ( y )
            *y = lineIndex - 1;
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        if ( x )
            *x = fallbackX;
        if ( y )
            *y = fallbackY;
        return true;
    }
}

void wxTextCtrl::ShowPosition(long pos)
{
    EnsurePositionVisible(
        wxWinUIClampTextPos(pos, GetLastPosition()));
}

wxTextCtrlHitTestResult wxTextCtrl::HitTest(const wxPoint& pt, long *pos) const
{
    const wxString value = m_value;
    const bool multiline = IsMultiLine();

    // Compute the model-only answer before crossing any XAML boundary. If a
    // layout callback destroys this control, returning this already-owned
    // value is safe; falling through and invoking another member is not. The
    // complete scan/measurement budget is shared across row lookup and the
    // selected line: an unavailable peer must never turn HitTest() into an
    // unbounded UI-thread walk.
    constexpr std::size_t MaxFallbackInspectedUnits = 4096;
    bool fallbackKnown = false;
    long fallbackPosition = 0;
    const wxTextCtrlHitTestResult fallbackResult = [&]()
    {
        const int lineHeight = wxMax(1, GetTextExtent(wxS("X")).y);
        if ( pt.y < 0 )
        {
            fallbackKnown = true;
            return wxTE_HT_BEFORE;
        }

        const long row = multiline ? pt.y / lineHeight : 0;
        if ( row < 0 )
        {
            fallbackKnown = true;
            return wxTE_HT_BEFORE;
        }

        std::size_t inspected = 0;
        std::size_t logical = 0;
        long publicPosition = 0;
        long currentRow = 0;
        while ( currentRow < row )
        {
            if ( logical == value.length() )
            {
                fallbackKnown = true;
                fallbackPosition = publicPosition;
                return wxTE_HT_BELOW;
            }
            if ( inspected == MaxFallbackInspectedUnits )
                return wxTE_HT_UNKNOWN;

            const wxUniChar ch = value[logical++];
            ++inspected;
            if ( ch == '\n' )
            {
                ++currentRow;
                publicPosition += multiline ? 2 : 1;
            }
            else
            {
                ++publicPosition;
            }
        }

        const long publicLineStart = publicPosition;
        const int contentX = pt.x - FromDIP(4);
        if ( contentX < 0 )
        {
            fallbackKnown = true;
            fallbackPosition = publicLineStart;
            return wxTE_HT_BEFORE;
        }
        if ( logical < value.length() &&
             value[logical] != '\n' &&
             contentX == 0 )
        {
            fallbackKnown = true;
            fallbackPosition = publicLineStart;
            return wxTE_HT_ON_TEXT;
        }

        const std::size_t prefixStart = logical;
        while ( logical < value.length() &&
                value[logical] != '\n' &&
                inspected < MaxFallbackInspectedUnits )
        {
            ++logical;
            ++inspected;
        }

        const bool reachedLineEnd =
            logical == value.length() || value[logical] == '\n';
        const wxString prefix =
            value.Mid(prefixStart, logical - prefixStart);
        wxArrayInt partialWidths;
        wxClientDC dc(const_cast<wxTextCtrl *>(this));
        dc.SetFont(GetFont());
        if ( !prefix.empty() &&
             !dc.GetPartialTextExtents(prefix, partialWidths) )
        {
            return wxTE_HT_UNKNOWN;
        }

        for ( std::size_t i = 0; i < partialWidths.size(); ++i )
        {
            if ( contentX < partialWidths[i] )
            {
                fallbackKnown = true;
                fallbackPosition =
                    publicLineStart + static_cast<long>(i);
                return wxTE_HT_ON_TEXT;
            }
        }

        if ( !reachedLineEnd )
            return wxTE_HT_UNKNOWN;

        fallbackKnown = true;
        fallbackPosition =
            publicLineStart + static_cast<long>(prefix.length());
        return wxTE_HT_BEYOND;
    }();

    const wxWeakRef<wxWindow> self(
        const_cast<wxTextCtrl *>(this));

    if ( m_winui && m_winui->richEditBox &&
         value.length() <=
            static_cast<std::size_t>(
                (std::numeric_limits<int32_t>::max)()) )
    {
        wxWinUITextCtrlImpl * const impl = m_winui.get();
        const std::shared_ptr<wxWinUITextCallbackState> callbackState =
            impl->callbackState;
        const std::uint64_t callbackGeneration =
            callbackState->GetGeneration();
        const std::uint64_t contentGeneration =
            impl->textContentGeneration;
        const MUXC::RichEditBox richEditBox = impl->richEditBox;
        const auto getLiveOwner = [&]() -> wxTextCtrl *
        {
            if ( callbackState->GetGeneration() != callbackGeneration )
                return nullptr;
            wxTextCtrl * const live =
                wxDynamicCast(self.get(), wxTextCtrl);
            return live && live->m_winui &&
                           live->m_winui.get() == impl &&
                           live->m_winui->callbackState == callbackState &&
                           live->m_winui->richEditBox == richEditBox &&
                           live->m_winui->textContentGeneration ==
                               contentGeneration &&
                           live->m_value == value
                     ? live
                     : nullptr;
        };

        try
        {
            richEditBox.UpdateLayout();
            if ( !getLiveOwner() )
                return wxTE_HT_UNKNOWN;

            winrt::Windows::Foundation::Point richPoint{};
            if ( !wxWinUIClientPointToRichElement(
                     *this, richEditBox, pt, &richPoint) ||
                 !getLiveOwner() )
            {
                return wxTE_HT_UNKNOWN;
            }

            const int32_t length =
                static_cast<int32_t>(value.length());
            const WUT::PointOptions options =
                wxWinUIRichClientPointOptions();
            const WUT::ITextDocument document = richEditBox.Document();

            wxTextCtrlHitTestResult exactResult = wxTE_HT_UNKNOWN;
            long exactLogical = 0;
            if ( wxWinUIHitTestBoundedRichTextGeometry(
                     richEditBox,
                     value,
                     richPoint,
                     getLiveOwner,
                     &exactResult,
                     &exactLogical) ||
                 wxWinUIHitTestRichTextLine(
                     richEditBox,
                     value,
                     richPoint,
                     getLiveOwner,
                     &exactResult,
                     &exactLogical) )
            {
                wxTextCtrl * const live = getLiveOwner();
                if ( !live )
                    return wxTE_HT_UNKNOWN;
                if ( pos )
                {
                    const wxWinUITextPositionMap map =
                        wxWinUIMakeTextPositionMap(*live, value);
                    *pos = map.LogicalToPublic(exactLogical);
                }
                return exactResult;
            }
            if ( !getLiveOwner() )
                return wxTE_HT_UNKNOWN;

            constexpr float coordinateTolerance = 0.75f;
            long logical = 0;
            wxTextCtrlHitTestResult result = wxTE_HT_UNKNOWN;
            // Do not query the document endpoints before the visible hit.
            // RichEdit virtualizes off-client ranges and GetPoint() may report
            // S_FALSE for the far endpoint even though GetRangeFromPoint() and
            // the visible physical line are fully available.
            const WUT::ITextRange nearest =
                document.GetRangeFromPoint(richPoint, options);
            logical = wxMax<long>(
                0, wxMin<long>(nearest.StartPosition(), length));

            const WUT::ITextRange line = nearest.GetClone();
            line.Expand(WUT::TextRangeUnit::Line);
            winrt::Windows::Foundation::Rect lineRect{};
            int32_t lineHit = 0;
            line.GetRect(options, lineRect, lineHit);
            wxUnusedVar(lineHit);
            if ( !getLiveOwner() || !wxWinUIIsFiniteRect(lineRect) )
                return wxTE_HT_UNKNOWN;

            const float lineRight = lineRect.X + lineRect.Width;
            const float lineBottom = lineRect.Y + lineRect.Height;
            const long lineStart = wxMax<long>(
                0, wxMin<long>(line.StartPosition(), length));
            const long lineEnd = wxMax<long>(
                lineStart, wxMin<long>(line.EndPosition(), length));
            if ( richPoint.Y < lineRect.Y - coordinateTolerance )
            {
                logical = lineStart;
                result = lineStart == 0
                           ? wxTE_HT_BEFORE
                           : wxTE_HT_UNKNOWN;
            }
            else if ( richPoint.Y > lineBottom + coordinateTolerance )
            {
                logical = lineEnd;
                result = lineEnd >= length
                           ? wxTE_HT_BELOW
                           : wxTE_HT_UNKNOWN;
            }
            else if ( richPoint.X < lineRect.X - coordinateTolerance )
            {
                logical = lineStart;
                result = wxTE_HT_BEFORE;
            }
            else if ( lineRect.Width <= coordinateTolerance ||
                      richPoint.X > lineRight + coordinateTolerance )
            {
                // GetRangeFromPoint already gives the nearest insertion point
                // on this physical line, including wrapped lines.
                result = wxTE_HT_BEYOND;
            }
            else
            {
                // TOM returns an insertion point. Convert it to the wx
                // character-under-point convention by testing the ranges
                // immediately on either side of that caret.
                bool foundCharacter = false;
                bool foundExactLeading = false;
                long firstContaining = logical;
                long closestLeading = logical;
                double closestLeadingDistance =
                    (std::numeric_limits<double>::max)();
                const long candidates[] = { logical - 1, logical };
                for ( const long candidate : candidates )
                {
                    if ( candidate < 0 || candidate >= length ||
                         value[static_cast<std::size_t>(candidate)] == '\n' )
                    {
                        continue;
                    }

                    const WUT::ITextRange character = document.GetRange(
                        static_cast<int32_t>(candidate),
                        static_cast<int32_t>(candidate + 1));
                    winrt::Windows::Foundation::Rect characterRect{};
                    int32_t characterHit = 0;
                    character.GetRect(options, characterRect, characterHit);
                    wxUnusedVar(characterHit);
                    if ( !getLiveOwner() )
                        return wxTE_HT_UNKNOWN;

                    if ( !wxWinUIIsFiniteRect(characterRect) )
                        continue;

                    winrt::Windows::Foundation::Point leading{};
                    const bool hasLeading =
                        wxWinUIGetRichTextPoint(
                            richEditBox, candidate, &leading) &&
                        getLiveOwner();
                    if ( !getLiveOwner() )
                        return wxTE_HT_UNKNOWN;

                    winrt::Windows::Foundation::Point rangeLeft{};
                    winrt::Windows::Foundation::Point rangeRight{};
                    const bool hasRangeEdges =
                        wxWinUIGetRichTextRangePoint(
                            character,
                            WUT::HorizontalCharacterAlignment::Left,
                            true,
                            &rangeLeft) &&
                        getLiveOwner() &&
                        wxWinUIGetRichTextRangePoint(
                            character,
                            WUT::HorizontalCharacterAlignment::Right,
                            true,
                            &rangeRight) &&
                        getLiveOwner();
                    if ( !getLiveOwner() )
                        return wxTE_HT_UNKNOWN;

                    double left = characterRect.X;
                    double right = characterRect.X + characterRect.Width;
                    if ( hasRangeEdges )
                    {
                        const double nativeLeft =
                            wxMin<double>(rangeLeft.X, rangeRight.X);
                        const double nativeRight =
                            wxMax<double>(rangeLeft.X, rangeRight.X);
                        if ( nativeRight > nativeLeft )
                        {
                            left = nativeLeft;
                            right = nativeRight;
                        }
                    }

                    if ( richPoint.X >= left - coordinateTolerance &&
                         richPoint.X <= right + coordinateTolerance &&
                         richPoint.Y >= characterRect.Y -
                                            coordinateTolerance &&
                         richPoint.Y <= characterRect.Y +
                                            characterRect.Height +
                                            coordinateTolerance )
                    {
                        if ( !foundCharacter )
                            firstContaining = candidate;
                        foundCharacter = true;

                        if ( hasLeading )
                        {
                            const double deltaX = richPoint.X - leading.X;
                            const double deltaY = richPoint.Y - leading.Y;
                            const double leadingDistance =
                                deltaX * deltaX + deltaY * deltaY;
                            if ( !foundExactLeading ||
                                 leadingDistance < closestLeadingDistance )
                            {
                                foundExactLeading = true;
                                closestLeading = candidate;
                                closestLeadingDistance = leadingDistance;
                            }
                        }
                    }
                }

                if ( foundCharacter )
                {
                    logical = foundExactLeading
                                ? closestLeading
                                : firstContaining;
                }

                result = foundCharacter
                           ? wxTE_HT_ON_TEXT
                           : wxTE_HT_BEYOND;
            }

            wxTextCtrl * const live = getLiveOwner();
            if ( !live )
                return wxTE_HT_UNKNOWN;
            if ( pos )
            {
                const wxWinUITextPositionMap map =
                    wxWinUIMakeTextPositionMap(*live, value);
                *pos = map.LogicalToPublic(logical);
            }
            return result;
        }
        catch ( const winrt::hresult_error& )
        {
            if ( !getLiveOwner() )
                return wxTE_HT_UNKNOWN;
        }

        // A model estimate cannot classify formatted, wrapped RichEdit text.
        return wxTE_HT_UNKNOWN;
    }

    if ( m_winui && m_winui->textBox )
    {
        wxWinUITextCtrlImpl * const impl = m_winui.get();
        const std::shared_ptr<wxWinUITextCallbackState> callbackState =
            impl->callbackState;
        const std::uint64_t callbackGeneration =
            callbackState->GetGeneration();
        const std::uint64_t contentGeneration =
            impl->textContentGeneration;
        const MUXC::TextBox textBox = impl->textBox;
        const auto getLiveOwner = [&]() -> wxTextCtrl *
        {
            if ( callbackState->GetGeneration() != callbackGeneration )
                return nullptr;

            wxTextCtrl * const live =
                wxDynamicCast(self.get(), wxTextCtrl);
            return live && live->m_winui &&
                           live->m_winui.get() == impl &&
                           live->m_winui->callbackState == callbackState &&
                           live->m_winui->textBox == textBox &&
                           live->m_winui->textContentGeneration ==
                               contentGeneration &&
                           live->m_value == value
                     ? live
                     : nullptr;
        };

        wxTextCtrlHitTestResult result = wxTE_HT_UNKNOWN;
        long logical = 0;
        winrt::Windows::Foundation::Point textPoint;
        if ( wxWinUIClientPointToTextBox(
                 *this, textBox, pt, &textPoint) )
        {
            if ( !getLiveOwner() )
                return wxTE_HT_UNKNOWN;

            if ( wxWinUIHitTestTextContent(
                     textBox,
                     value,
                     textPoint,
                     &result,
                     &logical) )
            {
                if ( !getLiveOwner() )
                    return wxTE_HT_UNKNOWN;

                if ( pos )
                {
                    wxTextCtrl * const live = getLiveOwner();
                    if ( !live )
                        return wxTE_HT_UNKNOWN;
                    const wxWinUITextPositionMap map =
                        wxWinUIMakeTextPositionMap(*live, value);
                    *pos = map.LogicalToPublic(logical);
                }
                return result;
            }
        }

        // A failed peer query is allowed to use the precomputed model answer,
        // but never touch this object again if the failure destroyed it.
        if ( !getLiveOwner() )
            return wxTE_HT_UNKNOWN;
    }

    if ( !fallbackKnown )
        return wxTE_HT_UNKNOWN;

    if ( pos )
        *pos = fallbackPosition;
    return fallbackResult;
}

void wxTextCtrl::DoEnable(bool enable)
{
    const wxWeakRef<wxWindow> self(this);
    wxControl::DoEnable(enable);

    wxTextCtrl *live = wxDynamicCast(self.get(), wxTextCtrl);
    if ( !live || !live->m_winui || !live->m_winui->control() )
        return;

    wxWinUITextCtrlImpl * const impl = live->m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t generation = callbackState->GetGeneration();
    const MUXC::Control control = impl->control();
    const auto getLiveOwner = [&]() -> wxTextCtrl *
    {
        if ( callbackState->GetGeneration() != generation )
            return nullptr;
        wxTextCtrl * const owner = wxDynamicCast(self.get(), wxTextCtrl);
        return owner && owner->m_winui &&
                       owner->m_winui.get() == impl &&
                       owner->m_winui->callbackState == callbackState &&
                       owner->m_winui->control() == control
                 ? owner
                 : nullptr;
    };

    try
    {
        control.IsEnabled(enable);
        live = getLiveOwner();
        if ( !live )
            return;
        // Final boundary: rendering can dispatch application callbacks.
        live->m_winui->host.ForceRender();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI text enabled state", e);
    }
}

void wxTextCtrl::DoSetValue(const wxString& value, int flags)
{
    wxString normalized = wxWinUITextPositionMap::NormalizeNewlines(value);
    if ( m_forceUpper )
        normalized.MakeUpper();
    const bool changed = normalized != m_value;
    m_value = normalized;

    // Match the native MSW edit: changing the value resets the caret to the
    // front, while assigning the same value preserves it. Programmatic value
    // assignment never sets the edit's dirty flag.
    if ( changed )
    {
        if ( m_winui )
        {
            if ( m_winui->securePasswordPeer )
            {
                m_winui->securePasswordUndo.clear();
                m_winui->securePasswordRedo.clear();
                m_winui->securePasswordCompositionHasSnapshot = false;
            }
            ++m_winui->textContentGeneration;
            m_winui->InvalidatePositionVisibilityRequest();
        }
        m_insertionPoint = 0;
    }
    else
        ClampInsertionPoint();
    m_selectionStart = m_selectionEnd = m_insertionPoint;
    m_modified = false;

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl ? impl->callbackState : nullptr;
    const std::uint64_t contentGeneration =
        impl ? impl->textContentGeneration : 0;
    const wxWeakRef<wxWindow> self(this);
    ApplyValueToPeer();

    if ( flags & SetValue_SendEvent )
    {
        wxTextCtrl * const live = wxDynamicCast(self.get(), wxTextCtrl);
        if ( live && live->m_winui.get() == impl &&
             live->m_value == normalized &&
             (!impl ||
              (live->m_winui->callbackState == callbackState &&
               live->m_winui->textContentGeneration == contentGeneration)) )
        {
            live->SendTextEvent();
        }
    }
}

wxString wxTextCtrl::DoGetValue() const
{
    return m_value;
}

wxPoint wxTextCtrl::DoPositionToCoords(long pos) const
{
    const wxString value = m_value;
    const wxWinUITextPositionMap map =
        wxWinUIMakeTextPositionMap(*this, value);
    long logical = 0;
    if ( !map.PublicToLogical(pos, &logical) )
        return wxDefaultPosition;

    // Own the fallback before any layout call can re-enter application code.
    long column = 0;
    long row = 0;
    wxPoint fallback = wxDefaultPosition;
    if ( map.PositionToXY(pos, &column, &row) )
    {
        wxArrayString lines = wxSplit(value, '\n', '\0');
        if ( lines.empty() )
            lines.Add(wxString());
        wxString prefix;
        if ( row >= 0 && static_cast<std::size_t>(row) < lines.size() )
            prefix = lines[static_cast<std::size_t>(row)].Left(column);

        const int lineHeight = wxMax(1, GetTextExtent(wxS("X")).y);
        fallback = wxPoint(
            GetTextExtent(prefix).x,
            static_cast<int>(row) * lineHeight);
    }

    const wxWeakRef<wxWindow> self(
        const_cast<wxTextCtrl *>(this));
    if ( m_winui && m_winui->richEditBox )
    {
        wxWinUITextCtrlImpl * const impl = m_winui.get();
        const std::shared_ptr<wxWinUITextCallbackState> callbackState =
            impl->callbackState;
        const std::uint64_t callbackGeneration =
            callbackState->GetGeneration();
        const std::uint64_t contentGeneration =
            impl->textContentGeneration;
        const MUXC::RichEditBox richEditBox = impl->richEditBox;
        const auto getLiveOwner = [&]() -> wxTextCtrl *
        {
            if ( callbackState->GetGeneration() != callbackGeneration )
                return nullptr;
            wxTextCtrl * const live =
                wxDynamicCast(self.get(), wxTextCtrl);
            return live && live->m_winui &&
                           live->m_winui.get() == impl &&
                           live->m_winui->callbackState == callbackState &&
                           live->m_winui->richEditBox == richEditBox &&
                           live->m_winui->textContentGeneration ==
                               contentGeneration &&
                           live->m_value == value
                     ? live
                     : nullptr;
        };

        try
        {
            richEditBox.UpdateLayout();
            if ( !getLiveOwner() )
                return wxDefaultPosition;

            winrt::Windows::Foundation::Point richPoint{};
            if ( wxWinUIGetRichTextPoint(
                     richEditBox, logical, &richPoint) &&
                 getLiveOwner() )
            {
                wxPoint clientPoint;
                if ( wxWinUIRichElementPointToClient(
                         self, richEditBox, richPoint, &clientPoint) &&
                     getLiveOwner() )
                {
                    return clientPoint;
                }
            }
        }
        catch ( const winrt::hresult_error& )
        {
        }

        // Rich formatting, visual wrapping and the TOM viewport cannot be
        // represented by the logical-line GDI estimate below. Report an
        // unavailable exact result rather than publishing fictitious geometry.
        return wxDefaultPosition;
    }

    if ( m_winui && m_winui->textBox )
    {
        const bool peerUsesWrapping =
            IsMultiLine() && !HasFlag(wxTE_DONTWRAP);
        wxWinUITextCtrlImpl * const impl = m_winui.get();
        const std::shared_ptr<wxWinUITextCallbackState> callbackState =
            impl->callbackState;
        const std::uint64_t callbackGeneration =
            callbackState->GetGeneration();
        const std::uint64_t contentGeneration =
            impl->textContentGeneration;
        const MUXC::TextBox textBox = impl->textBox;
        const auto getLiveOwner = [&]() -> wxTextCtrl *
        {
            if ( callbackState->GetGeneration() != callbackGeneration )
                return nullptr;

            wxTextCtrl * const live =
                wxDynamicCast(self.get(), wxTextCtrl);
            return live && live->m_winui &&
                           live->m_winui.get() == impl &&
                           live->m_winui->callbackState == callbackState &&
                           live->m_winui->textBox == textBox &&
                           live->m_winui->textContentGeneration ==
                               contentGeneration &&
                           live->m_value == value
                     ? live
                     : nullptr;
        };

        try
        {
            // Arrange first. Reading the edge before this point combines an
            // old wrapped position with a new viewport after SetSize().
            MUXC::ScrollViewer scroll =
                wxWinUIResolveTextScrollViewer(*this, textBox);
            if ( scroll && getLiveOwner() )
            {
                textBox.UpdateLayout();
                if ( !getLiveOwner() )
                    return wxDefaultPosition;
                scroll = wxWinUIGetCurrentTextScrollViewer(textBox);
                if ( !scroll || !getLiveOwner() )
                    return wxDefaultPosition;
                scroll.UpdateLayout();
                if ( !getLiveOwner() )
                    return wxDefaultPosition;
                if ( wxWinUIGetCurrentTextScrollViewer(textBox) != scroll ||
                     !getLiveOwner() )
                {
                    return wxDefaultPosition;
                }

                wxWinUITextEdge edge;
                if ( wxWinUIGetTextEdge(
                         textBox, value, logical, &edge) &&
                     wxWinUIIsPlausibleWrappedTextEdge(textBox, edge) )
                {
                    if ( !getLiveOwner() )
                        return wxDefaultPosition;

                    wxPoint clientPoint;
                    if ( wxWinUITextViewPointToClient(
                             self,
                             textBox,
                             scroll,
                             edge.point,
                             &clientPoint) )
                    {
                        if ( !getLiveOwner() )
                            return wxDefaultPosition;
                        return clientPoint;
                    }
                }
            }
        }
        catch ( const winrt::hresult_error& )
        {
        }

        if ( !getLiveOwner() )
            return wxDefaultPosition;

        // A logical-line GDI estimate cannot represent physical XAML wrapping.
        // Returning it as an exact coordinate would turn a virtualized range
        // into a fictitious point (typically a huge X on row zero). Let callers
        // retry after ShowPosition() realizes the range instead.
        if ( peerUsesWrapping )
            return wxDefaultPosition;
    }

    return fallback;
}

wxSize wxTextCtrl::DoGetBestSize() const
{
    return DoGetSizeFromTextSize(FromDIP(100));
}

wxSize wxTextCtrl::DoGetSizeFromTextSize(int xlen, int ylen) const
{
    // xlen/ylen describe the requested text box in physical pixels. Derive
    // the non-text chrome from the real XAML control whenever it is already
    // measurable, with stable DIP fallbacks for pre-template size queries.
    const int charHeight = wxMax(1, GetTextExtent(wxS("X")).y);
    const bool multiline = IsMultiLine();
    const bool hasHorizontalScroll = HasFlag(wxHSCROLL);
    const bool hasVerticalScroll = !HasFlag(wxTE_NO_VSCROLL);
    const int fallbackChrome = FromDIP(12);
    const int minimumChrome = FromDIP(1);
    const int horizontalScrollHeight = FromDIP(12);
    const int verticalScrollWidth = FromDIP(16);
    const wxSize dipScale = FromDIP(wxSize(96, 96));
    const int lineCount = multiline && ylen <= 0
                            ? wxMax(2, wxMin(GetNumberOfLines(), 10))
                            : 1;
    const MUXC::Control control = m_winui ? m_winui->control() : nullptr;

    int chromeHeight = fallbackChrome;
    int chromeWidth = fallbackChrome;
    if ( control )
    {
        // Padding and border are the only non-text dimensions needed here.
        // Query them from a retained projection instead of calling
        // wxWinUIControlHost::MeasureContent(): XAML property access may run
        // application code, and the host (and this wx object) may consequently
        // cease to exist before that helper returns.
        try
        {
            const MUX::Thickness padding = control.Padding();
            const MUX::Thickness border = control.BorderThickness();
            const int measuredChromeWidth = wxRound(
                std::ceil(padding.Left + padding.Right +
                          border.Left + border.Right) *
                dipScale.x / 96.0);
            const int measuredChromeHeight = wxRound(
                std::ceil(padding.Top + padding.Bottom +
                          border.Top + border.Bottom) *
                dipScale.y / 96.0);
            if ( measuredChromeWidth > 0 )
                chromeWidth = measuredChromeWidth;
            if ( measuredChromeHeight > 0 )
                chromeHeight = measuredChromeHeight;
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }

    int width = wxMax(0, xlen) + wxMax(minimumChrome, chromeWidth);
    int height = charHeight + chromeHeight;
    if ( !multiline )
    {
        // The RichEditBox template has a 32 DIP natural minimum on its inner
        // border. Sizing the peer below it clips the text viewport and can make
        // its internal vertical scrollbar appear in a single-line control.
        height = wxMax(height, FromDIP(32));
    }
    if ( multiline )
    {
        // Match native wxMSW: an implicit multiline height tracks its current
        // physical line count, with a useful two-line minimum and ten-line
        // cap. Explicit ylen remains authoritative below.
        if ( ylen <= 0 )
        {
            height = charHeight * lineCount + chromeHeight;
            if ( hasHorizontalScroll )
                height += horizontalScrollHeight;
        }

        if ( hasVerticalScroll )
            width += verticalScrollWidth;
    }

    if ( ylen > height )
        height = ylen;

    return wxSize(width, height);
}

void wxTextCtrl::ApplyValueToPeer()
{
    if ( !m_winui )
        return;

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const std::uint64_t contentGeneration =
        impl->textContentGeneration;
    const MUXC::TextBox textBox = impl->textBox;
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const MUXC::PasswordBox passwordBox = impl->passwordBox;
    const bool securePasswordPeer = impl->securePasswordPeer;
    const wxString value = m_value;
    const wxString peerTarget = securePasswordPeer
                                  ? wxWinUIMaskPassword(value)
                                  : value;
    const long selectionStart = m_selectionStart;
    const long selectionEnd = m_selectionEnd;
    const long insertionPoint = m_insertionPoint;
    const wxTextAttr defaultStyle = m_defaultStyle;
    const wxWinUITextPositionMap map =
        wxWinUIMakeTextPositionMap(*this, value);
    long logicalStart = 0;
    long logicalEnd = 0;
    map.PublicRangeToLogical(
        wxMin(selectionStart, selectionEnd),
        wxMax(selectionStart, selectionEnd),
        &logicalStart,
        &logicalEnd);
    const wxWeakRef<wxWindow> self(this);
    const auto getExactOwner =
        [self, impl, callbackState, callbackGeneration,
         textBox, richEditBox, passwordBox]() -> wxTextCtrl *
        {
            if ( callbackState->GetGeneration() != callbackGeneration )
                return nullptr;
            wxTextCtrl * const owner =
                wxDynamicCast(self.get(), wxTextCtrl);
            return owner && owner->m_winui &&
                           owner->m_winui.get() == impl &&
                           owner->m_winui->callbackState == callbackState &&
                           owner->m_winui->textBox == textBox &&
                           owner->m_winui->richEditBox == richEditBox &&
                           owner->m_winui->passwordBox == passwordBox
                     ? owner
                     : nullptr;
        };
    const auto getLiveOwner = [&]() -> wxTextCtrl *
    {
        wxTextCtrl * const owner = getExactOwner();
        return owner &&
                       owner->m_winui->textContentGeneration ==
                           contentGeneration &&
                       owner->m_value == value &&
                       owner->m_selectionStart == selectionStart &&
                       owner->m_selectionEnd == selectionEnd &&
                       owner->m_insertionPoint == insertionPoint
                 ? owner
                 : nullptr;
    };
    const bool wasUpdatingPeer = m_updatingPeer;
    m_updatingPeer = true;
    wxScopeGuard updatingGuard = wxMakeGuard(
        [getExactOwner, wasUpdatingPeer]()
    {
        if ( wxTextCtrl * const owner = getExactOwner() )
            owner->m_updatingPeer = wasUpdatingPeer;
    });
    wxUnusedVar(updatingGuard);

    bool setTextAttempted = false;
    bool setTextCompleted = false;
    bool restoreRichReadOnly = false;
    wxScopeGuard readOnlyGuard = wxMakeGuard(
        [getExactOwner, richEditBox, &restoreRichReadOnly]()
        {
            if ( !restoreRichReadOnly || !richEditBox )
                return;

            if ( wxTextCtrl * const owner = getExactOwner() )
            {
                try
                {
                    richEditBox.IsReadOnly(!owner->m_editable);
                }
                catch ( const winrt::hresult_error& )
                {
                }
            }
        });
    wxUnusedVar(readOnlyGuard);
    long insertedStart = wxNOT_FOUND;
    long insertedEnd = wxNOT_FOUND;
    try
    {
        wxString peerValue;
        if ( passwordBox )
            peerValue = wxWinUIFromHString(passwordBox.Password());
        else if ( richEditBox )
            peerValue = wxWinUIGetRichPlainText(richEditBox);
        else if ( textBox )
            peerValue = wxWinUIFromHString(textBox.Text());
        peerValue = wxWinUITextPositionMap::NormalizeNewlines(peerValue);
        if ( !getLiveOwner() )
            return;

        if ( peerValue != peerTarget )
        {
            impl->pendingPeerValue = peerTarget;
            impl->hasPendingPeerValue = true;

            setTextAttempted = true;
            if ( passwordBox )
            {
                passwordBox.Password(wxWinUIToHString(value));
            }
            else if ( richEditBox )
            {
                // RichEdit's TOM correctly rejects range mutation while the
                // peer is read-only. wxTE_READONLY, however, only rejects
                // user edits: SetValue()/ChangeValue() must remain valid just
                // as they are for the native MSW edit control. Temporarily
                // unlock this exact retained peer for the programmatic write
                // and restore the current wx editable state before returning.
                if ( richEditBox.IsReadOnly() )
                {
                    if ( !getLiveOwner() )
                        return;
                    richEditBox.IsReadOnly(false);
                    restoreRichReadOnly = true;
                    if ( !getLiveOwner() )
                        return;
                }

                std::size_t prefix = 0;
                std::size_t suffix = 0;
                wxWinUIFindSafeCommonTextEdges(
                    peerValue, peerTarget, &prefix, &suffix);

                const std::size_t oldEnd =
                    peerValue.length() - suffix;
                const std::size_t newEnd = peerTarget.length() - suffix;
                const WUT::ITextDocument document =
                    richEditBox.Document();
                if ( !document || !getLiveOwner() )
                    return;
                const WUT::ITextRange range = document.GetRange(
                    static_cast<int32_t>(prefix),
                    static_cast<int32_t>(oldEnd));
                if ( !range || !getLiveOwner() )
                    return;
                range.SetText(
                    WUT::TextSetOptions::None,
                    wxWinUIToHString(
                        peerTarget.Mid(prefix, newEnd - prefix)));
                if ( !getLiveOwner() )
                    return;
                if ( restoreRichReadOnly )
                {
                    wxTextCtrl * const owner = getLiveOwner();
                    if ( !owner )
                        return;
                    richEditBox.IsReadOnly(!owner->m_editable);
                    restoreRichReadOnly = false;
                    if ( !getLiveOwner() )
                        return;
                }
                insertedStart = static_cast<long>(prefix);
                insertedEnd = static_cast<long>(newEnd);
            }
            else if ( textBox )
            {
                textBox.Text(wxWinUIToHString(value));
            }
            setTextCompleted = true;
            if ( !getLiveOwner() )
                return;

            if ( richEditBox && !securePasswordPeer &&
                 insertedStart != wxNOT_FOUND &&
                 !defaultStyle.IsDefault() )
            {
                const WUT::ITextDocument document =
                    richEditBox.Document();
                if ( !document || !getLiveOwner() )
                    return;
                const WUT::ITextRange inserted = document.GetRange(
                    static_cast<int32_t>(insertedStart),
                    static_cast<int32_t>(insertedEnd));
                if ( !inserted || !getLiveOwner() )
                    return;
                wxWinUIApplyTextStyle(inserted, defaultStyle);
                if ( !getLiveOwner() )
                    return;
            }
        }

        // PasswordBox deliberately has no selection API.
        if ( textBox )
        {
            textBox.Select(
                static_cast<int32_t>(logicalStart),
                static_cast<int32_t>(logicalEnd - logicalStart));
            if ( !getLiveOwner() )
                return;
        }
        else if ( richEditBox )
        {
            const WUT::ITextDocument document = richEditBox.Document();
            if ( !document || !getLiveOwner() )
                return;
            const WUT::ITextSelection selection = document.Selection();
            if ( !selection || !getLiveOwner() )
                return;
            selection.SetRange(
                static_cast<int32_t>(logicalStart),
                static_cast<int32_t>(logicalEnd));
            if ( !getLiveOwner() )
                return;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        if ( setTextAttempted && !setTextCompleted )
        {
            if ( wxTextCtrl * const live = getLiveOwner() )
            {
                wxUnusedVar(live);
                impl->hasPendingPeerValue = false;
                impl->pendingPeerValue.clear();
            }
        }
        wxWinUILogException("WinUI TextBox value", e);
        return;
    }

    if ( wxTextCtrl * const live = getLiveOwner() )
    {
        if ( live->m_winui->richEditBox &&
             (live->m_winui->autoUrlContentGeneration !=
                  live->m_winui->textContentGeneration ||
              (!live->HasFlag(wxTE_AUTO_URL) &&
               !live->m_winui->autoUrlRanges.empty())) )
        {
            live->UpdateAutoUrlRanges();
        }
    }
}

void wxTextCtrl::ApplyVerticalScrollBarPolicy(unsigned retriesRemaining)
{
    if ( !m_winui || !IsMultiLine() )
    {
        return;
    }

    if ( m_winui->richEditBox )
    {
        wxWinUITextCtrlImpl * const impl = m_winui.get();
        const std::shared_ptr<wxWinUITextCallbackState> callbackState =
            impl->callbackState;
        const std::uint64_t generation = callbackState->GetGeneration();
        const MUXC::RichEditBox richEditBox = impl->richEditBox;
        const auto getLiveOwner =
            [callbackState, generation, impl,
             richEditBox]() -> wxTextCtrl *
            {
                if ( callbackState->GetGeneration() != generation )
                    return nullptr;
                wxTextCtrl * const owner =
                    callbackState->GetOwner<wxTextCtrl>();
                return owner && owner->m_winui &&
                               owner->m_winui.get() == impl &&
                               owner->m_winui->callbackState == callbackState &&
                               owner->m_winui->richEditBox == richEditBox &&
                               owner->IsMultiLine()
                         ? owner
                         : nullptr;
            };

        try
        {
            if ( wxWinUITopLevelHost * const host =
                    wxWinUITopLevelHost::FindSlotOwner(this) )
            {
                host->FlushSync();
                if ( !getLiveOwner() )
                    return;
            }

            richEditBox.ApplyTemplate();
            if ( !getLiveOwner() )
                return;
            richEditBox.UpdateLayout();
            if ( !getLiveOwner() )
                return;

            MUXC::ScrollViewer scroll{ nullptr };
            if ( const auto control =
                    richEditBox.try_as<MUXC::IControlProtected>() )
            {
                scroll = control.GetTemplateChild(L"ContentElement").
                    try_as<MUXC::ScrollViewer>();
            }
            wxTextCtrl * const liveOwner = getLiveOwner();
            if ( !liveOwner )
                return;
            if ( scroll )
            {
                scroll.VerticalScrollBarVisibility(
                    liveOwner->HasFlag(wxTE_NO_VSCROLL)
                        ? MUXC::ScrollBarVisibility::Hidden
                        : MUXC::ScrollBarVisibility::Auto);
                return;
            }

            if ( retriesRemaining == 0 )
                return;
            const MUXD::DispatcherQueue queue =
                richEditBox.DispatcherQueue();
            if ( !queue || !getLiveOwner() )
                return;
            queue.TryEnqueue(
                MUXD::DispatcherQueuePriority::Low,
                [callbackState, generation, impl,
                 richEditBox, retriesRemaining]()
                {
                    if ( callbackState->GetGeneration() != generation )
                        return;
                    wxTextCtrl * const owner =
                        callbackState->GetOwner<wxTextCtrl>();
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui.get() != impl ||
                         owner->m_winui->callbackState != callbackState ||
                         owner->m_winui->richEditBox != richEditBox )
                    {
                        return;
                    }
                    owner->ApplyVerticalScrollBarPolicy(
                        retriesRemaining - 1);
                });
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI RichEditBox vertical scrollbar policy", e);
        }
        return;
    }

    if ( !m_winui->textBox )
        return;

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t generation =
        callbackState->GetGeneration();
    const MUXC::TextBox textBox = impl->textBox;
    const auto getLiveOwner =
        [callbackState, generation, impl,
         textBox]() -> wxTextCtrl *
        {
            if ( callbackState->GetGeneration() != generation )
                return nullptr;

            wxTextCtrl * const owner =
                callbackState->GetOwner<wxTextCtrl>();
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != impl ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->textBox != textBox ||
                  !owner->IsMultiLine() )
            {
                return nullptr;
            }
            return owner;
        };

    try
    {
        MUXC::ScrollViewer scroll =
            wxWinUIResolveTextScrollViewer(*this, textBox);
        wxTextCtrl * const liveOwner = getLiveOwner();
        if ( !liveOwner )
            return;

        if ( scroll )
        {
            // Hidden suppresses the chrome while preserving programmatic
            // scrolling. Disabled would change the TextBox's scroll
            // semantics, and the ScrollViewer attached property on TextBox
            // itself is deliberately never used because it can deadlock a
            // multiline peer in an island.
            scroll.VerticalScrollBarVisibility(
                liveOwner->HasFlag(wxTE_NO_VSCROLL)
                    ? MUXC::ScrollBarVisibility::Hidden
                    : MUXC::ScrollBarVisibility::Auto);
            return;
        }

        if ( retriesRemaining == 0 )
            return;

        const MUXD::DispatcherQueue queue = textBox.DispatcherQueue();
        if ( !queue || !getLiveOwner() )
            return;

        // Template realization may lag the first Loaded/layout pass. Queue
        // one generation-bound attempt at a time, with a hard budget supplied
        // by Create(). A replacement peer or destruction invalidates the
        // callback before it can touch the control.
        queue.TryEnqueue(
            MUXD::DispatcherQueuePriority::Low,
            [callbackState, generation, impl,
             textBox, retriesRemaining]()
            {
                if ( callbackState->GetGeneration() != generation )
                    return;

                wxTextCtrl * const owner =
                    callbackState->GetOwner<wxTextCtrl>();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != impl ||
                     owner->m_winui->callbackState != callbackState ||
                     owner->m_winui->textBox != textBox )
                {
                    return;
                }

                owner->ApplyVerticalScrollBarPolicy(
                    retriesRemaining - 1);
            });
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI TextBox vertical scrollbar policy", e);
    }
}

void wxTextCtrl::ReadSelectionFromPeer()
{
    if ( !m_winui ||
         (!m_winui->textBox && !m_winui->richEditBox) )
        return;

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const MUXC::TextBox textBox = impl->textBox;
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const std::uint64_t contentGeneration =
        impl->textContentGeneration;
    const wxString value = m_value;
    const wxWinUITextPositionMap map =
        wxWinUIMakeTextPositionMap(*this, value);
    const wxWeakRef<wxWindow> self(this);
    const auto getLiveOwner = [&]() -> wxTextCtrl *
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return nullptr;
        wxTextCtrl * const owner = wxDynamicCast(self.get(), wxTextCtrl);
        return owner && owner->m_winui &&
                       owner->m_winui.get() == impl &&
                       owner->m_winui->callbackState == callbackState &&
                       owner->m_winui->textBox == textBox &&
                       owner->m_winui->richEditBox == richEditBox &&
                       owner->m_winui->textContentGeneration ==
                           contentGeneration &&
                       owner->m_value == value
                 ? owner
                 : nullptr;
    };

    try
    {
        long start = 0;
        long end = 0;
        if ( textBox )
        {
            start = textBox.SelectionStart();
            if ( !getLiveOwner() )
                return;
            const int length = textBox.SelectionLength();
            if ( !getLiveOwner() || length < 0 )
                return;
            end = start + length;
        }
        else
        {
            const WUT::ITextDocument document = richEditBox.Document();
            if ( !document || !getLiveOwner() )
                return;
            const WUT::ITextSelection selection = document.Selection();
            if ( !selection || !getLiveOwner() )
                return;
            start = selection.StartPosition();
            if ( !getLiveOwner() )
                return;
            end = selection.EndPosition();
        }
        wxTextCtrl * const live = getLiveOwner();
        if ( !live )
            return;
        live->m_selectionStart = map.LogicalToPublic(start);
        live->m_selectionEnd = map.LogicalToPublic(end);
        live->m_insertionPoint = live->m_selectionStart;
    }
    catch ( const winrt::hresult_error& )
    {
    }
}

void wxTextCtrl::UpdateWinUIAppearance()
{
    if ( !m_winui || !m_winui->control() )
        return;

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const MUXC::Control control = impl->control();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t generation = callbackState->GetGeneration();
    const bool hasFont = m_hasFont;
    const wxFont font = hasFont ? GetFont() : wxNullFont;
    const bool hasForeground = UseForegroundColour();
    const wxColour foreground =
        hasForeground ? GetForegroundColour() : wxNullColour;
    const bool hasBackground = UseBackgroundColour();
    const wxColour background =
        hasBackground ? GetBackgroundColour() : wxNullColour;
    const wxWeakRef<wxWindow> self(this);
    const auto getLiveOwner = [&]() -> wxTextCtrl *
    {
        if ( callbackState->GetGeneration() != generation )
            return nullptr;
        wxTextCtrl * const owner = wxDynamicCast(self.get(), wxTextCtrl);
        return owner && owner->m_winui &&
                       owner->m_winui.get() == impl &&
                       owner->m_winui->callbackState == callbackState &&
                       owner->m_winui->control() == control
                 ? owner
                 : nullptr;
    };

    try
    {
        if ( hasFont )
        {
            wxWinUIApplyControlFont(control, font);
            if ( !getLiveOwner() )
                return;
        }
        else
        {
            control.ClearValue(MUXC::Control::FontFamilyProperty());
            if ( !getLiveOwner() )
                return;
            control.ClearValue(MUXC::Control::FontSizeProperty());
            if ( !getLiveOwner() )
                return;
            control.ClearValue(MUXC::Control::FontWeightProperty());
            if ( !getLiveOwner() )
                return;
            control.ClearValue(MUXC::Control::FontStyleProperty());
            if ( !getLiveOwner() )
                return;
        }

        if ( hasForeground )
        {
            control.Foreground(wxWinUIBrush(
                foreground.Red(), foreground.Green(),
                foreground.Blue(), foreground.Alpha()));
        }
        else
        {
            control.ClearValue(MUXC::Control::ForegroundProperty());
        }
        if ( !getLiveOwner() )
            return;

        if ( hasBackground )
        {
            control.Background(wxWinUIBrush(
                background.Red(), background.Green(),
                background.Blue(), background.Alpha()));
        }
        else
        {
            control.ClearValue(MUXC::Control::BackgroundProperty());
        }
        if ( !getLiveOwner() )
            return;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TextBox appearance", e);
    }

    if ( wxTextCtrl * const live = getLiveOwner() )
    {
        // Final boundary: rendering can dispatch application callbacks.
        live->m_winui->host.ForceRender();
    }
}

bool wxTextCtrl::SetFont(const wxFont& font)
{
    const wxWeakRef<wxWindow> self(this);
    const bool rc = wxControl::SetFont(font);
    wxTextCtrl *live = wxDynamicCast(self.get(), wxTextCtrl);
    if ( !live )
        return rc;
    live->InvalidateBestSize();
    live = wxDynamicCast(self.get(), wxTextCtrl);
    if ( live )
        live->UpdateWinUIAppearance();
    return rc;
}

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITextCtrlTestAccess::HasLocalFontOverrides(
    const wxTextCtrl& textCtrl)
{
    if ( !textCtrl.m_winui || !textCtrl.m_winui->control() )
        return false;

    try
    {
        const MUXC::Control control = textCtrl.m_winui->control();
        const auto unset = MUX::DependencyProperty::UnsetValue();
        return control.ReadLocalValue(
                   MUXC::Control::FontFamilyProperty()) != unset ||
               control.ReadLocalValue(
                   MUXC::Control::FontSizeProperty()) != unset ||
               control.ReadLocalValue(
                   MUXC::Control::FontWeightProperty()) != unset ||
               control.ReadLocalValue(
                   MUXC::Control::FontStyleProperty()) != unset;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TextBox font state query", e);
        return false;
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITextCtrlTestAccess::RichClipboardUsesAllFormats(
    const wxTextCtrl& textCtrl)
{
    if ( !textCtrl.m_winui || !textCtrl.m_winui->richEditBox )
        return false;

    try
    {
        return textCtrl.m_winui->richEditBox.ClipboardCopyFormat() ==
               MUXC::RichEditClipboardFormat::AllFormats;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}
#endif // WXWINUI_TEST_SUPPORT

bool wxTextCtrl::SetForegroundColour(const wxColour& colour)
{
    const wxWeakRef<wxWindow> self(this);
    const bool rc = wxControl::SetForegroundColour(colour);
    if ( wxTextCtrl * const live =
            wxDynamicCast(self.get(), wxTextCtrl) )
    {
        live->UpdateWinUIAppearance();
    }
    return rc;
}

bool wxTextCtrl::SetBackgroundColour(const wxColour& colour)
{
    const wxWeakRef<wxWindow> self(this);
    const bool rc = wxControl::SetBackgroundColour(colour);
    if ( wxTextCtrl * const live =
            wxDynamicCast(self.get(), wxTextCtrl) )
    {
        live->UpdateWinUIAppearance();
    }
    return rc;
}

#if wxUSE_TOOLTIPS
void wxTextCtrl::DoSetToolTipText(const wxString& tip)
{
    wxControl::DoSetToolTipText(tip);
}

void wxTextCtrl::DoSetToolTip(wxToolTip *tip)
{
    wxControl::DoSetToolTip(tip);
}
#endif // wxUSE_TOOLTIPS

void wxTextCtrl::SendTextEvent()
{
    if ( !m_textEventsEnabled )
        return;

    wxCommandEvent event(wxEVT_TEXT, GetId());
    event.SetEventObject(this);
    event.SetString(m_value);
    ProcessCommand(event);
}

void wxTextCtrl::SendMaxLengthEvent()
{
    wxCommandEvent event(wxEVT_TEXT_MAXLEN, GetId());
    event.SetEventObject(this);
    event.SetString(m_value);
    ProcessCommand(event);
}

void wxTextCtrl::PushSecurePasswordUndoSnapshot()
{
    PushSecurePasswordUndoSnapshot(
        m_insertionPoint, m_selectionStart, m_selectionEnd);
}

void wxTextCtrl::PushSecurePasswordUndoSnapshot(
    long insertionPoint,
    long selectionStart,
    long selectionEnd)
{
    if ( !m_winui || !m_winui->securePasswordPeer )
        return;

    wxWinUISecurePasswordSnapshot snapshot;
    snapshot.value = m_value;
    snapshot.insertionPoint = insertionPoint;
    snapshot.selectionStart = selectionStart;
    snapshot.selectionEnd = selectionEnd;

    if ( m_winui->securePasswordUndo.size() ==
         wxWinUIMaxSecurePasswordUndoSnapshots )
    {
        m_winui->securePasswordUndo.erase(
            m_winui->securePasswordUndo.begin());
    }
    m_winui->securePasswordUndo.push_back(std::move(snapshot));
    m_winui->securePasswordRedo.clear();
}

void wxTextCtrl::ProcessSecurePasswordTextChanging()
{
    if ( !m_winui || !m_winui->securePasswordPeer ||
         !m_winui->richEditBox || m_updatingPeer ||
         m_winui->securePasswordMasking )
    {
        return;
    }

    if ( m_winui->securePasswordFailClosed )
    {
        // If dispatcher shutdown prevented the terminal detach, keep the
        // still-live peer safe on every subsequent pre-render edit. Never
        // reconstruct the mask or resume the application transaction.
        wxWinUITextCtrlImpl * const failedImpl = m_winui.get();
        const MUXC::RichEditBox failedRichEditBox =
            failedImpl->richEditBox;
        const std::shared_ptr<wxWinUITextCallbackState> failedState =
            failedImpl->callbackState;
        const bool wasUpdatingPeer = m_updatingPeer;
        const bool wasMasking = failedImpl->securePasswordMasking;
        m_updatingPeer = true;
        failedImpl->securePasswordMasking = true;
        wxScopeGuard failedMaskingGuard = wxMakeGuard(
            [failedState, failedImpl, failedRichEditBox,
             wasUpdatingPeer, wasMasking]()
            {
                wxTextCtrl * const owner =
                    failedState->GetOwner<wxTextCtrl>();
                if ( owner && owner->m_winui &&
                     owner->m_winui.get() == failedImpl &&
                     owner->m_winui->callbackState == failedState &&
                     owner->m_winui->richEditBox == failedRichEditBox )
                {
                    owner->m_updatingPeer = wasUpdatingPeer;
                    owner->m_winui->securePasswordMasking = wasMasking;
                }
            });
        wxUnusedVar(failedMaskingGuard);

        const wxWinUIPasswordScrubResult scrub =
            wxWinUIScrubPasswordDocument(
                failedRichEditBox, wxString(), 0, 0, E_FAIL);
        if ( !scrub.isEmpty )
        {
            try
            {
                failedRichEditBox.Opacity(0.0);
                failedRichEditBox.IsHitTestVisible(false);
                failedRichEditBox.IsTabStop(false);
            }
            catch ( const winrt::hresult_error& )
            {
            }
        }
        return;
    }

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const wxString before = m_value;
    const long modelSelectionStart = m_selectionStart;
    const long modelSelectionEnd = m_selectionEnd;
    const long oldSelectionStart =
        impl->securePasswordEditSelectionArmed
            ? impl->securePasswordEditSelectionStart
            : modelSelectionStart;
    const long oldSelectionEnd =
        impl->securePasswordEditSelectionArmed
            ? impl->securePasswordEditSelectionEnd
            : modelSelectionEnd;
#ifdef WXWINUI_TEST_SUPPORT
    const HRESULT forcedPasswordScrubFailure =
        static_cast<HRESULT>(m_nextPasswordScrubFailure);
    m_nextPasswordScrubFailure = 0;
    const bool forcePartialPasswordScrub =
        m_nextPasswordScrubPartialWrite;
    m_nextPasswordScrubPartialWrite = false;
    const bool hasExplicitInsertionForTesting =
        impl->securePasswordExplicitInsertionArmedForTesting;
    wxString explicitInsertionForTesting =
        std::move(impl->securePasswordExplicitInsertionForTesting);
    impl->securePasswordExplicitInsertionArmedForTesting = false;
    impl->securePasswordExplicitInsertionForTesting.clear();
#else
    const HRESULT forcedPasswordScrubFailure = S_OK;
    const bool forcePartialPasswordScrub = false;
#endif
    impl->securePasswordEditSelectionArmed = false;
    ++impl->securePasswordEditSelectionGeneration;
    const wxWeakRef<wxWindow> self(this);
    const auto getExactOwner = [&]() -> wxTextCtrl *
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return nullptr;
        wxTextCtrl * const owner = wxDynamicCast(self.get(), wxTextCtrl);
        return owner && owner->m_winui &&
                       owner->m_winui.get() == impl &&
                       owner->m_winui->callbackState == callbackState &&
                       owner->m_winui->richEditBox == richEditBox &&
                       owner->m_winui->securePasswordPeer &&
                       owner->m_value == before &&
                       owner->m_selectionStart == modelSelectionStart &&
                       owner->m_selectionEnd == modelSelectionEnd
                 ? owner
                 : nullptr;
    };

    try
    {
        // Any path out of this synchronous pre-render callback must leave a
        // fully masked document, including decode failures, WinRT failures,
        // and re-entrant destruction from the test observation hook. If no
        // mask can be proved by reading it back, clear and retire the peer.
        wxScopeGuard plaintextScrubGuard = wxMakeGuard(
            [callbackState, callbackGeneration, impl, richEditBox, before,
             oldSelectionStart, oldSelectionEnd,
             forcedPasswordScrubFailure,
             forcePartialPasswordScrub]()
            {
                wxString scrubValue = before;
                long scrubStart = oldSelectionStart;
                long scrubEnd = oldSelectionEnd;
                bool wasUpdatingPeer = false;
                bool wasMasking = false;
                wxTextCtrl *current =
                    callbackState->GetOwner<wxTextCtrl>();
                const bool haveCurrent =
                    current && current->m_winui &&
                    current->m_winui.get() == impl &&
                    current->m_winui->callbackState == callbackState &&
                    current->m_winui->richEditBox == richEditBox &&
                    current->m_winui->securePasswordPeer;
                if ( haveCurrent )
                {
                    scrubValue = current->m_value;
                    scrubStart = current->m_selectionStart;
                    scrubEnd = current->m_selectionEnd;
                    wasUpdatingPeer = current->m_updatingPeer;
                    wasMasking = current->m_winui->securePasswordMasking;
                    current->m_updatingPeer = true;
                    current->m_winui->securePasswordMasking = true;
                }

                const wxWinUIPasswordScrubResult scrub =
                    wxWinUIScrubPasswordDocument(
                        richEditBox,
                        wxWinUIMaskPassword(scrubValue),
                        scrubStart,
                        scrubEnd,
                        forcedPasswordScrubFailure,
                        forcePartialPasswordScrub);

                current = callbackState->GetOwner<wxTextCtrl>();
                const bool stillCurrent =
                    haveCurrent && current && current->m_winui &&
                    current->m_winui.get() == impl &&
                    current->m_winui->callbackState == callbackState &&
                    current->m_winui->richEditBox == richEditBox &&
                    current->m_winui->securePasswordPeer;

                if ( scrub.isMasked && scrub.selectionRestored )
                {
                    if ( stillCurrent )
                    {
                        current->m_updatingPeer = wasUpdatingPeer;
                        current->m_winui->securePasswordMasking = wasMasking;
                    }
                    return;
                }

                // A clear document is already safe for the remainder of this
                // pre-render callback. If even clearing cannot be proved, hide
                // the retained projection as the last synchronous barrier;
                // the peer is then detached on the first safe dispatcher turn.
                if ( !scrub.isMasked && !scrub.isEmpty )
                {
                    try
                    {
                        richEditBox.Opacity(0.0);
                        richEditBox.IsHitTestVisible(false);
                        richEditBox.IsTabStop(false);
                    }
                    catch ( const winrt::hresult_error& )
                    {
                        // A disconnected XAML object cannot render. A live
                        // object is still retired by the generation-bound path
                        // below and exposes no automation text/value pattern.
                    }
                }

                if ( !stillCurrent )
                    return;

                const HRESULT failure = FAILED(scrub.failure)
                                            ? scrub.failure
                                            : E_UNEXPECTED;
                current->m_passwordPeerFailClosedHresult =
                    static_cast<long>(failure);
                current->m_passwordPeerWasEmptyOnFailClosed =
                    scrub.isEmpty;
                impl->securePasswordFailClosed = true;
                const std::uint64_t failClosedGeneration =
                    ++impl->securePasswordFailClosedGeneration;
                impl->securePasswordDispatchPending = false;
                impl->securePasswordDispatchText = false;
                impl->securePasswordDispatchMaxLength = false;
                impl->hasPendingPeerValue = false;
                impl->pendingPeerValue.clear();

                MUXD::DispatcherQueue queue{ nullptr };
                try
                {
                    queue = richEditBox.DispatcherQueue();
                }
                catch ( const winrt::hresult_error& )
                {
                }
                if ( !queue )
                {
                    try
                    {
                        queue = MUXD::DispatcherQueue::GetForCurrentThread();
                    }
                    catch ( const winrt::hresult_error& )
                    {
                    }
                }
                if ( queue )
                {
                    try
                    {
                        queue.TryEnqueue(
                            [callbackState, callbackGeneration, impl,
                             richEditBox, failClosedGeneration]()
                            {
                                if ( callbackState->GetGeneration() !=
                                         callbackGeneration )
                                {
                                    return;
                                }
                                wxTextCtrl * const owner =
                                    callbackState->GetOwner<wxTextCtrl>();
                                if ( !owner || !owner->m_winui ||
                                     owner->m_winui.get() != impl ||
                                     owner->m_winui->callbackState !=
                                         callbackState ||
                                     owner->m_winui->richEditBox !=
                                         richEditBox ||
                                     !owner->m_winui->securePasswordFailClosed ||
                                     owner->m_winui->
                                         securePasswordFailClosedGeneration !=
                                             failClosedGeneration )
                                {
                                    return;
                                }

                                std::unique_ptr<wxWinUITextCtrlImpl>
                                    failedPeer = std::move(owner->m_winui);
                                failedPeer->Close();
                            });
                    }
                    catch ( const winrt::hresult_error& )
                    {
                        // TextChanged calls DispatchSecurePasswordChange()
                        // outside this restricted callback and performs the
                        // same teardown.
                    }
                }

                current = callbackState->GetOwner<wxTextCtrl>();
                if ( current && current->m_winui &&
                     current->m_winui.get() == impl &&
                     current->m_winui->callbackState == callbackState &&
                     current->m_winui->richEditBox == richEditBox &&
                     current->m_winui->securePasswordFailClosed )
                {
                    current->m_updatingPeer = wasUpdatingPeer;
                    current->m_winui->securePasswordMasking = wasMasking;
                }
            });
        wxUnusedVar(plaintextScrubGuard);

        const WUT::ITextDocument document = richEditBox.Document();
        if ( !document || !getExactOwner() )
            return;
        const wxString rawPeerValue = wxWinUIGetRichPlainText(richEditBox);
        if ( !getExactOwner() )
            return;

        const WUT::ITextSelection selection = document.Selection();
        if ( !selection || !getExactOwner() )
            return;
        const long newSelectionStart = selection.StartPosition();
        if ( !getExactOwner() )
            return;
        const long newSelectionEnd = selection.EndPosition();
        wxTextCtrl *live = getExactOwner();
        if ( !live )
            return;

#ifdef WXWINUI_TEST_SUPPORT
        if ( live->m_nextPasswordTextChangingHook )
        {
            const auto hook =
                live->m_nextPasswordTextChangingHook;
            void * const hookContext =
                live->m_nextPasswordTextChangingContext;
            live->m_nextPasswordTextChangingHook = nullptr;
            live->m_nextPasswordTextChangingContext = nullptr;
            hook(live, hookContext);

            live = getExactOwner();
            if ( !live )
                return;
        }
#endif // WXWINUI_TEST_SUPPORT

        if ( FAILED(forcedPasswordScrubFailure) ||
             forcePartialPasswordScrub )
            return;

        if ( !m_editable )
            return;

        std::size_t replaceStart = 0;
        std::size_t replaceEnd = 0;
        wxString inserted;
        long mappedSelectionStart = 0;
        long mappedSelectionEnd = 0;
        const int deleteDirection =
            impl->securePasswordDeleteDirection;
        impl->securePasswordDeleteDirection = 0;
        if ( !wxWinUIDecodePasswordEdit(
                 before,
                 oldSelectionStart,
                 oldSelectionEnd,
                 rawPeerValue,
                 newSelectionStart,
                 newSelectionEnd,
                 deleteDirection,
                 &replaceStart,
                 &replaceEnd,
                 &inserted,
                 &mappedSelectionStart,
                 &mappedSelectionEnd) )
        {
            return;
        }

#ifdef WXWINUI_TEST_SUPPORT
        if ( hasExplicitInsertionForTesting )
        {
            // The deterministic test seam writes a same-length, non-secret
            // sentinel solely to force the real TextChanging transaction for
            // an otherwise indistinguishable mask-for-mask replacement.
            if ( inserted.length() != explicitInsertionForTesting.length() )
                return;
            inserted = std::move(explicitInsertionForTesting);
        }
#endif // WXWINUI_TEST_SUPPORT

        wxString acceptedInsertion =
            m_forceUpper ? inserted.Upper() : inserted;
        bool wasTruncated = false;
        if ( m_maxLength )
        {
            const std::size_t effectiveLimit = wxMax<std::size_t>(
                m_maxLength, before.length());
            const std::size_t baseLength =
                before.length() - (replaceEnd - replaceStart);
            const std::size_t available =
                effectiveLimit > baseLength
                    ? effectiveLimit - baseLength
                    : 0;
            if ( acceptedInsertion.length() > available )
            {
                acceptedInsertion = wxWinUIFitTextPrefix(
                    acceptedInsertion, available, false);
                wasTruncated = true;
            }
        }

        wxString acceptedValue = before.substr(0, replaceStart);
        acceptedValue += acceptedInsertion;
        acceptedValue += before.substr(replaceEnd);
        const bool valueChanged = acceptedValue != before;

        if ( valueChanged )
        {
            if ( !impl->securePasswordCompositionActive ||
                 !impl->securePasswordCompositionHasSnapshot )
            {
                live->PushSecurePasswordUndoSnapshot(
                    oldSelectionStart,
                    oldSelectionStart,
                    oldSelectionEnd);
                live = getExactOwner();
                if ( !live )
                    return;
                if ( impl->securePasswordCompositionActive )
                {
                    impl->securePasswordCompositionHasSnapshot = true;
                }
            }

            ++impl->textContentGeneration;
            impl->InvalidatePositionVisibilityRequest();
        }

        const std::size_t rawInsertionLength = inserted.length();
        const std::size_t acceptedInsertionLength =
            acceptedInsertion.length();
        const auto mapSelection =
            [replaceStart, rawInsertionLength,
             acceptedInsertionLength](long position) -> long
            {
                if ( position <= 0 )
                    return 0;
                const std::size_t p = static_cast<std::size_t>(position);
                if ( p <= replaceStart )
                    return static_cast<long>(p);
                if ( p >= replaceStart + rawInsertionLength )
                {
                    return static_cast<long>(
                        p - rawInsertionLength +
                        acceptedInsertionLength);
                }
                return static_cast<long>(
                    replaceStart +
                    wxMin(p - replaceStart,
                          acceptedInsertionLength));
            };
        const long acceptedLast = static_cast<long>(acceptedValue.length());
        const long acceptedSelectionStart = wxWinUIClampTextPos(
            mapSelection(mappedSelectionStart), acceptedLast);
        const long acceptedSelectionEnd = wxWinUIClampTextPos(
            mapSelection(mappedSelectionEnd), acceptedLast);

        live->m_value = acceptedValue;
        live->m_selectionStart = acceptedSelectionStart;
        live->m_selectionEnd = acceptedSelectionEnd;
        live->m_insertionPoint = acceptedSelectionStart;
        impl->securePasswordDispatchPending = true;
        impl->securePasswordDispatchText = valueChanged;
        impl->securePasswordDispatchMaxLength = wasTruncated;

        const bool wasUpdatingPeer = live->m_updatingPeer;
        impl->securePasswordMasking = true;
        live->m_updatingPeer = true;
        wxScopeGuard maskingGuard = wxMakeGuard(
            [callbackState, impl, richEditBox,
             wasUpdatingPeer]()
            {
                wxTextCtrl * const owner =
                    callbackState->GetOwner<wxTextCtrl>();
                if ( owner && owner->m_winui &&
                     owner->m_winui.get() == impl &&
                     owner->m_winui->callbackState == callbackState &&
                     owner->m_winui->richEditBox == richEditBox )
                {
                    owner->m_winui->securePasswordMasking = false;
                    owner->m_updatingPeer = wasUpdatingPeer;
                }
            });
        wxUnusedVar(maskingGuard);

        const WUT::ITextRange changedRange = document.GetRange(
            static_cast<int32_t>(replaceStart),
            static_cast<int32_t>(replaceStart + rawInsertionLength));
        if ( !changedRange )
            return;
        changedRange.SetText(
            WUT::TextSetOptions::None,
            wxWinUIToHString(wxWinUIMaskPassword(acceptedInsertion)));

        wxTextCtrl * const current =
            callbackState->GetOwner<wxTextCtrl>();
        if ( !current || !current->m_winui ||
             current->m_winui.get() != impl ||
             current->m_winui->callbackState != callbackState ||
             current->m_winui->richEditBox != richEditBox )
        {
            return;
        }
        document.Selection().SetRange(
            static_cast<int32_t>(acceptedSelectionStart),
            static_cast<int32_t>(acceptedSelectionEnd));
        if ( wxWinUIGetRichPlainText(richEditBox) !=
             wxWinUIMaskPassword(acceptedValue) )
        {
            return;
        }
        plaintextScrubGuard.Dismiss();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI secure password masking", e);
    }
}

void wxTextCtrl::DispatchSecurePasswordChange()
{
    if ( !m_winui || !m_winui->securePasswordPeer ||
         !m_winui->richEditBox )
    {
        return;
    }

    if ( m_winui->securePasswordFailClosed )
    {
        // TextChanged is outside the synchronous TextChanging restriction, so
        // it is safe to remove the unusable peer from the visual tree here.
        // Move first: Close() is allowed to re-enter or destroy the wx owner.
        std::unique_ptr<wxWinUITextCtrlImpl> failedPeer =
            std::move(m_winui);
        failedPeer->Close();
        return;
    }

    // Document rewrites made by the masking transaction can themselves raise
    // TextChanged synchronously. The outer content change performs the sole
    // application dispatch after the verified mask is in place.
    if ( m_updatingPeer || m_winui->securePasswordMasking )
        return;

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const wxWeakRef<wxWindow> self(this);
    const auto getLiveOwner = [&]() -> wxTextCtrl *
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return nullptr;
        wxTextCtrl * const owner = wxDynamicCast(self.get(), wxTextCtrl);
        return owner && owner->m_winui &&
                       owner->m_winui.get() == impl &&
                       owner->m_winui->callbackState == callbackState &&
                       owner->m_winui->richEditBox == richEditBox &&
                       owner->m_winui->securePasswordPeer
                 ? owner
                 : nullptr;
    };

    wxString peerValue;
    try
    {
        peerValue = wxWinUIGetRichPlainText(richEditBox);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI secure password change", e);
        return;
    }
    wxTextCtrl *live = getLiveOwner();
    if ( !live )
        return;

    if ( impl->hasPendingPeerValue &&
         impl->pendingPeerValue == peerValue )
    {
        impl->hasPendingPeerValue = false;
        impl->pendingPeerValue.clear();
    }
    else
    {
        impl->hasPendingPeerValue = false;
        impl->pendingPeerValue.clear();
    }

    if ( peerValue != wxWinUIMaskPassword(live->m_value) )
    {
        // A non-content mutation should never expose an unmasked document.
        // Restore it before any application callback can observe the peer.
        live->ApplyValueToPeer();
        live = getLiveOwner();
        if ( !live )
            return;
    }

    const bool dispatchPending = impl->securePasswordDispatchPending;
    const bool sendText = impl->securePasswordDispatchText;
    const bool sendMaxLength = impl->securePasswordDispatchMaxLength;
    impl->securePasswordDispatchPending = false;
    impl->securePasswordDispatchText = false;
    impl->securePasswordDispatchMaxLength = false;
    if ( sendText )
        live->m_modified = true;

    // The native undo stack is disabled before the first edit; verify that it
    // remains disabled before user code runs. Undo()/Redo() below use only the
    // separately protected model snapshots.
    try
    {
        const WUT::ITextDocument document = richEditBox.Document();
        if ( !document )
            winrt::throw_hresult(E_POINTER);
        document.UndoLimit(0);
        if ( document.UndoLimit() != 0 )
        {
            winrt::throw_hresult(E_UNEXPECTED);
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI secure password undo reset", e);
        live = getLiveOwner();
        if ( live )
        {
            live->m_passwordPeerFailClosedHresult =
                static_cast<long>(e.code());
            live->m_passwordPeerWasEmptyOnFailClosed = false;
            std::unique_ptr<wxWinUITextCtrlImpl> failedPeer =
                std::move(live->m_winui);
            failedPeer->Close();
        }
        return;
    }
    live = getLiveOwner();
    if ( !live )
        return;

    if ( !dispatchPending )
        return;

    if ( sendText )
    {
        live->SendTextEvent();
        live = getLiveOwner();
        if ( !live )
            return;
    }
    if ( sendMaxLength )
        live->SendMaxLengthEvent();
}

bool wxTextCtrl::RestoreSecurePasswordSnapshot(bool redo)
{
    if ( !m_winui || !m_winui->securePasswordPeer )
        return false;

    std::vector<wxWinUISecurePasswordSnapshot>& source =
        redo ? m_winui->securePasswordRedo
             : m_winui->securePasswordUndo;
    std::vector<wxWinUISecurePasswordSnapshot>& destination =
        redo ? m_winui->securePasswordUndo
             : m_winui->securePasswordRedo;
    if ( source.empty() )
        return false;

    wxWinUISecurePasswordSnapshot current;
    current.value = m_value;
    current.insertionPoint = m_insertionPoint;
    current.selectionStart = m_selectionStart;
    current.selectionEnd = m_selectionEnd;
    if ( destination.size() == wxWinUIMaxSecurePasswordUndoSnapshots )
        destination.erase(destination.begin());
    destination.push_back(std::move(current));

    wxWinUISecurePasswordSnapshot snapshot = std::move(source.back());
    source.pop_back();
    m_value = std::move(snapshot.value);
    m_insertionPoint = snapshot.insertionPoint;
    m_selectionStart = snapshot.selectionStart;
    m_selectionEnd = snapshot.selectionEnd;
    ++m_winui->textContentGeneration;
    m_winui->InvalidatePositionVisibilityRequest();

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t contentGeneration = impl->textContentGeneration;
    const wxString restoredValue = m_value;
    const wxWeakRef<wxWindow> self(this);
    ApplyValueToPeer();
    wxTextCtrl * const live = wxDynamicCast(self.get(), wxTextCtrl);
    if ( !live || !live->m_winui || live->m_winui.get() != impl ||
         live->m_winui->callbackState != callbackState ||
         live->m_winui->textContentGeneration != contentGeneration ||
         live->m_value != restoredValue )
    {
        return true;
    }
    live->m_modified = true;
    live->SendTextEvent();
    return true;
}

void wxTextCtrl::ProcessPasswordPeerChange(
    const wxString& proposed,
    bool isContentChange)
{
    if ( !m_winui ||
         (!m_winui->passwordBox && !m_winui->securePasswordPeer) )
        return;

    // PasswordBox has no IsReadOnly property. Reject a content edit by
    // restoring the model while keeping the peer enabled and focusable,
    // matching wxTE_READONLY semantics.
    if ( !m_editable )
    {
        ApplyValueToPeer();
        return;
    }

    wxWinUITextCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const MUXC::PasswordBox passwordBox = impl->passwordBox;
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const bool securePasswordPeer = impl->securePasswordPeer;
    const wxString previousValue = m_value;
    const wxString casedProposal = m_forceUpper ? proposed.Upper() : proposed;
    bool wasTruncated = false;
    wxString acceptedValue = casedProposal;
    if ( isContentChange )
    {
        const wxWinUIConstrainedPeerEdit constrainedEdit =
            wxWinUIConstrainPeerEdit(
                *this, previousValue, casedProposal, m_maxLength);
        acceptedValue = constrainedEdit.value;
        wasTruncated = constrainedEdit.wasTruncated;
    }
    const bool valueChanged = acceptedValue != previousValue;
    if ( valueChanged && isContentChange && securePasswordPeer )
        PushSecurePasswordUndoSnapshot();
    if ( isContentChange )
    {
        ++impl->textContentGeneration;
        impl->InvalidatePositionVisibilityRequest();
    }
    m_value = acceptedValue;
    m_insertionPoint = static_cast<long>(m_value.length());
    m_selectionStart = m_selectionEnd = m_insertionPoint;

    const std::uint64_t contentGeneration = impl->textContentGeneration;
    const wxWeakRef<wxWindow> self(this);
    const auto getExactOwner = [&]() -> wxTextCtrl *
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return nullptr;
        wxTextCtrl * const live = wxDynamicCast(self.get(), wxTextCtrl);
        return live && live->m_winui && live->m_winui.get() == impl &&
                       live->m_winui->callbackState == callbackState &&
                       live->m_winui->passwordBox == passwordBox &&
                       live->m_winui->richEditBox == richEditBox &&
                       live->m_winui->securePasswordPeer ==
                           securePasswordPeer &&
                       live->m_winui->textContentGeneration ==
                           contentGeneration &&
                       live->m_value == acceptedValue
                 ? live
                 : nullptr;
    };
    if ( securePasswordPeer ||
         wasTruncated || casedProposal != proposed )
    {
        // The secure RichEdit peer always stores a mask, never acceptedValue.
        // Legacy PasswordBox only needs correction for truncation/casing.
        ApplyValueToPeer();
    }

    // Peer synchronization can re-enter XAML and either wx event can destroy
    // the control. Revalidate after every such boundary and dispatch TEXT for
    // the accepted part before MAXLEN for the rejected suffix.
    wxTextCtrl *live = getExactOwner();
    if ( !live )
        return;

    if ( valueChanged )
    {
        live->m_modified = true;
        live->SendTextEvent();
        live = getExactOwner();
        if ( !live )
            return;
    }

    if ( wasTruncated )
        live->SendMaxLengthEvent();
}

void wxTextCtrl::ClampInsertionPoint()
{
    m_insertionPoint =
        wxWinUIClampTextPos(m_insertionPoint, GetLastPosition());
}

void wxTextCtrl::EnsurePositionVisible(long pos)
{
    if ( !m_winui )
        return;

    m_winui->richPositionVisibilityTrace = 0;
    m_winui->richPositionVisibilityPassCount = 0;

    if ( m_winui->richEditBox )
    {
        wxWinUITextCtrlImpl * const impl = m_winui.get();
        const std::shared_ptr<wxWinUITextCallbackState> callbackState =
            impl->callbackState;
        const std::uint64_t callbackGeneration =
            callbackState->GetGeneration();
        const std::uint64_t contentGeneration =
            impl->textContentGeneration;
        const MUXC::RichEditBox richEditBox = impl->richEditBox;
        const wxString value = m_value;
        const wxWeakRef<wxWindow> self(this);
        const auto getLiveOwner = [&]() -> wxTextCtrl *
        {
            if ( callbackState->GetGeneration() != callbackGeneration )
                return nullptr;
            wxTextCtrl * const live =
                wxDynamicCast(self.get(), wxTextCtrl);
            return live && live->m_winui &&
                           live->m_winui.get() == impl &&
                           live->m_winui->callbackState == callbackState &&
                           live->m_winui->richEditBox == richEditBox &&
                           live->m_winui->textContentGeneration ==
                               contentGeneration &&
                           live->m_value == value
                      ? live
                      : nullptr;
        };
        if ( impl->richPositionVisibilityNeedsWrapRestore )
        {
            impl->RestoreRichPositionVisibilityWrapping();
            if ( !getLiveOwner() )
                return;
            impl->host.ForceRender();
            if ( !getLiveOwner() )
                return;
        }
        const wxWinUITextPositionMap map =
            wxWinUIMakeTextPositionMap(*this, value);
        long logical = 0;
        if ( !map.PublicToLogical(pos, &logical) ||
             logical > (std::numeric_limits<int32_t>::max)() )
        {
            return;
        }

        const std::uint64_t requestGeneration =
            ++impl->positionVisibilityGeneration;
        impl->positionVisibilityContentGeneration = contentGeneration;
        impl->positionVisibilityLogical = logical;
        impl->positionVisibilityHasEdge = false;
        impl->positionVisibilityPending = false;
        impl->positionVisibilityScroll = nullptr;
        impl->positionVisibilityView = nullptr;
        impl->positionVisibilityRetryCount = 0;
        const auto queueRichCompletion = [&]() -> bool
        {
            wxTextCtrl *live = getLiveOwner();
            if ( !live )
                return false;

            const unsigned used =
                live->m_winui->richPositionVisibilityPassCount;
            if ( used >= wxWinUIRichVisibilityMaxPasses )
                return false;

            const MUXD::DispatcherQueue queue =
                richEditBox.DispatcherQueue();
            live = getLiveOwner();
            if ( !queue || !live )
            {
                if ( live )
                    live->m_winui->RestoreRichPositionVisibilityWrapping();
                return false;
            }

            live->m_winui->positionVisibilityPending = true;
            const unsigned remainingAfterFirst =
                wxWinUIRichVisibilityMaxPasses - used - 1;
            const bool accepted = queue.TryEnqueue(
                MUXD::DispatcherQueuePriority::Low,
                [callbackState,
                 requestGeneration,
                 remainingAfterFirst]()
                {
                    wxTextCtrl * const owner =
                        callbackState->GetOwner<wxTextCtrl>();
                    if ( !owner || !owner->m_winui )
                        return;

                    wxWinUITextCtrlImpl * const current =
                        owner->m_winui.get();
                    if ( current->callbackState != callbackState ||
                         !current->positionVisibilityPending ||
                         current->positionVisibilityGeneration !=
                            requestGeneration )
                    {
                        return;
                    }

                    owner->FinishPositionVisibility(
                        requestGeneration, remainingAfterFirst);
                });
            live = getLiveOwner();
            if ( accepted && live )
            {
                ++live->m_winui->positionVisibilityRetryCount;
                return true;
            }

            if ( live )
            {
                live->m_winui->positionVisibilityPending = false;
                live->m_winui->RestoreRichPositionVisibilityWrapping();
            }
            return false;
        };

        bool deferReflowCompletion = false;

        try
        {
            // ShowPosition() is allowed immediately after Show(), SetSize()
            // or a document replacement. Publish current slot geometry and
            // synchronously measure the exact hosted RichEditBox before TOM
            // is asked for an off-client range.
            impl->host.ForceRender();
            if ( !getLiveOwner() )
                return;

            MUXC::ScrollViewer scroll =
                wxWinUIResolveTextScrollViewer(*this, richEditBox);
            if ( !scroll || !getLiveOwner() )
                return;

            richEditBox.UpdateLayout();
            if ( !getLiveOwner() )
                return;
            scroll = wxWinUIGetCurrentTextScrollViewer(richEditBox);
            if ( !scroll || !getLiveOwner() )
                return;
            scroll.UpdateLayout();
            if ( wxWinUIGetCurrentTextScrollViewer(richEditBox) != scroll ||
                 !getLiveOwner() )
            {
                return;
            }

            if ( richEditBox.TextWrapping() == MUX::TextWrapping::Wrap )
            {
                if ( !getLiveOwner() )
                    return;
                const double viewportHeight =
                    scroll.ViewportHeight() > 0.0
                        ? scroll.ViewportHeight()
                        : scroll.ActualHeight();
                if ( !getLiveOwner() )
                    return;
                const double scrollableHeight = scroll.ScrollableHeight();
                if ( !getLiveOwner() )
                    return;

                winrt::Windows::Foundation::Point documentEnd{};
                const bool hasDocumentEnd = wxWinUIGetRichTextPoint(
                    richEditBox,
                    static_cast<long>(value.length()),
                    &documentEnd);
                if ( !getLiveOwner() )
                    return;

                // RichEditBox can publish the two halves of a wrapped layout
                // from different generations when a width change and a
                // document replacement occur in the same XAML turn. Either
                // TOM can own the new final line while TextBoxView retains an
                // old extent, or TextBoxView can shrink first while TOM still
                // owns the old narrow-layout final line.
                //
                // TOM ClientCoordinates are TextBoxView document coordinates:
                // the host reports its complete extent as the RichEdit client
                // rectangle and reports the real viewport separately. A zero
                // extent, an extent far below the final TOM line, or a
                // final TOM line more than a complete viewport beyond the
                // available extent is an impossible single-generation result,
                // not a second coordinate space. Re-applying the exact
                // wrapping policy invalidates that stale measure without
                // changing the document, selection or focus.
                const double lineHeight =
                    wxMax(1.0, richEditBox.FontSize());
                const bool missingExtent =
                    scrollableHeight <= 0.5 &&
                    (!hasDocumentEnd ||
                     documentEnd.Y + lineHeight > viewportHeight + 0.5);
                const bool oversizedExtent =
                    hasDocumentEnd && documentEnd.Y >= 0.0f &&
                    scrollableHeight >
                        documentEnd.Y + lineHeight + viewportHeight + 0.5;
                const bool undersizedExtent =
                    hasDocumentEnd && documentEnd.Y >= 0.0f &&
                    documentEnd.Y + lineHeight >
                        scrollableHeight + 2.0 * viewportHeight + 0.5;
                if ( logical > 0 && viewportHeight > 0.0 &&
                     (missingExtent || oversizedExtent || undersizedExtent) )
                {
                    wxTextCtrl *live = getLiveOwner();
                    if ( !live )
                        return;
                    live->m_winui->richPositionVisibilityTrace |=
                        wxWinUIRichVisibilityReflow;

                    bool wrappingNeedsRestore = true;
                    const auto wrappingGuard = wxMakeGuard([&]()
                    {
                        if ( !wrappingNeedsRestore )
                            return;

                        try
                        {
                            richEditBox.TextWrapping(
                                MUX::TextWrapping::Wrap);
                        }
                        catch ( const winrt::hresult_error& )
                        {
                            wxLogTrace(
                                "winui",
                                "WinUI RichEditBox wrapping restoration "
                                "failed");
                        }
                    });
                    richEditBox.TextWrapping(MUX::TextWrapping::NoWrap);
                    if ( !getLiveOwner() )
                        return;
                    // A NoWrap->Wrap pair in one dispatcher turn is coalesced
                    // by the native TextBoxView and can leave its resize extent
                    // permanently at zero under fractional/raster scaling.
                    // Commit NoWrap through the host now; the generation-bound
                    // continuation restores Wrap on the next turn and proves
                    // the resulting extent on a later turn.
                    impl->host.ForceRender();
                    if ( !getLiveOwner() )
                        return;
                    richEditBox.UpdateLayout();
                    if ( !getLiveOwner() )
                        return;
                    scroll = wxWinUIGetCurrentTextScrollViewer(richEditBox);
                    if ( !scroll || !getLiveOwner() )
                        return;
                    scroll.UpdateLayout();
                    if ( wxWinUIGetCurrentTextScrollViewer(richEditBox) !=
                             scroll ||
                         !getLiveOwner() )
                    {
                        return;
                    }

                    live = getLiveOwner();
                    if ( !live )
                        return;
                    live->m_winui->
                        richPositionVisibilityNeedsWrapRestore = true;
                    wrappingNeedsRestore = false;
                    wrappingGuard.Dismiss();
                    deferReflowCompletion = true;
                }
            }

            if ( deferReflowCompletion )
            {
                if ( !queueRichCompletion() )
                {
                    if ( wxTextCtrl * const live = getLiveOwner() )
                    {
                        live->m_winui->
                            RestoreRichPositionVisibilityWrapping();
                        live->m_winui->host.ForceRender();
                    }
                }
                return;
            }

            const auto makeDocumentTarget = [&](const auto& point)
            {
                return winrt::Windows::Foundation::Rect
                {
                    point.X,
                    point.Y,
                    1.0f,
                    static_cast<float>(wxMax(1.0, richEditBox.FontSize()))
                };
            };
            const auto getTargetViewportState =
                [&](winrt::Windows::Foundation::Point *point)
                    -> wxWinUITextViewportState
                {
                    if ( !point ||
                         !wxWinUIGetRichTextPoint(
                             richEditBox, logical, point) ||
                         !getLiveOwner() )
                    {
                        return wxWinUITextViewportState::Unavailable;
                    }

                    scroll =
                        wxWinUIGetCurrentTextScrollViewer(richEditBox);
                    if ( !scroll || !getLiveOwner() )
                        return wxWinUITextViewportState::Unavailable;
                    const MUX::UIElement view =
                        scroll.Content().try_as<MUX::UIElement>();
                    if ( !view || !getLiveOwner() ||
                         !wxWinUIIsCurrentTextScrollView(
                             richEditBox, scroll, view) )
                    {
                        return wxWinUITextViewportState::Unavailable;
                    }

                    const wxWinUITextViewportState state =
                        wxWinUIProjectTextViewTargetToViewport(
                            richEditBox,
                            scroll,
                            view,
                            makeDocumentTarget(*point));
                    return getLiveOwner()
                               ? state
                               : wxWinUITextViewportState::Unavailable;
                };
            const auto requestExactDocumentPoint =
                [&](const winrt::Windows::Foundation::Point& point)
                {
                    scroll =
                        wxWinUIGetCurrentTextScrollViewer(richEditBox);
                    if ( !scroll || !getLiveOwner() )
                        return false;
                    const MUX::UIElement view =
                        scroll.Content().try_as<MUX::UIElement>();
                    if ( !view || !getLiveOwner() ||
                         !wxWinUIIsCurrentTextScrollView(
                             richEditBox, scroll, view) )
                    {
                        return false;
                    }

                    const auto target = makeDocumentTarget(point);
                    if ( wxWinUIChangeTextViewToTarget(
                             richEditBox, scroll, view, target) )
                    {
                        return getLiveOwner() != nullptr;
                    }
                    if ( !getLiveOwner() )
                        return false;

                    // A stale zero extent cannot accept ChangeView(), but the
                    // exact ContentElement target can still invalidate and
                    // realize that extent. This is a XAML transaction only;
                    // do not enqueue TOM ScrollIntoView against stale layout.
                    return wxWinUIStartTextViewBringIntoView(
                               richEditBox, scroll, view, target) &&
                           getLiveOwner();
                };
            const auto requestCoarsePosition = [&]()
            {
                scroll = wxWinUIGetCurrentTextScrollViewer(richEditBox);
                if ( !scroll || !getLiveOwner() )
                    return false;

                const double fraction = value.empty()
                    ? 0.0
                    : static_cast<double>(logical) /
                          static_cast<double>(value.length());
                const double targetHorizontal =
                    logical == 0
                        ? 0.0
                        : logical == static_cast<long>(value.length())
                            ? scroll.ScrollableWidth()
                            : scroll.ScrollableWidth() * fraction;
                if ( !getLiveOwner() )
                    return false;
                const double targetVertical =
                    logical == 0
                        ? 0.0
                        : logical == static_cast<long>(value.length())
                            ? scroll.ScrollableHeight()
                            : scroll.ScrollableHeight() * fraction;
                if ( !getLiveOwner() )
                    return false;

                const auto horizontal =
                    winrt::box_value(targetHorizontal).
                        as<winrt::Windows::Foundation::IReference<double>>();
                const auto vertical =
                    winrt::box_value(targetVertical).
                        as<winrt::Windows::Foundation::IReference<double>>();
                const bool accepted =
                    scroll.ChangeView(horizontal, vertical, nullptr, true);
                return accepted && getLiveOwner();
            };
            // GetPoint(AllowOffClient) is the authoritative document-space
            // geometry. If it is available, make its ContentElement request
            // the final scroll transaction. When it returns S_FALSE, use a
            // coarse XAML move and force layout to realize the paragraph.
            // Native TOM ScrollIntoView is deliberately not mixed into this
            // transaction: it can retain a request against the stale extent
            // and restore the old maximum after the exact XAML move.
            //
            // Re-resolve the actual ContentElement and its transform on every
            // bounded pass; neither an offset nor a visual identity is
            // retained across a layout boundary.
            bool deferCompletion = false;
            bool previousRequestAccepted = false;
            for ( unsigned pass = 0;
                  pass != wxWinUIRichVisibilityMaxPasses;
                  ++pass )
            {
                winrt::Windows::Foundation::Point point{};
                wxTextCtrl *live = getLiveOwner();
                if ( !live )
                    return;
#ifdef WXWINUI_TEST_SUPPORT
                const bool forceUnavailable =
                    live->m_winui->
                        forceNextRichPositionUnavailableForTesting;
                live->m_winui->
                    forceNextRichPositionUnavailableForTesting = false;
#else
                const bool forceUnavailable = false;
#endif
                const wxWinUITextViewportState targetState = forceUnavailable
                    ? wxWinUITextViewportState::Unavailable
                    : getTargetViewportState(&point);
                live = getLiveOwner();
                if ( !live )
                    return;
                ++live->m_winui->richPositionVisibilityPassCount;
                if ( targetState == wxWinUITextViewportState::Visible )
                {
                    live->m_winui->richPositionVisibilityTrace |=
                        wxWinUIRichVisibilityVisible;
                    break;
                }
                if ( previousRequestAccepted )
                {
                    // ChangeView()/StartBringIntoView acceptance is not layout
                    // completion. If the next same-stack observation is still
                    // outside, let one bounded Low-priority phase publish the
                    // new RichEdit extent instead of replaying the request in
                    // a tight synchronous loop.
                    deferCompletion = true;
                    break;
                }

                bool requestAccepted = false;
                if ( targetState != wxWinUITextViewportState::Unavailable )
                {
                    live->m_winui->richPositionVisibilityTrace |=
                        wxWinUIRichVisibilityExactRequested;
                    requestAccepted = requestExactDocumentPoint(point);
                    if ( requestAccepted )
                    {
                        live = getLiveOwner();
                        if ( !live )
                            return;
                        live->m_winui->richPositionVisibilityTrace |=
                            wxWinUIRichVisibilityExactAccepted;
                    }
                }
                else
                {
                    live->m_winui->richPositionVisibilityTrace |=
                        wxWinUIRichVisibilityUnavailable |
                        wxWinUIRichVisibilityCoarseRequested;
                    requestAccepted = requestCoarsePosition();
                    if ( requestAccepted )
                    {
                        live = getLiveOwner();
                        if ( !live )
                            return;
                        live->m_winui->richPositionVisibilityTrace |=
                            wxWinUIRichVisibilityCoarseAccepted;
                    }
                }
                if ( !getLiveOwner() )
                    return;

                impl->host.ForceRender();
                if ( !getLiveOwner() )
                    return;
                richEditBox.UpdateLayout();
                if ( !getLiveOwner() )
                    return;
                scroll = wxWinUIGetCurrentTextScrollViewer(richEditBox);
                if ( !scroll || !getLiveOwner() )
                    return;
                scroll.UpdateLayout();
                if ( wxWinUIGetCurrentTextScrollViewer(richEditBox) != scroll ||
                     !getLiveOwner() )
                {
                    return;
                }

                if ( !requestAccepted )
                {
                    deferCompletion = true;
                    break;
                }
                previousRequestAccepted = true;
            }

            wxTextCtrl * const live = getLiveOwner();
            if ( live && deferCompletion &&
                 (live->m_winui->richPositionVisibilityTrace &
                      wxWinUIRichVisibilityVisible) == 0 )
            {
                queueRichCompletion();
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            if ( wxTextCtrl * const live = getLiveOwner() )
                live->m_winui->RestoreRichPositionVisibilityWrapping();
            wxWinUILogException(
                "WinUI RichEditBox scroll position", e);
        }
        return;
    }

    if ( !m_winui->textBox )
        return;

    const MUXC::TextBox textBox = m_winui->textBox;
    wxWinUITextCtrlImpl * const exactImpl = m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        m_winui->callbackState;
    const wxString value = m_value;
    const wxWeakRef<wxWindow> self(this);

    const wxWinUITextPositionMap map =
        wxWinUIMakeTextPositionMap(*this, value);
    long logical = 0;
    if ( !map.PublicToLogical(pos, &logical) )
        return;
    if ( logical >
            static_cast<long>((std::numeric_limits<int32_t>::max)()) )
    {
        return;
    }

    const std::uint64_t requestGeneration =
        ++m_winui->positionVisibilityGeneration;
    const std::uint64_t contentGeneration =
        m_winui->textContentGeneration;
    m_winui->positionVisibilityContentGeneration =
        contentGeneration;
    m_winui->positionVisibilityLogical = logical;
    m_winui->positionVisibilityHasEdge = false;
    m_winui->positionVisibilityPending = true;
    m_winui->positionVisibilityScroll = nullptr;
    m_winui->positionVisibilityView = nullptr;
    m_winui->positionVisibilityRetryCount = 0;
    const auto getExactOwner =
        [&self, exactImpl, &callbackState, &textBox]() -> wxTextCtrl *
        {
            wxTextCtrl * const live =
                wxDynamicCast(self.get(), wxTextCtrl);
            return live && live->m_winui &&
                           live->m_winui.get() == exactImpl &&
                           live->m_winui->callbackState == callbackState &&
                           live->m_winui->textBox == textBox
                     ? live
                     : nullptr;
        };
    const auto getLiveOwner =
        [&getExactOwner,
         requestGeneration,
         contentGeneration,
         &value]() -> wxTextCtrl *
        {
            wxTextCtrl * const live = getExactOwner();
            if ( !live ||
                 live->m_winui->positionVisibilityGeneration !=
                    requestGeneration ||
                 live->m_winui->textContentGeneration !=
                    contentGeneration ||
                 live->m_value != value )
            {
                return nullptr;
            }
            return live;
        };
    const auto completeRequest = [&]()
    {
        if ( wxTextCtrl * const live = getLiveOwner() )
        {
            live->m_winui->positionVisibilityPending = false;
            live->m_winui->positionVisibilityHasEdge = false;
            live->m_winui->positionVisibilityScroll = nullptr;
            live->m_winui->positionVisibilityView = nullptr;
        }
    };
    const auto queueFirstLowAttempt = [&]() -> bool
    {
        wxTextCtrl *live = getLiveOwner();
        if ( !live )
            return false;

        const MUXD::DispatcherQueue queue = textBox.DispatcherQueue();
        live = getLiveOwner();
        if ( !queue || !live )
        {
            completeRequest();
            return false;
        }

        const bool accepted = queue.TryEnqueue(
            MUXD::DispatcherQueuePriority::Low,
            [callbackState, requestGeneration]()
            {
                wxTextCtrl * const owner =
                    callbackState->GetOwner<wxTextCtrl>();
                if ( !owner || !owner->m_winui )
                    return;

                wxWinUITextCtrlImpl * const impl =
                    owner->m_winui.get();
                if ( impl->callbackState != callbackState ||
                     !impl->positionVisibilityPending ||
                     impl->positionVisibilityGeneration !=
                        requestGeneration )
                {
                    return;
                }

                // The callback itself is the first of at most three Low
                // dispatcher turns; two more remain after this attempt.
                owner->FinishPositionVisibility(requestGeneration, 2);
            });
        live = getLiveOwner();
        if ( accepted && live )
        {
            ++live->m_winui->positionVisibilityRetryCount;
            return true;
        }

        completeRequest();
        return false;
    };

    bool forceDeferredCompletion = false;
#ifdef WXWINUI_TEST_SUPPORT
    if ( wxTextCtrl * const live = getLiveOwner() )
    {
        if ( live->m_winui->forceNextPositionVisibilityRetryForTesting )
        {
            live->m_winui->forceNextPositionVisibilityRetryForTesting = false;
            forceDeferredCompletion = true;
        }
    }
#endif // WXWINUI_TEST_SUPPORT

    try
    {
        MUXC::ScrollViewer scroll =
            wxWinUIResolveTextScrollViewer(*this, textBox);
        if ( !getLiveOwner() )
            return;
        if ( !scroll )
        {
            queueFirstLowAttempt();
            return;
        }

        // ShowPosition() may immediately follow a value or size mutation.
        // Realize the new wrapping and extents before reading the character
        // edge and ScrollViewer offsets used to compute the target view.
        textBox.UpdateLayout();
        if ( !getLiveOwner() )
            return;
        scroll = wxWinUIGetCurrentTextScrollViewer(textBox);
        if ( !scroll || !getLiveOwner() )
        {
            queueFirstLowAttempt();
            return;
        }
        scroll.UpdateLayout();
        if ( !getLiveOwner() )
            return;
        if ( wxWinUIGetCurrentTextScrollViewer(textBox) != scroll ||
             !getLiveOwner() )
        {
            queueFirstLowAttempt();
            return;
        }

        wxWinUITextEdge edge;
        bool haveUsableEdge =
            wxWinUIGetTextEdge(
                textBox, value, logical, &edge) &&
            wxWinUIIsPlausibleWrappedTextEdge(textBox, edge);
        if ( !wxWinUIIsCurrentTextScrollView(textBox, scroll) ||
             !getLiveOwner() )
        {
            queueFirstLowAttempt();
            return;
        }
        bool forceTemporarySelection = false;
#ifdef WXWINUI_TEST_SUPPORT
        if ( wxTextCtrl * const live = getLiveOwner() )
        {
            if ( live->m_winui->forceTemporarySelectionForTesting )
            {
                live->m_winui->forceTemporarySelectionForTesting = false;
                forceTemporarySelection = true;
                haveUsableEdge = false;
            }
        }
#endif // WXWINUI_TEST_SUPPORT
        bool needsDeferredCompletion = forceDeferredCompletion;

        if ( !haveUsableEdge )
        {
            // GetRectFromCharacterIndex() reports RichEdit's finite negative
            // sentinel for a virtualized range. Selecting that range without
            // focus does not realize it, and focusing here would violate
            // ShowPosition()'s contract. Move to a bounded coarse viewport
            // estimate first; the exact edge query and BringIntoView below
            // then finish the positioning without changing focus or
            // selection. Endpoints map exactly to the corresponding scroll
            // extent, which is the important non-heuristic case.
            const double length =
                static_cast<double>(value.length());
            const double fraction =
                length > 0.0
                    ? wxMax(0.0,
                            wxMin(1.0,
                                  static_cast<double>(logical) / length))
                    : 0.0;
            winrt::Windows::Foundation::IReference<double> horizontal{
                nullptr
            };
            winrt::Windows::Foundation::IReference<double> vertical{
                nullptr
            };
            const double scrollableHeight = scroll.ScrollableHeight();
            if ( !getLiveOwner() )
                return;
            const double scrollableWidth = scroll.ScrollableWidth();
            if ( !getLiveOwner() )
                return;
            if ( scrollableHeight > 0.0 )
            {
                vertical =
                    winrt::box_value(
                        fraction * scrollableHeight).
                    as<winrt::Windows::Foundation::IReference<double>>();
            }
            else if ( scrollableWidth > 0.0 )
            {
                horizontal =
                    winrt::box_value(
                        fraction * scrollableWidth).
                    as<winrt::Windows::Foundation::IReference<double>>();
            }

            if ( horizontal || vertical )
            {
                needsDeferredCompletion =
                    scroll.ChangeView(
                        horizontal, vertical, nullptr, true);
                if ( !getLiveOwner() )
                    return;
                textBox.UpdateLayout();
                if ( !getLiveOwner() )
                    return;
                scroll = wxWinUIGetCurrentTextScrollViewer(textBox);
                if ( !scroll || !getLiveOwner() )
                {
                    queueFirstLowAttempt();
                    return;
                }
                scroll.UpdateLayout();
                if ( !getLiveOwner() )
                    return;
                if ( wxWinUIGetCurrentTextScrollViewer(textBox) != scroll ||
                     !getLiveOwner() )
                {
                    queueFirstLowAttempt();
                    return;
                }

                haveUsableEdge =
                    wxWinUIGetTextEdge(
                        textBox, value, logical, &edge) &&
                    wxWinUIIsPlausibleWrappedTextEdge(textBox, edge);
                if ( !wxWinUIIsCurrentTextScrollView(textBox, scroll) ||
                     !getLiveOwner() )
                {
                    queueFirstLowAttempt();
                    return;
                }
            }
        }

        if ( forceTemporarySelection )
            haveUsableEdge = false;

        bool extentNeedsWrappedReflow = false;
        if ( !haveUsableEdge && !forceTemporarySelection )
        {
            const double scrollableHeight = scroll.ScrollableHeight();
            if ( !getLiveOwner() )
                return;
            extentNeedsWrappedReflow = scrollableHeight <= 0.5;

            // An unavailable target edge alone is normal RichEdit
            // virtualization and must stay on the bounded coarse/retry path.
            // Reflow only when the public extent itself contradicts the final
            // document line. Query the last character directly: unlike the
            // exact end-caret helper this is O(1), including for a document
            // ending in a very long paragraph.
            if ( !extentNeedsWrappedReflow && !value.empty() &&
                 value.length() <= static_cast<std::size_t>(
                                       (std::numeric_limits<int32_t>::max)()) )
            {
                const auto finalRect = textBox.GetRectFromCharacterIndex(
                    static_cast<int32_t>(value.length() - 1), true);
                if ( !getLiveOwner() )
                    return;
                const double viewportHeight =
                    scroll.ViewportHeight() > 0.0
                        ? scroll.ViewportHeight()
                        : scroll.ActualHeight();
                if ( !wxWinUIIsCurrentTextScrollView(textBox, scroll) ||
                     !getLiveOwner() )
                {
                    queueFirstLowAttempt();
                    return;
                }

                const double lineHeight = wxMax<double>(
                    wxMax<double>(finalRect.Height, textBox.FontSize()), 1.0);
                if ( !getLiveOwner() )
                    return;
                extentNeedsWrappedReflow =
                    wxWinUIIsUsableTextRect(finalRect) &&
                    finalRect.Y >= 0.0f && viewportHeight > 0.0 &&
                    scrollableHeight >
                        finalRect.Y + lineHeight + viewportHeight + 0.5;
            }
        }

        if ( extentNeedsWrappedReflow &&
             textBox.TextWrapping() == MUX::TextWrapping::Wrap )
        {
            // A fixed-size TextBoxView can retain the one-line
            // EN_REQUESTRESIZE result, or a positive extent from the previous
            // narrower arrangement, when a width change and wrapped content
            // replacement happen in the same layout turn.
            //
            // Re-applying the exact wrapping policy makes WinUI run
            // OnTextWrappingChanged()/InvalidateView() and recompute its
            // natural size. Both writes are synchronous and the final public
            // property is unchanged; unlike focusing or moving the caret,
            // this does not alter any wx-observable input state.
            textBox.TextWrapping(MUX::TextWrapping::NoWrap);
            bool wrappingNeedsRestore = true;
            const auto wrappingGuard = wxMakeGuard([&]()
            {
                if ( !wrappingNeedsRestore )
                    return;

                try
                {
                    textBox.TextWrapping(MUX::TextWrapping::Wrap);
                }
                catch ( const winrt::hresult_error& )
                {
                    wxLogTrace(
                        "winui",
                        "WinUI TextBox wrapping restoration failed");
                }
            });
            if ( !getLiveOwner() )
                return;
            textBox.TextWrapping(MUX::TextWrapping::Wrap);
            wrappingNeedsRestore = false;
            wrappingGuard.Dismiss();
            if ( !getLiveOwner() )
                return;
            textBox.UpdateLayout();
            if ( !getLiveOwner() )
                return;
            scroll = wxWinUIGetCurrentTextScrollViewer(textBox);
            if ( !scroll || !getLiveOwner() )
            {
                queueFirstLowAttempt();
                return;
            }
            scroll.UpdateLayout();
            if ( !getLiveOwner() )
                return;
            haveUsableEdge =
                wxWinUIGetTextEdge(
                    textBox, value, logical, &edge) &&
                wxWinUIIsPlausibleWrappedTextEdge(textBox, edge);
            if ( !wxWinUIIsCurrentTextScrollView(textBox, scroll) ||
                 !getLiveOwner() )
            {
                queueFirstLowAttempt();
                return;
            }
        }

        if ( !haveUsableEdge )
        {
            // RichEdit virtualizes wrapped ranges and returns a negative
            // sentinel until the caret range has been realized. Move the
            // native selection only inside one synchronous, callback-silent
            // transaction, force layout, capture the now-real edge, then
            // restore the exact selection before requesting any scroll.
            wxTextCtrl *live = getLiveOwner();
            if ( !live )
                return;

            const int selectionStart = textBox.SelectionStart();
            if ( !getLiveOwner() )
                return;
            const int selectionLength = textBox.SelectionLength();
            live = getLiveOwner();
            if ( !live )
                return;

            const int temporarySelectionStart =
                value.empty()
                    ? 0
                    : static_cast<int>(
                          logical < static_cast<long>(value.length())
                              ? logical
                              : logical - 1);
            const int temporarySelectionLength =
                value.empty() ? 0 : 1;
            const long modelSelectionStart = live->m_selectionStart;
            const long modelSelectionEnd = live->m_selectionEnd;
            const long modelInsertionPoint = live->m_insertionPoint;
            const auto getSelectionTransactionOwner =
                [&]() -> wxTextCtrl *
                {
                    wxTextCtrl * const current = getExactOwner();
                    if ( !current ||
                         current->m_winui->textContentGeneration !=
                            contentGeneration ||
                         current->m_value != value ||
                         current->m_selectionStart != modelSelectionStart ||
                         current->m_selectionEnd != modelSelectionEnd ||
                         current->m_insertionPoint != modelInsertionPoint )
                    {
                        return nullptr;
                    }

                    return current;
                };

            const bool wasUpdatingPeer = live->m_updatingPeer;
            live->m_updatingPeer = true;
            bool updatingPeerRestored = false;
            const auto restoreUpdatingPeer =
                [&]()
                {
                    if ( updatingPeerRestored )
                        return;
                    updatingPeerRestored = true;
                    if ( wxTextCtrl * const current = getExactOwner() )
                    {
                        current->m_updatingPeer = wasUpdatingPeer;
                    }
                };
            const auto updatingPeerGuard =
                wxMakeGuard([&restoreUpdatingPeer]()
                {
                    restoreUpdatingPeer();
                });
            bool restoreAttempted = false;
            bool restoreSucceeded = false;
            const auto restoreSelection =
                [&]()
                {
                    if ( restoreAttempted )
                        return;
                    restoreAttempted = true;

                    // Scrolling request supersession is independent from this
                    // lexical peer-selection transaction. A newer
                    // ShowPosition() must not strand the temporary caret, but
                    // a real content/selection mutation must win.
                    if ( !getSelectionTransactionOwner() )
                        return;

                    bool peerRestored = false;
                    try
                    {
                        const int currentStart =
                            textBox.SelectionStart();
                        if ( !getSelectionTransactionOwner() )
                            return;
                        const int currentLength =
                            textBox.SelectionLength();
                        if ( !getSelectionTransactionOwner() ||
                             currentStart != temporarySelectionStart ||
                             currentLength != temporarySelectionLength )
                        {
                            // A real peer-side selection change happened
                            // while our callbacks were suppressed. It is newer
                            // than this temporary realization transaction.
                            return;
                        }

                        textBox.Select(
                            selectionStart, selectionLength);
                        if ( !getSelectionTransactionOwner() )
                            return;
                        const int restoredStart =
                            textBox.SelectionStart();
                        if ( !getSelectionTransactionOwner() )
                            return;
                        const int restoredLength =
                            textBox.SelectionLength();
                        peerRestored =
                            restoredStart == selectionStart &&
                            restoredLength == selectionLength;
                    }
                    catch ( const winrt::hresult_error& )
                    {
                        wxLogTrace(
                            "winui",
                            "WinUI TextBox selection restoration failed");
                    }

                    restoreSucceeded =
                        peerRestored &&
                        getSelectionTransactionOwner() != nullptr;
                };
            const auto restoreGuard =
                wxMakeGuard([&restoreSelection]()
                {
                    restoreSelection();
                });

            // A zero-length selection in an unfocused TextBox doesn't force
            // RichEdit to realize a virtualized wrapped range. Select exactly
            // one adjacent UTF-16 unit so its highlight requires layout; the
            // public selection is restored within this guarded transaction.
            textBox.Select(
                temporarySelectionStart, temporarySelectionLength);
            live = getLiveOwner();
            if ( !live )
                return;
#ifdef WXWINUI_TEST_SUPPORT
            if ( live->m_nextTemporarySelectionHook )
            {
                const auto hook =
                    live->m_nextTemporarySelectionHook;
                void * const context =
                    live->m_nextTemporarySelectionContext;
                live->m_nextTemporarySelectionHook = nullptr;
                live->m_nextTemporarySelectionContext = nullptr;
                hook(live, context);
                if ( !getExactOwner() )
                    return;
            }
#endif // WXWINUI_TEST_SUPPORT

            // Select() can enqueue both range realization and an implicit
            // caret scroll even though it doesn't take focus. The original
            // selection is restored synchronously below, but both viewport
            // operations must drain before the exact visibility request.
            needsDeferredCompletion = true;
            textBox.UpdateLayout();
            if ( !getLiveOwner() )
                return;
            scroll = wxWinUIGetCurrentTextScrollViewer(textBox);
            if ( !scroll || !getLiveOwner() )
            {
                queueFirstLowAttempt();
                return;
            }
            scroll.UpdateLayout();
            if ( !getLiveOwner() )
                return;
            if ( wxWinUIGetCurrentTextScrollViewer(textBox) != scroll ||
                 !getLiveOwner() )
            {
                queueFirstLowAttempt();
                return;
            }
            if ( getLiveOwner() )
            {
                haveUsableEdge =
                    wxWinUIGetTextEdge(
                        textBox, value, logical, &edge) &&
                    wxWinUIIsPlausibleWrappedTextEdge(textBox, edge);
            }
            if ( !wxWinUIIsCurrentTextScrollView(textBox, scroll) ||
                 !getLiveOwner() )
            {
                queueFirstLowAttempt();
                return;
            }

            restoreSelection();
            restoreUpdatingPeer();
            if ( !restoreSucceeded )
            {
                // A failed restoration must still terminate the visibility
                // transaction and release its retained view. Reconcile the wx
                // selection model from whatever state the peer actually kept.
                completeRequest();
                if ( wxTextCtrl * const current =
                         getSelectionTransactionOwner() )
                {
                    current->ReadSelectionFromPeer();
                }
                return;
            }
            restoreGuard.Dismiss();
            updatingPeerGuard.Dismiss();
        }

        const MUX::UIElement requestView =
            scroll.Content().try_as<MUX::UIElement>();
        if ( !requestView ||
             !wxWinUIIsCurrentTextScrollView(
                  textBox, scroll, requestView) )
        {
            // Template realization can replace ContentElement between the
            // layout pass and this snapshot. Keep the generation-bound
            // request progressing, or terminate it if no dispatcher accepts
            // the first bounded attempt; never strand Pending=true.
            queueFirstLowAttempt();
            return;
        }
        wxTextCtrl *live = getLiveOwner();
        if ( !live )
            return;

        wxWinUITextCtrlImpl * const impl = live->m_winui.get();
        if ( impl->positionVisibilityGeneration != requestGeneration )
            return;

        impl->positionVisibilityHasEdge = haveUsableEdge;
        if ( haveUsableEdge )
            impl->positionVisibilityEdge = edge;
        impl->positionVisibilityScroll = scroll;
        impl->positionVisibilityView = requestView;

        if ( needsDeferredCompletion )
        {
            // ChangeView(), selection realization and selection restoration
            // all complete asynchronously. Running StartBringIntoView in this
            // stack lets the later restoration scroll win and leaves the
            // viewport at the old caret. Queue exactly one generation-bound
            // continuation after those operations instead. A newer request,
            // content replacement, peer replacement or destruction makes the
            // callback a no-op.
            queueFirstLowAttempt();
            return;
        }

        live->FinishPositionVisibility(requestGeneration, 3);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TextBox scroll position", e);
        completeRequest();
    }
}

void wxTextCtrl::FinishPositionVisibility(std::uint64_t generation,
                                          unsigned retriesRemaining)
{
    if ( !m_winui ||
         !m_winui->positionVisibilityPending ||
         m_winui->positionVisibilityGeneration != generation ||
         m_winui->positionVisibilityContentGeneration !=
            m_winui->textContentGeneration )
    {
        return;
    }

    if ( m_winui->richEditBox )
    {
        const MUXC::RichEditBox richEditBox = m_winui->richEditBox;
        const std::shared_ptr<wxWinUITextCallbackState> callbackState =
            m_winui->callbackState;
        const std::uint64_t contentGeneration =
            m_winui->textContentGeneration;
        const wxString expectedValue = m_value;
        const long logical = m_winui->positionVisibilityLogical;
        const wxWeakRef<wxWindow> self(this);
        const auto getLiveOwner =
            [&self,
             &callbackState,
             &richEditBox,
             generation,
             contentGeneration,
             &expectedValue]() -> wxTextCtrl *
            {
                wxTextCtrl * const live =
                    wxDynamicCast(self.get(), wxTextCtrl);
                if ( !live || !live->m_winui ||
                     live->m_winui->callbackState != callbackState ||
                     live->m_winui->richEditBox != richEditBox ||
                     !live->m_winui->positionVisibilityPending ||
                     live->m_winui->positionVisibilityGeneration !=
                        generation ||
                     live->m_winui->textContentGeneration !=
                        contentGeneration ||
                     live->m_value != expectedValue )
                {
                    return nullptr;
                }
                return live;
            };
        const auto completeRequest = [&]()
        {
            if ( wxTextCtrl * const live = getLiveOwner() )
            {
                live->m_winui->positionVisibilityPending = false;
                live->m_winui->positionVisibilityHasEdge = false;
                live->m_winui->positionVisibilityScroll = nullptr;
                live->m_winui->positionVisibilityView = nullptr;
                live->m_winui->RestoreRichPositionVisibilityWrapping();
            }
        };
        const auto queueRetry = [&](unsigned remaining)
        {
            wxTextCtrl *live = getLiveOwner();
            if ( !live )
                return false;
            if ( remaining == 0 )
            {
                completeRequest();
                return false;
            }

            const MUXD::DispatcherQueue queue =
                richEditBox.DispatcherQueue();
            live = getLiveOwner();
            if ( !queue || !live )
            {
                completeRequest();
                return false;
            }

            const bool accepted = queue.TryEnqueue(
                MUXD::DispatcherQueuePriority::Low,
                [callbackState, generation, remaining]()
                {
                    wxTextCtrl * const owner =
                        callbackState->GetOwner<wxTextCtrl>();
                    if ( !owner || !owner->m_winui )
                        return;

                    wxWinUITextCtrlImpl * const current =
                        owner->m_winui.get();
                    if ( current->callbackState != callbackState ||
                         !current->positionVisibilityPending ||
                         current->positionVisibilityGeneration != generation )
                    {
                        return;
                    }

                    owner->FinishPositionVisibility(
                        generation, remaining - 1);
                });
            live = getLiveOwner();
            if ( accepted && live )
            {
                ++live->m_winui->positionVisibilityRetryCount;
            }
            else
            {
                completeRequest();
            }
            return accepted;
        };

        try
        {
            wxTextCtrl *live = getLiveOwner();
            if ( !live )
                return;
            if ( live->m_winui->
                     richPositionVisibilityNeedsWrapRestore )
            {
                // The preceding turn committed NoWrap. Restore Wrap only now
                // so the native RichEdit resize notification cannot be
                // coalesced with the invalidation which created it. A further
                // dispatcher turn observes the resulting ScrollViewer extent
                // before any exact scroll request is issued.
                live->m_winui->RestoreRichPositionVisibilityWrapping();
                live = getLiveOwner();
                if ( !live ||
                     richEditBox.TextWrapping() !=
                         MUX::TextWrapping::Wrap )
                {
                    completeRequest();
                    return;
                }

                MUXC::ScrollViewer realizationScroll =
                    wxWinUIResolveTextScrollViewer(*live, richEditBox);
                live = getLiveOwner();
                if ( !realizationScroll || !live )
                {
                    queueRetry(retriesRemaining);
                    return;
                }
                const MUX::UIElement realizationView =
                    realizationScroll.Content().try_as<MUX::UIElement>();
                live = getLiveOwner();
                if ( !realizationView || !live ||
                     !wxWinUIIsCurrentTextScrollView(
                         richEditBox,
                         realizationScroll,
                         realizationView) )
                {
                    queueRetry(retriesRemaining);
                    return;
                }

                const double realizationWidth =
                    realizationScroll.ViewportWidth() > 0.0
                        ? realizationScroll.ViewportWidth()
                        : realizationScroll.ActualWidth();
                live = getLiveOwner();
                if ( !(realizationWidth > 0.0) ||
                     !std::isfinite(realizationWidth) || !live )
                {
                    completeRequest();
                    return;
                }

                // TOM has already formatted the final line, but the internal
                // TextBoxView can still own a zero DesiredSize. Realize that
                // exact view once with the finite wrapping width and an
                // unconstrained height. ScrollViewer remains its sole arranger;
                // manually arranging the view would corrupt offsets.
                realizationView.InvalidateMeasure();
                live = getLiveOwner();
                if ( !live ||
                     !wxWinUIIsCurrentTextScrollView(
                         richEditBox,
                         realizationScroll,
                         realizationView) )
                {
                    return;
                }
                realizationView.InvalidateArrange();
                live = getLiveOwner();
                if ( !live ||
                     !wxWinUIIsCurrentTextScrollView(
                         richEditBox,
                         realizationScroll,
                         realizationView) )
                {
                    return;
                }
                const float infinity =
                    (std::numeric_limits<float>::infinity)();
                realizationView.Measure(
                    { static_cast<float>(realizationWidth), infinity });
                live = getLiveOwner();
                if ( !live ||
                     !wxWinUIIsCurrentTextScrollView(
                         richEditBox,
                         realizationScroll,
                         realizationView) )
                {
                    return;
                }
                live->m_winui->richPositionVisibilityTrace |=
                    wxWinUIRichVisibilityMeasured;

                realizationScroll.InvalidateMeasure();
                live = getLiveOwner();
                if ( !live ||
                     !wxWinUIIsCurrentTextScrollView(
                         richEditBox,
                         realizationScroll,
                         realizationView) )
                    return;
                realizationScroll.InvalidateArrange();
                live = getLiveOwner();
                if ( !live ||
                     !wxWinUIIsCurrentTextScrollView(
                         richEditBox,
                         realizationScroll,
                         realizationView) )
                    return;
                richEditBox.InvalidateMeasure();
                live = getLiveOwner();
                if ( !live ||
                     !wxWinUIIsCurrentTextScrollView(
                         richEditBox,
                         realizationScroll,
                         realizationView) )
                    return;
                richEditBox.InvalidateArrange();
                live = getLiveOwner();
                if ( !live ||
                     !wxWinUIIsCurrentTextScrollView(
                         richEditBox,
                         realizationScroll,
                         realizationView) )
                    return;
                live->m_winui->host.ForceRender();
                if ( !getLiveOwner() )
                    return;
                richEditBox.UpdateLayout();
                if ( !getLiveOwner() )
                    return;
                realizationScroll =
                    wxWinUIGetCurrentTextScrollViewer(richEditBox);
                if ( !realizationScroll || !getLiveOwner() )
                {
                    queueRetry(retriesRemaining);
                    return;
                }
                realizationScroll.UpdateLayout();
                if ( wxWinUIGetCurrentTextScrollViewer(richEditBox) !=
                         realizationScroll ||
                     !getLiveOwner() )
                {
                    queueRetry(retriesRemaining);
                    return;
                }
                queueRetry(retriesRemaining);
                return;
            }
            if ( live->m_winui->richPositionVisibilityPassCount >=
                    wxWinUIRichVisibilityMaxPasses )
            {
                completeRequest();
                return;
            }

            live->m_winui->host.ForceRender();
            if ( !getLiveOwner() )
                return;
            MUXC::ScrollViewer scroll =
                wxWinUIResolveTextScrollViewer(*this, richEditBox);
            if ( !scroll || !getLiveOwner() )
            {
                queueRetry(retriesRemaining);
                return;
            }

            richEditBox.UpdateLayout();
            if ( !getLiveOwner() )
                return;
            scroll = wxWinUIGetCurrentTextScrollViewer(richEditBox);
            if ( !scroll || !getLiveOwner() )
            {
                queueRetry(retriesRemaining);
                return;
            }
            scroll.UpdateLayout();
            if ( wxWinUIGetCurrentTextScrollViewer(richEditBox) != scroll ||
                 !getLiveOwner() )
            {
                queueRetry(retriesRemaining);
                return;
            }

            const MUX::UIElement view =
                scroll.Content().try_as<MUX::UIElement>();
            if ( !view ||
                 !wxWinUIIsCurrentTextScrollView(
                     richEditBox, scroll, view) ||
                 !getLiveOwner() )
            {
                queueRetry(retriesRemaining);
                return;
            }

            winrt::Windows::Foundation::Point point{};
            const bool hasPoint = wxWinUIGetRichTextPoint(
                richEditBox, logical, &point);
            live = getLiveOwner();
            if ( !live )
                return;
            ++live->m_winui->richPositionVisibilityPassCount;

            const auto makeTarget = [&](const auto& targetPoint)
            {
                return winrt::Windows::Foundation::Rect
                {
                    targetPoint.X,
                    targetPoint.Y,
                    1.0f,
                    static_cast<float>(
                        wxMax(1.0, richEditBox.FontSize()))
                };
            };
            const wxWinUITextViewportState targetState = hasPoint
                ? wxWinUIProjectTextViewTargetToViewport(
                      richEditBox, scroll, view, makeTarget(point))
                : wxWinUITextViewportState::Unavailable;
            live = getLiveOwner();
            if ( !live )
                return;
            if ( targetState == wxWinUITextViewportState::Visible )
            {
                live->m_winui->richPositionVisibilityTrace |=
                    wxWinUIRichVisibilityVisible;
                completeRequest();
                return;
            }

            bool accepted = false;
            if ( targetState != wxWinUITextViewportState::Unavailable )
            {
                live->m_winui->richPositionVisibilityTrace |=
                    wxWinUIRichVisibilityExactRequested;
                const auto target = makeTarget(point);
                accepted = wxWinUIChangeTextViewToTarget(
                    richEditBox, scroll, view, target);
                if ( !accepted && getLiveOwner() )
                {
                    accepted = wxWinUIStartTextViewBringIntoView(
                        richEditBox, scroll, view, target);
                }
                live = getLiveOwner();
                if ( !live )
                    return;
                if ( accepted )
                {
                    live->m_winui->richPositionVisibilityTrace |=
                        wxWinUIRichVisibilityExactAccepted;
                }
            }
            else
            {
                live->m_winui->richPositionVisibilityTrace |=
                    wxWinUIRichVisibilityUnavailable |
                    wxWinUIRichVisibilityCoarseRequested;
                const double fraction = expectedValue.empty()
                    ? 0.0
                    : static_cast<double>(logical) /
                          static_cast<double>(expectedValue.length());
                const double targetHorizontal =
                    logical == 0
                        ? 0.0
                        : logical ==
                              static_cast<long>(expectedValue.length())
                            ? scroll.ScrollableWidth()
                            : scroll.ScrollableWidth() * fraction;
                if ( !getLiveOwner() )
                    return;
                const double targetVertical =
                    logical == 0
                        ? 0.0
                        : logical ==
                              static_cast<long>(expectedValue.length())
                            ? scroll.ScrollableHeight()
                            : scroll.ScrollableHeight() * fraction;
                if ( !getLiveOwner() )
                    return;
                const auto horizontal =
                    winrt::box_value(targetHorizontal).
                        as<winrt::Windows::Foundation::IReference<double>>();
                const auto vertical =
                    winrt::box_value(targetVertical).
                        as<winrt::Windows::Foundation::IReference<double>>();
                accepted =
                    scroll.ChangeView(horizontal, vertical, nullptr, true);
                live = getLiveOwner();
                if ( !live )
                    return;
                if ( accepted )
                {
                    live->m_winui->richPositionVisibilityTrace |=
                        wxWinUIRichVisibilityCoarseAccepted;
                }
            }

            // The continuation is deliberately one realization request per
            // dispatcher turn. Force the host/layout publication now, then
            // prove visibility on the next generation-bound turn.
            live->m_winui->host.ForceRender();
            if ( !getLiveOwner() )
                return;
            richEditBox.UpdateLayout();
            if ( !getLiveOwner() )
                return;
            queueRetry(retriesRemaining);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI RichEditBox deferred scroll position", e);
            completeRequest();
        }
        return;
    }

    if ( !m_winui->textBox )
        return;

    const MUXC::TextBox textBox = m_winui->textBox;
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        m_winui->callbackState;
    const std::uint64_t contentGeneration =
        m_winui->textContentGeneration;
    const wxString expectedValue = m_value;
    const long logical = m_winui->positionVisibilityLogical;
    const wxWinUITextEdge cachedEdge =
        m_winui->positionVisibilityEdge;
    const MUXC::ScrollViewer cachedScroll =
        m_winui->positionVisibilityScroll;
    const MUX::UIElement cachedView =
        m_winui->positionVisibilityView;
    const bool cachedEdgeWasFlagged =
        m_winui->positionVisibilityHasEdge;

    const wxWeakRef<wxWindow> self(this);
    const auto getLiveOwner =
        [&self,
         &callbackState,
         &textBox,
         generation,
         contentGeneration,
         &expectedValue]() -> wxTextCtrl *
        {
            wxTextCtrl * const live =
                wxDynamicCast(self.get(), wxTextCtrl);
            if ( !live || !live->m_winui ||
                 live->m_winui->callbackState != callbackState ||
                 live->m_winui->textBox != textBox ||
                 !live->m_winui->positionVisibilityPending ||
                 live->m_winui->positionVisibilityGeneration != generation ||
                 live->m_winui->textContentGeneration !=
                    contentGeneration ||
                 live->m_value != expectedValue )
            {
                return nullptr;
            }
            return live;
        };
    const auto completeRequest = [&]()
    {
        if ( wxTextCtrl * const live = getLiveOwner() )
        {
            live->m_winui->positionVisibilityPending = false;
            live->m_winui->positionVisibilityHasEdge = false;
            live->m_winui->positionVisibilityScroll = nullptr;
            live->m_winui->positionVisibilityView = nullptr;
        }
    };
    const auto queueRetry = [&](unsigned remaining)
    {
        wxTextCtrl *live = getLiveOwner();
        if ( !live )
            return false;
        if ( remaining == 0 )
        {
            completeRequest();
            return false;
        }

        const MUXD::DispatcherQueue queue = textBox.DispatcherQueue();
        live = getLiveOwner();
        if ( !queue || !live )
        {
            completeRequest();
            return false;
        }

        const bool accepted = queue.TryEnqueue(
            MUXD::DispatcherQueuePriority::Low,
            [callbackState, generation, remaining]()
            {
                wxTextCtrl * const owner =
                    callbackState->GetOwner<wxTextCtrl>();
                if ( !owner || !owner->m_winui )
                    return;

                wxWinUITextCtrlImpl * const impl =
                    owner->m_winui.get();
                if ( impl->callbackState != callbackState ||
                     !impl->positionVisibilityPending ||
                     impl->positionVisibilityGeneration != generation )
                {
                    return;
                }

                owner->FinishPositionVisibility(
                    generation, remaining - 1);
            });
        live = getLiveOwner();
        if ( accepted && live )
        {
            ++live->m_winui->positionVisibilityRetryCount;
        }
        else
        {
            completeRequest();
        }
        return accepted;
    };

    try
    {
        MUXC::ScrollViewer scroll =
            wxWinUIResolveTextScrollViewer(*this, textBox);
        if ( !getLiveOwner() )
            return;
        if ( !scroll )
        {
            queueRetry(retriesRemaining);
            return;
        }

        textBox.UpdateLayout();
        if ( !getLiveOwner() )
            return;
        scroll = wxWinUIGetCurrentTextScrollViewer(textBox);
        if ( !scroll || !getLiveOwner() )
        {
            queueRetry(retriesRemaining);
            return;
        }
        scroll.UpdateLayout();
        if ( !getLiveOwner() )
            return;
        if ( wxWinUIGetCurrentTextScrollViewer(textBox) != scroll ||
             !getLiveOwner() )
        {
            queueRetry(retriesRemaining);
            return;
        }

        bool cachedEdgeWasUsable = false;
        if ( cachedEdgeWasFlagged )
        {
            cachedEdgeWasUsable =
                wxWinUIIsPlausibleWrappedTextEdge(textBox, cachedEdge);
            if ( !getLiveOwner() )
                return;
        }

        // The document endpoints have exact ScrollViewer representations.
        // Re-assert them after the temporary-selection transaction has drained:
        // restoring the real selection can enqueue a later implicit caret
        // scroll, and a cached viewport-relative RichEdit edge must not make
        // that stale viewport look final. This is also the deterministic path
        // for RichEdit's finite "unwrapped final edge" sentinel.
        if ( logical == 0 ||
             logical == static_cast<long>(expectedValue.length()) )
        {
            const double horizontalOffset = scroll.HorizontalOffset();
            if ( !getLiveOwner() )
                return;
            const double verticalOffset = scroll.VerticalOffset();
            if ( !getLiveOwner() )
                return;
            const double scrollableWidth = scroll.ScrollableWidth();
            if ( !getLiveOwner() )
                return;
            const double scrollableHeight = scroll.ScrollableHeight();
            if ( !wxWinUIIsCurrentTextScrollView(textBox, scroll) ||
                 !getLiveOwner() )
            {
                queueRetry(retriesRemaining);
                return;
            }

            const double targetHorizontal =
                logical == 0 ? 0.0 : scrollableWidth;
            const double targetVertical =
                logical == 0 ? 0.0 : scrollableHeight;
            constexpr double OffsetTolerance = 0.5;
            if ( std::fabs(horizontalOffset - targetHorizontal) >
                    OffsetTolerance ||
                 std::fabs(verticalOffset - targetVertical) >
                    OffsetTolerance )
            {
                const auto horizontal =
                    winrt::box_value(targetHorizontal).
                    as<winrt::Windows::Foundation::IReference<double>>();
                const auto vertical =
                    winrt::box_value(targetVertical).
                    as<winrt::Windows::Foundation::IReference<double>>();
                scroll.ChangeView(horizontal, vertical, nullptr, true);
                if ( !wxWinUIIsCurrentTextScrollView(textBox, scroll) ||
                     !getLiveOwner() )
                {
                    // ChangeView can synchronously replace ContentElement.
                    // Re-resolve it in the next bounded generation-bound pass,
                    // or let queueRetry() complete the request at exhaustion.
                    queueRetry(retriesRemaining);
                    return;
                }
                queueRetry(retriesRemaining);
                return;
            }
        }

        wxWinUITextEdge edge;
        const bool haveCurrentEdge =
            wxWinUIGetTextEdge(
                textBox, expectedValue, logical, &edge) &&
            wxWinUIIsPlausibleWrappedTextEdge(textBox, edge);
        if ( !wxWinUIIsCurrentTextScrollView(textBox, scroll) ||
             !getLiveOwner() )
        {
            queueRetry(retriesRemaining);
            return;
        }
        if ( !haveCurrentEdge )
        {
            if ( !cachedEdgeWasUsable )
            {
                queueRetry(retriesRemaining);
                return;
            }
            edge = cachedEdge;
        }

        // ContentElement hosts the TextBoxView, which is the ScrollViewer's
        // real IScrollInfo implementation. Ask that view to reveal the exact
        // character extent after all earlier coarse/caret scrolls have
        // drained. This does not change focus, caret or selection.
        const MUX::UIElement view =
            scroll.Content().try_as<MUX::UIElement>();
        if ( !view ||
             !wxWinUIIsCurrentTextScrollView(textBox, scroll, view) ||
             !getLiveOwner() )
        {
            queueRetry(retriesRemaining);
            return;
        }

        const bool hasCachedEdge =
            cachedEdgeWasUsable &&
            cachedScroll == scroll &&
            cachedView == view;
        if ( !haveCurrentEdge && !hasCachedEdge )
        {
            queueRetry(retriesRemaining);
            return;
        }

        const winrt::Windows::Foundation::Rect target
        {
            edge.point.X,
            edge.point.Y,
            1.0f,
            static_cast<float>(wxMax(1.0, edge.height))
        };
        const wxWinUITextViewportState viewportState =
            wxWinUIProjectTextViewTargetToViewport(
                textBox, scroll, view, target);
        if ( viewportState == wxWinUITextViewportState::Unavailable ||
             !getLiveOwner() )
        {
            queueRetry(retriesRemaining);
            return;
        }

        if ( viewportState == wxWinUITextViewportState::Visible )
        {
            completeRequest();
            return;
        }

        if ( !wxWinUIStartTextViewBringIntoView(
                 textBox, scroll, view, target) ||
             !getLiveOwner() )
        {
            queueRetry(retriesRemaining);
            return;
        }
        queueRetry(retriesRemaining);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TextBox scroll position", e);
        completeRequest();
    }
}

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITextCtrlTestAccess::GetNativeCaretShown(
    const wxTextCtrl& textCtrl,
    bool *shown)
{
    if ( !shown || !textCtrl.m_winui || !textCtrl.m_winui->richEditBox )
        return false;

    wxWinUITextCtrlImpl * const impl = textCtrl.m_winui.get();
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t generation = callbackState->GetGeneration();
    const wxWeakRef<wxWindow> self(
        const_cast<wxTextCtrl *>(&textCtrl));
    const auto isCurrent = [&]()
    {
        if ( callbackState->GetGeneration() != generation )
            return false;
        const wxTextCtrl * const owner =
            wxDynamicCast(self.get(), wxTextCtrl);
        return owner && owner->m_winui &&
               owner->m_winui.get() == impl &&
               owner->m_winui->callbackState == callbackState &&
               owner->m_winui->richEditBox == richEditBox;
    };

    try
    {
        const WUT::ITextDocument document = richEditBox.Document();
        if ( !document || !isCurrent() )
            return false;
        const WUT::CaretType caret = document.CaretType();
        if ( !isCurrent() )
            return false;
        *shown = caret != WUT::CaretType::Null;
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITextCtrlTestAccess::HasNoHideSelectionProjection(
    const wxTextCtrl& textCtrl)
{
    if ( !textCtrl.m_winui )
        return false;

    wxWinUITextCtrlImpl * const impl = textCtrl.m_winui.get();
    const MUXC::TextBox textBox = impl->textBox;
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t generation = callbackState->GetGeneration();
    const wxWeakRef<wxWindow> self(
        const_cast<wxTextCtrl *>(&textCtrl));
    const auto isCurrent = [&]()
    {
        if ( callbackState->GetGeneration() != generation )
            return false;
        const wxTextCtrl * const owner =
            wxDynamicCast(self.get(), wxTextCtrl);
        return owner && owner->m_winui &&
               owner->m_winui.get() == impl &&
               owner->m_winui->callbackState == callbackState &&
               owner->m_winui->textBox == textBox &&
               owner->m_winui->richEditBox == richEditBox;
    };

    try
    {
        winrt::Windows::Foundation::IInspectable local{ nullptr };
        if ( textBox )
        {
            local = textBox.ReadLocalValue(
                MUXC::TextBox::
                    SelectionHighlightColorWhenNotFocusedProperty());
        }
        else if ( richEditBox )
        {
            local = richEditBox.ReadLocalValue(
                MUXC::RichEditBox::
                    SelectionHighlightColorWhenNotFocusedProperty());
        }
        else
        {
            return false;
        }
        return isCurrent() &&
               local != MUX::DependencyProperty::UnsetValue();
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITextCtrlTestAccess::GetPasswordPeerSecurity(
    const wxTextCtrl& textCtrl,
    bool *isPassword,
    bool *hasValuePattern,
    bool *hasTextPattern,
    wxString *documentText,
    bool *textPredictionEnabled)
{
    if ( !isPassword || !hasValuePattern || !hasTextPattern ||
         !textPredictionEnabled ||
         !textCtrl.m_winui || !textCtrl.m_winui->securePasswordPeer ||
         !textCtrl.m_winui->richEditBox )
    {
        return false;
    }

    wxWinUITextCtrlImpl * const impl = textCtrl.m_winui.get();
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t generation = callbackState->GetGeneration();
    const wxWeakRef<wxWindow> self(
        const_cast<wxTextCtrl *>(&textCtrl));
    const auto isCurrent = [&]()
    {
        if ( callbackState->GetGeneration() != generation )
            return false;
        const wxTextCtrl * const owner =
            wxDynamicCast(self.get(), wxTextCtrl);
        return owner && owner->m_winui &&
               owner->m_winui.get() == impl &&
               owner->m_winui->callbackState == callbackState &&
               owner->m_winui->securePasswordPeer &&
               owner->m_winui->richEditBox == richEditBox;
    };

    try
    {
        const MUXAP::AutomationPeer peer =
            MUXAP::FrameworkElementAutomationPeer::CreatePeerForElement(
                richEditBox);
        if ( !peer || !isCurrent() )
            return false;
        const bool password = peer.IsPassword();
        if ( !isCurrent() )
            return false;
        const bool valuePattern =
            peer.GetPattern(MUXAP::PatternInterface::Value) != nullptr;
        if ( !isCurrent() )
            return false;
        const bool textPattern =
            peer.GetPattern(MUXAP::PatternInterface::Text) != nullptr ||
            peer.GetPattern(MUXAP::PatternInterface::Text2) != nullptr ||
            peer.GetPattern(MUXAP::PatternInterface::TextEdit) != nullptr;
        if ( !isCurrent() )
            return false;
        wxString peerDocument;
        if ( documentText )
        {
            peerDocument = wxWinUIGetRichPlainText(richEditBox);
            if ( !isCurrent() )
                return false;
        }
        const bool predictionEnabled =
            richEditBox.IsTextPredictionEnabled();
        if ( !isCurrent() )
            return false;

        *isPassword = password;
        *hasValuePattern = valuePattern;
        *hasTextPattern = textPattern;
        if ( documentText )
            *documentText = peerDocument;
        *textPredictionEnabled = predictionEnabled;
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUITextCtrlTestAccess::ForceNextPasswordScrubFailure(
    wxTextCtrl& textCtrl,
    long hresult)
{
    const HRESULT failure = static_cast<HRESULT>(hresult);
    textCtrl.m_nextPasswordScrubFailure =
        static_cast<long>(FAILED(failure) ? failure : E_FAIL);
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUITextCtrlTestAccess::ForceNextPasswordScrubPartialWrite(
    wxTextCtrl& textCtrl)
{
    textCtrl.m_nextPasswordScrubPartialWrite = true;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITextCtrlTestAccess::GetPasswordFailClosed(
    const wxTextCtrl& textCtrl,
    long *hresult,
    bool *documentWasEmpty)
{
    if ( !textCtrl.m_passwordPeerFailClosedHresult )
        return false;

    if ( hresult )
        *hresult = textCtrl.m_passwordPeerFailClosedHresult;
    if ( documentWasEmpty )
    {
        *documentWasEmpty =
            textCtrl.m_passwordPeerWasEmptyOnFailClosed;
    }
    return true;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
unsigned wxWinUITextCtrlTestAccess::GetPasswordClipboardExportAttemptCount(
    const wxTextCtrl& textCtrl)
{
    return textCtrl.m_winui && textCtrl.m_winui->securePasswordPeer
               ? textCtrl.m_winui->passwordClipboardExportAttemptCount
               : 0;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
unsigned wxWinUITextCtrlTestAccess::GetAutoUrlRangeCount(
    const wxTextCtrl& textCtrl)
{
    return textCtrl.m_winui && textCtrl.HasFlag(wxTE_AUTO_URL)
             ? static_cast<unsigned>(textCtrl.m_winui->autoUrlRanges.size())
             : 0;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITextCtrlTestAccess::GetAutoUrlRange(
    const wxTextCtrl& textCtrl,
    unsigned n,
                                                long *from,
                                                long *to,
                                                wxString *target)
{
    if ( !textCtrl.m_winui || !textCtrl.HasFlag(wxTE_AUTO_URL) ||
         n >= textCtrl.m_winui->autoUrlRanges.size() )
    {
        return false;
    }

    const wxWinUIAutoUrlRange range = textCtrl.m_winui->autoUrlRanges[n];
    const wxWinUITextPositionMap map =
        wxWinUIMakeTextPositionMap(textCtrl, textCtrl.m_value);
    if ( from )
        *from = map.LogicalToPublic(range.start);
    if ( to )
        *to = map.LogicalToPublic(range.end);
    if ( target )
        *target = range.target;
    return true;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
unsigned wxWinUITextCtrlTestAccess::GetAutoCompleteSuggestionCount(
    const wxTextCtrl& textCtrl)
{
    if ( !textCtrl.m_winui || !textCtrl.m_winui->autoCompleteFlyout )
        return 0;

    wxWinUITextCtrlImpl * const impl = textCtrl.m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t generation = callbackState->GetGeneration();
    const MUXC::MenuFlyout flyout = impl->autoCompleteFlyout;
    try
    {
        const unsigned count = flyout.Items().Size();
        const wxTextCtrl * const owner =
            callbackState->GetOwner<wxTextCtrl>();
        return callbackState->GetGeneration() == generation &&
                       owner == &textCtrl && owner->m_winui &&
                       owner->m_winui.get() == impl &&
                       owner->m_winui->callbackState == callbackState &&
                       owner->m_winui->autoCompleteFlyout == flyout
                 ? count
                 : 0;
    }
    catch ( const winrt::hresult_error& )
    {
        return 0;
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
wxString wxWinUITextCtrlTestAccess::GetAutoCompleteSuggestion(
    const wxTextCtrl& textCtrl,
    unsigned n)
{
    if ( !textCtrl.m_winui || !textCtrl.m_winui->autoCompleteFlyout )
        return wxString();

    wxWinUITextCtrlImpl * const impl = textCtrl.m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t generation = callbackState->GetGeneration();
    const MUXC::MenuFlyout flyout = impl->autoCompleteFlyout;
    try
    {
        const auto items = flyout.Items();
        if ( n >= items.Size() )
            return wxString();
        const MUXC::MenuFlyoutItem item =
            items.GetAt(n).try_as<MUXC::MenuFlyoutItem>();
        const wxString value =
            item ? wxWinUIFromHString(item.Text()) : wxString();
        const wxTextCtrl * const owner =
            callbackState->GetOwner<wxTextCtrl>();
        return callbackState->GetGeneration() == generation &&
                       owner == &textCtrl && owner->m_winui &&
                       owner->m_winui.get() == impl &&
                       owner->m_winui->callbackState == callbackState &&
                       owner->m_winui->autoCompleteFlyout == flyout
                 ? value
                 : wxString();
    }
    catch ( const winrt::hresult_error& )
    {
        return wxString();
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITextCtrlTestAccess::InvokeAutoCompleteSuggestion(
    wxTextCtrl& textCtrl,
    unsigned n)
{
    if ( !textCtrl.m_winui || !textCtrl.m_winui->autoCompleteFlyout )
        return false;

    wxWinUITextCtrlImpl * const impl = textCtrl.m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t generation = callbackState->GetGeneration();
    const MUXC::MenuFlyout flyout = impl->autoCompleteFlyout;
    try
    {
        const auto items = flyout.Items();
        if ( callbackState->GetGeneration() != generation ||
             n >= items.Size() )
        {
            return false;
        }
        const MUXC::MenuFlyoutItem item =
            items.GetAt(n).try_as<MUXC::MenuFlyoutItem>();
        wxTextCtrl * const owner = callbackState->GetOwner<wxTextCtrl>();
        if ( !item || owner != &textCtrl || !owner->m_winui ||
             owner->m_winui.get() != impl ||
             owner->m_winui->callbackState != callbackState ||
             owner->m_winui->autoCompleteFlyout != flyout )
        {
            return false;
        }

        // Invoke the real automation provider so &textCtrl exercises the actual
        // MenuFlyoutItem Click delegate and all of its lifetime guards.
        MUXAP::MenuFlyoutItemAutomationPeer(item).Invoke();
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI TextCtrl auto-complete test invocation", e);
        return false;
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITextCtrlTestAccess::GetPeerSelection(
    const wxTextCtrl& textCtrl,
    long *from, long *to)
{
    if ( !textCtrl.m_winui ||
         (!textCtrl.m_winui->textBox && !textCtrl.m_winui->richEditBox) )
        return false;

    wxWinUITextCtrlImpl * const impl = textCtrl.m_winui.get();
    const MUXC::TextBox textBox = impl->textBox;
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->GetGeneration();
    const std::uint64_t contentGeneration =
        impl->textContentGeneration;
    const wxString value = textCtrl.m_value;
    const wxWeakRef<wxWindow> self(
        const_cast<wxTextCtrl *>(&textCtrl));
    const auto getLiveOwner = [&]() -> wxTextCtrl *
    {
        if ( callbackState->GetGeneration() != callbackGeneration )
            return nullptr;

        wxTextCtrl * const live =
            wxDynamicCast(self.get(), wxTextCtrl);
        return live && live->m_winui &&
                       live->m_winui.get() == impl &&
                       live->m_winui->callbackState == callbackState &&
                       live->m_winui->textBox == textBox &&
                       live->m_winui->richEditBox == richEditBox &&
                       live->m_winui->textContentGeneration ==
                           contentGeneration &&
                       live->m_value == value
                 ? live
                 : nullptr;
    };

    try
    {
        long start = 0;
        long end = 0;
        if ( textBox )
        {
            start = textBox.SelectionStart();
            if ( !getLiveOwner() )
                return false;
            const int length = textBox.SelectionLength();
            if ( length < 0 )
                return false;
            end = start + length;
        }
        else
        {
            const WUT::ITextDocument document = richEditBox.Document();
            if ( !document || !getLiveOwner() )
                return false;
            const WUT::ITextSelection selection = document.Selection();
            if ( !selection || !getLiveOwner() )
                return false;
            start = selection.StartPosition();
            if ( !getLiveOwner() )
                return false;
            end = selection.EndPosition();
        }
        wxTextCtrl * const live = getLiveOwner();
        if ( !live || start < 0 || end < 0 )
            return false;

        const long logicalStart =
            wxMin<long>(start, static_cast<long>(value.length()));
        const long logicalEnd =
            wxMin<long>(
                end,
                static_cast<long>(value.length()));
        const wxWinUITextPositionMap map =
            wxWinUIMakeTextPositionMap(*live, value);
        if ( !getLiveOwner() )
            return false;

        if ( from )
            *from = map.LogicalToPublic(logicalStart);
        if ( to )
            *to = map.LogicalToPublic(logicalEnd);
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITextCtrlTestAccess::GetScrollState(
    const wxTextCtrl& textCtrl,
    double *horizontalOffset,
    double *verticalOffset,
    double *viewportWidth,
    double *viewportHeight,
    double *rasterizationScale,
    int *stage,
    double *scrollableWidth,
    double *scrollableHeight,
    double *viewOriginX,
    double *viewOriginY,
    double *viewUnitXScale,
    double *viewUnitYScale)
{
    if ( stage )
        *stage = 0;
    if ( !textCtrl.m_winui ||
         (!textCtrl.m_winui->textBox && !textCtrl.m_winui->richEditBox) )
    {
        if ( stage )
            *stage = 1;
        return false;
    }

    wxWinUITextCtrlImpl * const impl = textCtrl.m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t generation =
        callbackState->GetGeneration();
    const MUXC::TextBox textBox = impl->textBox;
    const MUXC::RichEditBox richEditBox = impl->richEditBox;
    const wxWeakRef<wxWindow> self(
        const_cast<wxTextCtrl *>(&textCtrl));
    const auto getLiveOwner = [&]() -> wxTextCtrl *
    {
        if ( callbackState->GetGeneration() != generation )
            return nullptr;
        wxTextCtrl * const owner =
            wxDynamicCast(self.get(), wxTextCtrl);
        return owner && owner->m_winui &&
                       owner->m_winui.get() == impl &&
                       owner->m_winui->callbackState == callbackState &&
                       owner->m_winui->textBox == textBox &&
                       owner->m_winui->richEditBox == richEditBox
                 ? owner
                 : nullptr;
    };

    try
    {
        const MUXC::ScrollViewer scroll = textBox
            ? wxWinUIResolveTextScrollViewer(textCtrl, textBox)
            : wxWinUIResolveTextScrollViewer(textCtrl, richEditBox);
        wxTextCtrl *live = getLiveOwner();
        if ( !scroll || !live )
        {
            if ( stage )
                *stage = 2;
            return false;
        }
        wxWinUITopLevelHost * const host =
            wxWinUITopLevelHost::FindSlotOwner(live);
        if ( !host )
        {
            if ( stage )
                *stage = 3;
            return false;
        }

        const double horizontal = scroll.HorizontalOffset();
        if ( !getLiveOwner() )
            return false;
        const double vertical = scroll.VerticalOffset();
        if ( !getLiveOwner() )
            return false;
        const double width = scroll.ViewportWidth();
        if ( !getLiveOwner() )
            return false;
        const double height = scroll.ViewportHeight();
        live = getLiveOwner();
        if ( !live ||
             wxWinUITopLevelHost::FindSlotOwner(live) != host )
        {
            return false;
        }
        const double scale = host->GetScale();
        live = getLiveOwner();
        if ( !live ||
             wxWinUITopLevelHost::FindSlotOwner(live) != host )
        {
            return false;
        }

        if ( horizontalOffset )
            *horizontalOffset = horizontal;
        if ( verticalOffset )
            *verticalOffset = vertical;
        if ( viewportWidth )
            *viewportWidth = width;
        if ( viewportHeight )
            *viewportHeight = height;
        if ( rasterizationScale )
            *rasterizationScale = scale;
        if ( scrollableWidth )
            *scrollableWidth = scroll.ScrollableWidth();
        if ( !getLiveOwner() )
            return false;
        if ( scrollableHeight )
            *scrollableHeight = scroll.ScrollableHeight();
        if ( !getLiveOwner() )
            return false;
        if ( viewOriginX || viewOriginY ||
             viewUnitXScale || viewUnitYScale )
        {
            const MUX::UIElement view =
                scroll.Content().try_as<MUX::UIElement>();
            const bool currentView = textBox
                ? wxWinUIIsCurrentTextScrollView(textBox, scroll, view)
                : wxWinUIIsCurrentTextScrollView(richEditBox, scroll, view);
            if ( !view || !currentView || !getLiveOwner() )
                return false;

            const auto transform = view.TransformToVisual(scroll);
            if ( !transform || !getLiveOwner() )
                return false;
            const auto origin = transform.TransformPoint({ 0.0f, 0.0f });
            if ( !getLiveOwner() )
                return false;
            const auto unitX = transform.TransformPoint({ 1.0f, 0.0f });
            if ( !getLiveOwner() )
                return false;
            const auto unitY = transform.TransformPoint({ 0.0f, 1.0f });
            if ( !getLiveOwner() ||
                 !wxWinUIIsFinitePoint(origin) ||
                 !wxWinUIIsFinitePoint(unitX) ||
                 !wxWinUIIsFinitePoint(unitY) )
            {
                return false;
            }

            if ( viewOriginX )
                *viewOriginX = origin.X;
            if ( viewOriginY )
                *viewOriginY = origin.Y;
            if ( viewUnitXScale )
            {
                *viewUnitXScale = std::hypot(
                    static_cast<double>(unitX.X - origin.X),
                    static_cast<double>(unitX.Y - origin.Y));
            }
            if ( viewUnitYScale )
            {
                *viewUnitYScale = std::hypot(
                    static_cast<double>(unitY.X - origin.X),
                    static_cast<double>(unitY.Y - origin.Y));
            }
        }
        if ( stage )
            *stage = 4;
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        if ( stage )
            *stage = 5;
        return false;
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITextCtrlTestAccess::GetVerticalScrollBarVisibility(
    const wxTextCtrl& textCtrl,
    ScrollBarVisibility *visibility)
{
    if ( !visibility || !textCtrl.m_winui ||
         (!textCtrl.m_winui->textBox && !textCtrl.m_winui->richEditBox) )
        return false;

    wxWinUITextCtrlImpl * const impl = textCtrl.m_winui.get();
    const std::shared_ptr<wxWinUITextCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t generation =
        callbackState->GetGeneration();
    const MUXC::TextBox textBox = impl->textBox;
    const MUXC::RichEditBox richEditBox = impl->richEditBox;

    try
    {
        const MUXC::ScrollViewer scroll = textBox
            ? wxWinUIResolveTextScrollViewer(textCtrl, textBox)
            : wxWinUIResolveTextScrollViewer(textCtrl, richEditBox);

        if ( callbackState->GetGeneration() != generation )
            return false;

        wxTextCtrl * const owner =
            callbackState->GetOwner<wxTextCtrl>();
        if ( !owner || owner != &textCtrl || !owner->m_winui ||
              owner->m_winui.get() != impl ||
              owner->m_winui->callbackState != callbackState ||
              owner->m_winui->textBox != textBox ||
              owner->m_winui->richEditBox != richEditBox ||
              !scroll )
        {
            return false;
        }

        const MUXC::ScrollBarVisibility actual =
            scroll.VerticalScrollBarVisibility();
        if ( callbackState->GetGeneration() != generation ||
              callbackState->GetOwner<wxTextCtrl>() != owner ||
              !owner->m_winui || owner->m_winui.get() != impl ||
              owner->m_winui->textBox != textBox ||
              owner->m_winui->richEditBox != richEditBox )
        {
            return false;
        }

        switch ( actual )
        {
            case MUXC::ScrollBarVisibility::Disabled:
                *visibility =
                    ScrollBarVisibility::Disabled;
                break;

            case MUXC::ScrollBarVisibility::Auto:
                *visibility = ScrollBarVisibility::Auto;
                break;

            case MUXC::ScrollBarVisibility::Hidden:
                *visibility = ScrollBarVisibility::Hidden;
                break;

            case MUXC::ScrollBarVisibility::Visible:
                *visibility = ScrollBarVisibility::Visible;
                break;

            default:
                return false;
        }
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI TextBox vertical scrollbar state query", e);
        return false;
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITextCtrlTestAccess::GetPositionVisibilityState(
    const wxTextCtrl& textCtrl,
    bool *pending,
    unsigned *queuedRetries,
    bool *hasRetainedPeerReferences,
    unsigned *richTrace,
    unsigned *richPassCount)
{
    if ( !textCtrl.m_winui )
        return false;

    if ( pending )
        *pending = textCtrl.m_winui->positionVisibilityPending;
    if ( queuedRetries )
        *queuedRetries = textCtrl.m_winui->positionVisibilityRetryCount;
    if ( hasRetainedPeerReferences )
    {
        *hasRetainedPeerReferences =
            static_cast<bool>(textCtrl.m_winui->positionVisibilityScroll) ||
            static_cast<bool>(textCtrl.m_winui->positionVisibilityView);
    }
    if ( richTrace )
        *richTrace = textCtrl.m_winui->richPositionVisibilityTrace;
    if ( richPassCount )
        *richPassCount = textCtrl.m_winui->richPositionVisibilityPassCount;
    return true;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUITextCtrlTestAccess::UseTextBoxPeer(
    wxTextCtrl& textCtrl)
{
    // Peer kind is a creation-time decision. Ignore a late request rather
    // than replacing a live native peer behind callbacks that own its identity.
    if ( !textCtrl.m_winui )
        textCtrl.m_useTextBoxPeer = true;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUITextCtrlTestAccess::ForceNextPositionVisibilityRetry(
    wxTextCtrl& textCtrl)
{
    if ( textCtrl.m_winui )
    {
        if ( textCtrl.m_winui->richEditBox && !textCtrl.m_winui->securePasswordPeer )
        {
            // Force the Rich path's first-pass S_FALSE state so its bounded
            // coarse-realization transaction (synchronous or deferred) can be
            // tested deterministically.
            textCtrl.m_winui->forceNextRichPositionUnavailableForTesting = true;
        }
        else
        {
            textCtrl.m_winui->forceNextPositionVisibilityRetryForTesting = true;
        }
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUITextCtrlTestAccess::SetNextTemporarySelectionHook(
    wxTextCtrl& textCtrl,
    CallbackHook hook,
    void *context)
{
    textCtrl.m_nextTemporarySelectionHook = hook;
    textCtrl.m_nextTemporarySelectionContext = hook ? context : nullptr;
    if ( textCtrl.m_winui )
        textCtrl.m_winui->forceTemporarySelectionForTesting = hook != nullptr;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUITextCtrlTestAccess::SetNextPasswordTextChangingHook(
    wxTextCtrl& textCtrl,
    CallbackHook hook,
    void *context)
{
    textCtrl.m_nextPasswordTextChangingHook = hook;
    textCtrl.m_nextPasswordTextChangingContext = hook ? context : nullptr;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUITextCtrlTestAccess::SetNextCreateLoadedHook(
    wxTextCtrl& textCtrl,
    CallbackHook hook,
    void *context)
{
    textCtrl.m_nextCreateLoadedHook = hook;
    textCtrl.m_nextCreateLoadedContext = hook ? context : nullptr;
}
#endif // WXWINUI_TEST_SUPPORT

wxArrayString wxTextCtrl::GetLines() const
{
    wxArrayString lines = wxSplit(m_value, '\n', '\0');
    if ( lines.empty() )
        lines.Add(wxString());
    return lines;
}

WXHWND wxTextCtrl::GetEditHWND() const
{
    return GetHWND();
}

#endif // wxUSE_TEXTCTRL
