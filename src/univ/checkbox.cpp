/////////////////////////////////////////////////////////////////////////////
// Name:        univ/checkbox.cpp
// Purpose:     wxCheckBox implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     25.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "univcheckbox.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_CHECKBOX

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/checkbox.h"
    #include "wx/validate.h"
#endif

#include "wx/univ/theme.h"
#include "wx/univ/renderer.h"
#include "wx/univ/colschem.h"

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxCheckBox, wxControl)

// ----------------------------------------------------------------------------
// wxCheckBox
// ----------------------------------------------------------------------------

void wxCheckBox::Init()
{
    m_checkMargin = -1;
    m_status = Status_Unchecked;
}

bool wxCheckBox::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString &label,
                        const wxPoint &pos,
                        const wxSize &size,
                        long style,
                        const wxValidator& validator,
                        const wxString &name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return FALSE;

    SetLabel(label);
    SetBestSize(size);

    return TRUE;
}

// ----------------------------------------------------------------------------
// checkbox interface
// ----------------------------------------------------------------------------

bool wxCheckBox::GetValue() const
{
    return m_status == Status_Checked;
}

void wxCheckBox::SetValue(bool value)
{
    Status status = value ? Status_Checked : Status_Unchecked;
    if ( status != m_status )
    {
        m_status = status;

        Refresh();
    }
}

// ----------------------------------------------------------------------------
// indicator bitmaps
// ----------------------------------------------------------------------------

wxBitmap wxCheckBox::GetBitmap(State state, Status status) const
{
    wxBitmap bmp = m_bitmaps[state][m_status];
    if ( !bmp.Ok() )
        bmp = m_bitmaps[State_Normal][m_status];
    if ( !bmp.Ok() )
        bmp = wxTheme::Get()->GetColourScheme()->Get(state, m_status);

    return bmp;
}

// ----------------------------------------------------------------------------
// drawing
// ----------------------------------------------------------------------------

void wxCheckBox::DoDraw(wxControlRenderer *renderer)
{
    State state;
    int flags = GetStateFlags();
    if ( flags & wxCONTROL_DISABLED )
        state = State_Disabled;
    else if ( flags & wxCONTROL_PRESSED )
        state = State_Pressed;
    else if ( flags & wxCONTROL_CURRENT )
        state = State_Current;
    else
        state = State_Normal;

    renderer->DrawLabelBox(GetBitmap(state, m_status), m_checkMargin);
}

// ----------------------------------------------------------------------------
// geometry calculations
// ----------------------------------------------------------------------------

wxSize wxCheckBox::DoGetBestClientSize() const
{
    wxClientDC dc(wxConstCast(this, wxCheckBox));
    wxCoord width, height;
    dc.GetMultiLineTextExtent(GetLabel(), &width, &height);

    wxBitmap bmp = GetBitmap(State_Normal, Status_Checked);
    if ( height < bmp.GetHeight() )
        height = bmp.GetHeight();
    height += GetCharHeight();

    width += bmp.GetWidth() + 2*GetCharWidth();

    return wxSize(width, height);
}

// ----------------------------------------------------------------------------
// input handling
// ----------------------------------------------------------------------------

bool wxCheckBox::PerformAction(const wxControlAction& action,
                               long numArg,
                               const wxString& strArg)
{
    if ( action == wxACTION_CHECKBOX_CHECK )
        SetValue(TRUE);
    else if ( action == wxACTION_CHECKBOX_CLEAR )
        SetValue(FALSE);
    else if ( action == wxACTION_CHECKBOX_TOGGLE )
        SetValue(!GetValue());
    else
        return wxControl::PerformAction(action, numArg, strArg);

    return TRUE;
}

#endif // wxUSE_CHECKBOX

