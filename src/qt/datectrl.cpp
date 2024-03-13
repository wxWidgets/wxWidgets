/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/datectrl.cpp
// Purpose:     wxDatePickerCtrl implementation
// Author:      Ali Kettab
// Created:     2023-10-12
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

#include "wx/datectrl.h"
#include "wx/dateevt.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QDateEdit>

wxIMPLEMENT_DYNAMIC_CLASS(wxDatePickerCtrl, wxControl);

// ============================================================================
// implementation
// ============================================================================

class wxQtDateEdit : public wxQtEventSignalHandler< QDateEdit, wxDatePickerCtrl >
{
public:
    wxQtDateEdit( wxWindow* parent, wxDatePickerCtrl* handler );

private:
    void dateChanged(const QDate& date);
};

wxQtDateEdit::wxQtDateEdit( wxWindow* parent, wxDatePickerCtrl* handler )
    : wxQtEventSignalHandler< QDateEdit, wxDatePickerCtrl >( parent, handler )
{
    connect(this, &QDateTimeEdit::dateChanged,
            this, &wxQtDateEdit::dateChanged);
}

void wxQtDateEdit::dateChanged(const QDate& date)
{
    wxDateEvent event(GetHandler(), wxQtConvertDate(date), wxEVT_DATE_CHANGED);
    EmitEvent( event );
}

// ----------------------------------------------------------------------------
// wxDatePickerCtrl creation
// ----------------------------------------------------------------------------

bool
wxDatePickerCtrl::Create(wxWindow *parent,
                         wxWindowID id,
                         const wxDateTime& dt,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxValidator& validator,
                         const wxString& name)
{
    m_qtDateEdit = new wxQtDateEdit( parent, this );
    m_qtDateEdit->setDate( dt.IsValid() ? wxQtConvertDate(dt) :
                                          wxQtConvertDate(wxDateTime::Today()) );
    m_qtDateEdit->setCalendarPopup(style & wxDP_DROPDOWN);
    m_qtDateEdit->setDisplayFormat(QLocale::system().dateFormat(QLocale::ShortFormat));

    return wxDatePickerCtrlBase::Create( parent, id, pos, size, style, validator, name );
}

// ----------------------------------------------------------------------------
// wxDatePickerCtrl operations
// ----------------------------------------------------------------------------

void wxDatePickerCtrl::SetValue(const wxDateTime& dt)
{
    wxASSERT_MSG( dt.IsValid() || HasFlag(wxDP_ALLOWNONE),
                 "this control must have a valid date" );

    const auto qtDate = wxQtConvertDate(dt);

    if ( qtDate >= m_qtDateEdit->minimumDate() &&
         qtDate <= m_qtDateEdit->maximumDate() )
    {
        wxQtEnsureSignalsBlocked blocker(m_qtDateEdit);
        m_qtDateEdit->setDate( qtDate );
    }
}

wxDateTime wxDatePickerCtrl::GetValue() const
{
    return wxQtConvertDate( m_qtDateEdit->date() );
}

void wxDatePickerCtrl::SetRange(const wxDateTime& dt1, const wxDateTime& dt2)
{
    m_qtDateEdit->setDateRange( wxQtConvertDate(dt1), wxQtConvertDate(dt2) );
}

bool wxDatePickerCtrl::GetRange(wxDateTime *dt1, wxDateTime *dt2) const
{
    if ( dt1 )
        *dt1 = wxQtConvertDate( m_qtDateEdit->minimumDate() );

    if ( dt2 )
        *dt2 = wxQtConvertDate( m_qtDateEdit->maximumDate() );

    return true;
}

QWidget* wxDatePickerCtrl::GetHandle() const
{
    return m_qtDateEdit;
}

#endif // wxUSE_DATEPICKCTRL
