/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/statboxcmn.cpp
// Purpose:     wxStaticBox common code
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_STATBOX

#include "wx/statbox.h"

extern WXDLLEXPORT_DATA(const char) wxStaticBoxNameStr[] = "groupBox";

wxStaticBoxBase::wxStaticBoxBase()
{
    m_labelWin = NULL;

#ifndef __WXGTK__
    m_container.DisableSelfFocus();
#endif
}

void wxStaticBoxBase::GetBordersForSizer(int *borderTop, int *borderOther) const
{
    const int BORDER = FromDIP(5); // FIXME: hardcoded value

    if ( m_labelWin )
    {
        *borderTop = m_labelWin->GetSize().y;
    }
    else
    {
        *borderTop = GetLabel().empty() ? BORDER : GetCharHeight();
    }

    *borderOther = BORDER;
}

void wxStaticBoxBase::WXDestroyWithoutChildren()
{
    // Notice that we must make a copy of the list as it will be changed by
    // Reparent() calls in the loop.
    const wxWindowList children = GetChildren();
    wxWindow* const parent = GetParent();
    for ( wxWindowList::const_iterator i = children.begin();
          i != children.end();
          ++i )
    {
        // The label window doesn't count as our child, it's really a part of
        // static box itself and it makes no sense to leave it alive when the
        // box is destroyed, so do it even when it's supposed to be destroyed
        // without destroying its children -- by not reparenting it, we ensure
        // that it's destroyed when this object itself is below.
        if ( *i != m_labelWin )
        {
            (*i)->Reparent(parent);
        }
    }

    delete this;
}

bool wxStaticBoxBase::Enable(bool enable)
{
#ifdef wxHAS_WINDOW_LABEL_IN_STATIC_BOX
    // We want to keep the window label enabled even if the static box is
    // disabled because this label is often used to enable/disable the box
    // (e.g. a checkbox or a radio button is commonly used for this purpose)
    // and it would be impossible to re-enable the box back if disabling it
    // also disabled the label control.
    //
    // Unfortunately it is _not_ enough to just disable the box and then enable
    // the label window as it would still remain disabled under some platforms
    // (those where wxHAS_NATIVE_ENABLED_MANAGEMENT is defined, e.g. wxGTK) for
    // as long as its parent is disabled. So we avoid disabling the box at all
    // in this case and only disable its children, but still pretend that the
    // box is disabled by updating its m_isEnabled, as it would be surprising
    // if IsEnabled() didn't return false after disabling the box, for example.
    //
    // Finally note that this really needs to be done only when disabling the
    // box and not when re-enabling it, at least for the platforms without
    // native enabled state management (e.g. wxMSW) because otherwise we could
    // have a bug in the following scenario:
    //
    //  0. The box is initially enabled
    //  1. Its parent gets disabled, calling DoEnable(false) on the box and all
    //     its children from its NotifyWindowOnEnableChange() (including the
    //     label).
    //  2. The box is itself disabled (for whatever app logic reason).
    //  3. The parent gets enabled but this time doesn't do anything with the
    //     box, because it should continue being disabled.
    //  4. The box is re-enabled -- but remains actually disabled as
    //     DoEnable(true) was never called on it (nor on the label).
    //
    // To avoid this possibility, we always call the base class version, which
    // does call DoEnable(true) on the box itself and all its children,
    // including the label, when re-enabling it.
    if ( m_labelWin && !enable )
    {
        if ( enable == IsThisEnabled() )
            return false;

        const wxWindowList& children = GetChildren();
        for ( wxWindowList::const_iterator i = children.begin();
              i != children.end();
              ++i )
        {
            if ( *i != m_labelWin )
                (*i)->NotifyWindowOnEnableChange(enable);
        }

        m_isEnabled = enable;

        return true;
    }
#endif // wxHAS_WINDOW_LABEL_IN_STATIC_BOX

    return wxNavigationEnabled<wxControl>::Enable(enable);
}

// ----------------------------------------------------------------------------
// XTI
// ----------------------------------------------------------------------------

wxDEFINE_FLAGS( wxStaticBoxStyle )
wxBEGIN_FLAGS( wxStaticBoxStyle )
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
wxEND_FLAGS( wxStaticBoxStyle )

wxIMPLEMENT_DYNAMIC_CLASS_XTI(wxStaticBox, wxControl, "wx/statbox.h");

wxBEGIN_PROPERTIES_TABLE(wxStaticBox)
    wxPROPERTY( Label, wxString, SetLabel, GetLabel, wxString(), 0 /*flags*/, \
                wxT("Helpstring"), wxT("group"))
    wxPROPERTY_FLAGS( WindowStyle, wxStaticBoxStyle, long, SetWindowStyleFlag, \
                      GetWindowStyleFlag, wxEMPTY_PARAMETER_VALUE, 0 /*flags*/, \
                      wxT("Helpstring"), wxT("group")) // style
wxEND_PROPERTIES_TABLE()

wxEMPTY_HANDLERS_TABLE(wxStaticBox)

wxCONSTRUCTOR_6( wxStaticBox, wxWindow*, Parent, wxWindowID, Id, \
                 wxString, Label, wxPoint, Position, wxSize, Size, \
                 long, WindowStyle )

#endif // wxUSE_STATBOX
