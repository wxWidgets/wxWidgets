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
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QBoxLayout>

class wxQtRadioBox : public wxQtEventSignalHandler< QGroupBox, wxRadioBox >
{
public:
    wxQtRadioBox( wxWindow *parent, wxRadioBox *handler ):
        wxQtEventSignalHandler< QGroupBox, wxRadioBox >( parent, handler ){}
};


class wxQtButtonGroup : public QButtonGroup, public wxQtSignalHandler
{
public:
    wxQtButtonGroup( QGroupBox *parent, wxRadioBox *handler )
        : QButtonGroup(parent),
          wxQtSignalHandler(handler)
    {
        connect(this,
                static_cast<void (QButtonGroup::*)(int index)>(&QButtonGroup::buttonClicked),
                this, &wxQtButtonGroup::buttonClicked);
    }

    wxRadioBox* GetRadioBox() const
    {
        return static_cast<wxRadioBox*>(wxQtSignalHandler::GetHandler());
    }

private:
    void buttonClicked(int index);
};

void wxQtButtonGroup::buttonClicked(int index)
{
    wxRadioBox *handler = GetRadioBox();
    if ( handler )
    {
        wxCommandEvent event( wxEVT_RADIOBOX, handler->GetId() );
        event.SetInt(index);
        event.SetString(wxQtConvertString(button(index)->text()));
        EmitEvent( event );
    }
}


wxIMPLEMENT_DYNAMIC_CLASS(wxRadioBox, wxControl);


wxRadioBox::wxRadioBox() :
    m_qtGroupBox(nullptr),
    m_qtButtonGroup(nullptr),
    m_qtGridLayout(nullptr)
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


static void AddChoices( QButtonGroup *qtButtonGroup, QGridLayout *qtGridLayout, int count, const wxString choices[], int style, int majorDim )
{
    if ( count <= 0 )
        return;

    // wxRA_SPECIFY_COLS means that we arrange buttons in
    // left to right order and GetMajorDim() is the number of columns while
    // wxRA_SPECIFY_ROWS means that the buttons are arranged top to bottom and
    // GetMajorDim() is the number of rows.

   const bool columnMajor = style & wxRA_SPECIFY_COLS;
   const int numMajor = majorDim > 0 ? majorDim : count;

    bool isFirst = true;

    for ( int i = 0; i < count; ++i )
    {
        QRadioButton *btn = new QRadioButton(wxQtConvertString (choices[i]) );
        qtButtonGroup->addButton( btn, i );

        int row;
        int col;

        if (columnMajor)
        {
            col = i % numMajor;
            row = i / numMajor;

        }
        else
        {
            col = i / numMajor;
            row = i % numMajor;
        }

        qtGridLayout->addWidget( btn, row, col );

        if (isFirst)
        {
            btn->setChecked( true );
            isFirst = false;
        }
    }
}


bool wxRadioBox::Create(wxWindow *parent,
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
    m_qtGroupBox = new wxQtRadioBox( parent, this );
    m_qtGroupBox->setTitle( wxQtConvertString( title ) );
    m_qtButtonGroup = new wxQtButtonGroup( m_qtGroupBox, this );

    if ( !(style & (wxRA_SPECIFY_ROWS | wxRA_SPECIFY_COLS)) )
        style |= wxRA_SPECIFY_COLS;

    m_qtGridLayout = new QGridLayout;

    AddChoices( m_qtButtonGroup, m_qtGridLayout, n, choices, style, majorDim );

    QVBoxLayout *vertLayout = new QVBoxLayout;
    vertLayout->addLayout(m_qtGridLayout);
    vertLayout->addStretch();

    QHBoxLayout *horzLayout = new QHBoxLayout;
    horzLayout->addLayout(vertLayout);
    horzLayout->addStretch();

    m_qtGroupBox->setLayout(horzLayout);

    SetMajorDim(majorDim == 0 ? n : majorDim, style);
    return wxControl::Create( parent, id, pos, size, style, val, name );
}

static QAbstractButton *GetButtonAt( const QButtonGroup *group, unsigned int n )
{
    // It might be possible to coerce QButtonGroup::setId() and QButtonGroup::button( id )
    // to retrieve the button via an index. But for now the approach via QButtonGroup::buttons()
    // should be fast enough.

    QList< QAbstractButton * > buttons = group->buttons();
    return n < static_cast< unsigned >( buttons.size() ) ? buttons.at( n ) : nullptr;
}

#define INVALID_INDEX_MESSAGE wxT( "invalid radio box index" )

#define CHECK_BUTTON( button, rc ) \
    wxCHECK_MSG( button != nullptr, rc, INVALID_INDEX_MESSAGE )

bool wxRadioBox::Enable(unsigned int n, bool enable)
{
    if ( enable && !m_qtGroupBox->isEnabled() )
    {
        m_qtGroupBox->setEnabled( true );

        for ( unsigned int i = 0; i < GetCount(); ++i )
        {
            QAbstractButton *qtButton = GetButtonAt( m_qtButtonGroup, i );
            CHECK_BUTTON( qtButton, false );

            qtButton->setEnabled( i == n );
        }
    }
    else
    {
        QAbstractButton *qtButton = GetButtonAt( m_qtButtonGroup, n );
        CHECK_BUTTON( qtButton, false );
        qtButton->setEnabled(enable);
    }

    return true;
}

bool wxRadioBox::Enable( bool enable )
{
    if ( m_qtGroupBox->isEnabled() == enable )
    {
        for ( unsigned int i = 0; i < GetCount(); ++i )
        {
            QAbstractButton *qtButton = GetButtonAt( m_qtButtonGroup, i );
            CHECK_BUTTON( qtButton, false );
            qtButton->setEnabled( enable );
        }
    }

    m_qtGroupBox->setEnabled( enable );

    return true;
}

bool wxRadioBox::Show(unsigned int n, bool show)
{
    if ( show && !m_qtGroupBox->isVisible() )
    {
        m_qtGroupBox->setVisible(true);

        for ( unsigned int i = 0; i < GetCount(); ++i )
        {
            QAbstractButton *qtButton = GetButtonAt( m_qtButtonGroup, i );
            CHECK_BUTTON( qtButton, false );

            i == n ? qtButton->setVisible( true ) : qtButton->setVisible( false );
        }
    }
    else
    {
        QAbstractButton *qtButton = GetButtonAt( m_qtButtonGroup, n );
        CHECK_BUTTON( qtButton, false );

        qtButton->setVisible( show );
    }

    return true;
}

bool wxRadioBox::Show( bool show )
{
    if ( !wxControl::Show(show) )
        return false;

    if ( !m_qtGroupBox )
        return false;

    if( m_qtGroupBox->isVisible() == show )
    {
        for( unsigned int i = 0; i < GetCount(); ++i )
        {
            QAbstractButton *qtButton = GetButtonAt( m_qtButtonGroup, i );
            CHECK_BUTTON( qtButton, false );
            qtButton->setVisible( show );
        }
    }

    m_qtGroupBox->setVisible( show );

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

unsigned int wxRadioBox::GetCount() const
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
    wxCHECK_RET( qtButton != nullptr, INVALID_INDEX_MESSAGE );

    qtButton->setText( wxQtConvertString( s ));
}

void wxRadioBox::SetSelection(int n)
{
    QAbstractButton *qtButton = GetButtonAt( m_qtButtonGroup, n );
    wxCHECK_RET( qtButton != nullptr, INVALID_INDEX_MESSAGE );
    qtButton->setChecked( true );
}

int wxRadioBox::GetSelection() const
{
    QAbstractButton *qtButton = m_qtButtonGroup->checkedButton();
    if ( qtButton != nullptr )
    {
        QList< QAbstractButton * > buttons = m_qtButtonGroup->buttons();
        return buttons.indexOf( qtButton );
    }
    else
        return wxNOT_FOUND;
}

QWidget *wxRadioBox::GetHandle() const
{
    return m_qtGroupBox;
}

void wxRadioBox::SetLabel(const wxString& label)
{
    wxControlBase::SetLabel( label );

    m_qtGroupBox->setTitle( wxQtConvertString( label ) );
}

wxString wxRadioBox::GetLabel() const
{
    return wxQtConvertString( m_qtGroupBox->title() );
}
