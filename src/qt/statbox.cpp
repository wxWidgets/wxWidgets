/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/statbox.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/statbox.h"
#include "wx/window.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"
#include <QtWidgets/QGroupBox>


class wxQtGroupBox : public wxQtEventSignalHandler< QGroupBox, wxStaticBox >
{
public:
    wxQtGroupBox( wxWindow *parent, wxStaticBox *handler ):
        wxQtEventSignalHandler< QGroupBox, wxStaticBox >( parent, handler ){}
};


wxStaticBox::wxStaticBox() :
    m_qtGroupBox(NULL)
{
}

wxStaticBox::wxStaticBox(wxWindow *parent, wxWindowID id,
            const wxString& label,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
{
    Create( parent, id, label, pos, size, style, name );
}

bool wxStaticBox::Create(wxWindow *parent, wxWindowID id,
            const wxString& label,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
{
    m_qtGroupBox = new wxQtGroupBox( parent, this );
    m_qtGroupBox->setTitle( wxQtConvertString( label ) );

    return QtCreateControl( parent, id, pos, size, style, wxDefaultValidator, name );
}

QWidget *wxStaticBox::GetHandle() const
{
    return m_qtGroupBox;
}

void wxStaticBox::SetLabel(const wxString& label)
{
    m_qtGroupBox->setTitle(wxQtConvertString(label));
}

wxString wxStaticBox::GetLabel() const
{
    return wxQtConvertString(m_qtGroupBox->title());
}

void wxStaticBox::GetBordersForSizer(int *borderTop, int *borderOther) const
{
    wxStaticBoxBase::GetBordersForSizer(borderTop, borderOther);

    // need extra space for the label:
    *borderTop += GetCharHeight();
}
