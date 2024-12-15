/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/stattext.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_STATTEXT

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
    m_qtWindow = new wxQtStaticText( parent, this );

    // Set the buddy to itself to get the mnemonic key but ensure that we don't have
    // any unwanted side effects, so disable the interaction:

    GetQLabel()->setBuddy( GetQLabel() );
    GetQLabel()->setTextInteractionFlags( Qt::NoTextInteraction );

    // Translate the WX horizontal alignment flags to Qt alignment flags
    // (notice that wxALIGN_LEFT is default and has the value of 0).
    if ( style & wxALIGN_CENTER_HORIZONTAL )
        GetQLabel()->setAlignment(Qt::AlignHCenter);
    else if ((style & wxALIGN_MASK) == wxALIGN_RIGHT)
        GetQLabel()->setAlignment(Qt::AlignRight);
    else
        GetQLabel()->setAlignment(Qt::AlignLeft);

    SetLabel(label);

    return wxStaticTextBase::Create(parent, id, pos, size, style, wxDefaultValidator, name);
}

QLabel* wxStaticText::GetQLabel() const
{
    return static_cast<QLabel*>(m_qtWindow);
}

void wxStaticText::SetLabel(const wxString& label)
{
    // If the label doesn't really change, avoid flicker by not doing anything.
    if ( label == m_labelOrig )
        return;

    // save the label in m_labelOrig with both the markup (if any) and
    // the mnemonics characters (if any)
    m_labelOrig = label;

    WXSetVisibleLabel(GetEllipsizedLabel());

    AutoResizeIfNecessary();
}

void wxStaticText::WXSetVisibleLabel(const wxString& label)
{
    GetQLabel()->setText( wxQtConvertString( label ) );
}

wxString wxStaticText::WXGetVisibleLabel() const
{
    return wxQtConvertString( GetQLabel()->text() );
}

#endif // wxUSE_STATTEXT
