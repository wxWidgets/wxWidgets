/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/datectlg.cpp
// Purpose:     generic wxDatePickerCtrlGeneric implementation
// Author:      Andreas Pflug
// Created:     2005-01-19
// Copyright:   (c) 2005 Andreas Pflug <pgadmin@pse-consulting.de>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"


#if wxUSE_DATEPICKCTRL

#ifndef WX_PRECOMP
    #include "wx/dialog.h"
    #include "wx/dcmemory.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/msgdlg.h"
    #include "wx/panel.h"
    #include "wx/textctrl.h"
#endif

#include "wx/calctrl.h"
#include "wx/combo.h"

#include "wx/datectrl.h"
#include "wx/generic/datectrl.h"
#include "wx/uilocale.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------


wxBEGIN_EVENT_TABLE(wxCalendarComboPopup, wxCalendarCtrl)
    EVT_KEY_DOWN(wxCalendarComboPopup::OnCalKey)
    EVT_CALENDAR_SEL_CHANGED(wxID_ANY, wxCalendarComboPopup::OnSelChange)
    EVT_CALENDAR_PAGE_CHANGED(wxID_ANY, wxCalendarComboPopup::OnSelChange)
    EVT_CALENDAR(wxID_ANY, wxCalendarComboPopup::OnSelChange)
wxEND_EVENT_TABLE()


// ============================================================================
// wxDatePickerCtrlGeneric implementation
// ============================================================================

wxBEGIN_EVENT_TABLE(wxDatePickerCtrlGeneric, wxDatePickerCtrlBase)
    EVT_TEXT(wxID_ANY, wxDatePickerCtrlGeneric::OnText)
    EVT_SIZE(wxDatePickerCtrlGeneric::OnSize)
wxEND_EVENT_TABLE()

#ifndef wxHAS_NATIVE_DATEPICKCTRL
    wxIMPLEMENT_DYNAMIC_CLASS(wxDatePickerCtrl, wxControl);
#endif

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

bool wxDatePickerCtrlGeneric::Create(wxWindow *parent,
                                     wxWindowID id,
                                     const wxDateTime& date,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style,
                                     const wxValidator& validator,
                                     const wxString& name)
{
    wxASSERT_MSG( !(style & wxDP_SPIN),
                  wxT("wxDP_SPIN style not supported, use wxDP_DEFAULT") );

    if ( !wxControl::Create(parent, id, pos, size,
                            style | wxCLIP_CHILDREN | wxWANTS_CHARS | wxBORDER_NONE,
                            validator, name) )
    {
        return false;
    }

    InheritAttributes();

    m_combo = new wxComboCtrl(this, -1, wxEmptyString,
                              wxDefaultPosition, wxDefaultSize);

    m_combo->SetCtrlMainWnd(this);

    m_popup = new wxCalendarComboPopup();

#if defined(__WXMSW__)
    // without this keyboard navigation in month control doesn't work
    m_combo->UseAltPopupWindow();
#endif
    m_combo->SetPopupControl(m_popup);

    m_popup->SetDateValue(date.IsValid() ? date : wxDateTime::Today());

    SetInitialSize(size);

    return true;
}


void wxDatePickerCtrlGeneric::Init()
{
    m_combo = nullptr;
    m_popup = nullptr;
}

wxDatePickerCtrlGeneric::~wxDatePickerCtrlGeneric()
{
}

bool wxDatePickerCtrlGeneric::Destroy()
{
    if ( m_combo )
        m_combo->Destroy();

    m_combo = nullptr;
    m_popup = nullptr;

    return wxControl::Destroy();
}

// ----------------------------------------------------------------------------
// overridden base class methods
// ----------------------------------------------------------------------------

wxSize wxDatePickerCtrlGeneric::DoGetBestSize() const
{
    wxSize size = m_combo->GetButtonSize();

    wxTextCtrl* const text = m_combo->GetTextCtrl();
    int w;
    text->GetTextExtent(text->GetValue(), &w, nullptr);
    size.x += text->GetSizeFromTextSize(w + 1).x;

    return size;
}

wxWindowList wxDatePickerCtrlGeneric::GetCompositeWindowParts() const
{
    wxWindowList parts;
    parts.push_back(m_combo);
    parts.push_back(m_popup);
    return parts;
}

// ----------------------------------------------------------------------------
// wxDatePickerCtrlGeneric API
// ----------------------------------------------------------------------------

void 
wxDatePickerCtrlGeneric::SetFormat(const wxString& format)
{
    m_popup->SetFormat(format);
}

bool
wxDatePickerCtrlGeneric::SetDateRange(const wxDateTime& lowerdate,
                                      const wxDateTime& upperdate)
{
    if ( !m_popup->SetDateRange(lowerdate, upperdate) )
        return false;

    // If the limits were, check that our current value lies between them and
    // adjust it if it doesn't.
    const wxDateTime old = GetValue();
    if ( old.IsValid() )
    {
        if ( lowerdate.IsValid() && old < lowerdate )
            SetValue(lowerdate);
        else if ( upperdate.IsValid() && old > upperdate )
            SetValue(upperdate);
    }

    return true;
}


wxDateTime wxDatePickerCtrlGeneric::GetValue() const
{
    if ( HasFlag(wxDP_ALLOWNONE) && m_popup->IsTextEmpty() )
        return wxInvalidDateTime;
    return m_popup->GetDate();
}


void wxDatePickerCtrlGeneric::SetValue(const wxDateTime& date)
{
    m_popup->SetDateValue(date);
}


bool wxDatePickerCtrlGeneric::GetRange(wxDateTime *dt1, wxDateTime *dt2) const
{
    return m_popup->GetDateRange(dt1, dt2);
}


void
wxDatePickerCtrlGeneric::SetRange(const wxDateTime &dt1, const wxDateTime &dt2)
{
    SetDateRange(dt1, dt2);
}

wxCalendarCtrl *wxDatePickerCtrlGeneric::GetCalendar() const
{
    return m_popup;
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------


void wxDatePickerCtrlGeneric::OnSize(wxSizeEvent& event)
{
    if ( m_combo )
        m_combo->SetSize(GetClientSize());

    event.Skip();
}


void wxDatePickerCtrlGeneric::OnText(wxCommandEvent &ev)
{
    ev.SetEventObject(this);
    ev.SetId(GetId());
    GetParent()->GetEventHandler()->ProcessEvent(ev);

    if ( m_popup )
        m_popup->ChangeDateAndNotifyIfValid();
}

#endif // wxUSE_DATEPICKCTRL
