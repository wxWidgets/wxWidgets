/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/timectrl.cpp
// Purpose:     wxTimePickerCtrl implementation
// Author:      Ali Kettab
// Created:     2023-10-13
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"


#if wxUSE_TIMEPICKCTRL


#include "wx/timectrl.h"
#include "wx/dateevt.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QTimeEdit>

wxIMPLEMENT_DYNAMIC_CLASS(wxTimePickerCtrl, wxControl);

// ============================================================================
// implementation
// ============================================================================

class wxQtTimeEdit : public wxQtEventSignalHandler< QTimeEdit, wxTimePickerCtrl >
{
public:
    wxQtTimeEdit( wxWindow* parent, wxTimePickerCtrl* handler );

private:
    void timeChanged(const QTime& time);

};

wxQtTimeEdit::wxQtTimeEdit( wxWindow* parent, wxTimePickerCtrl* handler )
    : wxQtEventSignalHandler< QTimeEdit, wxTimePickerCtrl >( parent, handler )
{
    connect(this, &QDateTimeEdit::timeChanged,
            this, &wxQtTimeEdit::timeChanged);
}

void wxQtTimeEdit::timeChanged(const QTime& time)
{
    wxDateEvent event(GetHandler(), wxQtConvertTime(time), wxEVT_TIME_CHANGED);
    EmitEvent( event );
}

// ----------------------------------------------------------------------------
// wxTimePickerCtrl creation
// ----------------------------------------------------------------------------

bool
wxTimePickerCtrl::Create(wxWindow *parent,
                         wxWindowID id,
                         const wxDateTime& dt,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxValidator& validator,
                         const wxString& name)
{
    m_qtWindow = new wxQtTimeEdit( parent, this );

    GetQTimeEdit()->setTime( dt.IsValid() ? wxQtConvertTime(dt) :
                                           wxQtConvertTime(wxDateTime::Now()) );
    GetQTimeEdit()->setDisplayFormat(QLocale::system().timeFormat(QLocale::ShortFormat));

    return wxTimePickerCtrlBase::Create( parent, id, pos, size, style, validator, name );
}

QTimeEdit* wxTimePickerCtrl::GetQTimeEdit() const
{
    return static_cast<QTimeEdit*>(m_qtWindow);
}

// ----------------------------------------------------------------------------
// wxTimePickerCtrl operations
// ----------------------------------------------------------------------------

void wxTimePickerCtrl::SetValue(const wxDateTime& dt)
{
    wxQtEnsureSignalsBlocked blocker(GetQTimeEdit());
    GetQTimeEdit()->setTime( dt.IsValid() ? wxQtConvertTime(dt) : QTime() );
}

wxDateTime wxTimePickerCtrl::GetValue() const
{
    return wxQtConvertTime( GetQTimeEdit()->time() );
}

#endif // wxUSE_TIMEPICKCTRL
