/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/radiobox.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/radiobox.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QRadioButton>


class wxQtRadioBox : public wxQtEventSignalHandler< QGroupBox, wxRadioBox >
{
public:
    wxQtRadioBox( wxWindow *parent, wxRadioBox *handler ):
        wxQtEventSignalHandler< QGroupBox, wxRadioBox >( parent, handler ){}
};


class wxQtButtonGroup : public wxQtSignalHandler< wxRadioBox >, public QButtonGroup
{
public:
    wxQtButtonGroup( QGroupBox *parent, wxRadioBox *handler ):
        wxQtSignalHandler< wxRadioBox >(handler ),
        QButtonGroup(parent)
    {
        connect(this,
                static_cast<void (QButtonGroup::*)(int index)>(&QButtonGroup::buttonClicked),
                this, &wxQtButtonGroup::buttonClicked);
    }
private:
    void buttonClicked(int index);
};

void wxQtButtonGroup::buttonClicked(int index) {
    wxRadioBox *handler = GetHandler();
    if ( handler )
    {
        wxCommandEvent event( wxEVT_RADIOBOX, handler->GetId() );
        event.SetInt(index);
        event.SetString(wxQtConvertString(button(index)->text()));
        EmitEvent( event );
    }
}


wxIMPLEMENT_DYNAMIC_CLASS(wxRadioBox, wxControl);


wxRadioBox::wxRadioBox()
{
}

wxRadioBox::wxRadioBox(wxWindow *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           int n, const wxString choices[],
           int majorDim,
           long style,
           const wxValidator& val,
           const wxString& name)
{
    Create( parent, id, title, pos, size, n, choices, majorDim, style, val, name );
}

wxRadioBox::wxRadioBox(wxWindow *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           const wxArrayString& choices,
           int majorDim,
           long style,
           const wxValidator& val,
           const wxString& name)
{
    Create( parent, id, title, pos, size, choices, majorDim, style, val, name );
}



bool wxRadioBox::Create(wxWindow *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            int majorDim,
            long style,
            const wxValidator& val,
            const wxString& name)
{
    return Create( parent, id, title, pos, size, choices.size(), &choices[ 0 ],
        majorDim, style, val, name );

}


template < typename Button >
static void AddChoices( QButtonGroup *qtButtonGroup, QBoxLayout *qtBoxLayout, int count, const wxString choices[] )
{
    Button *btn;
    while ( count-- > 0 )
    {
        btn = new Button( wxQtConvertString( *choices++ ));
        qtButtonGroup->addButton( btn );
        qtBoxLayout->addWidget( btn );
    }
}


bool wxRadioBox::Create(wxWindow *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos,
            const wxSize& size,
            int n, const wxString choices[],
            int WXUNUSED(majorDim),
            long style,
            const wxValidator& val,
            const wxString& name)
{
    m_qtGroupBox = new wxQtRadioBox( parent, this );
    m_qtGroupBox->setTitle( wxQtConvertString( title ) );
    m_qtButtonGroup = new wxQtButtonGroup( m_qtGroupBox, this );

    if ( style & wxRA_SPECIFY_ROWS )
        m_qtBoxLayout = new QHBoxLayout;
    else if ( style & wxRA_SPECIFY_COLS )
        m_qtBoxLayout = new QVBoxLayout;

    AddChoices< QRadioButton >( m_qtButtonGroup, m_qtBoxLayout, n, choices );
    m_qtBoxLayout->addStretch(1);
    m_qtGroupBox->setLayout(m_qtBoxLayout);

    return QtCreateControl( parent, id, pos, size, style, val, name );
}

static QAbstractButton *GetButtonAt( const QButtonGroup *group, unsigned int n )
{
    // It might be possible to coerce QButtonGroup::setId() and QButtonGroup::button( id )
    // to retrieve the button via an index. But for now the approach via QButtonGroup::buttons()
    // should be fast enough.

    QList< QAbstractButton * > buttons = group->buttons();
    return n < static_cast< unsigned >( buttons.size() ) ? buttons.at( n ) : NULL;
}

#define INVALID_INDEX_MESSAGE wxT( "invalid radio box index" )

#define CHECK_BUTTON( button, rc ) \
    wxCHECK_MSG( button != NULL, rc, INVALID_INDEX_MESSAGE )

bool wxRadioBox::Enable(unsigned int n, bool enable)
{
    QAbstractButton *qtButton = GetButtonAt( m_qtButtonGroup, n );
    CHECK_BUTTON( qtButton, false );

    qtButton->setEnabled( enable );
    return true;
}

bool wxRadioBox::Show(unsigned int n, bool show)
{
    QAbstractButton *qtButton = GetButtonAt( m_qtButtonGroup, n );
    CHECK_BUTTON( qtButton, false );

    qtButton->setVisible( show );
    return true;
}

bool wxRadioBox::IsItemEnabled(unsigned int n) const
{
    QAbstractButton *qtButton = GetButtonAt( m_qtButtonGroup, n );
    CHECK_BUTTON( qtButton, false );

    return qtButton->isEnabled();
}

bool wxRadioBox::IsItemShown(unsigned int n) const
{
    QAbstractButton *qtButton = GetButtonAt( m_qtButtonGroup, n );
    CHECK_BUTTON( qtButton, false );

    return qtButton->isVisible();
}

unsigned wxRadioBox::GetCount() const
{
    QList< QAbstractButton * > buttons = m_qtButtonGroup->buttons();
    return buttons.size();
}

wxString wxRadioBox::GetString(unsigned int n) const
{
    QAbstractButton *qtButton = GetButtonAt( m_qtButtonGroup, n );
    CHECK_BUTTON( qtButton, wxEmptyString );

    return wxQtConvertString( qtButton->text() );
}

void wxRadioBox::SetString(unsigned int n, const wxString& s)
{
    QAbstractButton *qtButton = GetButtonAt( m_qtButtonGroup, n );
    wxCHECK_RET( qtButton != NULL, INVALID_INDEX_MESSAGE );

    qtButton->setText( wxQtConvertString( s ));
}

void wxRadioBox::SetSelection(int n)
{
    QAbstractButton *qtButton = GetButtonAt( m_qtButtonGroup, n );
    wxCHECK_RET( qtButton != NULL, INVALID_INDEX_MESSAGE );
    qtButton->setChecked( true );
}

int wxRadioBox::GetSelection() const
{
    QAbstractButton *qtButton = m_qtButtonGroup->checkedButton();
    if ( qtButton != NULL )
    {
        QList< QAbstractButton * > buttons = m_qtButtonGroup->buttons();
        return buttons.indexOf( qtButton );
    }
    else
        return wxNOT_FOUND;
}

QGroupBox *wxRadioBox::GetHandle() const
{
    return m_qtGroupBox;
}

