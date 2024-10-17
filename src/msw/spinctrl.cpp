/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/spinctrl.cpp
// Purpose:     wxSpinCtrl class implementation for Win32
// Author:      Vadim Zeitlin
// Created:     22.07.99
// Copyright:   (c) 1999-2005 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_SPINCTRL

#include "wx/spinctrl.h"

#ifndef WX_PRECOMP
    #include "wx/msw/wrapcctl.h" // include <commctrl.h> "properly"
    #include "wx/event.h"
    #include "wx/textctrl.h"
    #include "wx/wxcrtvararg.h"
#endif

#include "wx/private/spinctrl.h"

#include "wx/msw/private.h"
#include "wx/msw/private/winstyle.h"

#include "wx/scopeguard.h"

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif // wxUSE_TOOLTIPS

#include <limits.h>         // for INT_MIN

#include <unordered_map>

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(wxSpinCtrl, wxSpinButton)
    EVT_CHAR(wxSpinCtrl::OnChar)
    EVT_SET_FOCUS(wxSpinCtrl::OnSetFocus)
    EVT_KILL_FOCUS(wxSpinCtrl::OnKillFocus)
wxEND_EVENT_TABLE()

#define GetBuddyHwnd()      (HWND)(m_hwndBuddy)


// ---------------------------------------------------------------------------
// global vars
// ---------------------------------------------------------------------------

namespace
{

// Global hash used to find the spin control corresponding to the given buddy
// text control HWND.
using SpinForTextCtrl = std::unordered_map<HWND, wxSpinCtrl*>;

SpinForTextCtrl gs_spinForTextCtrl;

} // anonymous namespace

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wnd proc for the buddy text ctrl
// ----------------------------------------------------------------------------

LRESULT APIENTRY
wxBuddyTextWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    wxSpinCtrl * const spin = wxSpinCtrl::GetSpinForTextCtrl(hwnd);

    // forward some messages (mostly the key and focus ones) to the spin ctrl
    switch ( message )
    {
        case WM_SETFOCUS:
            // if the focus comes from the spin control itself, don't set it
            // back to it -- we don't want to go into an infinite loop
            if ( (WXHWND)wParam == spin->GetHWND() )
                break;
            //else: fall through
            wxFALLTHROUGH;

        case WM_KILLFOCUS:
        case WM_CHAR:
        case WM_DEADCHAR:
        case WM_KEYUP:
        case WM_KEYDOWN:
        // we need to forward WM_HELP too to ensure that the context help
        // associated with wxSpinCtrl is shown when the text control part of it
        // is clicked with the "?" cursor
        case WM_HELP:
            {
                WXLRESULT result;
                if ( spin->MSWHandleMessage(&result, message, wParam, lParam) )
                {
                    // Do not let the message be processed by the window proc
                    // of the text control if it had been already handled at wx
                    // level, this is consistent with what happens for normal,
                    // non-composite controls.
                    return 0;
                }

                // The control may have been deleted at this point, so check.
                if ( !::IsWindow(hwnd) )
                    return 0;
            }
            break;

        case WM_GETDLGCODE:
            if ( spin->HasFlag(wxTE_PROCESS_ENTER) )
            {
                long dlgCode = ::CallWindowProc
                                 (
                                    CASTWNDPROC spin->GetBuddyWndProc(),
                                    hwnd,
                                    message,
                                    wParam,
                                    lParam
                                 );
                dlgCode |= DLGC_WANTMESSAGE;
                return dlgCode;
            }
            break;
    }

    return ::CallWindowProc(CASTWNDPROC spin->GetBuddyWndProc(),
                            hwnd, message, wParam, lParam);
}

/* static */
wxSpinCtrl *wxSpinCtrl::GetSpinForTextCtrl(WXHWND hwndBuddy)
{
    const SpinForTextCtrl::const_iterator
        it = gs_spinForTextCtrl.find(hwndBuddy);
    if ( it == gs_spinForTextCtrl.end() )
        return nullptr;

    wxSpinCtrl * const spin = it->second;

    // sanity check
    wxASSERT_MSG( spin->m_hwndBuddy == hwndBuddy,
                  wxT("wxSpinCtrl has incorrect buddy HWND!") );

    return spin;
}

// process a WM_COMMAND generated by the buddy text control
bool wxSpinCtrl::ProcessTextCommand(WXWORD cmd, WXWORD WXUNUSED(id))
{
    if ( (cmd == EN_CHANGE) && (!m_blockEvent ))
    {
        wxCommandEvent event(wxEVT_TEXT, GetId());
        event.SetEventObject(this);
        wxString val = wxGetWindowText(m_hwndBuddy);
        event.SetString(val);
        event.SetInt(GetValue());
        return HandleWindowEvent(event);
    }

    // not processed
    return false;
}

void wxSpinCtrl::OnChar(wxKeyEvent& event)
{
    switch ( event.GetKeyCode() )
    {
        case WXK_RETURN:
            {
                wxCommandEvent evt(wxEVT_TEXT_ENTER, m_windowId);
                InitCommandEvent(evt);
                wxString val = wxGetWindowText(m_hwndBuddy);
                evt.SetString(val);
                evt.SetInt(GetValue());
                if ( HandleWindowEvent(evt) )
                    return;
                break;
            }

        case WXK_TAB:
            // always produce navigation event - even if we process TAB
            // ourselves the fact that we got here means that the user code
            // decided to skip processing of this TAB - probably to let it
            // do its default job.
            {
                wxNavigationKeyEvent eventNav;
                eventNav.SetDirection(!event.ShiftDown());
                eventNav.SetWindowChange(event.ControlDown());
                eventNav.SetEventObject(this);

                if ( GetParent()->HandleWindowEvent(eventNav) )
                    return;
            }
            break;
    }

    // no, we didn't process it
    event.Skip();
}

void wxSpinCtrl::OnKillFocus(wxFocusEvent& event)
{
    // ensure that a correct value is shown by the control
    NormalizeValue();
    event.Skip();
}

void wxSpinCtrl::MSWUpdateFontOnDPIChange(const wxSize& newDPI)
{
    wxSpinButton::MSWUpdateFontOnDPIChange(newDPI);

    if ( m_font.IsOk() )
        wxSetWindowFont(GetBuddyHwnd(), m_font);
}

void wxSpinCtrl::OnSetFocus(wxFocusEvent& event)
{
    // when we get focus, give it to our buddy window as it needs it more than
    // we do
    ::SetFocus((HWND)m_hwndBuddy);

    event.Skip();
}

void wxSpinCtrl::NormalizeValue()
{
    const int value = GetValue();
    const bool changed = value != m_oldValue;

    // notice that we have to call SetValue() even if the value didn't change
    // because otherwise we could be left with empty buddy control when value
    // is 0, see comment in SetValue()
    SetValue(value);

    if ( changed )
    {
        m_oldValue = value;
        SendSpinUpdate(value);
    }
}

// ----------------------------------------------------------------------------
// construction
// ----------------------------------------------------------------------------

void wxSpinCtrl::Init()
{
    m_blockEvent = false;
    m_hwndBuddy = nullptr;
    m_wndProcBuddy = nullptr;
    m_oldValue = INT_MIN;
}

bool wxSpinCtrl::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        int min, int max, int initial,
                        const wxString& name)
{
    // set style for the base class
    style |= wxSP_VERTICAL;

    // the border is only used for the text control part
    if ( (style & wxBORDER_MASK) == wxBORDER_DEFAULT )
        style |= DoTranslateBorder(wxBORDER_THEME);

    SetWindowStyle(style);

    WXDWORD exStyle = 0;
    WXDWORD msStyle = MSWGetStyle(GetWindowStyle(), & exStyle) ;

    // Scroll text automatically if there is not enough space to show all of
    // it, this is better than not allowing to enter more digits at all.
    msStyle |= ES_AUTOHSCROLL;

    // propagate text alignment style to text ctrl
    if ( style & wxALIGN_RIGHT )
        msStyle |= ES_RIGHT;
    else if ( style & wxALIGN_CENTER )
        msStyle |= ES_CENTER;

    // we must create the text control before the spin button for the purpose
    // of the dialog navigation: if there is a static text just before the spin
    // control, activating it by Alt-letter should give focus to the text
    // control, not the spin and the dialog navigation code will give focus to
    // the next control (at Windows level), not the one after it

    // create the text window

    m_hwndBuddy = MSWCreateWindowAtAnyPosition
                  (
                   exStyle,                // sunken border
                   wxT("EDIT"),            // window class
                   nullptr,                // no window title
                   msStyle,                // style (will be shown later)
                   pos.x, pos.y,           // position
                   0, 0,                   // size (will be set later)
                   GetHwndOf(parent),      // parent
                   -1                      // control id
                  );

    if ( !m_hwndBuddy )
    {
        wxLogLastError(wxT("CreateWindow(buddy text window)"));

        return false;
    }


    // create the spin button without any border as it doesn't make sense for
    // it (even if it doesn't seem to be actually taken into account anyhow)
    if ( !wxSpinButton::Create(parent, id, pos, wxSize(0, 0),
                               (style & ~wxBORDER_MASK) | wxBORDER_NONE, name) )
    {
        return false;
    }

    wxSpinButtonBase::SetRange(min, max);

    // subclass the text ctrl to be able to intercept some events
    gs_spinForTextCtrl[GetBuddyHwnd()] = this;

    m_wndProcBuddy = wxSetWindowProc(GetBuddyHwnd(), wxBuddyTextWndProc);

    // associate the text window with the spin button
    (void)::SendMessage(GetHwnd(), UDM_SETBUDDY, (WPARAM)m_hwndBuddy, 0);

    // set up fonts and colours  (This is nomally done in MSWCreateControl)
    InheritAttributes();
    if (!m_hasFont)
        SetFont(GetDefaultAttributes().font);

    // If the initial text value is actually a number, it overrides the
    // "initial" argument specified later.
    long initialFromText;
    if ( value.ToLong(&initialFromText) )
        initial = initialFromText;

    // Set the range in the native control: notice that we must do it before
    // calling SetValue() to use the correct validity checks for the initial
    // value.
    SetRange(min, max);
    SetValue(initial);

    // Also set the text part of the control if it was specified independently.
    if ( !value.empty() )
        SetValue(value);

    // Finally deal with the size: notice that this can only be done now both
    // windows are created and the text one is set up as buddy because
    // UDM_SETBUDDY changes its size using some unknown algorithm, so setting
    // the sizes earlier is useless. Do it after setting the range and the base
    // because GetBestSize() uses them.
    if ( size.x > 0 && size.x < GetBestSize().x )
    {
        wxLogDebug(wxS("wxSpinCtrl \"%s\": initial width %d is too small, ")
                   wxS("at least %d pixels needed."),
                   name, size.x, GetBestSize().x);
    }

    SetInitialSize(size);

    (void)::ShowWindow(GetBuddyHwnd(), SW_SHOW);

    return true;
}

wxSpinCtrl::~wxSpinCtrl()
{
    // destroy the buddy window because this pointer which wxBuddyTextWndProc
    // uses will not soon be valid any more
    ::DestroyWindow( GetBuddyHwnd() );

    gs_spinForTextCtrl.erase(GetBuddyHwnd());
}

void wxSpinCtrl::Refresh(bool eraseBackground, const wxRect *rect)
{
    wxControl::Refresh(eraseBackground, rect);

    UINT flags = RDW_INVALIDATE;
    if ( eraseBackground )
        flags |= RDW_ERASE;

    // Don't bother computing the intersection of the given rectangle with the
    // buddy control, just always refresh it entirely, as it's much simpler.
    ::RedrawWindow(GetBuddyHwnd(), nullptr, nullptr, flags);
}

// ----------------------------------------------------------------------------
// wxSpinCtrl-specific methods
// ----------------------------------------------------------------------------

int wxSpinCtrl::GetBase() const
{
    return ::SendMessage(GetHwnd(), UDM_GETBASE, 0, 0);
}

bool wxSpinCtrl::SetBase(int base)
{
    // For negative values in the range only base == 10 is allowed
    if ( !wxSpinCtrlImpl::IsBaseCompatibleWithRange(m_min, m_max, base) )
        return false;

    if ( !::SendMessage(GetHwnd(), UDM_SETBASE, base, 0) )
        return false;

    // DoGetBestSize uses the base.
    InvalidateBestSize();

    // Whether we need to be able enter "x" or not influences whether we should
    // use ES_NUMBER for the buddy control.
    UpdateBuddyStyle();

    // Update the displayed text after changing the base it uses.
    SetValue(GetValue());

    return true;
}

// ----------------------------------------------------------------------------
// wxTextCtrl-like methods
// ----------------------------------------------------------------------------

wxString wxSpinCtrl::GetTextValue() const
{
    return wxGetWindowText(m_hwndBuddy);
}

void wxSpinCtrl::SetValue(const wxString& text)
{
    m_blockEvent = true;
    wxON_BLOCK_EXIT_SET(m_blockEvent, false);

    if ( !::SetWindowText(GetBuddyHwnd(), text.c_str()) )
    {
        wxLogLastError(wxT("SetWindowText(buddy)"));
    }
}

void  wxSpinCtrl::SetValue(int val)
{
    m_blockEvent = true;
    wxON_BLOCK_EXIT_SET(m_blockEvent, false);

    wxSpinButton::SetValue(val);

    // Normally setting the value of the spin button is enough as it updates
    // its buddy control automatically but in a couple of situations it doesn't
    // do it, for whatever reason, do it explicitly then:
    const wxString text = wxGetWindowText(m_hwndBuddy);

    // First case is when the text control is empty and the value is 0: the
    // spin button just leaves it empty in this case, while we want to show 0
    // in it.
    if ( text.empty() && !val )
    {
        ::SetWindowText(GetBuddyHwnd(), wxT("0"));
    }

    // Another one is when we're using hexadecimal base but the user input
    // doesn't start with "0x" -- we prefer to show it to avoid ambiguity
    // between decimal and hexadecimal.
    if ( GetBase() == 16 &&
            (text.length() < 3 || text[0] != '0' ||
                (text[1] != 'x' && text[1] != 'X')) )
    {
        ::SetWindowText(GetBuddyHwnd(),
                        wxSpinCtrlImpl::FormatAsHex(val, m_max).t_str());
    }

    m_oldValue = GetValue();
}

int wxSpinCtrl::GetValue() const
{
    const wxString val = wxGetWindowText(m_hwndBuddy);

    long n;
    if ( !val.ToLong(&n, GetBase()) )
        n = INT_MIN;

    if ( n < m_min )
        n = m_min;
    if ( n > m_max )
        n = m_max;

    return n;
}

void wxSpinCtrl::SetSelection(long from, long to)
{
    // if from and to are both -1, it means (in wxWidgets) that all text should
    // be selected - translate into Windows convention
    if ( (from == -1) && (to == -1) )
    {
        from = 0;
    }

    ::SendMessage(GetBuddyHwnd(), EM_SETSEL, (WPARAM)from, (LPARAM)to);
}

void wxSpinCtrl::SetLayoutDirection(wxLayoutDirection dir)
{
    // Buddy text field is plain EDIT control so we need to set its layout
    // direction in a specific way.
    wxUpdateEditLayoutDirection(GetBuddyHwnd(), dir);

    wxSpinButton::SetLayoutDirection(dir);

    // Reposition the child windows according to the new layout.
    SetSize(-1, -1, -1, -1, wxSIZE_AUTO | wxSIZE_FORCE);
}

WXHWND wxSpinCtrl::MSWGetFocusHWND() const
{
    // Return the buddy hwnd because it shuld be focused instead of the
    // wxSpinCtrl itself.
    return m_hwndBuddy;
}

// ----------------------------------------------------------------------------
// wxSpinButton methods
// ----------------------------------------------------------------------------

void wxSpinCtrl::SetRange(int minVal, int maxVal)
{
    // Negative values in the range are allowed only if base == 10
    if ( !wxSpinCtrlImpl::IsBaseCompatibleWithRange(minVal, maxVal, GetBase()) )
    {
        return;
    }

    // Manually adjust the old value to avoid an event being sent from
    // NormalizeValue() called from inside the base class SetRange() as we're
    // not supposed to generate any events from here.
    if ( minVal <= maxVal )
    {
        if ( m_oldValue < minVal )
            m_oldValue = minVal;
        else if ( m_oldValue > maxVal )
            m_oldValue = maxVal;
    }
    else // reversed range
    {
        if ( m_oldValue > minVal )
            m_oldValue = minVal;
        else if ( m_oldValue < maxVal )
            m_oldValue = maxVal;
    }

    wxSpinButton::SetRange(minVal, maxVal);

    InvalidateBestSize();

    UpdateBuddyStyle();
}

void wxSpinCtrl::UpdateBuddyStyle()
{
    // this control is used for numeric entry so restrict the input to numeric
    // keys only -- but only if we don't need to be able to enter "-" in it as
    // otherwise this would become impossible and also if we don't use
    // hexadecimal as entering "x" of the "0x" prefix wouldn't be allowed
    // either then
    wxMSWWinStyleUpdater(GetBuddyHwnd())
        .TurnOnOrOff(m_min >= 0 && GetBase() == 10, ES_NUMBER);
}

// ----------------------------------------------------------------------------
// forward some methods to subcontrols
// ----------------------------------------------------------------------------

bool wxSpinCtrl::SetFont(const wxFont& font)
{
    if ( !wxWindowBase::SetFont(font) )
    {
        // nothing to do
        return false;
    }

    if ( m_font.IsOk() )
        wxSetWindowFont(GetBuddyHwnd(), m_font);

    return true;
}

bool wxSpinCtrl::Show(bool show)
{
    if ( !wxControl::Show(show) )
    {
        return false;
    }

    ::ShowWindow(GetBuddyHwnd(), show ? SW_SHOW : SW_HIDE);

    return true;
}

bool wxSpinCtrl::Reparent(wxWindowBase *newParent)
{
    // Reparenting both the updown control and its buddy does not seem to work:
    // they continue to be connected somehow, but visually there is no feedback
    // on the buddy edit control. To avoid this problem, we reparent the buddy
    // window normally, but we recreate the updown control and reassign its
    // buddy.

    // Get the position before changing the parent as it would be offset after
    // changing it.
    const wxRect rect = GetRect();

    if ( !wxWindowBase::Reparent(newParent) )
        return false;

    newParent->GetChildren().DeleteObject(this);

    // destroy the old spin button after detaching it from this wxWindow object
    // (notice that m_hWnd will be reset by UnsubclassWin() so save it first)
    const HWND hwndOld = GetHwnd();
    UnsubclassWin();
    if ( !::DestroyWindow(hwndOld) )
    {
        wxLogLastError(wxT("DestroyWindow"));
    }

    // create and initialize the new one
    if ( !wxSpinButton::Create(GetParent(), GetId(),
                               wxPoint(0, 0), wxSize(0, 0), // it will have a buddy
                               GetWindowStyle(), GetName()) )
        return false;

    // reapply our values to wxSpinButton
    wxSpinButton::SetValue(GetValue());
    SetRange(m_min, m_max);

    // associate it with the buddy control again
    ::SetParent(GetBuddyHwnd(), GetHwndOf(GetParent()));
    (void)::SendMessage(GetHwnd(), UDM_SETBUDDY, (WPARAM)GetBuddyHwnd(), 0);

    // also set the size again with wxSIZE_ALLOW_MINUS_ONE flag: this is
    // necessary if our original position used -1 for either x or y
    SetSize(rect, wxSIZE_ALLOW_MINUS_ONE);

    return true;
}

bool wxSpinCtrl::Enable(bool enable)
{
    if ( !wxControl::Enable(enable) )
    {
        return false;
    }

    MSWEnableHWND(GetBuddyHwnd(), enable);

    return true;
}

#if wxUSE_TOOLTIPS

void wxSpinCtrl::DoSetToolTip(wxToolTip *tip)
{
    wxSpinButton::DoSetToolTip(tip);

    if ( tip )
        tip->AddOtherWindow(m_hwndBuddy);
}

#endif // wxUSE_TOOLTIPS

// ----------------------------------------------------------------------------
// events processing and generation
// ----------------------------------------------------------------------------

void wxSpinCtrl::SendSpinUpdate(int value)
{
    wxSpinEvent event(wxEVT_SPINCTRL, GetId());
    event.SetEventObject(this);
    event.SetInt(value);
    (void)HandleWindowEvent(event);
}

bool wxSpinCtrl::MSWOnScroll(int WXUNUSED(orientation), WXWORD wParam,
                             WXWORD WXUNUSED(pos), WXHWND control)
{
    wxCHECK_MSG( control, false, wxT("scrolling what?") );

    if ( wParam != SB_THUMBPOSITION )
    {
        // probable SB_ENDSCROLL - we don't react to it
        return false;
    }

    // Notice that we can't use "pos" from WM_VSCROLL as it is 16 bit and we
    // might be using 32 bit range.
    int new_value = GetValue();
    if (m_oldValue != new_value)
    {
        m_oldValue = new_value;
        SendSpinUpdate(new_value);
    }

    return true;
}

bool wxSpinCtrl::MSWOnNotify(int WXUNUSED(idCtrl), WXLPARAM lParam, WXLPARAM *result)
{
    NM_UPDOWN *lpnmud = (NM_UPDOWN *)lParam;

    if (lpnmud->hdr.hwndFrom != GetHwnd()) // make sure it is the right control
        return false;

    *result = 0;  // never reject UP and DOWN events

    return TRUE;
}

// Reuse the function defined in src/msw/textentry.cpp.
extern bool wxMSWTextEntryShouldPreProcessMessage(WXMSG* msg);

bool wxSpinCtrl::MSWShouldPreProcessMessage(WXMSG* msg)
{
    return wxMSWTextEntryShouldPreProcessMessage(msg);
}

// ----------------------------------------------------------------------------
// size calculations
// ----------------------------------------------------------------------------

int wxSpinCtrl::GetOverlap() const
{
    // Don't use FromDIP here. The gap between the control border and the
    // button seems to be always 1px.
    return 2;
}

wxSize wxSpinCtrl::DoGetBestSize() const
{
    return wxSpinCtrlImpl::GetBestSize(this, GetMin(), GetMax(), GetBase());
}

wxSize wxSpinCtrl::DoGetSizeFromTextSize(int xlen, int ylen) const
{
    wxSize sizeBtn = wxSpinButton::DoGetBestSize();

    // Create a temporary wxTextCtrl wrapping our existing HWND in order to be
    // able to reuse its GetSizeFromTextSize() implementation.
    wxTextCtrl text;
    TempHWNDSetter set(&text, m_hwndBuddy);

    // It's unnecessary to actually change the font used by the buddy control,
    // but we do need to ensure that the helper wxTextCtrl wxFont matches what
    // it is used as GetSizeFromTextSize() uses the current font.
    text.wxWindowBase::SetFont(GetFont());

    // Increase the width to accommodate the button, which should fit inside
    // the text control while taking account of the overlap.
    return text.GetSizeFromTextSize(xlen + sizeBtn.x - GetOverlap(), ylen);
}

void wxSpinCtrl::DoMoveWindow(int x, int y, int width, int height)
{
    // make sure the given width will be the total of both controls
    const int overlap = GetOverlap();
    int widthBtn = wxSpinButton::DoGetBestSize().x;
    int widthText = width - widthBtn + overlap;

    if ( widthText < 0 )
    {
        // This can happen during the initial window layout when it's total
        // size is too small to accommodate all the controls and usually is not
        // a problem because the window will be relaid out with enough space
        // later. Of course, if it isn't and this is our final size, then we
        // have a real problem but as we don't know if this is going to be the
        // case or not, just hope for the best -- we used to give a debug
        // warning here and this was annoying as it could result in dozens of
        // perfectly harmless warnings.
        widthText = 0;
    }

    if ( widthBtn > width )
        widthBtn = width;

    // Because both subcontrols are positioned relatively
    // to the parent which can have different layout direction
    // then our control, we need to mirror their positions manually.
    if ( GetParent()->GetLayoutDirection() == GetLayoutDirection() )
    {
        // Logical positions: x(Text) < x(Button)
        // 1) The buddy window
        DoMoveSibling(m_hwndBuddy, x, y, widthText, height);

        // 2) The button window
        wxSpinButton::DoMoveWindow(x + widthText - overlap, y, widthBtn, height);
    }
    else
    {
        // Logical positions: x(Button) < x(Text)
        // 1) The button window
        wxSpinButton::DoMoveWindow(x, y, widthBtn, height);

        // 2) The buddy window
        DoMoveSibling(m_hwndBuddy, x + widthBtn - overlap, y, widthText, height);
    }
}

// get total size of the control
void wxSpinCtrl::DoGetSize(int *x, int *y) const
{
    RECT spinrect, textrect, ctrlrect;
    GetWindowRect(GetHwnd(), &spinrect);
    GetWindowRect(GetBuddyHwnd(), &textrect);
    UnionRect(&ctrlrect,&textrect, &spinrect);

    if ( x )
        *x = ctrlrect.right - ctrlrect.left;
    if ( y )
        *y = ctrlrect.bottom - ctrlrect.top;
}

void wxSpinCtrl::DoGetClientSize(int *x, int *y) const
{
    RECT spinrect = wxGetClientRect(GetHwnd());
    RECT textrect = wxGetClientRect(GetBuddyHwnd());
    RECT ctrlrect;
    UnionRect(&ctrlrect,&textrect, &spinrect);

    if ( x )
        *x = ctrlrect.right - ctrlrect.left;
    if ( y )
        *y = ctrlrect.bottom - ctrlrect.top;
}

void wxSpinCtrl::DoGetPosition(int *x, int *y) const
{
    // Because both subcontrols are mirrored manually
    // (for layout direction purposes, see note)
    // and leftmost control can be either spin or buddy text
    // we need to get positions for both controls
    // and return this with lower horizonal value.
    // Note:
    // Logical positions in manual mirroring:
    // our layout == parent layout  =>  x(Text) < x(Button)
    // our layout != parent layout  =>  x(Button) < x(Text)

    // hack: pretend that our HWND is the text control just for a moment
    int xBuddy;
    WXHWND hWnd = GetHWND();
    wxConstCast(this, wxSpinCtrl)->m_hWnd = m_hwndBuddy;
    wxSpinButton::DoGetPosition(&xBuddy, y);

    int xText;
    wxConstCast(this, wxSpinCtrl)->m_hWnd = hWnd;
    wxSpinButton::DoGetPosition(&xText, y);

    *x = wxMin(xBuddy, xText);
}

void wxSpinCtrl::DoScreenToClient(int *x, int *y) const
{
    wxWindow::MSWDoScreenToClient(GetBuddyHwnd(), x, y);
}

void wxSpinCtrl::DoClientToScreen(int *x, int *y) const
{
    wxWindow::MSWDoClientToScreen(GetBuddyHwnd(), x, y);
}
#endif // wxUSE_SPINCTRL
