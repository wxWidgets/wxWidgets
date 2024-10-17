/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/listbox.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_LISTBOX

#include "wx/listbox.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QListWidget>

// We need a way to generate wxEVT_CHECKLISTBOX events whenever the checkbox's
// state has changed. using QListWidget::itemChanged() signal to do that is not
// going to work as you might think, because it is also emitted when the other
// data of item has changed (text, icon, ...). The solution is to derive from
// QListWidgetItem and override setData(). from which we can generate the event.
class wxQtListWidgetItem : public QListWidgetItem
{
public:
    // Inherit QListWidgetItem constructors
    using QListWidgetItem::QListWidgetItem;

    virtual void setData(int role, const QVariant& value) override;
};

class wxQtListWidget : public wxQtEventSignalHandler< QListWidget, wxListBox >
{
public:
    wxQtListWidget( wxWindow *parent, wxListBox *handler );

private:
    void OnCurrentItemChange(QListWidgetItem *current, QListWidgetItem *previous);
    void doubleClicked( const QModelIndex &index );
};

wxQtListWidget::wxQtListWidget( wxWindow *parent, wxListBox *handler )
    : wxQtEventSignalHandler< QListWidget, wxListBox >( parent, handler )
{
    connect(this, &QListWidget::currentItemChanged, this, &wxQtListWidget::OnCurrentItemChange);
    connect(this, &QListWidget::doubleClicked, this, &wxQtListWidget::doubleClicked);
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

void wxQtListWidgetItem::setData(int role, const QVariant& value)
{
    QListWidgetItem::setData(role, value);

    if ( role == Qt::CheckStateRole )
    {
        const auto list = static_cast<wxQtListWidget*>(listWidget());
        if ( list && list->isVisible() && !list->signalsBlocked() )
        {
            const auto handler = list->GetHandler();
            if ( handler )
            {
                const int rowIndex = list->row(this);
                handler->QtSendEvent(wxEVT_CHECKLISTBOX, rowIndex, true);
            }
        }
    }
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
        QListWidgetItem* item = new wxQtListWidgetItem();
        item->setText(wxQtConvertString( *choices++ ));
        if ( m_hasCheckBoxes )
        {
            item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            item->setCheckState(Qt::Unchecked);
        }
        GetQListWidget()->addItem(item);
    }
    return wxListBoxBase::Create( parent, id, pos, size,
                                  style | wxVSCROLL | wxHSCROLL,
                                  validator, name );
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

    // Notice that we should explicitly call QListWidgetItem::setCheckState() here if
    // the listbox has check boxes. otherwise the checkboxes won't be shown on screen.
    //
    // From https://doc.qt.io/qt-5/qt.html#ItemFlag-enum:
    //   Note that checkable items need to be given both a suitable set of flags and an initial state,
    //   indicating whether the item is checked or not. This is handled automatically for model/view components,
    //   but needs to be explicitly set for instances of QListWidgetItem, QTableWidgetItem, and QTreeWidgetItem.

    for ( const auto& choice : choices )
    {
        QListWidgetItem* item = new wxQtListWidgetItem;
        item->setText(wxQtConvertString( choice ));
        if ( m_hasCheckBoxes )
        {
            item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            item->setCheckState(Qt::Unchecked);
        }
        GetQListWidget()->addItem(item);
    }

    return wxListBoxBase::Create( parent, id, pos, size,
                                  style | wxVSCROLL | wxHSCROLL,
                                  validator, name );
}

void wxListBox::DoCreate(wxWindow* parent, long style)
{
    m_qtWindow = new wxQtListWidget( parent, this );

    if ( style & wxLB_SORT )
    {
        GetQListWidget()->setSortingEnabled(true);
    }

    // The following styles are mutually exclusive
    if ( style & wxLB_SINGLE )
    {
        GetQListWidget()->setSelectionMode(QAbstractItemView::SingleSelection);
    }
    else if ( style & wxLB_MULTIPLE )
    {
        GetQListWidget()->setSelectionMode(QAbstractItemView::MultiSelection);
    }
    else if ( style & wxLB_EXTENDED )
    {
        GetQListWidget()->setSelectionMode(QAbstractItemView::ExtendedSelection);
    }
}

QListWidget* wxListBox::GetQListWidget() const
{
    return static_cast<QListWidget*>(m_qtWindow);
}

bool wxListBox::IsSelected(int n) const
{
    QListWidgetItem* item = GetQListWidget()->item(n);
    return item->isSelected();
}

int wxListBox::GetSelections(wxArrayInt& aSelections) const
{
    aSelections.clear();

    Q_FOREACH(QListWidgetItem* l, GetQListWidget()->selectedItems())
    {
        aSelections.push_back( GetQListWidget()->row(l) );
    }

    return aSelections.size();
}

unsigned wxListBox::GetCount() const
{
    return GetQListWidget()->count();
}

wxString wxListBox::GetString(unsigned int n) const
{
    QListWidgetItem* item = GetQListWidget()->item(n);
    wxCHECK_MSG(item != nullptr, wxString(), wxT("wrong listbox index") );
    return wxQtConvertString( item->text() );
}

void wxListBox::SetString(unsigned int n, const wxString& s)
{
    QListWidgetItem* item = GetQListWidget()->item(n);
    wxCHECK_RET(item != nullptr, wxT("wrong listbox index") );
    item->setText(wxQtConvertString(s));
}

int wxListBox::GetSelection() const
{
    if ( GetQListWidget()->selectedItems().empty() )
    {
        return wxNOT_FOUND;
    }


    QListWidgetItem* item = GetQListWidget()->selectedItems().first();

    return GetQListWidget()->row(item);
}

void wxListBox::EnsureVisible(int n)
{
    wxCHECK_RET( n >= 0 && n < static_cast<int>(GetCount()),
                "invalid index in wxListBox::EnsureVisible" );

    GetQListWidget()->scrollToItem(GetQListWidget()->item(n));
}

int wxListBox::GetTopItem() const
{
    const auto item = GetQListWidget()->itemAt(2, 2);

    return item ? GetQListWidget()->row(item) : -1;
}

int wxListBox::GetCountPerPage() const
{
    wxCHECK_MSG(GetCount() > 0, 0,
        "wxListBox needs at least one item to calculate the count per page");

    // this may not be exact but should be a good approximation:
    const int h = GetQListWidget()->visualItemRect(
                    GetQListWidget()->item(0)).height();
    if ( h )
        return GetQListWidget()->viewport()->height() / h;

    return 0;
}

void wxListBox::DoSetFirstItem(int n)
{
    GetQListWidget()->scrollToItem(GetQListWidget()->item(n), QAbstractItemView::PositionAtTop);
}

void wxListBox::DoSetSelection(int n, bool select)
{
    if ( n == wxNOT_FOUND )
    {
        UnSelectAll();
        return;
    }

    GetQListWidget()->item(n)->setSelected(select);
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
    QListWidgetItem* item = new wxQtListWidgetItem();
    item->setText(wxQtConvertString( text ));
    if ( m_hasCheckBoxes )
    {
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setCheckState(Qt::Unchecked);
    }
    GetQListWidget()->insertItem(pos, item);
    return IsSorted() ? GetQListWidget()->row(item) : pos;
}

void wxListBox::DoSetItemClientData(unsigned int n, void *clientData)
{
    QListWidgetItem* item = GetQListWidget()->item(n);
    QVariant variant = QVariant::fromValue(clientData);
    item->setData(Qt::UserRole, variant);
}

void *wxListBox::DoGetItemClientData(unsigned int n) const
{
    QListWidgetItem* item = GetQListWidget()->item(n);
    QVariant variant = item->data(Qt::UserRole);
    return variant.value<void *>();
}

void wxListBox::DoClear()
{
    GetQListWidget()->clear();
}

void wxListBox::DoDeleteOneItem(unsigned int pos)
{
    QListWidgetItem* item = GetQListWidget()->item(pos);
    delete item;
}

int wxListBox::DoListHitTest(const wxPoint& point) const
{
    QListWidgetItem* item = GetQListWidget()->itemAt( wxQtConvertPoint(point) );

    return item ? GetQListWidget()->row(item) : wxNOT_FOUND;
}

void wxListBox::QtSendEvent(wxEventType evtType, int rowIndex, bool selected)
{
    SendEvent(evtType, rowIndex, selected);
}

void wxListBox::UnSelectAll()
{
    Q_FOREACH(QListWidgetItem* l, GetQListWidget()->selectedItems())
    {
        l->setSelected(false);
    }
}

#endif // wxUSE_LISTBOX
