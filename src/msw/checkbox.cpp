/////////////////////////////////////////////////////////////////////////////
// Name:        checkbox.cpp
// Purpose:     wxCheckBox
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "checkbox.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/checkbox.h"
    #include "wx/brush.h"
#endif

#include "wx/msw/private.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxCheckBox, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxBitmapCheckBox, wxCheckBox)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxCheckBox
// ----------------------------------------------------------------------------

bool wxCheckBox::MSWCommand(WXUINT WXUNUSED(param), WXWORD WXUNUSED(id))
{
    wxCommandEvent event(wxEVT_COMMAND_CHECKBOX_CLICKED, m_windowId);
    event.SetInt(GetValue());
    event.SetEventObject(this);
    ProcessCommand(event);
    return TRUE;
}

// Single check box item
bool wxCheckBox::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& label,
                        const wxPoint& pos,
                        const wxSize& size, long style,
                        const wxValidator& validator,
                        const wxString& name)
{
    SetName(name);
#if wxUSE_VALIDATORS
    SetValidator(validator);
#endif // wxUSE_VALIDATORS
    if (parent) parent->AddChild(this);

    SetBackgroundColour(parent->GetBackgroundColour()) ;
    SetForegroundColour(parent->GetForegroundColour()) ;

    m_windowStyle = style;

    wxString Label = label;
    if (Label == wxT(""))
        Label = wxT(" "); // Apparently needed or checkbox won't show

    if ( id == -1 )
        m_windowId = NewControlId();
    else
        m_windowId = id;

    int x = pos.x;
    int y = pos.y;
    int width = size.x;
    int height = size.y;

    long msStyle = BS_AUTOCHECKBOX | WS_TABSTOP | WS_CHILD | WS_VISIBLE;
    if ( style & wxALIGN_RIGHT )
        msStyle |= BS_LEFTTEXT;

    // We perhaps have different concepts of 3D here - a 3D border,
    // versus a 3D button.
    // So we only wish to give a border if this is specified
    // in the style.
    bool want3D;
    WXDWORD exStyle = Determine3DEffects(0, &want3D) ;

    // Even with extended styles, need to combine with WS_BORDER
    // for them to look right.
    /*
       if ( want3D || wxStyleHasBorder(m_windowStyle) )
       msStyle |= WS_BORDER;
     */

    m_hWnd = (WXHWND)CreateWindowEx(exStyle, wxT("BUTTON"), Label,
            msStyle,
            0, 0, 0, 0,
            (HWND)parent->GetHWND(), (HMENU)m_windowId,
            wxGetInstance(), NULL);

#if wxUSE_CTL3D
    if (want3D)
    {
        Ctl3dSubclassCtl(GetHwnd());
        m_useCtl3D = TRUE;
    }
#endif

    // Subclass again for purposes of dialog editing mode
    SubclassWin(m_hWnd);

    SetFont(parent->GetFont());

    SetSize(x, y, width, height);

    return TRUE;
}

void wxCheckBox::SetLabel(const wxString& label)
{
  SetWindowText(GetHwnd(), label);
}

wxSize wxCheckBox::DoGetBestSize() const
{
    int wCheckbox, hCheckbox;

    wxString str = wxGetWindowText(GetHWND());

    if ( !str.IsEmpty() )
    {
        GetTextExtent(str, &wCheckbox, &hCheckbox);
        wCheckbox += RADIO_SIZE;

        if ( hCheckbox < RADIO_SIZE )
            hCheckbox = RADIO_SIZE;
    }
    else
    {
        wCheckbox = RADIO_SIZE;
        hCheckbox = RADIO_SIZE;
    }

    return wxSize(wCheckbox, hCheckbox);
}

void wxCheckBox::SetValue(bool val)
{
    SendMessage(GetHwnd(), BM_SETCHECK, val, 0);
}

#ifndef BST_CHECKED
#define BST_CHECKED 0x0001
#endif

bool wxCheckBox::GetValue() const
{
#ifdef __WIN32__
  return (SendMessage(GetHwnd(), BM_GETCHECK, 0, 0) == BST_CHECKED);
#else
  return ((0x003 & SendMessage(GetHwnd(), BM_GETCHECK, 0, 0)) == 0x003);
#endif
}

void wxCheckBox::Command (wxCommandEvent & event)
{
  SetValue ((event.GetInt() != 0));
  ProcessCommand (event);
}

// ----------------------------------------------------------------------------
// wxBitmapCheckBox
// ----------------------------------------------------------------------------

bool wxBitmapCheckBox::Create(wxWindow *parent, wxWindowID id, const wxBitmap *label,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
  SetName(name);
#if wxUSE_VALIDATORS
  SetValidator(validator);
#endif // wxUSE_VALIDATORS
  if (parent) parent->AddChild(this);

  SetBackgroundColour(parent->GetBackgroundColour()) ;
  SetForegroundColour(parent->GetForegroundColour()) ;
  m_windowStyle = style;

  if ( id == -1 )
      m_windowId = NewControlId();
  else
      m_windowId = id;

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  checkWidth = -1 ;
  checkHeight = -1 ;
  long msStyle = CHECK_FLAGS;

  HWND wx_button = CreateWindowEx(MakeExtendedStyle(m_windowStyle), CHECK_CLASS, wxT("toggle"),
                    msStyle,
                    0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)m_windowId,
                    wxGetInstance(), NULL);

#if wxUSE_CTL3D
  if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
  {
    Ctl3dSubclassCtl(wx_button);
    m_useCtl3D = TRUE;
  }
#endif

  m_hWnd = (WXHWND)wx_button;

  // Subclass again for purposes of dialog editing mode
  SubclassWin((WXHWND)wx_button);

  SetSize(x, y, width, height);

  ShowWindow(wx_button, SW_SHOW);

  return TRUE;
}

void wxBitmapCheckBox::SetLabel(const wxBitmap& bitmap)
{
    wxFAIL_MSG(wxT("not implemented"));
}
