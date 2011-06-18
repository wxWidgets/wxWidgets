/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/stattext.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/stattext.h"
#include "wx/qt/converter.h"

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
    m_qtLabel = new QLabel( wxQtConvertString( label ), parent->GetHandle() );

    // Set the buddy to itself to get the mnemonic key but ensure that we don't have
    // any unwanted side effects, so disable the interaction:

    m_qtLabel->setBuddy( m_qtLabel );
    m_qtLabel->setTextInteractionFlags( Qt::NoTextInteraction );

    return QtCreateControl( parent, id, pos, size, style, wxDefaultValidator, name );
}


QLabel *wxStaticText::GetHandle() const
{
    return m_qtLabel;
}
