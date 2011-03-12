/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/gauge.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/gauge.h"
#include "wx/qt/converter.h"

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
    m_qtProgressBar = new QProgressBar( parent->GetHandle() );
    m_qtProgressBar->setOrientation( wxQtConvertOrientation( style, wxGA_HORIZONTAL ));
    m_qtProgressBar->setRange( 0, range );

    return QtCreateControl( parent, id, pos, size, style, validator, name );
}


QProgressBar *wxGauge::GetHandle() const
{
    return m_qtProgressBar;
}
