/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/gauge.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_GAUGE

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
    m_qtWindow = new wxQtProgressBar( parent, this);

    GetQProgressBar()->setOrientation( wxQtConvertOrientation( style, wxGA_HORIZONTAL ));
    GetQProgressBar()->setRange( 0, range );
    GetQProgressBar()->setTextVisible( style & wxGA_TEXT );
    GetQProgressBar()->setValue(0);

    return wxControl::Create( parent, id, pos, size, style, validator, name );
}

QProgressBar* wxGauge::GetQProgressBar() const
{
    return static_cast<QProgressBar*>(m_qtWindow);
}

// set/get the control range and value

void wxGauge::SetRange(int range)
{
    // note that in wx minimun range is fixed at 0
    GetQProgressBar()->setMaximum(range);
}

int wxGauge::GetRange() const
{
    return GetQProgressBar()->maximum();
}

void wxGauge::SetValue(int pos)
{
    GetQProgressBar()->setValue(pos);
}

int wxGauge::GetValue() const
{
    return GetQProgressBar()->value();
}

#endif // wxUSE_GAUGE
