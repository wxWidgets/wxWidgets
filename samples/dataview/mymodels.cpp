/////////////////////////////////////////////////////////////////////////////
// Name:        mymodels.cpp
// Purpose:     wxDataViewCtrl wxWidgets sample
// Author:      Robert Roebling
// Modified by: Francesco Montorsi, Bo Yang
// Created:     06/01/06
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/dataview.h"
#include "mymodels.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

#include "null.xpm"
#include "wx_small.xpm"


// ----------------------------------------------------------------------------
// MyMusicTreeModel
// ----------------------------------------------------------------------------

MyMusicTreeModel::MyMusicTreeModel()
{
    m_root = new MyMusicTreeModelNode( NULL, wxT("My Music" ));

    // setup pop music
    m_pop = new MyMusicTreeModelNode( m_root, wxT("Pop music") );
    m_pop->Append(
        new MyMusicTreeModelNode( m_pop, wxT("You are not alone"), wxT("Michael Jackson"), 1995 ) );
    m_pop->Append(
        new MyMusicTreeModelNode( m_pop, wxT("Take a bow"), wxT("Madonna"), 1994 ) );
    m_root->Append( m_pop );

    // setup classical music
    m_classical = new MyMusicTreeModelNode( m_root, wxT("Classical music") );
    m_ninth = new MyMusicTreeModelNode( m_classical, wxT("Ninth symphony"),
                                        wxT("Ludwig van Beethoven"), 1824 );
    m_classical->Append( m_ninth );
    m_classical->Append( new MyMusicTreeModelNode( m_classical, wxT("German Requiem"),
                                                   wxT("Johannes Brahms"), 1868 ) );
    m_root->Append( m_classical );

    m_classicalMusicIsKnownToControl = false;
}

wxString MyMusicTreeModel::GetTitle( const wxDataViewItem &item ) const
{
    MyMusicTreeModelNode *node = (MyMusicTreeModelNode*) item.GetID();
    if (!node)      // happens if item.IsOk()==false
        return wxEmptyString;

    return node->m_title;
}

int MyMusicTreeModel::GetYear( const wxDataViewItem &item ) const
{
    MyMusicTreeModelNode *node = (MyMusicTreeModelNode*) item.GetID();
    if (!node)      // happens if item.IsOk()==false
        return 2000;

    return node->m_year;
}

void MyMusicTreeModel::AddToClassical( const wxString &title, const wxString &artist,
                                       unsigned int year )
{
    if (!m_classical)
    {
        wxASSERT(m_root);

        // it was removed: restore it
        m_classical = new MyMusicTreeModelNode( m_root, wxT("Classical music") );
        m_root->Append( m_classical );
    }

    // add to the classical music node a new node:
    MyMusicTreeModelNode *child_node =
        new MyMusicTreeModelNode( m_classical, title, artist, year );
    m_classical->Append( child_node );

    if (m_classicalMusicIsKnownToControl)
    {
        // notify control
        wxDataViewItem child( (void*) child_node );
        wxDataViewItem parent( (void*) m_classical );
        ItemAdded( parent, child );
    }
}

void MyMusicTreeModel::Delete( const wxDataViewItem &item )
{
    MyMusicTreeModelNode *node = (MyMusicTreeModelNode*) item.GetID();
    if (!node)      // happens if item.IsOk()==false
        return;

    wxDataViewItem parent( node->GetParent() );
    if (!parent.IsOk())
    {
        wxASSERT(node == m_root);

        // don't make the control completely empty:
        wxLogError("Cannot remove the root item!");
        return;
    }

    // is the node one of those we keep stored in special pointers?
    if (node == m_pop)
        m_pop = NULL;
    else if (node == m_classical)
        m_classical = NULL;
    else if (node == m_ninth)
        m_ninth = NULL;

    // first remove the node from the parent's array of children;
    // NOTE: MyMusicTreeModelNodePtrArray is only an array of _pointers_
    //       thus removing the node from it doesn't result in freeing it
    node->GetParent()->GetChildren().Remove( node );

    // free the node
    delete node;

    // notify control
    ItemDeleted( parent, item );
}

int MyMusicTreeModel::Compare( const wxDataViewItem &item1, const wxDataViewItem &item2,
                               unsigned int column, bool ascending )
{
    wxASSERT(item1.IsOk() && item2.IsOk());
        // should never happen

    if (IsContainer(item1) && IsContainer(item2))
    {
        wxVariant value1, value2;
        GetValue( value1, item1, 0 );
        GetValue( value2, item2, 0 );

        wxString str1 = value1.GetString();
        wxString str2 = value2.GetString();
        int res = str1.Cmp( str2 );
        if (res) return res;

        // items must be different
        wxUIntPtr litem1 = (wxUIntPtr) item1.GetID();
        wxUIntPtr litem2 = (wxUIntPtr) item2.GetID();

        return litem1-litem2;
    }

    return wxDataViewModel::Compare( item1, item2, column, ascending );
}

void MyMusicTreeModel::GetValue( wxVariant &variant,
                                 const wxDataViewItem &item, unsigned int col ) const
{
    wxASSERT(item.IsOk());

    MyMusicTreeModelNode *node = (MyMusicTreeModelNode*) item.GetID();
    switch (col)
    {
    case 0:
        variant = node->m_title;
        break;
    case 1:
        variant = node->m_artist;
        break;
    case 2:
        variant = (long) node->m_year;
        break;
    case 3:
        variant = node->m_quality;
        break;
    case 4:
        // wxMac doesn't conceal the popularity progress renderer, return 0 for containers
        if (IsContainer(item))
            variant = (long) 0;
        else
            variant = (long) 80;  // all music is very 80% popular
        break;
    case 5:
        // Make size of red square depend on year
        if (GetYear(item) < 1900)
            variant = (long) 35;
        else
            variant = (long) 25;
        break;

    default:
        wxLogError( wxT("MyMusicTreeModel::GetValue: wrong column %d"), col );
    }
}

bool MyMusicTreeModel::SetValue( const wxVariant &variant,
                                 const wxDataViewItem &item, unsigned int col )
{
    wxASSERT(item.IsOk());

    MyMusicTreeModelNode *node = (MyMusicTreeModelNode*) item.GetID();
    switch (col)
    {
        case 0:
            node->m_title = variant.GetString();
            return true;
        case 1:
            node->m_artist = variant.GetString();
            return true;
        case 2:
            node->m_year = variant.GetLong();
            return true;
        case 3:
            node->m_quality = variant.GetString();
            return true;

        default:
            wxLogError( wxT("MyMusicTreeModel::SetValue: wrong column") );
    }
    return false;
}

wxDataViewItem MyMusicTreeModel::GetParent( const wxDataViewItem &item ) const
{
    // the invisible root node has no parent
    if (!item.IsOk())
        return wxDataViewItem(0);

    MyMusicTreeModelNode *node = (MyMusicTreeModelNode*) item.GetID();

    // "MyMusic" also has no parent
    if (node == m_root)
        return wxDataViewItem(0);

    return wxDataViewItem( (void*) node->GetParent() );
}

bool MyMusicTreeModel::IsContainer( const wxDataViewItem &item ) const
{
    // the invisble root node can have children
    // (in our model always "MyMusic")
    if (!item.IsOk())
        return true;

    MyMusicTreeModelNode *node = (MyMusicTreeModelNode*) item.GetID();
    return node->IsContainer();
}

unsigned int MyMusicTreeModel::GetChildren( const wxDataViewItem &parent,
                                            wxDataViewItemArray &array ) const
{
    MyMusicTreeModelNode *node = (MyMusicTreeModelNode*) parent.GetID();
    if (!node)
    {
        array.Add( wxDataViewItem( (void*) m_root ) );
        return 1;
    }

    if (node == m_classical)
    {
        MyMusicTreeModel *model = (MyMusicTreeModel*)(const MyMusicTreeModel*) this;
        model->m_classicalMusicIsKnownToControl = true;
    }

    if (node->GetChildCount() == 0)
    {
        return 0;
    }

    unsigned int count = node->GetChildren().GetCount();
    for (unsigned int pos = 0; pos < count; pos++)
    {
        MyMusicTreeModelNode *child = node->GetChildren().Item( pos );
        array.Add( wxDataViewItem( (void*) child ) );
    }

    return count;
}



// ----------------------------------------------------------------------------
// MyListModel
// ----------------------------------------------------------------------------

static int my_sort_reverse( int *v1, int *v2 )
{
   return *v2-*v1;
}

static int my_sort( int *v1, int *v2 )
{
   return *v1-*v2;
}

#ifdef __WXMAC__
    #define INITIAL_NUMBER_OF_ITEMS 100
#else
    #define INITIAL_NUMBER_OF_ITEMS 100000
#endif

MyListModel::MyListModel() :
        wxDataViewVirtualListModel( INITIAL_NUMBER_OF_ITEMS )
{
    m_virtualItems = INITIAL_NUMBER_OF_ITEMS;

    // the first 100 items are really stored in this model;
    // all the others are synthetized on request
    for (unsigned int i = 0; i < 100; i++)
    {
        wxString str;
        str.Printf( wxT("real row %d"), i );
        m_array.Add( str );
    }

    m_icon[0] = wxIcon( null_xpm );
    m_icon[1] = wxIcon( wx_small_xpm );
}

void MyListModel::Prepend( const wxString &text )
{
    m_array.Insert( text, 0 );
    RowPrepended();
}

void MyListModel::DeleteItem( const wxDataViewItem &item )
{
    unsigned int row = GetRow( item );
    if (row >= m_array.GetCount())
        return;

    m_array.RemoveAt( row );
    RowDeleted( row );
}

void MyListModel::DeleteItems( const wxDataViewItemArray &items )
{
    unsigned i;
    wxArrayInt rows;
    for (int i = 0; i < items.GetCount(); i++)
    {
        unsigned int row = GetRow( items[i] );
        if (row < m_array.GetCount())
            rows.Add( row );
    }

    if (rows.GetCount() == 0)
    {
        // none of the selected items were in the range of the items
        // which we store... for simplicity, don't allow removing them
        wxLogError("Cannot remove rows with an index greater than %d", m_array.GetCount());
        return;
    }

    // Sort in descending order so that the last
    // row will be deleted first. Otherwise the
    // remaining indeces would all be wrong.
    rows.Sort( my_sort_reverse );
    for (i = 0; i < rows.GetCount(); i++)
        m_array.RemoveAt( rows[i] );

    // This is just to test if wxDataViewCtrl can
    // cope with removing rows not sorted in
    // descending order
    rows.Sort( my_sort );
    RowsDeleted( rows );
}

void MyListModel::AddMany()
{
    m_virtualItems += 1000;
    Reset( m_array.GetCount() + m_virtualItems );
}

void MyListModel::GetValueByRow( wxVariant &variant,
                                 unsigned int row, unsigned int col ) const
{
    if (col==0)
    {
        if (row >= m_array.GetCount())
            variant = wxString::Format( wxT("virtual row %d"), row );
        else
            variant = m_array[ row ];
    }
    else if (col==1)
    {
        wxDataViewIconText data( wxT("test"), m_icon[ row%2 ] );
        variant << data;
    }
    else if (col==2)
    {
        if (row >= m_array.GetCount())
            variant = wxT("plain");
        else
            variant = wxT("blue/green/red");
    }
}

bool MyListModel::GetAttrByRow( unsigned int row, unsigned int col,
                                wxDataViewItemAttr &attr )
{
    if (col != 2)
        return false;

    if (row < m_array.GetCount())
    {
        attr.SetColour( (row%2) == 0 ? *wxBLUE :
                            ((row%3) == 0 ? *wxGREEN : *wxRED) );
        attr.SetItalic( (row%2) == 5 );
    }

    return true;
}

bool MyListModel::SetValueByRow( const wxVariant &variant,
                                 unsigned int row, unsigned int col )
{
    if (col == 0)
    {
        if (row >= m_array.GetCount())
        {
            // the item is not in the range of the items
            // which we store... for simplicity, don't allow editing it
            wxLogError("Cannot edit rows with an index greater than %d", m_array.GetCount());
            return false;
        }

        m_array[row] = variant.GetString();
        return true;
    }

    return false;
}
