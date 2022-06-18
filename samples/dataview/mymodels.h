/////////////////////////////////////////////////////////////////////////////
// Name:        mymodels.h
// Purpose:     wxDataViewCtrl wxWidgets sample
// Author:      Robert Roebling
// Modified by: Francesco Montorsi, Bo Yang
// Created:     06/01/06
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/hashmap.h"
#include "wx/vector.h"

WX_DECLARE_HASH_MAP(unsigned, wxString, wxIntegerHash, wxIntegerEqual,
                    IntToStringMap);

// ----------------------------------------------------------------------------
// MyMusicTreeModelNode: a node inside MyMusicTreeModel
// ----------------------------------------------------------------------------

class MyMusicTreeModelNode;
WX_DEFINE_ARRAY_PTR( MyMusicTreeModelNode*, MyMusicTreeModelNodePtrArray );

class MyMusicTreeModelNode
{
public:
    MyMusicTreeModelNode( MyMusicTreeModelNode* parent,
                          const wxString &title, const wxString &artist,
                          int year )
    {
        m_parent = parent;

        m_title = title;
        m_artist = artist;
        m_year = year;
        m_quality = "good";

        m_container = false;
    }

    MyMusicTreeModelNode( MyMusicTreeModelNode* parent,
                          const wxString &branch )
    {
        m_parent = parent;

        m_title = branch;
        m_year = -1;

        m_container = true;
    }

    ~MyMusicTreeModelNode()
    {
        // free all our children nodes
        size_t count = m_children.GetCount();
        for (size_t i = 0; i < count; i++)
        {
            MyMusicTreeModelNode *child = m_children[i];
            delete child;
        }
    }

    bool IsContainer() const
        { return m_container; }

    MyMusicTreeModelNode* GetParent()
        { return m_parent; }
    MyMusicTreeModelNodePtrArray& GetChildren()
        { return m_children; }
    MyMusicTreeModelNode* GetNthChild( unsigned int n )
        { return m_children.Item( n ); }
    void Insert( MyMusicTreeModelNode* child, unsigned int n)
        { m_children.Insert( child, n); }
    void Append( MyMusicTreeModelNode* child )
        { m_children.Add( child ); }
    unsigned int GetChildCount() const
        { return m_children.GetCount(); }

public:     // public to avoid getters/setters
    wxString                m_title;
    wxString                m_artist;
    int                     m_year;
    wxString                m_quality;

    // TODO/FIXME:
    // the GTK version of wxDVC (in particular wxDataViewCtrlInternal::ItemAdded)
    // needs to know in advance if a node is or _will be_ a container.
    // Thus implementing:
    //   bool IsContainer() const
    //    { return m_children.GetCount()>0; }
    // doesn't work with wxGTK when MyMusicTreeModel::AddToClassical is called
    // AND the classical node was removed (a new node temporary without children
    // would be added to the control)
    bool m_container;

private:
    MyMusicTreeModelNode          *m_parent;
    MyMusicTreeModelNodePtrArray   m_children;
};


// ----------------------------------------------------------------------------
// MyMusicTreeModel
// ----------------------------------------------------------------------------

/*
Implement this data model
            Title               Artist               Year        Judgement
--------------------------------------------------------------------------
1: My Music:
    2:  Pop music
        3:  You are not alone   Michael Jackson      1995        good
        4:  Take a bow          Madonna              1994        good
    5:  Classical music
        6:  Ninth Symphony      Ludwig v. Beethoven  1824        good
        7:  German Requiem      Johannes Brahms      1868        good
*/

class MyMusicTreeModel: public wxDataViewModel
{
public:
    MyMusicTreeModel();
    ~MyMusicTreeModel()
    {
        delete m_root;
    }

    // helper method for wxLog

    wxString GetTitle( const wxDataViewItem &item ) const;
    wxString GetArtist( const wxDataViewItem &item ) const;
    int GetYear( const wxDataViewItem &item ) const;

    // helper methods to change the model

    void AddToClassical( const wxString &title, const wxString &artist,
                         unsigned int year );
    void Delete( const wxDataViewItem &item );
    void Clear();

    wxDataViewItem GetNinthItem() const
    {
       return wxDataViewItem( m_ninth );
    }

    // override sorting to always sort branches ascendingly

    int Compare( const wxDataViewItem &item1, const wxDataViewItem &item2,
                 unsigned int column, bool ascending ) const wxOVERRIDE;

    // implementation of base class virtuals to define model

    virtual void GetValue( wxVariant &variant,
                           const wxDataViewItem &item, unsigned int col ) const wxOVERRIDE;
    virtual bool SetValue( const wxVariant &variant,
                           const wxDataViewItem &item, unsigned int col ) wxOVERRIDE;

    virtual bool IsEnabled( const wxDataViewItem &item,
                            unsigned int col ) const wxOVERRIDE;

    virtual wxDataViewItem GetParent( const wxDataViewItem &item ) const wxOVERRIDE;
    virtual bool IsContainer( const wxDataViewItem &item ) const wxOVERRIDE;
    virtual unsigned int GetChildren( const wxDataViewItem &parent,
                                      wxDataViewItemArray &array ) const wxOVERRIDE;

private:
    MyMusicTreeModelNode*   m_root;

    // pointers to some "special" nodes of the tree:
    MyMusicTreeModelNode*   m_pop;
    MyMusicTreeModelNode*   m_classical;
    MyMusicTreeModelNode*   m_ninth;

    // ??
    bool                    m_classicalMusicIsKnownToControl;
};


// ----------------------------------------------------------------------------
// MyLongMusicTreeModel
// ----------------------------------------------------------------------------

class MyLongMusicTreeModel : public MyMusicTreeModel
{
public:
    MyLongMusicTreeModel();
};


// ----------------------------------------------------------------------------
// MyListModel
// ----------------------------------------------------------------------------

class MyListModel: public wxDataViewVirtualListModel
{
public:
    enum
    {
        Col_ToggleIconText,
        Col_EditableText,
        Col_Date,
        Col_TextWithAttr,
        Col_Custom
    };

    MyListModel(int modelFlags);

    // helper methods to change the model

    void Prepend( const wxString &text );
    void DeleteItem( const wxDataViewItem &item );
    void DeleteItems( const wxDataViewItemArray &items );
    void AddMany();


    // implementation of base class virtuals to define model

    virtual void GetValueByRow( wxVariant &variant,
                                unsigned int row, unsigned int col ) const wxOVERRIDE;
    virtual bool GetAttrByRow( unsigned int row, unsigned int col,
                               wxDataViewItemAttr &attr ) const wxOVERRIDE;
    virtual bool SetValueByRow( const wxVariant &variant,
                                unsigned int row, unsigned int col ) wxOVERRIDE;

private:
    wxVector<bool>   m_toggleColValues;
    wxArrayString    m_textColValues;
    wxArrayString    m_iconColValues;
    IntToStringMap   m_customColValues;
    wxBitmapBundle   m_icon[2];
};

// ----------------------------------------------------------------------------
// MyListStoreDerivedModel
// ----------------------------------------------------------------------------

class MyListStoreDerivedModel : public wxDataViewListStore
{
public:
    virtual bool IsEnabledByRow(unsigned int row, unsigned int col) const wxOVERRIDE;
};

// ----------------------------------------------------------------------------
// MyListStoreHasValueModel
// ----------------------------------------------------------------------------

class MyListStoreHasValueModel : public MyListStoreDerivedModel
{
public:
    virtual bool HasValue(const wxDataViewItem &item, unsigned int col) const wxOVERRIDE;
};

// ----------------------------------------------------------------------------
// MyIndexListModel
// ----------------------------------------------------------------------------

class MyIndexListModel : public wxDataViewIndexListModel
{
public:
    MyIndexListModel() { }

    void Fill(const wxArrayString& strings)
    {
        m_strings = strings;

        Reset(m_strings.size());
    }

    // Implement base class pure virtual methods.
    unsigned GetCount() const wxOVERRIDE { return m_strings.size(); }
    void GetValueByRow(wxVariant& val, unsigned row, unsigned) const wxOVERRIDE
    {
        val = m_strings[row];
    }
    bool SetValueByRow(const wxVariant&, unsigned, unsigned) wxOVERRIDE
    {
        return false;
    }

private:
    wxArrayString m_strings;

    wxDECLARE_NO_COPY_CLASS(MyIndexListModel);
};

enum ModelFlags
{
    MODEL_USE_TALL_ROWS = 1 << 0,
    MODEL_KEEP_LOGO_SMALL = 1 << 1,
    MODEL_USE_MULTI_LINE_TEXT = 1 << 2
};

inline wxSize GetIconSizeFromModelFlags(int modelFlags)
{
    wxSize size(16, 16);
    if ( (modelFlags & MODEL_USE_TALL_ROWS) && !(modelFlags & MODEL_KEEP_LOGO_SMALL) )
        size *= 2;

    return size;
}
