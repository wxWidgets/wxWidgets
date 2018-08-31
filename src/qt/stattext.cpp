/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/stattext.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/stattext.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QLabel>

class wxQtStaticText : public wxQtEventSignalHandler< QLabel, wxStaticText >
{
public:
    wxQtStaticText( wxWindow *parent, wxStaticText *handler ):
        wxQtEventSignalHandler< QLabel, wxStaticText >( parent, handler ){}
};


wxStaticText::wxStaticText()
{
}

wxStaticText::wxStaticText(wxWindow *parent,
             wxWindowID id,
             const wxString &label,
             const wxPoint &pos,
             const wxSize &size,
             long style,
             const wxString &name)
{
    Create( parent, id, label, pos, size, style, name );
}

bool wxStaticText::Create(wxWindow *parent,
            wxWindowID id,
            const wxString &label,
            const wxPoint &pos,
            const wxSize &size,
            long style,
            const wxString &name)
{
    m_qtLabel = new wxQtStaticText( parent, this );
    m_qtLabel->setText( wxQtConvertString( label ) );

    // Set the buddy to itself to get the mnemonic key but ensure that we don't have
    // any unwanted side effects, so disable the interaction:

    m_qtLabel->setBuddy( m_qtLabel );
    m_qtLabel->setTextInteractionFlags( Qt::NoTextInteraction );

    return QtCreateControl( parent, id, pos, size, style, wxDefaultValidator, name );
}

void wxStaticText::SetLabel(const wxString& label)
{
    m_qtLabel->setText( wxQtConvertString( label ) );
}

QWidget *wxStaticText::GetHandle() const
{
    return m_qtLabel;
}
