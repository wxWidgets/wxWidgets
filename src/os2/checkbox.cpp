/////////////////////////////////////////////////////////////////////////////
// Name:        checkbox.cpp
// Purpose:     wxCheckBox
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/checkbox.h"
    #include "wx/brush.h"
#endif

#include "wx/os2/private.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxCheckBox, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxBitmapCheckBox, wxCheckBox)
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxCheckBox
// ----------------------------------------------------------------------------

bool wxCheckBox::OS2Command(WXUINT WXUNUSED(param), WXWORD WXUNUSED(id))
{
    wxCommandEvent event(wxEVT_COMMAND_CHECKBOX_CLICKED, m_windowId);
    event.SetInt(GetValue());
    event.SetEventObject(this);
    ProcessCommand(event);
    return TRUE;
}

// Single check box item
bool wxCheckBox::Create(wxWindow *parent, wxWindowID id, const wxString& label,
           const wxPoint& pos,
           const wxSize& size, long style,
#if wxUSE_VALIDATORS
           const wxValidator& validator,
#endif
           const wxString& name)
{
    SetName(name);
#if wxUSE_VALIDATORS
    SetValidator(validator);
#endif
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

    // TODO: create checkbox

    // Subclass again for purposes of dialog editing mode
    SubclassWin(m_hWnd);

    SetFont(parent->GetFont());

    SetSize(x, y, width, height);

    return FALSE;
}

void wxCheckBox::SetLabel(const wxString& label)
{
    // TODO
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
    // TODO
}

#ifndef BST_CHECKED
#define BST_CHECKED 0x0001
#endif

bool wxCheckBox::GetValue() const
{
    // TODO
    return FALSE;
}

WXHBRUSH wxCheckBox::OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
                                WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
  // TODO:
  /*
#if wxUSE_CTL3D
  if ( m_useCtl3D )
  {
    HBRUSH hbrush = Ctl3dCtlColorEx(message, wParam, lParam);

      return (WXHBRUSH) hbrush;
  }
#endif

  if (GetParent()->GetTransparentBackground())
    SetBkMode((HDC) pDC, TRANSPARENT);
  else
    SetBkMode((HDC) pDC, OPAQUE);

  ::SetBkColor((HDC) pDC, RGB(GetBackgroundColour().Red(), GetBackgroundColour().Green(), GetBackgroundColour().Blue()));
  ::SetTextColor((HDC) pDC, RGB(GetForegroundColour().Red(), GetForegroundColour().Green(), GetForegroundColour().Blue()));

*/

  wxBrush *backgroundBrush = wxTheBrushList->FindOrCreateBrush(GetBackgroundColour(), wxSOLID);


  // Note that this will be cleaned up in wxApp::OnIdle, if backgroundBrush
  // has a zero usage count.
// backgroundBrush->RealizeResource();
   return (WXHBRUSH) backgroundBrush->GetResourceHandle();
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
#if wxUSE_VALIDATORS
           const wxValidator& validator,
#endif
           const wxString& name)
{
    SetName(name);
#if wxUSE_VALIDATORS
    SetValidator(validator);
#endif
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
//    long msStyle = CHECK_FLAGS;

    HWND wx_button = 0; // TODO: Create the bitmap checkbox

    m_hWnd = (WXHWND)wx_button;

    // Subclass again for purposes of dialog editing mode
    SubclassWin((WXHWND)wx_button);

    SetSize(x, y, width, height);

// TODO:    ShowWindow(wx_button, SW_SHOW);

    return TRUE;
}

void wxBitmapCheckBox::SetLabel(const wxBitmap& bitmap)
{
    wxFAIL_MSG(wxT("not implemented"));
}

