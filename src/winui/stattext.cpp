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

#include "private.h"

#if wxUSE_MARKUP
    #include "wx/private/markupparser.h"
#endif

#include <winrt/Microsoft.UI.Xaml.Documents.h>
#include <winrt/Microsoft.UI.Text.h>
#include <winrt/Windows.UI.Text.h>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXD = winrt::Microsoft::UI::Xaml::Documents;

class wxWinUIStaticTextImpl
{
public:
    wxWinUIControlHost host;
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
                run.Foreground(wxWinUIBrush(fg.Red(), fg.Green(), fg.Blue(),
                                            fg.Alpha()));
            inlines.Append(run);
        }

        if ( nl == wxNOT_FOUND )
            break;
        remaining = remaining.Mid(nl + 1);
    }
}

// Append a plain label, underlining the mnemanic character (the one following
// a single '&') as classic wxStaticText does.
void wxWinUIAppendLabel(InlineCollection const& inlines, const wxString& label)
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
            wxWinUIAppendStyledText(inlines, chunk, false, false, false, wxColour());
            chunk.clear();
            wxWinUIAppendStyledText(inlines, wxString(next), false, false, true, wxColour());
            ++i;
            continue;
        }

        chunk += ch;
    }

    wxWinUIAppendStyledText(inlines, chunk, false, false, false, wxColour());
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
        wxWinUIAppendStyledText(m_inlines, text,
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

wxStaticText::~wxStaticText() = default;

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

    try
    {
        m_winui->textBlock = MUX::Controls::TextBlock();
        // Multi-line labels (embedded newlines) are rendered via explicit line
        // breaks in BuildInlines(), so disable wrapping.
        m_winui->textBlock.TextWrapping(MUX::TextWrapping::NoWrap);
        m_winui->textBlock.VerticalAlignment(MUX::VerticalAlignment::Top);

        MUX::TextAlignment align = MUX::TextAlignment::Left;
        if ( style & wxALIGN_RIGHT )
            align = MUX::TextAlignment::Right;
        else if ( style & wxALIGN_CENTRE_HORIZONTAL )
            align = MUX::TextAlignment::Center;
        m_winui->textBlock.TextAlignment(align);

        if ( style & (wxST_ELLIPSIZE_START | wxST_ELLIPSIZE_MIDDLE |
                      wxST_ELLIPSIZE_END) )
            m_winui->textBlock.TextTrimming(MUX::TextTrimming::CharacterEllipsis);

        UpdateWinUIContent();
        m_winui->host.SetContent(m_winui->textBlock);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TextBlock creation", e);
        return false;
    }

    return true;
}

void wxStaticText::SetLabel(const wxString& label)
{
    m_markup.clear();

    if ( !UpdateLabelOrig(label) )
        return;

    InvalidateBestSize();
    WXSetVisibleLabel(label);
    if ( GetContainingSizer() )
        Refresh();
    else
        AutoResizeIfNecessary();
}

bool wxStaticText::SetFont(const wxFont& font)
{
    const bool rc = wxControl::SetFont(font);
    UpdateWinUIContent();
    AutoResizeIfNecessary();
    return rc;
}

#if wxUSE_MARKUP
bool wxStaticText::DoSetLabelMarkup(const wxString& markup)
{
    if ( !wxControlBase::DoSetLabelMarkup(markup) )
        return false;

    // Keep the original markup so we can rebuild the rich runs, and remember
    // the stripped text for measuring.
    m_markup = markup;
    m_visibleLabel = GetLabel();

    InvalidateBestSize();
    UpdateWinUIContent();
    AutoResizeIfNecessary();
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

wxString wxStaticText::WXGetVisibleLabel() const
{
    return m_visibleLabel;
}

void wxStaticText::WXSetVisibleLabel(const wxString& str)
{
    m_visibleLabel = str;
    UpdateWinUIContent();
}

void wxStaticText::UpdateWinUIContent()
{
    if ( !m_winui || !m_winui->textBlock )
        return;

    try
    {
        auto inlines = m_winui->textBlock.Inlines();
        inlines.Clear();

#if wxUSE_MARKUP
        if ( !m_markup.empty() )
        {
            wxWinUIMarkupToInlines output(inlines);
            wxMarkupParser parser(output);
            parser.Parse(m_markup);
        }
        else
#endif // wxUSE_MARKUP
        {
            wxWinUIAppendLabel(inlines, m_visibleLabel);
        }

        m_winui->textBlock.UpdateLayout();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TextBlock content", e);
    }

    m_winui->host.ForceRender();
}

#endif // wxUSE_STATTEXT
