/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/calctrl.cpp
// Purpose:     wxCalendarCtrl control implementation for wxQt
// Author:      Kolya Kosenko
// Created:     2010-05-12
// Copyright:   (c) 2010 Kolya Kosenko
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_CALENDARCTRL

#include "wx/calctrl.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"

#include <QtGui/QTextCharFormat>
#include <QtWidgets/QCalendarWidget>

class wxQtCalendarWidget : public wxQtEventSignalHandler< QCalendarWidget, wxCalendarCtrl >
{

public:
    wxQtCalendarWidget( wxWindow *parent, wxCalendarCtrl *handler );

private:
    void selectionChanged();
    void activated(const QDate &date);

private:
    QDate m_date;
};


void wxCalendarCtrl::Init()
{
    for ( size_t n = 0; n < WXSIZEOF(m_attrs); n++ )
    {
        m_attrs[n] = nullptr;
    }
}

wxCalendarCtrl::~wxCalendarCtrl()
{
    for ( size_t n = 0; n < WXSIZEOF(m_attrs); n++ )
    {
        delete m_attrs[n];
    }
}

bool wxCalendarCtrl::Create(wxWindow *parent, wxWindowID id, const wxDateTime& date,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name )
{
    m_qtWindow = new wxQtCalendarWidget( parent, this );

    GetQCalendarWidget()->resize(GetQCalendarWidget()->sizeHint());

    {
        // Init holiday colours
        const QTextCharFormat format = GetQCalendarWidget()->weekdayTextFormat(Qt::Sunday);
        m_colHolidayFg = format.foreground().color();

        wxMISSING_IMPLEMENTATION( "Get holiday background color" );

        // Bug in Qt: returned background color is incorrect
        //m_colHolidayBg = format.background().color();
    }

    {
        // synchronize header colours
        QTextCharFormat format = GetQCalendarWidget()->headerTextFormat();

        bool sync = false;

        if ( m_colHeaderFg.IsOk() )
            sync = true;
        else
            m_colHeaderFg = format.foreground().color();

        if ( m_colHeaderBg.IsOk() )
            sync = true;
        else
            m_colHeaderFg = format.background().color();

        if ( sync )
            SetHeaderColours(m_colHeaderFg, m_colHeaderBg);
    }

    UpdateStyle();

    if ( date.IsValid() )
        SetDate(date);

    return wxCalendarCtrlBase::Create( parent, id, pos, size, style, wxDefaultValidator, name );
}

QCalendarWidget* wxCalendarCtrl::GetQCalendarWidget() const
{
    return static_cast<QCalendarWidget*>(m_qtWindow);
}

void wxCalendarCtrl::UpdateStyle()
{
    if ( !GetHandle() )
        return;

    if ( WeekStartsOnMonday() )
        GetQCalendarWidget()->setFirstDayOfWeek(Qt::Monday);
    else
        GetQCalendarWidget()->setFirstDayOfWeek(Qt::Sunday);

    if ( m_windowStyle & wxCAL_SHOW_WEEK_NUMBERS )
        GetQCalendarWidget()->setVerticalHeaderFormat(QCalendarWidget::ISOWeekNumbers);
    else
        GetQCalendarWidget()->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    RefreshHolidays();
}

void wxCalendarCtrl::SetWindowStyleFlag(long style)
{
    const long styleOld = GetWindowStyleFlag();

    wxCalendarCtrlBase::SetWindowStyleFlag(style);

    if ( styleOld != GetWindowStyleFlag() )
        UpdateStyle();
}

bool wxCalendarCtrl::SetDate(const wxDateTime& date)
{
    wxCHECK_MSG( date.IsValid(), false, "invalid date" );
    if ( !GetHandle() )
        return false;

    if ( wxQtConvertDate( date ) > GetQCalendarWidget()->maximumDate() ||
            wxQtConvertDate( date ) < GetQCalendarWidget()->minimumDate() )
        return false;

    wxQtEnsureSignalsBlocked blocker(GetQCalendarWidget());
    GetQCalendarWidget()->setSelectedDate(wxQtConvertDate(date));

    return true;
}

wxDateTime wxCalendarCtrl::GetDate() const
{
    if ( !GetHandle() )
        return wxDateTime();

    return wxQtConvertDate(GetQCalendarWidget()->selectedDate());
}

bool wxCalendarCtrl::SetDateRange(const wxDateTime& lowerdate,
                                  const wxDateTime& upperdate)
{
    if ( !GetHandle() )
        return false;

    wxQtEnsureSignalsBlocked blocker(GetQCalendarWidget());
    GetQCalendarWidget()->setMinimumDate(wxQtConvertDate(lowerdate));
    GetQCalendarWidget()->setMaximumDate(wxQtConvertDate(upperdate));

    return true;
}

bool wxCalendarCtrl::GetDateRange(wxDateTime *lowerdate,
                                  wxDateTime *upperdate) const
{
    if ( !GetHandle() )
        return false;

    bool status = false;

    if ( lowerdate )
    {
        *lowerdate = wxQtConvertDate(GetQCalendarWidget()->minimumDate());
        status = true;
    }

    if ( upperdate )
    {
        *upperdate = wxQtConvertDate(GetQCalendarWidget()->maximumDate());
        status = true;
    }

    return status;
}

// Copied from wxMSW
bool wxCalendarCtrl::EnableMonthChange(bool enable)
{
    if ( !wxCalendarCtrlBase::EnableMonthChange(enable) )
        return false;

    wxDateTime dtStart, dtEnd;
    if ( !enable )
    {
        dtStart = GetDate();
        dtStart.SetDay(1);

        dtEnd = dtStart.GetLastMonthDay();
    }
    //else: leave them invalid to remove the restriction

    SetDateRange(dtStart, dtEnd);

    return true;
}

void wxCalendarCtrl::Mark(size_t day, bool mark)
{
    wxCHECK_RET( day > 0 && day < 32, "invalid day" );

    if ( !GetHandle() )
        return;

    QDate date = GetQCalendarWidget()->selectedDate();
    date.setDate(date.year(), date.month(), day);

    QTextCharFormat format = GetQCalendarWidget()->dateTextFormat(date);
    format.setFontWeight(mark ? QFont::Bold : QFont::Normal);
    GetQCalendarWidget()->setDateTextFormat(date, format);
}

void wxCalendarCtrl::SetHoliday(size_t day)
{
    wxCHECK_RET( day > 0 && day < 32, "invalid day" );

    if ( !(m_windowStyle & wxCAL_SHOW_HOLIDAYS) )
        return;

    if ( !GetHandle() )
        return;

    QDate date = GetQCalendarWidget()->selectedDate();
    date.setDate(date.year(), date.month(), day);

    QTextCharFormat format = GetQCalendarWidget()->dateTextFormat(date);
    format.setForeground(m_colHolidayFg.GetQColor());
    GetQCalendarWidget()->setDateTextFormat(date, format);
}

void wxCalendarCtrl::SetHolidayColours(const wxColour& colFg, const wxColour& colBg)
{
    m_colHolidayFg = colFg;
    m_colHolidayBg = colBg;
    RefreshHolidays();
}

void wxCalendarCtrl::RefreshHolidays()
{
    if ( !GetHandle() )
        return;

    QTextCharFormat format;
    if ( m_windowStyle & wxCAL_SHOW_HOLIDAYS )
    {
        if ( m_colHolidayFg.IsOk() )
            format.setForeground(m_colHolidayFg.GetQColor());
        if ( m_colHolidayBg.IsOk() )
            format.setBackground(m_colHolidayBg.GetQColor());
    }
    else
    {
        format.setForeground(GetQCalendarWidget()->weekdayTextFormat(Qt::Monday)
            .foreground().color());

        wxMISSING_IMPLEMENTATION( "Set holiday background color" );

        // Bug in Qt: returned background color is incorrect
        //format.setBackground(GetQCalendarWidget()->weekdayTextFormat(Qt::Monday)
        //    .background().color());
    }
    GetQCalendarWidget()->setWeekdayTextFormat(Qt::Saturday, format);
    GetQCalendarWidget()->setWeekdayTextFormat(Qt::Sunday,   format);
}

void wxCalendarCtrl::SetHeaderColours(const wxColour& colFg, const wxColour& colBg)
{
    m_colHeaderFg = colFg;
    m_colHeaderBg = colBg;

    if ( !GetHandle() )
        return;

    QTextCharFormat format = GetQCalendarWidget()->headerTextFormat();
    if ( m_colHeaderFg.IsOk() )
        format.setForeground(m_colHeaderFg.GetQColor());
    if ( m_colHeaderBg.IsOk() )
        format.setBackground(m_colHeaderBg.GetQColor());
    GetQCalendarWidget()->setHeaderTextFormat(format);
}

wxCalendarDateAttr *wxCalendarCtrl::GetAttr(size_t day) const
{
    wxCHECK_MSG( day > 0 && day < 32, nullptr, wxT("invalid day") );

    return m_attrs[day - 1];
}

void wxCalendarCtrl::SetAttr(size_t day, wxCalendarDateAttr *attr)
{
    wxCHECK_RET( day > 0 && day < 32, wxT("invalid day") );

    delete m_attrs[day - 1];
    m_attrs[day - 1] = attr;

    QDate date = GetQCalendarWidget()->selectedDate();
    date.setDate(date.year(), date.month(), day);

    QTextCharFormat format = GetQCalendarWidget()->dateTextFormat(date);
    if ( attr->HasTextColour() )
        format.setForeground(attr->GetTextColour().GetQColor());
    if ( attr->HasBackgroundColour() )
        format.setBackground(attr->GetBackgroundColour().GetQColor());

    wxMISSING_IMPLEMENTATION( "Setting font" );

    // wxFont is not implemented yet
    //if ( attr->HasFont() )
    //    format.setFont(attr->GetFont().GetQFont());

    GetQCalendarWidget()->setDateTextFormat(date, format);
}

//=============================================================================

wxQtCalendarWidget::wxQtCalendarWidget( wxWindow *parent, wxCalendarCtrl *handler )
    : wxQtEventSignalHandler< QCalendarWidget, wxCalendarCtrl >( parent, handler )
    , m_date(selectedDate())
{
    connect(this, &QCalendarWidget::selectionChanged, this, &wxQtCalendarWidget::selectionChanged);
    connect(this, &QCalendarWidget::activated, this, &wxQtCalendarWidget::activated);
}

void wxQtCalendarWidget::selectionChanged()
{
    wxCalendarCtrl *win = GetHandler();
    if ( win )
    {
        GetHandler()->GenerateAllChangeEvents(wxQtConvertDate(m_date));
        m_date = selectedDate();
    }
}

void wxQtCalendarWidget::activated(const QDate &WXUNUSED(date))
{
    wxCalendarCtrl *handler = GetHandler();
    if ( handler )
    {
        handler->GenerateEvent(wxEVT_CALENDAR_DOUBLECLICKED);
    }
}

#endif // wxUSE_CALENDARCTRL
