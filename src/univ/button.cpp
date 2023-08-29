/////////////////////////////////////////////////////////////////////////////
// Name:        src/univ/button.cpp
// Purpose:     wxButton
// Author:      Vadim Zeitlin
// Modified by:
// Created:     14.08.00
// Copyright:   (c) 2000 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"


#if wxUSE_BUTTON

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/dcscreen.h"
    #include "wx/button.h"
    #include "wx/validate.h"
    #include "wx/settings.h"
#endif

#include "wx/stockitem.h"
// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// default margins around the image
static const wxCoord DEFAULT_BTN_MARGIN_X = 0;  // We should give space for the border, at least.
static const wxCoord DEFAULT_BTN_MARGIN_Y = 0;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

void wxButton::Init()
{
    m_isPressed =
    m_isDefault = false;
}

bool wxButton::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxBitmapBundle& bitmap,
                      const wxString &lbl,
                      const wxPoint &pos,
                      const wxSize &size,
                      long style,
                      const wxValidator& validator,
                      const wxString &name)
{
    wxString label(lbl);
    if (label.empty() && wxIsStockID(id))
        label = wxGetStockLabel(id);

    long ctrl_style = style & ~wxBU_ALIGN_MASK;
    ctrl_style = ctrl_style & ~wxALIGN_MASK;

    if((style & wxBU_RIGHT) == wxBU_RIGHT)
        ctrl_style |= wxALIGN_RIGHT;
    else if((style & wxBU_LEFT) == wxBU_LEFT)
        ctrl_style |= wxALIGN_LEFT;
    else
        ctrl_style |= wxALIGN_CENTRE_HORIZONTAL;

    if((style & wxBU_TOP) == wxBU_TOP)
        ctrl_style |= wxALIGN_TOP;
    else if((style & wxBU_BOTTOM) == wxBU_BOTTOM)
        ctrl_style |= wxALIGN_BOTTOM;
    else
        ctrl_style |= wxALIGN_CENTRE_VERTICAL;

    if ( !wxControl::Create(parent, id, pos, size, ctrl_style, validator, name) )
        return false;

    SetLabel(label);

    if (bitmap.IsOk())
        SetBitmap(bitmap); // SetInitialSize called by SetBitmap()
    else
        SetInitialSize(size);

    CreateInputHandler(wxINP_HANDLER_BUTTON);

    return true;
}

wxButton::~wxButton()
{
}

// ----------------------------------------------------------------------------
// size management
// ----------------------------------------------------------------------------

/* static */
wxSize wxButtonBase::GetDefaultSize(wxWindow* WXUNUSED(win))
{
    static wxSize s_sizeBtn;

    if ( s_sizeBtn.x == 0 )
    {
        wxScreenDC dc;

        // this corresponds more or less to wxMSW standard in Win32 theme (see
        // wxWin32Renderer::AdjustSize())
//        s_sizeBtn.x = 8*dc.GetCharWidth();
//        s_sizeBtn.y = (11*dc.GetCharHeight())/10 + 2;
        // Otto Wyss, Patch 664399
        s_sizeBtn.x = dc.GetCharWidth()*10 + 2;
        s_sizeBtn.y = dc.GetCharHeight()*11/10 + 2;
    }

    return s_sizeBtn;
}


// ----------------------------------------------------------------------------
// input processing
// ----------------------------------------------------------------------------

void wxButton::Click()
{
    wxCommandEvent event(wxEVT_BUTTON, GetId());
    InitCommandEvent(event);
    Command(event);
}

// ----------------------------------------------------------------------------
// misc
// ----------------------------------------------------------------------------

wxBitmap wxButton::DoGetBitmap(State WXUNUSED(which)) const
{
    return m_bitmap;
}

void wxButton::DoSetBitmap(const wxBitmapBundle& bitmap, State which)
{
    // we support only one bitmap right now, although this wouldn't be
    // difficult to change
    if ( which == State_Normal )
        m_bitmap = bitmap.GetBitmap(wxDefaultSize); // TODO-HIDPI

    SetBitmapMargins(DEFAULT_BTN_MARGIN_X, DEFAULT_BTN_MARGIN_Y);
}

void wxButton::DoSetBitmapMargins(wxCoord x, wxCoord y)
{
    m_marginBmpX = x + 2;
    m_marginBmpY = y + 2;

    SetInitialSize(wxDefaultSize);
}

wxWindow *wxButton::SetDefault()
{
    m_isDefault = true;

    return wxButtonBase::SetDefault();
}

#endif // wxUSE_BUTTON

