/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/stattext.cpp
// Purpose:     wxWinUI wxStaticText implementation
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_STATTEXT

#include "wx/stattext.h"
#include "wx/weakref.h"

#include "private.h"
#include "wx/winui/private/appearance.h"
#include "wx/winui/private/tlwhostmsw.h"

#ifdef WXWINUI_TEST_SUPPORT
    #include "static-test-access.h"
#endif

#if wxUSE_MARKUP
    #include "wx/private/markupparser.h"
#endif

#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Documents.h>
#include <winrt/Microsoft.UI.Text.h>
#include <winrt/Windows.UI.Text.h>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXD = winrt::Microsoft::UI::Xaml::Documents;

class wxWinUIStaticTextImpl
{
public:
    ~wxWinUIStaticTextImpl()
    {
        Close();
    }

    void Close()
    {
        // Detach from the shared island while every projected object is still
        // alive. There are no element callbacks, so teardown is idempotent.
        host.Close();
        textBlock = nullptr;
        root = nullptr;
    }

    wxWinUIControlHost host;
    MUX::Controls::Border root{ nullptr };
    MUX::Controls::TextBlock textBlock{ nullptr };
};

namespace
{

using InlineCollection =
    winrt::Windows::Foundation::Collections::IVector<MUXD::Inline>;

// Append a run of text with the given attributes, turning embedded newlines
// into XAML line breaks so multi-line labels render on several lines.
void wxWinUIAppendStyledText(InlineCollection const& inlines,
                             const wxString& text,
                             bool bold, bool italic, bool underline,
                             const wxColour& fg)
{
    wxString remaining = text;
    bool first = true;
    for ( ;; )
    {
        const int nl = remaining.Find('\n');
        const wxString line = nl == wxNOT_FOUND ? remaining
                                                : remaining.Left(nl);

        if ( !first )
            inlines.Append(MUXD::LineBreak());
        first = false;

        if ( !line.empty() )
        {
            MUXD::Run run;
            run.Text(wxWinUIToHString(line));
            if ( bold )
                run.FontWeight(winrt::Microsoft::UI::Text::FontWeights::Bold());
            if ( italic )
                run.FontStyle(winrt::Windows::UI::Text::FontStyle::Italic);
            if ( underline )
                run.TextDecorations(winrt::Windows::UI::Text::TextDecorations::Underline);
            if ( fg.IsOk() )
                run.Foreground(wxWinUICreateColourBrush(fg));
            inlines.Append(run);
        }

        if ( nl == wxNOT_FOUND )
            break;
        remaining = remaining.Mid(nl + 1);
    }
}

// Append a label using the current markup style while interpreting wx's
// mnemonic markers.  This is shared by plain and markup labels: markup only
// changes the style of a run, it must not make '&' or '&&' visible.
void wxWinUIAppendStyledLabel(InlineCollection const& inlines,
                              const wxString& label,
                              bool bold, bool italic, bool underline,
                              const wxColour& fg)
{
    wxString chunk;
    for ( size_t i = 0; i < label.length(); ++i )
    {
        const wxChar ch = label[i];
        if ( ch == '&' && i + 1 < label.length() )
        {
            const wxChar next = label[i + 1];
            if ( next == '&' )
            {
                chunk += '&';
                ++i;
                continue;
            }

            // Flush the normal text, then emit the underlined mnemonic char.
            wxWinUIAppendStyledText(
                inlines, chunk, bold, italic, underline, fg);
            chunk.clear();
            wxWinUIAppendStyledText(
                inlines, wxString(next), bold, italic, true, fg);
            ++i;
            continue;
        }

        chunk += ch;
    }

    wxWinUIAppendStyledText(
        inlines, chunk, bold, italic, underline, fg);
}

// Append a plain label, underlining the mnemonic character (the one following
// a single '&') as classic wxStaticText does.
void wxWinUIAppendLabel(InlineCollection const& inlines, const wxString& label)
{
    wxWinUIAppendStyledLabel(
        inlines, label, false, false, false, wxColour());
}

#if wxUSE_MARKUP
// Turns wx markup into a sequence of styled runs appended to a TextBlock.
class wxWinUIMarkupToInlines : public wxMarkupParserOutput
{
public:
    explicit wxWinUIMarkupToInlines(InlineCollection const& inlines)
        : m_inlines(inlines) {}

    void OnText(const wxString& text) override
    {
        wxWinUIAppendStyledLabel(
            m_inlines, text,
            m_bold > 0, m_italic > 0, m_underline > 0, m_fg);
    }

    void OnBoldStart() override { ++m_bold; }
    void OnBoldEnd() override { --m_bold; }
    void OnItalicStart() override { ++m_italic; }
    void OnItalicEnd() override { --m_italic; }
    void OnUnderlinedStart() override { ++m_underline; }
    void OnUnderlinedEnd() override { --m_underline; }
    void OnStrikethroughStart() override {}
    void OnStrikethroughEnd() override {}
    void OnBigStart() override {}
    void OnBigEnd() override {}
    void OnSmallStart() override {}
    void OnSmallEnd() override {}
    void OnTeletypeStart() override {}
    void OnTeletypeEnd() override {}

    void OnSpanStart(const wxMarkupSpanAttributes& attrs) override
    {
        m_fgStack.push_back(m_fg);
        if ( !attrs.m_fgCol.empty() )
        {
            const wxColour col(attrs.m_fgCol);
            if ( col.IsOk() )
                m_fg = col;
        }
    }
    void OnSpanEnd(const wxMarkupSpanAttributes& WXUNUSED(attrs)) override
    {
        m_fg = m_fgStack.back();
        m_fgStack.pop_back();
    }

private:
    InlineCollection m_inlines;
    int m_bold = 0;
    int m_italic = 0;
    int m_underline = 0;
    wxColour m_fg;
    std::vector<wxColour> m_fgStack;
};
#endif // wxUSE_MARKUP

} // namespace

wxStaticText::wxStaticText()
{
}

wxStaticText::wxStaticText(wxWindow *parent,
                           wxWindowID id,
                           const wxString& label,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style,
                           const wxString& name)
{
    Create(parent, id, label, pos, size, style, name);
}

wxStaticText::~wxStaticText()
{
    if ( m_winui )
        m_winui->Close();
}

bool wxStaticText::Create(wxWindow *parent,
                          wxWindowID id,
                          const wxString& label,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxString& name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    UpdateLabelOrig(label);
    m_visibleLabel = label;

    m_winui.reset(new wxWinUIStaticTextImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;
    wxWinUIStaticTextImpl * const createImpl = m_winui.get();
    const wxWeakRef<wxStaticText> lifetime(this);

    try
    {
        createImpl->textBlock = MUX::Controls::TextBlock();
        createImpl->root = MUX::Controls::Border();
        createImpl->root.IsHitTestVisible(false);

        // Multi-line labels (embedded newlines) are rendered via explicit line
        // breaks in BuildInlines(). wxST_WRAP, unlike Wrap(), asks the native
        // peer to wrap dynamically with its current width.
        createImpl->textBlock.TextWrapping(
            style & wxST_WRAP ? MUX::TextWrapping::Wrap
                              : MUX::TextWrapping::NoWrap);
        createImpl->textBlock.VerticalAlignment(
            MUX::VerticalAlignment::Top);

        MUX::TextAlignment align = MUX::TextAlignment::Left;
        if ( style & wxALIGN_RIGHT )
            align = MUX::TextAlignment::Right;
        else if ( style & wxALIGN_CENTRE_HORIZONTAL )
            align = MUX::TextAlignment::Center;
        createImpl->textBlock.TextAlignment(align);

        // WinUI only implements trailing trimming. START/MIDDLE are projected
        // through wxStaticTextBase::Ellipsize() and must not be trimmed a
        // second time by XAML.
        if ( (style & wxST_ELLIPSIZE_END) &&
             !(style & (wxST_ELLIPSIZE_START | wxST_ELLIPSIZE_MIDDLE)) )
        {
            createImpl->textBlock.TextTrimming(
                MUX::TextTrimming::CharacterEllipsis);
        }
        else
        {
            createImpl->textBlock.TextTrimming(
                MUX::TextTrimming::None);
        }

        createImpl->root.Child(createImpl->textBlock);
        if ( !UpdateWinUIContent() )
        {
            wxStaticText * const owner = lifetime.get();
            if ( owner && owner->m_winui.get() == createImpl )
                owner->m_winui.reset();
            return false;
        }
        const bool contentSet =
            createImpl->host.SetContent(
                createImpl->root, createImpl->textBlock);
        wxStaticText *owner = lifetime.get();
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != createImpl )
        {
            return false;
        }
        if ( !contentSet )
        {
            owner->m_winui.reset();
            return false;
        }

        if ( owner->UsesManualEllipsization() )
        {
            owner->UpdateLabel();
            owner = lifetime.get();
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != createImpl )
            {
                return false;
            }
        }

        owner->SetInitialSize(size);
        owner = lifetime.get();
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != createImpl )
        {
            return false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TextBlock creation", e);
        wxStaticText * const owner = lifetime.get();
        if ( owner && owner->m_winui &&
             owner->m_winui.get() == createImpl )
        {
            owner->m_winui.reset();
        }
        return false;
    }

    return true;
}

void wxStaticText::SetLabel(const wxString& label)
{
    wxWinUIStaticTextImpl * const impl = m_winui.get();
    const wxWeakRef<wxStaticText> alive(this);
    const bool wasMarkup = !m_markup.empty();
    m_markup.clear();

    // DoSetLabelMarkup() stores the stripped text in m_labelOrig. Switching
    // back to the same plain text is still a real peer mutation: the old
    // styled runs must be discarded even though the wx label is unchanged.
    if ( !UpdateLabelOrig(label) && !wasMarkup )
        return;

    if ( !HasFlag(wxST_NO_AUTORESIZE) )
        InvalidateBestSize();
    WXSetVisibleLabel(UsesManualEllipsization() ? GetEllipsizedLabel()
                                               : label);
    wxStaticText * const owner = alive.get();
    if ( !owner || owner->m_winui.get() != impl )
        return;

    // The TextBlock is now the slot's semantic target while the Border stays
    // the visual root. Updating the visible runs does not necessarily resize
    // the HWND (fixed-size labels are common), so explicitly invalidate the
    // shared state adapter to refresh AutomationProperties::Name.
    wxWinUITLWHostNotifySlotState(owner);

    if ( owner->GetContainingSizer() )
        owner->Refresh();
    else
        owner->AutoResizeIfNecessary();
}

bool wxStaticText::SetFont(const wxFont& font)
{
    wxWinUIStaticTextImpl * const impl = m_winui.get();
    const wxWeakRef<wxStaticText> alive(this);
    const bool rc = wxControl::SetFont(font);
    wxStaticText *owner = alive.get();
    if ( !owner || owner->m_winui.get() != impl ||
         !owner->ApplyWinUIAppearance() )
    {
        return rc;
    }

    owner = alive.get();
    if ( !owner || owner->m_winui.get() != impl )
        return rc;
    if ( owner->UsesManualEllipsization() )
    {
        owner->UpdateLabel();
        owner = alive.get();
        if ( !owner || owner->m_winui.get() != impl )
            return rc;
    }
    if ( rc )
        owner->AutoResizeIfNecessary();
    return rc;
}

bool wxStaticText::SetForegroundColour(const wxColour& colour)
{
    wxWinUIStaticTextImpl * const impl = m_winui.get();
    const wxWeakRef<wxStaticText> alive(this);
    const bool rc = wxControl::SetForegroundColour(colour);
    wxStaticText * const owner = alive.get();
    if ( owner && owner->m_winui.get() == impl )
        owner->ApplyWinUIAppearance();
    return rc;
}

bool wxStaticText::SetBackgroundColour(const wxColour& colour)
{
    wxWinUIStaticTextImpl * const impl = m_winui.get();
    const wxWeakRef<wxStaticText> alive(this);
    const bool rc = wxControl::SetBackgroundColour(colour);
    wxStaticText * const owner = alive.get();
    if ( owner && owner->m_winui.get() == impl )
        owner->ApplyWinUIAppearance();
    return rc;
}

#if wxUSE_MARKUP
bool wxStaticText::DoSetLabelMarkup(const wxString& markup)
{
    wxWinUIStaticTextImpl * const beforeImpl = m_winui.get();
    const wxWeakRef<wxStaticText> beforeAlive(this);
    if ( !wxControlBase::DoSetLabelMarkup(markup) )
        return false;
    wxStaticText * const afterBase = beforeAlive.get();
    if ( !afterBase || afterBase->m_winui.get() != beforeImpl )
        return true;

    // Keep the original markup so we can rebuild the rich runs, and remember
    // the stripped text for measuring.
    afterBase->m_markup = markup;
    afterBase->m_visibleLabel = afterBase->GetLabel();

    if ( !afterBase->HasFlag(wxST_NO_AUTORESIZE) )
        afterBase->InvalidateBestSize();
    if ( afterBase->UsesManualEllipsization() )
    {
        // START/MIDDLE ellipsization cannot preserve arbitrary nested markup.
        // Prefer a correctly clipped accessible label; setting a new markup
        // value or enlarging the control rebuilds from the original source.
        afterBase->m_visibleLabel = afterBase->GetEllipsizedLabel();
    }
    wxWinUIStaticTextImpl * const impl = afterBase->m_winui.get();
    if ( !afterBase->UpdateWinUIContent() )
        return true;
    wxStaticText * const owner = beforeAlive.get();
    if ( owner && owner->m_winui.get() == impl )
        owner->AutoResizeIfNecessary();
    return true;
}
#endif // wxUSE_MARKUP

wxSize wxStaticText::DoGetBestClientSize() const
{
    // Ask the TextBlock itself: wxGetTextExtent() measures with the classic GDI
    // font, which is noticeably narrower than the WinUI one, so every label
    // would end up clipped by a character or two.
    if ( m_winui )
    {
        const wxSize size = m_winui->host.MeasureContent();
        if ( size != wxDefaultSize )
            return size;
    }

    // Not realised yet: measure line by line with the GDI font, leaving enough
    // slack for the wider WinUI one until the real size becomes available.
    const wxString text = wxControl::GetLabelText(m_visibleLabel);

    wxSize best(0, 0);
    wxString remaining = text;
    for ( ;; )
    {
        const int nl = remaining.Find('\n');
        const wxString line = nl == wxNOT_FOUND ? remaining : remaining.Left(nl);

        const wxSize lineSize = GetTextExtent(line.empty() ? wxString(" ") : line);
        best.x = wxMax(best.x, lineSize.x);
        best.y += lineSize.y;

        if ( nl == wxNOT_FOUND )
            break;
        remaining = remaining.Mid(nl + 1);
    }

    // Scale rather than add a fixed margin: a fixed one is never enough for
    // long strings, which is exactly where the clipping was visible.
    best.x = best.x * 11 / 10 + FromDIP(4);
    best.y += FromDIP(8);
    return best;
}

void wxStaticText::DoSetSize(int x, int y, int width, int height,
                             int sizeFlags)
{
    const wxWeakRef<wxStaticText> alive(this);
    wxStaticTextBase::DoSetSize(x, y, width, height, sizeFlags);
    if ( wxStaticText * const owner = alive.get() )
    {
        if ( owner->UsesManualEllipsization() )
            owner->UpdateLabel();
    }
}

wxString wxStaticText::WXGetVisibleLabel() const
{
    return m_visibleLabel;
}

void wxStaticText::WXSetVisibleLabel(const wxString& str)
{
    m_visibleLabel = str;
    UpdateWinUIContent();
}

bool wxStaticText::UsesManualEllipsization() const
{
    return HasFlag(wxST_ELLIPSIZE_START) ||
           HasFlag(wxST_ELLIPSIZE_MIDDLE);
}

bool wxStaticText::ApplyWinUIAppearance()
{
    if ( !m_winui || !m_winui->root || !m_winui->textBlock )
        return false;

    wxWinUIStaticTextImpl * const impl = m_winui.get();
    const wxWeakRef<wxStaticText> alive(this);
    const MUX::Controls::Border root = impl->root;
    const MUX::Controls::TextBlock textBlock = impl->textBlock;
    const wxFont font = m_hasFont ? GetFont() : wxNullFont;
    const wxColour foreground =
        m_hasFgCol ? GetForegroundColour() : wxNullColour;
    const wxColour background =
        m_hasBgCol ? GetBackgroundColour() : wxNullColour;
    const wxString label = GetLabel();

    try
    {
        wxWinUIApplyFont(textBlock, font);
        wxWinUIApplyForeground(textBlock, foreground);
        wxWinUIApplyBackground(root, background);

        // Border has no useful automation peer. Keep it Raw and expose the
        // TextBlock peer, whose text is the native accessible value. The
        // shared slot adapter remains the only writer of the semantic
        // TextBlock's AutomationProperties.Name.
        wxWinUIApplyAccessKey(textBlock, label);
        MUX::Automation::AutomationProperties::SetAccessibilityView(
            root,
            MUX::Automation::Peers::AccessibilityView::Raw);
        MUX::Automation::AutomationProperties::SetAccessibilityView(
            textBlock,
            MUX::Automation::Peers::AccessibilityView::Control);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI StaticText appearance", e);
    }

    return alive && alive->m_winui.get() == impl;
}

bool wxStaticText::UpdateWinUIContent()
{
    if ( !m_winui || !m_winui->textBlock )
        return false;

    wxWinUIStaticTextImpl * const impl = m_winui.get();
    const wxWeakRef<wxStaticText> alive(this);
    const MUX::Controls::TextBlock textBlock = impl->textBlock;
#if wxUSE_MARKUP
    const wxString markup = m_markup;
#endif
    const wxString visibleLabel = m_visibleLabel;
    const bool manualEllipsization = UsesManualEllipsization();

    try
    {
        auto inlines = textBlock.Inlines();
        inlines.Clear();

#if wxUSE_MARKUP
        if ( !markup.empty() && !manualEllipsization )
        {
            wxWinUIMarkupToInlines output(inlines);
            wxMarkupParser parser(output);
            parser.Parse(markup);
        }
        else
#endif // wxUSE_MARKUP
        {
            wxWinUIAppendLabel(inlines, visibleLabel);
        }

        if ( !alive || alive->m_winui.get() != impl ||
             !alive->ApplyWinUIAppearance() )
        {
            return false;
        }
        textBlock.UpdateLayout();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TextBlock content", e);
    }

    wxStaticText * const owner = alive.get();
    if ( !owner || owner->m_winui.get() != impl )
        return false;

    // The host flush may destroy owner. It is the final implementation access;
    // only the weak reference is consulted afterwards.
    impl->host.ForceRender();
    return alive && alive->m_winui.get() == impl;
}

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUIStaticTestAccess::GetAppearance(
    const wxStaticText& text, wxWinUIAppearanceSnapshot *snapshot)
{
    if ( !snapshot || !text.m_winui || !text.m_winui->root ||
         !text.m_winui->textBlock )
        return false;

    try
    {
        *snapshot = wxWinUICaptureAppearance(
            text.m_winui->textBlock, text.m_winui->root,
            text.m_winui->textBlock);
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

wxString wxWinUIStaticTestAccess::GetVisibleLabel(const wxStaticText& text)
{
    return text.m_visibleLabel;
}

wxString wxWinUIStaticTestAccess::GetRenderedText(const wxStaticText& text)
{
    if ( !text.m_winui || !text.m_winui->textBlock )
        return wxString();

    wxString rendered;
    try
    {
        const auto inlines = text.m_winui->textBlock.Inlines();
        for ( std::uint32_t i = 0; i < inlines.Size(); ++i )
        {
            if ( const auto run = inlines.GetAt(i).try_as<MUXD::Run>() )
                rendered += wxString(run.Text().c_str());
            else if ( inlines.GetAt(i).try_as<MUXD::LineBreak>() )
                rendered += '\n';
        }
    }
    catch ( const winrt::hresult_error& )
    {
        return wxString();
    }

    return rendered;
}

int wxWinUIStaticTestAccess::GetTextTrimming(const wxStaticText& text)
{
    if ( !text.m_winui || !text.m_winui->textBlock )
        return -1;

    try
    {
        return static_cast<int>(text.m_winui->textBlock.TextTrimming());
    }
    catch ( const winrt::hresult_error& )
    {
        return -1;
    }
}

bool wxWinUIStaticTestAccess::HasLocalBoldInline(const wxStaticText& text)
{
    if ( !text.m_winui || !text.m_winui->textBlock )
        return false;

    try
    {
        const auto inlines = text.m_winui->textBlock.Inlines();
        for ( std::uint32_t i = 0; i < inlines.Size(); ++i )
        {
            const auto run = inlines.GetAt(i).try_as<MUXD::Run>();
            if ( run &&
                 run.ReadLocalValue(
                     MUXD::TextElement::FontWeightProperty()) !=
                     MUX::DependencyProperty::UnsetValue() &&
                 run.FontWeight().Weight >=
                     winrt::Microsoft::UI::Text::FontWeights::Bold().Weight )
            {
                return true;
            }
        }
    }
    catch ( const winrt::hresult_error& )
    {
    }

    return false;
}

bool wxWinUIStaticTestAccess::HasLocalUnderlineInline(const wxStaticText& text)
{
    if ( !text.m_winui || !text.m_winui->textBlock )
        return false;

    try
    {
        const auto inlines = text.m_winui->textBlock.Inlines();
        for ( std::uint32_t i = 0; i < inlines.Size(); ++i )
        {
            const auto run = inlines.GetAt(i).try_as<MUXD::Run>();
            if ( run &&
                 run.ReadLocalValue(
                     MUXD::TextElement::TextDecorationsProperty()) !=
                     MUX::DependencyProperty::UnsetValue() &&
                 run.TextDecorations() ==
                     winrt::Windows::UI::Text::TextDecorations::Underline )
            {
                return true;
            }
        }
    }
    catch ( const winrt::hresult_error& )
    {
    }

    return false;
}

#endif // WXWINUI_TEST_SUPPORT

#endif // wxUSE_STATTEXT
