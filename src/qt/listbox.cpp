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
    void OnCurrentItemChange(QListWidgetItem *current, QListWidgetItem *previous);
    void doubleClicked( const QModelIndex &index );
    void itemChanged(QListWidgetItem *item);
};

wxQtListWidget::wxQtListWidget( wxWindow *parent, wxListBox *handler )
    : wxQtEventSignalHandler< QListWidget, wxListBox >( parent, handler )
{
    connect(this, &QListWidget::currentItemChanged, this, &wxQtListWidget::OnCurrentItemChange);
    connect(this, &QListWidget::doubleClicked, this, &wxQtListWidget::doubleClicked);
    connect(this, &QListWidget::itemChanged, this, &wxQtListWidget::itemChanged);
}

void wxQtListWidget::OnCurrentItemChange(QListWidgetItem *current, QListWidgetItem *)
{
    if ( !current )
        return;

    wxListBox *handler = GetHandler();
    if ( handler )
    {
        const QModelIndex &index = indexFromItem(current);
        handler->QtSendEvent(wxEVT_LISTBOX, index.row(), true);
    }
}

void wxQtListWidget::doubleClicked( const QModelIndex &index )
{
    wxListBox *handler = GetHandler();
    if ( handler )
        handler->QtSendEvent(wxEVT_LISTBOX_DCLICK, index.row(), true);
}

void wxQtListWidget::itemChanged(QListWidgetItem *item)
{
    if ( item->flags() & Qt::ItemIsUserCheckable )
    {
        wxListBox *handler = GetHandler();
        if ( handler )
        {
            int rowIndex = this->row(item);
            handler->QtSendEvent(wxEVT_CHECKLISTBOX, rowIndex, true);
        }
    }
}

wxListBox::wxListBox() :
    m_qtListWidget(NULL)
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
    DoCreate(parent, style);

    while ( n-- > 0 )
    {
        QListWidgetItem* item;
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
            const wxArrayString& choices,
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    DoCreate(parent, style);

    QStringList items;

    for (size_t i = 0; i < choices.size(); ++i)
        items.push_back(wxQtConvertString(choices[i]));

    m_qtListWidget->addItems(items);

    return wxListBoxBase::Create( parent, id, pos, size, style, validator, name );
}

void wxListBox::DoCreate(wxWindow* parent, long style)
{
    Init();

    m_qtWindow =
    m_qtListWidget = new wxQtListWidget( parent, this );

    if ( style & wxLB_SORT )
    {
        m_qtListWidget->setSortingEnabled(true);
    }

    // The following styles are mutually exclusive
    if ( style & wxLB_SINGLE )
    {
        m_qtListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    }
    else if ( style & wxLB_MULTIPLE )
    {
        m_qtListWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    }
    else if ( style & wxLB_EXTENDED )
    {
        m_qtListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    }
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

int wxListBox::GetSelections(wxArrayInt& aSelections) const
{
    aSelections.clear();

    Q_FOREACH(QListWidgetItem* l, m_qtListWidget->selectedItems())
    {
        aSelections.push_back( m_qtListWidget->row(l) );
    }

    return aSelections.size();
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

void wxListBox::SetString(unsigned int n, const wxString& s)
{
    QListWidgetItem* item = m_qtListWidget->item(n);
    wxCHECK_RET(item != NULL, wxT("wrong listbox index") );
    if ( m_hasCheckBoxes )
    {
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setCheckState(Qt::Unchecked);
    }
    item->setText(wxQtConvertString(s));
}

int wxListBox::GetSelection() const
{
    if ( m_qtListWidget->selectedItems().empty() )
    {
        return wxNOT_FOUND;
    }


    QListWidgetItem* item = m_qtListWidget->selectedItems().first();

    return m_qtListWidget->row(item);
}

void wxListBox::DoSetFirstItem(int n)
{
    m_qtListWidget->scrollToItem(m_qtListWidget->item(n), QAbstractItemView::PositionAtTop);
}

void wxListBox::DoSetSelection(int n, bool select)
{
    if ( n == wxNOT_FOUND )
    {
        UnSelectAll();
        return;
    }

    m_qtListWidget->setItemSelected( m_qtListWidget->item(n), select);
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

void wxListBox::QtSendEvent(wxEventType evtType, int rowIndex, bool selected)
{
    SendEvent(evtType, rowIndex, selected);
}

QScrollArea *wxListBox::QtGetScrollBarsContainer() const
{
    return (QScrollArea *) m_qtListWidget;
}

void wxListBox::UnSelectAll()
{
    Q_FOREACH(QListWidgetItem* l, m_qtListWidget->selectedItems())
    {
        m_qtListWidget->setItemSelected( l, false );
    }
}
