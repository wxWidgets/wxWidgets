/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/listbox.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/listbox.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QListWidget>

class wxQtListWidget : public wxQtEventSignalHandler< QListWidget, wxListBox >
{
public:
    wxQtListWidget( wxWindow *parent, wxListBox *handler );

private:
    void clicked( const QModelIndex &index );
    void doubleClicked( const QModelIndex &index );
};

wxQtListWidget::wxQtListWidget( wxWindow *parent, wxListBox *handler )
    : wxQtEventSignalHandler< QListWidget, wxListBox >( parent, handler )
{
    connect(this, &QListWidget::clicked, this, &wxQtListWidget::clicked);
    connect(this, &QListWidget::doubleClicked, this, &wxQtListWidget::doubleClicked);
}

void wxQtListWidget::clicked(const QModelIndex &index )
{
    wxListBox *handler = GetHandler();
    if ( handler )
        handler->QtSendEvent(wxEVT_LISTBOX, index, true);
}

void wxQtListWidget::doubleClicked( const QModelIndex &index )
{
    wxListBox *handler = GetHandler();
    if ( handler )
        handler->QtSendEvent(wxEVT_LISTBOX_DCLICK, index, true);
}


wxListBox::wxListBox()
{
    Init();
}

wxListBox::wxListBox(wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        int n, const wxString choices[],
        long style,
        const wxValidator& validator,
        const wxString& name)
{
    Create( parent, id, pos, size, n, choices, style, validator, name );
}

wxListBox::wxListBox(wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        const wxArrayString& choices,
        long style,
        const wxValidator& validator,
        const wxString& name)
{
    Create( parent, id, pos, size, choices, style, validator, name );
}

wxListBox::~wxListBox()
{
    Clear();
}

bool wxListBox::Create(wxWindow *parent, wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            int n, const wxString choices[],
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    Init();
    QListWidgetItem* item;
    m_qtWindow = m_qtListWidget = new wxQtListWidget( parent, this );

    while ( n-- > 0 )
    {
        item = new QListWidgetItem();
        item->setText(wxQtConvertString( *choices++ ));
        if ( m_hasCheckBoxes )
        {
            item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            item->setCheckState(Qt::Unchecked);
        }
        m_qtListWidget->addItem(item);
    }
    return wxListBoxBase::Create( parent, id, pos, size, style, validator, name );
}

bool wxListBox::Create(wxWindow *parent, wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& WXUNUSED(choices),
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    Init();
    m_qtWindow = m_qtListWidget = new wxQtListWidget( parent, this );

    return wxListBoxBase::Create( parent, id, pos, size, style, validator, name );
}

void wxListBox::Init()
{
#if wxUSE_CHECKLISTBOX
    m_hasCheckBoxes = false;
#endif // wxUSE_CHECKLISTBOX
}

bool wxListBox::IsSelected(int n) const
{
    QListWidgetItem* item = m_qtListWidget->item(n);
    return item->isSelected();
}

int wxListBox::GetSelections(wxArrayInt& WXUNUSED(aSelections)) const
{
    return 0;
}

unsigned wxListBox::GetCount() const
{
    return m_qtListWidget->count();
}

wxString wxListBox::GetString(unsigned int n) const
{    
    QListWidgetItem* item = m_qtListWidget->item(n);
    wxCHECK_MSG(item != NULL, wxString(), wxT("wrong listbox index") );
    return wxQtConvertString( item->text() );
}

void wxListBox::SetString(unsigned int n, const wxString& WXUNUSED(s))
{
    QListWidgetItem* item = m_qtListWidget->item(n);
    wxCHECK_RET(item != NULL, wxT("wrong listbox index") );
    if ( m_hasCheckBoxes )
    {
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setCheckState(Qt::Unchecked);
    }
}

int wxListBox::GetSelection() const
{
    return m_qtListWidget->currentIndex().row();
}

void wxListBox::DoSetFirstItem(int WXUNUSED(n))
{
}

void wxListBox::DoSetSelection(int n, bool select)
{
    return m_qtListWidget->setCurrentRow(n, select ? QItemSelectionModel::Select : QItemSelectionModel::Deselect );
}

int wxListBox::DoInsertItems(const wxArrayStringsAdapter & items,
                          unsigned int pos,
                          void **clientData,
                          wxClientDataType type)
{
    InvalidateBestSize();
    int n = DoInsertItemsInLoop(items, pos, clientData, type);
    UpdateOldSelections();
    return n;
}

int wxListBox::DoInsertOneItem(const wxString& text, unsigned int pos)
{
    QListWidgetItem* item = new QListWidgetItem();
    item->setText(wxQtConvertString( text ));
    if ( m_hasCheckBoxes )
    {
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setCheckState(Qt::Unchecked);
    }
    m_qtListWidget->insertItem(pos, item);
    return pos;
}

void wxListBox::DoSetItemClientData(unsigned int n, void *clientData)
{
    QListWidgetItem* item = m_qtListWidget->item(n);
    QVariant variant = qVariantFromValue(clientData);
    item->setData(Qt::UserRole, variant);
}

void *wxListBox::DoGetItemClientData(unsigned int n) const
{
    QListWidgetItem* item = m_qtListWidget->item(n);
    QVariant variant = item->data(Qt::UserRole);
    return variant.value<void *>();
}

void wxListBox::DoClear()
{
    m_qtListWidget->clear();
}

void wxListBox::DoDeleteOneItem(unsigned int pos)
{
    QListWidgetItem* item = m_qtListWidget->item(pos);
    delete item;
}

QWidget *wxListBox::GetHandle() const
{
    return m_qtListWidget;
}

void wxListBox::QtSendEvent(wxEventType evtType, const QModelIndex &index, bool selected)
{
    SendEvent(evtType, index.row(), selected);
}

QScrollArea *wxListBox::QtGetScrollBarsContainer() const
{
    return (QScrollArea *) m_qtListWidget;
}
