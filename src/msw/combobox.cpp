/////////////////////////////////////////////////////////////////////////////
// Name:        msw/combobox.cpp
// Purpose:     wxComboBox class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "combobox.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_COMBOBOX

#ifndef WX_PRECOMP
    #include "wx/settings.h"
    #include "wx/log.h"
    // for wxEVT_COMMAND_TEXT_ENTER
    #include "wx/textctrl.h"
#endif

#include "wx/combobox.h"
#include "wx/brush.h"
#include "wx/clipbrd.h"
#include "wx/msw/private.h"

#if wxUSE_TOOLTIPS
    #if !defined(__GNUWIN32_OLD__) || defined(__CYGWIN10__)
        #include <commctrl.h>
    #endif
    #include "wx/tooltip.h"
#endif // wxUSE_TOOLTIPS

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

#if wxUSE_EXTENDED_RTTI
WX_DEFINE_FLAGS( wxComboBoxStyle )

wxBEGIN_FLAGS( wxComboBoxStyle )
    // new style border flags, we put them first to
    // use them for streaming out
    wxFLAGS_MEMBER(wxBORDER_SIMPLE)
    wxFLAGS_MEMBER(wxBORDER_SUNKEN)
    wxFLAGS_MEMBER(wxBORDER_DOUBLE)
    wxFLAGS_MEMBER(wxBORDER_RAISED)
    wxFLAGS_MEMBER(wxBORDER_STATIC)
    wxFLAGS_MEMBER(wxBORDER_NONE)

    // old style border flags
    wxFLAGS_MEMBER(wxSIMPLE_BORDER)
    wxFLAGS_MEMBER(wxSUNKEN_BORDER)
    wxFLAGS_MEMBER(wxDOUBLE_BORDER)
    wxFLAGS_MEMBER(wxRAISED_BORDER)
    wxFLAGS_MEMBER(wxSTATIC_BORDER)
    wxFLAGS_MEMBER(wxBORDER)

    // standard window styles
    wxFLAGS_MEMBER(wxTAB_TRAVERSAL)
    wxFLAGS_MEMBER(wxCLIP_CHILDREN)
    wxFLAGS_MEMBER(wxTRANSPARENT_WINDOW)
    wxFLAGS_MEMBER(wxWANTS_CHARS)
    wxFLAGS_MEMBER(wxFULL_REPAINT_ON_RESIZE)
    wxFLAGS_MEMBER(wxALWAYS_SHOW_SB )
    wxFLAGS_MEMBER(wxVSCROLL)
    wxFLAGS_MEMBER(wxHSCROLL)

    wxFLAGS_MEMBER(wxCB_SIMPLE)
    wxFLAGS_MEMBER(wxCB_SORT)
    wxFLAGS_MEMBER(wxCB_READONLY)
    wxFLAGS_MEMBER(wxCB_DROPDOWN)

wxEND_FLAGS( wxComboBoxStyle )

IMPLEMENT_DYNAMIC_CLASS_XTI(wxComboBox, wxControl,"wx/combobox.h")

wxBEGIN_PROPERTIES_TABLE(wxComboBox)
    wxEVENT_PROPERTY( Select , wxEVT_COMMAND_COMBOBOX_SELECTED , wxCommandEvent )
    wxEVENT_PROPERTY( TextEnter , wxEVT_COMMAND_TEXT_ENTER , wxCommandEvent )

    // TODO DELEGATES
    wxPROPERTY( Font , wxFont , SetFont , GetFont  , EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY_COLLECTION( Choices , wxArrayString , wxString , AppendString , GetStrings , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY( Value ,wxString, SetValue, GetValue, EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY( Selection ,int, SetSelection, GetSelection, EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY_FLAGS( WindowStyle , wxComboBoxStyle , long , SetWindowStyleFlag , GetWindowStyleFlag , EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // style
wxEND_PROPERTIES_TABLE()

wxBEGIN_HANDLERS_TABLE(wxComboBox)
wxEND_HANDLERS_TABLE()

wxCONSTRUCTOR_5( wxComboBox , wxWindow* , Parent , wxWindowID , Id , wxString , Value , wxPoint , Position , wxSize , Size )
#else
IMPLEMENT_DYNAMIC_CLASS(wxComboBox, wxControl)
#endif

// ----------------------------------------------------------------------------
// function prototypes
// ----------------------------------------------------------------------------

LRESULT APIENTRY _EXPORT wxComboEditWndProc(HWND hWnd,
                                            UINT message,
                                            WPARAM wParam,
                                            LPARAM lParam);

// ---------------------------------------------------------------------------
// global vars
// ---------------------------------------------------------------------------

// the pointer to standard radio button wnd proc
static WNDPROC gs_wndprocEdit = (WNDPROC)NULL;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wnd proc for subclassed edit control
// ----------------------------------------------------------------------------

LRESULT APIENTRY _EXPORT wxComboEditWndProc(HWND hWnd,
                                            UINT message,
                                            WPARAM wParam,
                                            LPARAM lParam)
{
    HWND hwndCombo = ::GetParent(hWnd);
    wxWindow *win = wxFindWinFromHandle((WXHWND)hwndCombo);

    switch ( message )
    {
        // forward some messages to the combobox to generate the appropriate
        // wxEvents from them
        case WM_KEYUP:
        case WM_KEYDOWN:
        case WM_CHAR:
        case WM_SETFOCUS:
        case WM_KILLFOCUS:
            {
                wxComboBox *combo = wxDynamicCast(win, wxComboBox);
                if ( !combo )
                {
                    // we can get WM_KILLFOCUS while our parent is already half
                    // destroyed and hence doesn't look like a combobx any
                    // longer, check for it to avoid bogus assert failures
                    if ( !win->IsBeingDeleted() )
                    {
                        wxFAIL_MSG( _T("should have combo as parent") );
                    }
                }
                else if ( combo->MSWProcessEditMsg(message, wParam, lParam) )
                {
                    // handled by parent
                    return 0;
                }
            }
            break;

        case WM_GETDLGCODE:
            {
                wxCHECK_MSG( win, 0, _T("should have a parent") );

                if ( win->GetWindowStyle() & wxPROCESS_ENTER )
                {
                    // need to return a custom dlg code or we'll never get it
                    return DLGC_WANTMESSAGE;
                }
            }
            break;

        // deal with tooltips here
#if wxUSE_TOOLTIPS && defined(TTN_NEEDTEXT)
        case WM_NOTIFY:
            {
                wxCHECK_MSG( win, 0, _T("should have a parent") );

                NMHDR* hdr = (NMHDR *)lParam;
                if ( hdr->code == TTN_NEEDTEXT )
                {
                    wxToolTip *tooltip = win->GetToolTip();
                    if ( tooltip )
                    {
                        TOOLTIPTEXT *ttt = (TOOLTIPTEXT *)lParam;
                        ttt->lpszText = (wxChar *)tooltip->GetTip().c_str();
                    }

                    // processed
                    return 0;
                }
            }
            break;
#endif // wxUSE_TOOLTIPS
    }

    return ::CallWindowProc(CASTWNDPROC gs_wndprocEdit, hWnd, message, wParam, lParam);
}

WXHBRUSH wxComboBox::OnCtlColor(WXHDC pDC,
                                WXHWND WXUNUSED(pWnd),
                                WXUINT WXUNUSED(nCtlColor),
                                WXUINT WXUNUSED(message),
                                WXWPARAM WXUNUSED(wParam),
                                WXLPARAM WXUNUSED(lParam))
{
    HDC hdc = (HDC)pDC;
    wxColour colBack = GetBackgroundColour();

    if (!IsEnabled())
        colBack = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);

    ::SetBkColor(hdc, wxColourToRGB(colBack));
    ::SetTextColor(hdc, wxColourToRGB(GetForegroundColour()));

    wxBrush *brush = wxTheBrushList->FindOrCreateBrush(colBack, wxSOLID);

    return (WXHBRUSH)brush->GetResourceHandle();
}

// ----------------------------------------------------------------------------
// wxComboBox callbacks
// ----------------------------------------------------------------------------

WXLRESULT wxComboBox::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    // handle WM_CTLCOLOR messages from our EDIT control to be able to set its
    // colour correctly (to be the same as our own one)
    switch ( nMsg )
    {
        // we have to handle both: one for the normal case and the other for
        // wxCB_READONLY
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORSTATIC:
            WXWORD nCtlColor;
            WXHDC hdc;
            WXHWND hwnd;
            UnpackCtlColor(wParam, lParam, &nCtlColor, &hdc, &hwnd);

            return (WXLRESULT)OnCtlColor(hdc, hwnd, nCtlColor, nMsg, wParam, lParam);
    }

    return wxChoice::MSWWindowProc(nMsg, wParam, lParam);
}

bool wxComboBox::MSWProcessEditMsg(WXUINT msg, WXWPARAM wParam, WXLPARAM lParam)
{
    switch ( msg )
    {
        case WM_CHAR:
            // for compatibility with wxTextCtrl, generate a special message
            // when Enter is pressed
            if ( wParam == VK_RETURN )
            {
                wxCommandEvent event(wxEVT_COMMAND_TEXT_ENTER, m_windowId);
                InitCommandEvent(event);
                event.SetString(GetValue());
                event.SetInt(GetSelection());
                ProcessCommand(event);
            }

            return HandleChar(wParam, lParam, true /* isASCII */);

        case WM_KEYDOWN:
            return HandleKeyDown(wParam, lParam);

        case WM_KEYUP:
            return HandleKeyUp(wParam, lParam);

        case WM_SETFOCUS:
            return HandleSetFocus((WXHWND)wParam);

        case WM_KILLFOCUS:
            return HandleKillFocus((WXHWND)wParam);
    }

    return false;
}

bool wxComboBox::MSWCommand(WXUINT param, WXWORD WXUNUSED(id))
{
    wxString value;
    int sel = -1;
    switch ( param )
    {
        case CBN_SELCHANGE:
            sel = GetSelection();

            // somehow we get 2 CBN_SELCHANGE events with the same index when
            // the user selects an item in the combobox -- ignore duplicates
            if ( sel > -1 && sel != m_selectionOld )
            {
                m_selectionOld = sel;

                // GetValue() would still return the old value from here but
                // according to the docs we should return the new value if the
                // user calls it in his event handler, so update internal
                // m_value
                m_value = GetString(sel);

                wxCommandEvent event(wxEVT_COMMAND_COMBOBOX_SELECTED, GetId());
                event.SetInt(sel);
                event.SetEventObject(this);
                event.SetString(m_value);
                ProcessCommand(event);
            }
            else // no valid selection
            {
                m_selectionOld = sel;

                // hence no EVT_TEXT neither
                break;
            }

            // fall through: for compability with wxGTK, also send the text
            // update event when the selection changes (this also seems more
            // logical as the text does change)

        case CBN_EDITCHANGE:
            {
                wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, GetId());

                // if sel != -1, value was initialized above (and we can't use
                // GetValue() here as it would return the old selection and we
                // want the new one)
                if ( sel == -1 )
                {
                    m_value = wxGetWindowText(GetHwnd());
                }
                else // we're synthesizing text updated event from sel change
                {
                    // We need to retrieve the current selection because the
                    // user may have changed it in the previous handler (for
                    // CBN_SELCHANGE above).
                    sel = GetSelection();
                    if ( sel > -1 )
                    {
                        m_value = GetString(sel);
                    }
                }

                event.SetString(m_value);
                event.SetEventObject(this);
                ProcessCommand(event);
            }
            break;
    }

    // there is no return value for the CBN_ notifications, so always return
    // false from here to pass the message to DefWindowProc()
    return false;
}

WXHWND wxComboBox::GetEditHWND() const
{
    // this function should not be called for wxCB_READONLY controls, it is
    // the callers responsability to check this
    wxASSERT_MSG( !(GetWindowStyle() & wxCB_READONLY),
                  _T("read-only combobox doesn't have any edit control") );

    POINT pt;
    pt.x = pt.y = 4;
    HWND hwndEdit = ::ChildWindowFromPoint(GetHwnd(), pt);
    if ( !hwndEdit || hwndEdit == GetHwnd() )
    {
        wxFAIL_MSG(_T("not read only combobox without edit control?"));
    }

    return (WXHWND)hwndEdit;
}

// ----------------------------------------------------------------------------
// wxComboBox creation
// ----------------------------------------------------------------------------

bool wxComboBox::Create(wxWindow *parent, wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size,
                        int n, const wxString choices[],
                        long style,
                        const wxValidator& validator,
                        const wxString& name)
{
    // pretend that wxComboBox is hidden while it is positioned and resized and
    // show it only right before leaving this method because otherwise there is
    // some noticeable flicker while the control rearranges itself
    m_isShown = false;

    if ( !CreateAndInit(parent, id, pos, size, n, choices, style,
                        validator, name) )
        return false;

    // we shouldn't call SetValue() for an empty string because this would
    // (correctly) result in an assert with a read only combobox and is useless
    // for the other ones anyhow
    if ( !value.empty() )
        SetValue(value);

    // a (not read only) combobox is, in fact, 2 controls: the combobox itself
    // and an edit control inside it and if we want to catch events from this
    // edit control, we must subclass it as well
    if ( !(style & wxCB_READONLY) )
    {
        gs_wndprocEdit = wxSetWindowProc((HWND)GetEditHWND(),
                                         wxComboEditWndProc);
    }

    // and finally, show the control
    Show(true);

    return true;
}

bool wxComboBox::Create(wxWindow *parent, wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size,
                        const wxArrayString& choices,
                        long style,
                        const wxValidator& validator,
                        const wxString& name)
{
    wxCArrayString chs(choices);
    return Create(parent, id, value, pos, size, chs.GetCount(),
                  chs.GetStrings(), style, validator, name);
}

WXDWORD wxComboBox::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    // we never have an external border
    WXDWORD msStyle = wxChoice::MSWGetStyle
                      (
                        (style & ~wxBORDER_MASK) | wxBORDER_NONE, exstyle
                      );

    // remove the style always added by wxChoice
    msStyle &= ~CBS_DROPDOWNLIST;

    if ( style & wxCB_READONLY )
        msStyle |= CBS_DROPDOWNLIST;
#ifndef __WXWINCE__
    else if ( style & wxCB_SIMPLE )
        msStyle |= CBS_SIMPLE; // A list (shown always) and edit control
#endif
    else
        msStyle |= CBS_DROPDOWN;

    // there is no reason to not always use CBS_AUTOHSCROLL, so do use it
    msStyle |= CBS_AUTOHSCROLL;

    // NB: we used to also add CBS_NOINTEGRALHEIGHT here but why?

    return msStyle;
}

// ----------------------------------------------------------------------------
// wxComboBox text control-like methods
// ----------------------------------------------------------------------------

void wxComboBox::SetValue(const wxString& value)
{
    if ( HasFlag(wxCB_READONLY) )
        SetStringSelection(value);
    else
        SetWindowText(GetHwnd(), value.c_str());

    m_value = value;
    m_selectionOld = GetSelection();
}

// Clipboard operations
void wxComboBox::Copy()
{
  SendMessage(GetHwnd(), WM_COPY, 0, 0L);
}

void wxComboBox::Cut()
{
  SendMessage(GetHwnd(), WM_CUT, 0, 0L);
}

void wxComboBox::Paste()
{
  SendMessage(GetHwnd(), WM_PASTE, 0, 0L);
}

void wxComboBox::SetEditable(bool WXUNUSED(editable))
{
  // Can't implement in MSW?
//  HWND hWnd = GetHwnd();
//  SendMessage(hWnd, EM_SETREADONLY, (WPARAM)!editable, (LPARAM)0L);
}

void wxComboBox::SetInsertionPoint(long pos)
{
    if ( GetWindowStyle() & wxCB_READONLY )
        return;

#ifdef __WIN32__
    HWND hWnd = GetHwnd();
    ::SendMessage(hWnd, CB_SETEDITSEL, 0, MAKELPARAM(pos, pos));
    HWND hEditWnd = (HWND) GetEditHWND() ;
    if ( hEditWnd )
    {
        // Scroll insertion point into view
        SendMessage(hEditWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
        // Why is this necessary? (Copied from wxTextCtrl::SetInsertionPoint)
        SendMessage(hEditWnd, EM_REPLACESEL, 0, (LPARAM) wxEmptyString);
    }
#endif // __WIN32__
}

void wxComboBox::SetInsertionPointEnd()
{
    // setting insertion point doesn't make sense for read only comboboxes
    if ( !(GetWindowStyle() & wxCB_READONLY) )
    {
        long pos = GetLastPosition();
        SetInsertionPoint(pos);
    }
}

long wxComboBox::GetInsertionPoint() const
{
#ifdef __WIN32__
    DWORD Pos=(DWORD)SendMessage(GetHwnd(), CB_GETEDITSEL, 0, 0L);
    return Pos&0xFFFF;
#else
    return 0;
#endif
}

long wxComboBox::GetLastPosition() const
{
    HWND hEditWnd = (HWND) GetEditHWND();

    // Get number of characters in the last (only) line. We'll add this to the character
    // index for the last line, 1st position.
    int lineLength = (int)SendMessage(hEditWnd, EM_LINELENGTH, (WPARAM) 0, (LPARAM)0L);

    return (long)(lineLength);
}

void wxComboBox::Replace(long from, long to, const wxString& value)
{
#if wxUSE_CLIPBOARD
    Remove(from, to);

    // Now replace with 'value', by pasting.
    wxSetClipboardData(wxDF_TEXT, (wxObject *)(const wxChar *)value, 0, 0);

    // Paste into edit control
    SendMessage(GetHwnd(), WM_PASTE, (WPARAM)0, (LPARAM)0L);
#endif
}

void wxComboBox::Remove(long from, long to)
{
    // Set selection and remove it
    SetSelection(from, to);
    SendMessage(GetHwnd(), WM_CUT, (WPARAM)0, (LPARAM)0);
}

void wxComboBox::SetSelection(long from, long to)
{
    HWND hWnd = GetHwnd();
    long fromChar = from;
    long toChar = to;
    // if from and to are both -1, it means
    // (in wxWidgets) that all text should be selected.
    // This translates into Windows convention
    if ((from == -1) && (to == -1))
    {
      fromChar = 0;
      toChar = -1;
    }

    if ( SendMessage(hWnd, CB_SETEDITSEL, (WPARAM)0, (LPARAM)MAKELONG(fromChar, toChar)) == CB_ERR )
    {
        wxLogDebug(_T("CB_SETEDITSEL failed"));
    }
}

#endif // wxUSE_COMBOBOX

