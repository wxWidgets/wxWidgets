/////////////////////////////////////////////////////////////////////////////
// Name:        mymodels.cpp
// Purpose:     wxDataViewCtrl wxWidgets sample
// Author:      Robert Roebling
// Modified by: Francesco Montorsi, Bo Yang
// Created:     06/01/06
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/artprov.h"
#include "wx/dataview.h"
#include "mymodels.h"

// ----------------------------------------------------------------------------
// MyMusicTreeModel
// ----------------------------------------------------------------------------

MyMusicTreeModel::MyMusicTreeModel()
{
    m_root = new MyMusicTreeModelNode( nullptr, "My Music" );

    // setup pop music
    m_pop = new MyMusicTreeModelNode( m_root, "Pop music" );
    m_pop->Append(
        new MyMusicTreeModelNode( m_pop, "You are not alone", "Michael Jackson", 1995 ) );
    m_pop->Append(
        new MyMusicTreeModelNode( m_pop, "Yesterday", "The Beatles", -1 /* not specified */ ) );
    m_pop->Append(
        new MyMusicTreeModelNode( m_pop, "Take a bow", "Madonna", 1994 ) );
    m_root->Append( m_pop );

    // setup classical music
    m_classical = new MyMusicTreeModelNode( m_root, "Classical music" );
    m_ninth = new MyMusicTreeModelNode( m_classical, "Ninth symphony",
                                        "Ludwig van Beethoven", 1824 );
    m_classical->Append( m_ninth );
    m_classical->Append( new MyMusicTreeModelNode( m_classical, "German Requiem",
                                                   "Johannes Brahms", 1868 ) );
    m_root->Append( m_classical );
}

wxString MyMusicTreeModel::GetTitle( const wxDataViewItem &item ) const
{
    MyMusicTreeModelNode *node = (MyMusicTreeModelNode*) item.GetID();
    if (!node)      // happens if item.IsOk()==false
        return wxEmptyString;

    return node->m_title;
}

wxString MyMusicTreeModel::GetArtist( const wxDataViewItem &item ) const
{
    MyMusicTreeModelNode *node = (MyMusicTreeModelNode*) item.GetID();
    if (!node)      // happens if item.IsOk()==false
        return wxEmptyString;

    return node->m_artist;
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
        m_classical = new MyMusicTreeModelNode( m_root, "Classical music" );
        m_root->Append( m_classical );

        // notify control
        wxDataViewItem child( (void*) m_classical );
        wxDataViewItem parent( (void*) m_root );
        ItemAdded( parent, child );
    }

    // add to the classical music node a new node:
    MyMusicTreeModelNode *child_node =
        new MyMusicTreeModelNode( m_classical, title, artist, year );
    m_classical->Append( child_node );

    // notify control
    wxDataViewItem child( (void*) child_node );
    wxDataViewItem parent( (void*) m_classical );
    ItemAdded( parent, child );
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
        wxLogError( "Cannot remove the root item!" );
        return;
    }

    // is the node one of those we keep stored in special pointers?
    if (node == m_pop)
        m_pop = nullptr;
    else if (node == m_classical)
        m_classical = nullptr;
    else if (node == m_ninth)
        m_ninth = nullptr;

    // first remove the node from the parent's array of children;
    auto& siblings = node->GetParent()->GetChildren();
    for ( auto it = siblings.begin(); it != siblings.end(); ++it )
    {
        if ( it->get() == node )
        {
            siblings.erase(it);
            break;
        }
    }

    // notify control
    ItemDeleted( parent, item );
}
void MyMusicTreeModel::Clear()
{
    m_pop       = nullptr;
    m_classical = nullptr;
    m_ninth     = nullptr;

    m_root->GetChildren().clear();

    Cleared();
}

int MyMusicTreeModel::Compare( const wxDataViewItem &item1, const wxDataViewItem &item2,
                               unsigned int column, bool ascending ) const
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
        if (node->m_year != -1)
            variant = (long) node->m_year;
        break;
    case 3:
        variant = node->m_quality;
        break;
    case 4:
        variant = 80L;  // all music is very 80% popular
        break;
    case 5:
        if (node->m_year == -1)
            variant = "n/a";
        else if (node->m_year < 1900)
            variant = "old";
        else
            variant = "new";
        break;

    default:
        wxLogError( "MyMusicTreeModel::GetValue: wrong column %d", col );
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
            wxLogError( "MyMusicTreeModel::SetValue: wrong column" );
    }
    return false;
}

bool MyMusicTreeModel::IsEnabled( const wxDataViewItem &item,
                                  unsigned int col ) const
{
    wxASSERT(item.IsOk());

    MyMusicTreeModelNode *node = (MyMusicTreeModelNode*) item.GetID();

    // disable Beethoven's ratings, his pieces can only be good
    if (col == 3 && node->m_artist.EndsWith("Beethoven"))
        return false;

    // also disable editing the year when it's not specified, this doesn't work
    // because the editor needs some initial value
    if (col == 2 && node->m_year == -1)
        return false;

    // otherwise allow editing
    return true;
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
    // the invisible root node can have children
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

    if (node->GetChildCount() == 0)
    {
        return 0;
    }

    for ( const auto& child : node->GetChildren() )
    {
        array.Add( wxDataViewItem( child.get() ) );
    }

    return array.size();
}


// ----------------------------------------------------------------------------
// MyLongMusicTreeModel
// ----------------------------------------------------------------------------

MyLongMusicTreeModel::MyLongMusicTreeModel() : MyMusicTreeModel()
{
    for (int i = 0; i < 50; i++)
    {
        AddToClassical("The Four Seasons", "Antonio Vivaldi", 1721);
        AddToClassical("La costanza trionfante degl'amori e de gl'odii", "Antonio Vivaldi", 1716);
    }
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

#define INITIAL_NUMBER_OF_ITEMS 10000

MyListModel::MyListModel(int modelFlags) :
        wxDataViewVirtualListModel( INITIAL_NUMBER_OF_ITEMS )
{
    const wxString multiLineText = L"top (\u1ED6)\ncentre\nbottom (g)";
    const bool useMultiLine = (modelFlags & MODEL_USE_MULTI_LINE_TEXT) != 0;

    // the first 100 items are really stored in this model;
    // all the others are synthesized on request
    static const unsigned NUMBER_REAL_ITEMS = 100;

    m_toggleColValues.reserve(NUMBER_REAL_ITEMS);
    m_textColValues.reserve(NUMBER_REAL_ITEMS);
    m_toggleColValues.push_back(false);
    m_textColValues.push_back(useMultiLine
        ? multiLineText
        : wxString("first row with long label to test ellipsization"));
    for (unsigned int i = 1; i < NUMBER_REAL_ITEMS; i++)
    {
        m_toggleColValues.push_back(false);
        m_textColValues.push_back(wxString::Format("real row %d", i));
    }

    m_iconColValues.assign(NUMBER_REAL_ITEMS,
        useMultiLine ? multiLineText : wxString("test"));

    const wxSize size = GetIconSizeFromModelFlags(modelFlags);

    m_icon[0] = wxArtProvider::GetBitmapBundle(wxART_QUESTION, wxART_LIST, size);
    m_icon[1] = wxArtProvider::GetBitmapBundle(wxART_WX_LOGO, wxART_LIST, size);
}

void MyListModel::Prepend( const wxString &text )
{
    m_toggleColValues.insert( m_toggleColValues.begin(), 0 );
    m_textColValues.Insert( text, 0 );
    RowPrepended();
}

void MyListModel::DeleteItem( const wxDataViewItem &item )
{
    unsigned int row = GetRow( item );

    if (row >= m_toggleColValues.size())
        return;

    m_toggleColValues.erase( m_toggleColValues.begin()+row );

    if (row >= m_textColValues.GetCount())
        return;

    m_textColValues.RemoveAt( row );
    RowDeleted( row );
}

void MyListModel::DeleteItems( const wxDataViewItemArray &items )
{
    unsigned i;
    wxArrayInt rows;
    for (i = 0; i < items.GetCount(); i++)
    {
        unsigned int row = GetRow( items[i] );
        if (row < m_textColValues.GetCount())
        {
            wxASSERT(row < m_toggleColValues.size());
            rows.Add( row );
        }
    }

    if (rows.GetCount() == 0)
    {
        // none of the selected items were in the range of the items
        // which we store... for simplicity, don't allow removing them
        wxLogError( "Cannot remove rows with an index greater than %u", unsigned(m_textColValues.GetCount()) );
        return;
    }

    // Sort in descending order so that the last
    // row will be deleted first. Otherwise the
    // remaining indices would all be wrong.
    rows.Sort( my_sort_reverse );
    for (i = 0; i < rows.GetCount(); i++)
    {
        m_toggleColValues.erase( m_toggleColValues.begin()+rows[i] );
        m_textColValues.RemoveAt( rows[i] );
    }

    // This is just to test if wxDataViewCtrl can
    // cope with removing rows not sorted in
    // descending order
    rows.Sort( my_sort );
    RowsDeleted( rows );
}

void MyListModel::AddMany()
{
    Reset( GetCount()+1000 );
}

void MyListModel::GetValueByRow( wxVariant &variant,
                                 unsigned int row, unsigned int col ) const
{
    switch ( col )
    {
        case Col_EditableText:
            if (row >= m_textColValues.GetCount())
                variant = wxString::Format( "virtual row %d", row );
            else
                variant = m_textColValues[ row ];
            break;

        case Col_ToggleIconText:
            {
                wxString text;
                wxCheckBoxState state;
                if ( row >= m_iconColValues.GetCount() )
                {
                    text = "virtual icon";
                    state = wxCHK_UNDETERMINED;
                }
                else
                {
                    text = m_iconColValues[row];
                    state = m_toggleColValues[row] ? wxCHK_CHECKED : wxCHK_UNCHECKED;
                }

                variant << wxDataViewCheckIconText(text, m_icon[row % 2], state);
            }
            break;

        case Col_Date:
            variant = wxDateTime(1, wxDateTime::Jan, 2000).Add(wxTimeSpan(row));
            break;

        case Col_TextWithAttr:
            {
                static const char *labels[5] =
                {
                    // These strings will look wrong without wxUSE_MARKUP, but
                    // it's just a sample, so we don't care.
                    "<span color=\"#87ceeb\">light</span> and "
                        "<span color=\"#000080\">dark</span> blue",
                    "<big>growing green</big>",
                    "<i>emphatic &amp; red</i>",
                    "<b>bold &amp;&amp; cyan</b>",
                    "<small><tt>dull default</tt></small>",
                };

                variant = labels[row % 5];
            }
            break;

        case Col_Custom:
            {
                const auto it = m_customColValues.find(row);
                if ( it != m_customColValues.end() )
                    variant = it->second;
                else
                    variant = wxString::Format("%d", row % 100);
            }
            break;
    }
}

bool MyListModel::GetAttrByRow( unsigned int row, unsigned int col,
                                wxDataViewItemAttr &attr ) const
{
    switch ( col )
    {
        case Col_EditableText:
        case Col_Date:
            if (row < m_toggleColValues.size())
            {
                if (m_toggleColValues[row])
                {
                    attr.SetColour( wxColour( *wxLIGHT_GREY ) );
                    attr.SetStrikethrough( true );
                    return true;
                }
            }
            return false;

        case Col_ToggleIconText:
            if ( !(row % 2) )
                return false;
            attr.SetColour(*wxYELLOW);
            attr.SetBackgroundColour(*wxLIGHT_GREY);
            break;

        case Col_TextWithAttr:
            if (row < m_toggleColValues.size())
            {
                if (m_toggleColValues[row])
                {
                    attr.SetColour( wxColour( *wxLIGHT_GREY ) );
                    attr.SetStrikethrough( true );
                    return true;
                }
            }
            wxFALLTHROUGH;

        case Col_Custom:
            // do what the labels defined in GetValueByRow() hint at
            switch ( row % 5 )
            {
                case 0:
                    attr.SetColour(*wxBLUE);
                    break;

                case 1:
                    attr.SetColour(*wxGREEN);
                    break;

                case 2:
                    attr.SetColour(*wxRED);
                    break;

                case 3:
                    attr.SetColour(*wxCYAN);
                    attr.SetBold(true);
                    break;

                case 4:
                    return false;
            }
            break;
    }

    return true;
}

bool MyListModel::SetValueByRow( const wxVariant &variant,
                                 unsigned int row, unsigned int col )
{
    switch ( col )
    {
        case Col_EditableText:
        case Col_ToggleIconText:
            if (row >= m_textColValues.GetCount())
            {
                // the item is not in the range of the items
                // which we store... for simplicity, don't allow editing it
                wxLogError( "Cannot edit rows with an index greater than %zu",
                            m_textColValues.GetCount() );
                return false;
            }

            if ( col == Col_EditableText )
            {
                m_textColValues[row] = variant.GetString();
            }
            else // col == Col_ToggleIconText
            {
                wxDataViewCheckIconText checkIconText;
                checkIconText << variant;
                m_toggleColValues[row] =
                    checkIconText.GetCheckedState() == wxCHK_CHECKED;
                m_iconColValues[row] = checkIconText.GetText();
            }
            return true;

        case Col_Date:
        case Col_TextWithAttr:
            wxLogError("Cannot edit the column %d", col);
            break;

        case Col_Custom:
            m_customColValues[row] = variant.GetString();
            break;
    }

    return false;
}


// ----------------------------------------------------------------------------
// MyListStoreDerivedModel
// ----------------------------------------------------------------------------

bool MyListStoreDerivedModel::IsEnabledByRow(unsigned int row, unsigned int col) const
{
    // disabled the last two checkboxes
    return !(col == 0 && 8 <= row && row <= 9);
}

// ----------------------------------------------------------------------------
// MyListStoreHasValueModel
// ----------------------------------------------------------------------------

bool MyListStoreHasValueModel::HasValue(const wxDataViewItem &item, unsigned int col) const
{
    unsigned int row = GetRow( item );
    // the diagonal entries don't have values. This is just a silly example to demonstrate the
    // usage of overriding HasValue to specify that some columns don't have values for some items
    return row != col;
}
