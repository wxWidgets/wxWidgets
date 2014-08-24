/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/colordlg.cpp
// Author:      Sean D'Epagnier
// Copyright:   (c) Sean D'Epagnier 2014
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/qt/private/winevent.h"
#include "wx/colordlg.h"

class wxQtColorDialog : public wxQtEventSignalHandler< QColorDialog, wxDialog >
{
public:
    wxQtColorDialog( wxWindow *parent, wxDialog *handler)
        : wxQtEventSignalHandler(parent, handler)
        { }
};

bool wxColourDialog::Create(wxWindow *parent, wxColourData *data )
{
    m_qtWindow = new wxQtColorDialog( parent, this );

    if(data)
        m_data = *data;

    if ( m_data.GetChooseFull() )
    {
        for (int i=0; i<wxColourData::NUM_CUSTOM; i++)
            QColorDialog::setCustomColor(i, m_data.GetCustomColour(i).GetHandle());
    }

    GetHandle()->setCurrentColor(m_data.GetColour().GetHandle());

    return wxTopLevelWindow::Create( parent, wxID_ANY, "");
}

wxColourData &wxColourDialog::GetColourData()
{
    for (int i=0; i<wxColourData::NUM_CUSTOM; i++)
        m_data.SetCustomColour(i, GetHandle()->customColor(i));
    
    m_data.SetColour(GetHandle()->currentColor());
    
    return m_data;
}

