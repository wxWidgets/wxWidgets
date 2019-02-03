/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/gauge.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/gauge.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QProgressBar>

class wxQtProgressBar : public wxQtEventSignalHandler< QProgressBar, wxGauge >
{
public:
    wxQtProgressBar( wxWindow *parent, wxGauge *handler );

private:
    void valueChanged(int value);
};

wxQtProgressBar::wxQtProgressBar( wxWindow *parent, wxGauge *handler )
    : wxQtEventSignalHandler< QProgressBar, wxGauge >( parent, handler )
{
}


wxGauge::wxGauge() :
    m_qtProgressBar(NULL)
{
}

wxGauge::wxGauge(wxWindow *parent,
        wxWindowID id,
        int range,
        const wxPoint& pos,
        const wxSize& size,
        long style,
        const wxValidator& validator,
        const wxString& name)
{
    Create( parent, id, range, pos, size, style, validator, name );
}

bool wxGauge::Create(wxWindow *parent,
            wxWindowID id,
            int range,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    m_qtProgressBar = new wxQtProgressBar( parent, this);
    m_qtProgressBar->setOrientation( wxQtConvertOrientation( style, wxGA_HORIZONTAL ));
    m_qtProgressBar->setRange( 0, range );
    m_qtProgressBar->setTextVisible( style & wxGA_TEXT );
    m_qtProgressBar->setValue(0);

    return QtCreateControl( parent, id, pos, size, style, validator, name );
}


QWidget *wxGauge::GetHandle() const
{
    return m_qtProgressBar;
}

// set/get the control range and value

void wxGauge::SetRange(int range)
{
    // note that in wx minimun range is fixed at 0
    m_qtProgressBar->setMaximum(range);
}

int wxGauge::GetRange() const
{
    return m_qtProgressBar->maximum();
}

void wxGauge::SetValue(int pos)
{
    m_qtProgressBar->setValue(pos);
}

int wxGauge::GetValue() const
{
    return m_qtProgressBar->value();
}
