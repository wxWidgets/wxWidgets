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


wxStaticText::wxStaticText() :
    m_qtLabel(NULL)
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

    // Set the buddy to itself to get the mnemonic key but ensure that we don't have
    // any unwanted side effects, so disable the interaction:

    m_qtLabel->setBuddy( m_qtLabel );
    m_qtLabel->setTextInteractionFlags( Qt::NoTextInteraction );

    // Translate the WX horizontal alignment flags to Qt alignment flags
    // (notice that wxALIGN_LEFT is default and has the value of 0).
    if ( style & wxALIGN_CENTER_HORIZONTAL )
        m_qtLabel->setAlignment(Qt::AlignHCenter);
    else if ((style & wxALIGN_MASK) == wxALIGN_RIGHT)
        m_qtLabel->setAlignment(Qt::AlignRight);
    else
        m_qtLabel->setAlignment(Qt::AlignLeft);

    if ( !QtCreateControl(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    SetLabel(label);

    return true;
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
    m_qtLabel->setText( wxQtConvertString( label ) );
}

wxString wxStaticText::WXGetVisibleLabel() const
{
    return wxQtConvertString( m_qtLabel->text() );
}

QWidget *wxStaticText::GetHandle() const
{
    return m_qtLabel;
}
