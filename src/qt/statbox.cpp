/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/statbox.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_STATBOX

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
    m_qtWindow = new wxQtGroupBox( parent, this );

    GetQGroupBox()->setTitle( wxQtConvertString( label ) );

    return wxStaticBoxBase::Create( parent, id, pos, size, style, wxDefaultValidator, name );
}

QGroupBox* wxStaticBox::GetQGroupBox() const
{
    return static_cast<QGroupBox*>(m_qtWindow);
}

void wxStaticBox::SetLabel(const wxString& label)
{
    wxStaticBoxBase::SetLabel( label );

    GetQGroupBox()->setTitle( wxQtConvertString( label ) );
}

wxString wxStaticBox::GetLabel() const
{
    return wxQtConvertString( GetQGroupBox()->title() );
}

void wxStaticBox::GetBordersForSizer(int *borderTop, int *borderOther) const
{
    wxStaticBoxBase::GetBordersForSizer(borderTop, borderOther);

    // need extra space for the label:
    *borderTop += GetCharHeight();
}

#endif // wxUSE_STATBOX
