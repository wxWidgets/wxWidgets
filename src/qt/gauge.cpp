/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/gauge.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/gauge.h"
#include "wx/qt/converter.h"


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


wxGauge::wxGauge()
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

    return QtCreateControl( parent, id, pos, size, style, validator, name );
}


QProgressBar *wxGauge::GetHandle() const
{
    return m_qtProgressBar;
}
