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

#include "wx/arrstr.h"
#include "wx/clipbrd.h"
#include "wx/dataobj.h"

#include "private.h"

class wxWinUITextCtrlImpl
{
public:
    wxWinUIControlHost host;
    winrt::Microsoft::UI::Xaml::Controls::TextBox textBox{ nullptr };
    winrt::event_token textChangedToken{};
};

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

} // namespace

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
    // The WinUI TextBox draws its own border, so suppress the native control
    // border to avoid an extra grey frame around the island.
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

    try
    {
        m_winui->textBox = winrt::Microsoft::UI::Xaml::Controls::TextBox();
        m_winui->textBox.AcceptsReturn((style & wxTE_MULTILINE) != 0);
        m_winui->textBox.TextWrapping(
            (style & wxTE_DONTWRAP)
                ? winrt::Microsoft::UI::Xaml::TextWrapping::NoWrap
                : winrt::Microsoft::UI::Xaml::TextWrapping::Wrap);
        m_winui->textBox.IsReadOnly(!m_editable);

        m_winui->textChangedToken = m_winui->textBox.TextChanged(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs const&)
            {
                if ( !m_winui || m_updatingPeer )
                    return;

                m_value = wxWinUIFromHString(m_winui->textBox.Text());
                m_insertionPoint = m_value.length();
                m_selectionStart = m_selectionEnd = m_insertionPoint;
                m_modified = true;
                SendTextEvent();
            });

        // Text insertion is owned by the native WinUI TextBox; m_value is kept
        // in sync from TextChanged above.  Do NOT insert characters manually
        // from a KeyDown handler -- doing so double-inserts layout-dependent
        // keys (e.g. '(', '&', ''' on AZERTY) and triggers the Windows error
        // beep because the key message ends up handled twice.

        ApplyValueToPeer();
        m_winui->host.SetContent(m_winui->textBox);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TextBox creation", e);
        return false;
    }

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
}

void wxTextCtrl::Redo()
{
}

bool wxTextCtrl::CanUndo() const
{
    return false;
}

bool wxTextCtrl::CanRedo() const
{
    return false;
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

wxPoint wxTextCtrl::DoPositionToCoords(long WXUNUSED(pos)) const
{
    return wxDefaultPosition;
}

wxSize wxTextCtrl::DoGetBestSize() const
{
    // A WinUI TextBox needs a bit more than the classic 32px height for its
    // text not to be clipped by the control's internal padding.
    return wxWindow::FromDIP(IsMultiLine() ? wxSize(240, 96) : wxSize(180, 40),
                             const_cast<wxTextCtrl *>(this));
}

void wxTextCtrl::ApplyValueToPeer()
{
    if ( !m_winui || !m_winui->textBox )
        return;

    m_updatingPeer = true;
    m_winui->textBox.Text(wxWinUIToHString(m_value));
    m_winui->textBox.SelectionStart(static_cast<int32_t>(m_selectionStart));
    const long selectionLength = m_selectionEnd > m_selectionStart
        ? m_selectionEnd - m_selectionStart
        : m_selectionStart - m_selectionEnd;
    m_winui->textBox.SelectionLength(static_cast<int32_t>(selectionLength));
    m_updatingPeer = false;
    m_winui->host.ForceRender();
}

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
