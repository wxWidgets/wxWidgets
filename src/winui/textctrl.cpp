/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/textctrl.cpp
// Purpose:     wxWinUI wxTextCtrl implementation
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_TEXTCTRL

#include "wx/textctrl.h"
#include "wx/winui/winui.h"

#ifndef WX_PRECOMP
    #include "wx/font.h"
#endif

#include "wx/arrstr.h"
#include "wx/clipbrd.h"
#include "wx/dataobj.h"

#include "private.h"

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

#include <winrt/Microsoft.UI.Text.h>
#include <winrt/Windows.UI.Text.h>

#include <cmath>
#include <limits>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

namespace
{

long wxWinUIClampTextPos(long pos, long len)
{
    if ( pos < 0 )
        return len;
    if ( pos > len )
        return len;
    return pos;
}

// Apply a wxFont to a WinUI Control; no-op for an invalid font.
void wxWinUIApplyControlFont(const MUXC::Control& control, const wxFont& font)
{
    if ( !control || !font.IsOk() )
        return;

    const wxString face = font.GetFaceName();
    if ( !face.empty() )
        control.FontFamily(
            winrt::Microsoft::UI::Xaml::Media::FontFamily(wxWinUIToHString(face)));

    const double pt = font.GetFractionalPointSize();
    control.FontSize(pt > 0.0 ? pt * 96.0 / 72.0 : 14.0);
    control.FontWeight(font.GetNumericWeight() >= wxFONTWEIGHT_BOLD
        ? winrt::Microsoft::UI::Text::FontWeights::Bold()
        : winrt::Microsoft::UI::Text::FontWeights::Normal());
    control.FontStyle(font.GetStyle() == wxFONTSTYLE_NORMAL
        ? winrt::Windows::UI::Text::FontStyle::Normal
        : winrt::Windows::UI::Text::FontStyle::Italic);
}

} // namespace

// A wxTextCtrl is backed by a WinUI TextBox, except for password fields which
// use a PasswordBox (the TextBox has no masking).  The impl hides the
// difference behind a few small accessors.
class wxWinUITextCtrlImpl
{
public:
    wxWinUIControlHost host;
    MUXC::TextBox textBox{ nullptr };
    MUXC::PasswordBox passwordBox{ nullptr };
    winrt::event_token changedToken{};
    winrt::event_token keyDownToken{};
    winrt::event_token selectionChangedToken{};
    winrt::event_token maxLenKeyToken{};

    MUXC::Control control() const
    {
        if ( passwordBox )
            return passwordBox;
        return textBox;
    }

    MUX::UIElement element() const
    {
        if ( passwordBox )
            return passwordBox;
        return textBox;
    }

    wxString GetText() const
    {
        if ( passwordBox )
            return wxWinUIFromHString(passwordBox.Password());
        if ( textBox )
            return wxWinUIFromHString(textBox.Text());
        return wxString();
    }

    void SetText(const wxString& s)
    {
        if ( passwordBox )
            passwordBox.Password(wxWinUIToHString(s));
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

wxTextCtrl::~wxTextCtrl() = default;

bool wxTextCtrl::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxValidator& validator,
                        const wxString& name)
{
    // The WinUI control draws its own border, so suppress the native one.
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    m_value = value;
    m_insertionPoint = m_value.length();
    m_selectionStart = m_selectionEnd = m_insertionPoint;
    m_editable = (style & wxTE_READONLY) == 0;

    m_winui.reset(new wxWinUITextCtrlImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    const bool password = (style & wxTE_PASSWORD) != 0;
    const bool multiline = (style & wxTE_MULTILINE) != 0;

    try
    {
        if ( password )
        {
            m_winui->passwordBox = MUXC::PasswordBox();
            m_winui->changedToken = m_winui->passwordBox.PasswordChanged(
                [this](winrt::Windows::Foundation::IInspectable const&,
                       MUX::RoutedEventArgs const&)
                {
                    if ( !m_winui || m_updatingPeer )
                        return;
                    m_value = m_winui->GetText();
                    m_insertionPoint = m_value.length();
                    m_selectionStart = m_selectionEnd = m_insertionPoint;
                    m_modified = true;
                    SendTextEvent();
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

            // NB: do NOT set ScrollViewer.VerticalScrollBarVisibility on the
            // TextBox here -- doing so on a multiline TextBox hosted in a XAML
            // island crashes/deadlocks the island.  The multiline TextBox shows
            // its scrollbar automatically; wxTE_NO_VSCROLL is not honoured for
            // now (to be revisited).

            m_winui->textBox = textBox;
            m_winui->changedToken = textBox.TextChanged(
                [this](winrt::Windows::Foundation::IInspectable const&,
                       MUXC::TextChangedEventArgs const&)
                {
                    if ( !m_winui || m_updatingPeer )
                        return;
                    m_value = m_winui->GetText();
                    ReadSelectionFromPeer();
                    m_modified = true;
                    SendTextEvent();
                });

            // Keep the wx-side caret/selection in sync with the real one so
            // that GetInsertionPoint()/GetSelection() reflect user clicks.
            m_winui->selectionChangedToken = textBox.SelectionChanged(
                [this](winrt::Windows::Foundation::IInspectable const&,
                       MUX::RoutedEventArgs const&)
                {
                    if ( !m_winui || m_updatingPeer )
                        return;
                    ReadSelectionFromPeer();
                });
        }

        if ( m_maxLength )
        {
            if ( m_winui->textBox )
                m_winui->textBox.MaxLength(static_cast<int32_t>(m_maxLength));
            else if ( m_winui->passwordBox )
                m_winui->passwordBox.MaxLength(static_cast<int32_t>(m_maxLength));
        }

        // wxTE_PROCESS_ENTER: report Enter as wxEVT_TEXT_ENTER.
        if ( style & wxTE_PROCESS_ENTER )
        {
            m_winui->keyDownToken = m_winui->element().KeyDown(
                [this](winrt::Windows::Foundation::IInspectable const&,
                       MUX::Input::KeyRoutedEventArgs const& args)
                {
                    if ( args.Key() != winrt::Windows::System::VirtualKey::Enter )
                        return;

                    wxCommandEvent event(wxEVT_TEXT_ENTER, GetId());
                    event.SetEventObject(this);
                    event.SetString(m_value);
                    if ( GetEventHandler()->ProcessEvent(event) )
                        args.Handled(true);
                });
        }

        // wxEVT_TEXT_MAXLEN: report typing rejected at the length limit, as
        // wxMSW does on EN_MAXTEXT.
        m_winui->maxLenKeyToken = m_winui->element().KeyDown(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   MUX::Input::KeyRoutedEventArgs const& args)
            {
                if ( !m_winui || !m_maxLength ||
                     m_value.length() < m_maxLength )
                    return;

                // Ignore editing/navigation keys which are still accepted.
                using winrt::Windows::System::VirtualKey;
                switch ( args.Key() )
                {
                    case VirtualKey::Back:
                    case VirtualKey::Delete:
                    case VirtualKey::Left:
                    case VirtualKey::Right:
                    case VirtualKey::Up:
                    case VirtualKey::Down:
                    case VirtualKey::Home:
                    case VirtualKey::End:
                    case VirtualKey::PageUp:
                    case VirtualKey::PageDown:
                    case VirtualKey::Tab:
                    case VirtualKey::Enter:
                    case VirtualKey::Escape:
                    case VirtualKey::Shift:
                    case VirtualKey::Control:
                    case VirtualKey::Menu:
                    case VirtualKey::CapitalLock:
                        return;
                    default:
                        break;
                }

                // A non-empty selection would be replaced, not appended to.
                if ( m_selectionStart != m_selectionEnd )
                    return;

                wxCommandEvent event(wxEVT_TEXT_MAXLEN, GetId());
                event.SetEventObject(this);
                event.SetString(m_value);
                ProcessCommand(event);
            });

        ApplyValueToPeer();
        UpdateWinUIAppearance();
        ApplyToolTip();
        m_winui->host.SetContent(m_winui->element());
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TextBox creation", e);
        return false;
    }

    SetInitialSize(size);
    return true;
}

wxString wxTextCtrl::GetRange(long from, long to) const
{
    const long len = m_value.length();
    from = wxWinUIClampTextPos(from, len);
    to = wxWinUIClampTextPos(to, len);
    if ( to < from )
        return wxString();

    return m_value.Mid(from, to - from);
}

bool wxTextCtrl::IsEmpty() const
{
    return m_value.empty();
}

void wxTextCtrl::WriteText(const wxString& text)
{
    long from = m_selectionStart;
    long to = m_selectionEnd;
    if ( from > to )
        wxSwap(from, to);

    if ( from == to )
        from = to = m_insertionPoint;

    const wxString newValue = m_value.Left(from) + text + m_value.Mid(to);
    DoSetValue(newValue, SetValue_SendEvent);
    m_insertionPoint = from + text.length();
    m_selectionStart = m_selectionEnd = m_insertionPoint;
    ApplyValueToPeer();
}

void wxTextCtrl::AppendText(const wxString& text)
{
    SetInsertionPointEnd();
    WriteText(text);
}

void wxTextCtrl::Remove(long from, long to)
{
    const long len = m_value.length();
    from = wxWinUIClampTextPos(from, len);
    to = to < 0 ? len : wxWinUIClampTextPos(to, len);
    if ( to < from )
        wxSwap(from, to);

    DoSetValue(m_value.Left(from) + m_value.Mid(to), SetValue_SendEvent);
    m_insertionPoint = from;
    m_selectionStart = m_selectionEnd = from;
    ApplyValueToPeer();
}

void wxTextCtrl::Clear()
{
    Remove(0, -1);
}

void wxTextCtrl::Copy()
{
#if wxUSE_CLIPBOARD
    if ( !HasSelection() || !wxTheClipboard )
        return;

    if ( wxTheClipboard->Open() )
    {
        wxTheClipboard->SetData(new wxTextDataObject(GetStringSelection()));
        wxTheClipboard->Close();
    }
#endif
}

void wxTextCtrl::Cut()
{
    if ( !m_editable )
        return;

    Copy();
    RemoveSelection();
}

void wxTextCtrl::Paste()
{
#if wxUSE_CLIPBOARD
    if ( !m_editable || !wxTheClipboard )
        return;

    if ( wxTheClipboard->Open() )
    {
        if ( wxTheClipboard->IsSupported(wxDF_TEXT) )
        {
            wxTextDataObject data;
            if ( wxTheClipboard->GetData(data) )
                WriteText(data.GetText());
        }
        wxTheClipboard->Close();
    }
#endif
}

void wxTextCtrl::Undo()
{
    if ( m_winui && m_winui->textBox )
        m_winui->textBox.Undo();
}

void wxTextCtrl::Redo()
{
    if ( m_winui && m_winui->textBox )
        m_winui->textBox.Redo();
}

bool wxTextCtrl::CanUndo() const
{
    return m_winui && m_winui->textBox && m_winui->textBox.CanUndo();
}

bool wxTextCtrl::CanRedo() const
{
    return m_winui && m_winui->textBox && m_winui->textBox.CanRedo();
}

void wxTextCtrl::SetInsertionPoint(long pos)
{
    m_insertionPoint = wxWinUIClampTextPos(pos, m_value.length());
    m_selectionStart = m_selectionEnd = m_insertionPoint;
    ApplyValueToPeer();
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
    return m_value.length();
}

void wxTextCtrl::SetSelection(long from, long to)
{
    const long len = m_value.length();
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
    m_insertionPoint = to;
    ApplyValueToPeer();
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
    if ( m_winui && m_winui->textBox )
        m_winui->textBox.IsReadOnly(!editable);
}

void wxTextCtrl::SetMaxLength(unsigned long len)
{
    m_maxLength = len;
    if ( m_winui && m_winui->textBox )
        m_winui->textBox.MaxLength(static_cast<int32_t>(len));
    else if ( m_winui && m_winui->passwordBox )
        m_winui->passwordBox.MaxLength(static_cast<int32_t>(len));
}

int wxTextCtrl::GetLineLength(long lineNo) const
{
    const wxArrayString lines = GetLines();
    if ( lineNo < 0 || static_cast<size_t>(lineNo) >= lines.size() )
        return 0;

    return lines[lineNo].length();
}

wxString wxTextCtrl::GetLineText(long lineNo) const
{
    const wxArrayString lines = GetLines();
    if ( lineNo < 0 || static_cast<size_t>(lineNo) >= lines.size() )
        return wxString();

    return lines[lineNo];
}

int wxTextCtrl::GetNumberOfLines() const
{
    return GetLines().size();
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
    const int keycode = event.GetUnicodeKey();
    if ( keycode == WXK_NONE )
        return false;

    WriteText(wxString(static_cast<wxUniChar>(keycode)));
    return true;
}

long wxTextCtrl::XYToPosition(long x, long y) const
{
    const wxArrayString lines = GetLines();
    if ( y < 0 || static_cast<size_t>(y) >= lines.size() )
        return wxInvalidTextCoord;

    long pos = 0;
    for ( long n = 0; n < y; ++n )
        pos += lines[n].length() + 1;

    return pos + wxMin<long>(x, lines[y].length());
}

bool wxTextCtrl::PositionToXY(long pos, long *x, long *y) const
{
    if ( pos < 0 || pos > GetLastPosition() )
        return false;

    const wxArrayString lines = GetLines();
    long current = 0;
    for ( size_t n = 0; n < lines.size(); ++n )
    {
        const long lineLen = lines[n].length();
        if ( pos <= current + lineLen )
        {
            if ( x )
                *x = pos - current;
            if ( y )
                *y = n;
            return true;
        }
        current += lineLen + 1;
    }

    return false;
}

void wxTextCtrl::ShowPosition(long pos)
{
    SetInsertionPoint(pos);
}

wxTextCtrlHitTestResult wxTextCtrl::HitTest(const wxPoint& WXUNUSED(pt), long *pos) const
{
    if ( pos )
        *pos = wxOutOfRangeTextCoord;
    return wxTE_HT_UNKNOWN;
}

void wxTextCtrl::DoEnable(bool enable)
{
    wxControl::DoEnable(enable);

    if ( m_winui && m_winui->control() )
    {
        m_winui->control().IsEnabled(enable);
        m_winui->host.ForceRender();
    }
}

void wxTextCtrl::DoSetValue(const wxString& value, int flags)
{
    m_value = value;
    if ( m_maxLength && m_value.length() > m_maxLength )
        m_value.Truncate(m_maxLength);

    ClampInsertionPoint();
    m_selectionStart = m_selectionEnd = m_insertionPoint;
    m_modified = true;
    ApplyValueToPeer();

    if ( flags & SetValue_SendEvent )
        SendTextEvent();
}

wxString wxTextCtrl::DoGetValue() const
{
    return m_value;
}

wxPoint wxTextCtrl::DoPositionToCoords(long pos) const
{
    if ( m_winui && m_winui->textBox )
    {
        try
        {
            const auto rect = m_winui->textBox.GetRectFromCharacterIndex(
                static_cast<int32_t>(wxWinUIClampTextPos(pos, m_value.length())),
                false);
            return wxPoint(static_cast<int>(std::lround(rect.X)),
                           static_cast<int>(std::lround(rect.Y)));
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }

    return wxDefaultPosition;
}

wxSize wxTextCtrl::DoGetBestSize() const
{
    if ( IsMultiLine() )
        return wxWindow::FromDIP(wxSize(180, 90), const_cast<wxTextCtrl *>(this));

    // Single line: use the WinUI control's own (smaller) natural height instead
    // of a hard-coded, too-tall value.
    int height = FromDIP(32);
    if ( m_winui && m_winui->control() )
    {
        try
        {
            const float inf = std::numeric_limits<float>::infinity();
            m_winui->control().Measure({ inf, inf });
            const auto desired = m_winui->control().DesiredSize();
            if ( desired.Height > 0 )
                height = static_cast<int>(std::ceil(desired.Height));
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }

    return wxSize(FromDIP(120), height);
}

void wxTextCtrl::ApplyValueToPeer()
{
    if ( !m_winui )
        return;

    m_updatingPeer = true;
    try
    {
        m_winui->SetText(m_value);

        // Selection only applies to the TextBox (PasswordBox has no API).
        if ( m_winui->textBox )
        {
            m_winui->textBox.SelectionStart(static_cast<int32_t>(m_selectionStart));
            const long selectionLength = m_selectionEnd > m_selectionStart
                ? m_selectionEnd - m_selectionStart
                : m_selectionStart - m_selectionEnd;
            m_winui->textBox.SelectionLength(static_cast<int32_t>(selectionLength));
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TextBox value", e);
    }
    m_updatingPeer = false;
    m_winui->host.ForceRender();
}

void wxTextCtrl::ReadSelectionFromPeer()
{
    if ( !m_winui || !m_winui->textBox )
        return;

    try
    {
        const long start = m_winui->textBox.SelectionStart();
        const long length = m_winui->textBox.SelectionLength();
        m_selectionStart = start;
        m_selectionEnd = start + length;
        m_insertionPoint = m_selectionEnd;
    }
    catch ( const winrt::hresult_error& )
    {
    }
}

void wxTextCtrl::UpdateWinUIAppearance()
{
    if ( !m_winui || !m_winui->control() )
        return;

    try
    {
        const MUXC::Control control = m_winui->control();

        if ( m_hasFont )
            wxWinUIApplyControlFont(control, GetFont());
        else
            control.ClearValue(MUXC::Control::FontSizeProperty());

        if ( UseForegroundColour() )
        {
            const wxColour& c = GetForegroundColour();
            control.Foreground(wxWinUIBrush(c.Red(), c.Green(), c.Blue(), c.Alpha()));
        }
        else
        {
            control.ClearValue(MUXC::Control::ForegroundProperty());
        }

        if ( UseBackgroundColour() )
        {
            const wxColour& c = GetBackgroundColour();
            control.Background(wxWinUIBrush(c.Red(), c.Green(), c.Blue(), c.Alpha()));
        }
        else
        {
            control.ClearValue(MUXC::Control::BackgroundProperty());
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TextBox appearance", e);
    }

    m_winui->host.ForceRender();
}

void wxTextCtrl::ApplyToolTip()
{
#if wxUSE_TOOLTIPS
    if ( m_winui && m_winui->element() )
        wxWinUISetToolTip(m_winui->element(), m_tooltipText);
#endif // wxUSE_TOOLTIPS
}

bool wxTextCtrl::SetFont(const wxFont& font)
{
    const bool rc = wxControl::SetFont(font);
    InvalidateBestSize();
    UpdateWinUIAppearance();
    return rc;
}

bool wxTextCtrl::SetForegroundColour(const wxColour& colour)
{
    const bool rc = wxControl::SetForegroundColour(colour);
    UpdateWinUIAppearance();
    return rc;
}

bool wxTextCtrl::SetBackgroundColour(const wxColour& colour)
{
    const bool rc = wxControl::SetBackgroundColour(colour);
    UpdateWinUIAppearance();
    return rc;
}

#if wxUSE_TOOLTIPS
void wxTextCtrl::DoSetToolTipText(const wxString& tip)
{
    m_tooltipText = tip;
    ApplyToolTip();
}

void wxTextCtrl::DoSetToolTip(wxToolTip *tip)
{
    m_tooltipText = tip ? tip->GetTip() : wxString();
    delete tip;
    ApplyToolTip();
}
#endif // wxUSE_TOOLTIPS

void wxTextCtrl::SendTextEvent()
{
    wxCommandEvent event(wxEVT_TEXT, GetId());
    event.SetEventObject(this);
    event.SetString(m_value);
    ProcessCommand(event);
}

void wxTextCtrl::ClampInsertionPoint()
{
    m_insertionPoint = wxWinUIClampTextPos(m_insertionPoint, m_value.length());
}

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
