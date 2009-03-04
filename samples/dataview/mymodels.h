/////////////////////////////////////////////////////////////////////////////
// Name:        mymodels.h
// Purpose:     wxDataViewCtrl wxWidgets sample
// Author:      Robert Roebling
// Modified by: Francesco Montorsi, Bo Yang
// Created:     06/01/06
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


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

class MyMusicTreeModelNode;
WX_DEFINE_ARRAY_PTR( MyMusicTreeModelNode*, MyMusicTreeModelNodes );

class MyMusicTreeModelNode
{
public:
    MyMusicTreeModelNode( MyMusicTreeModelNode* parent,
                      const wxString &title, const wxString &artist, int year )
    {
        m_parent = parent;
        m_title = title;
        m_artist = artist;
        m_year = year;
        m_quality = "good";
        m_isContainer = false;
    }

    MyMusicTreeModelNode( MyMusicTreeModelNode* parent,
                      const wxString &branch )
    {
        m_parent = parent;
        m_title = branch;
        m_year = -1;
        m_isContainer = true;
    }

    ~MyMusicTreeModelNode()
    {
        size_t count = m_children.GetCount();
        size_t i;
        for (i = 0; i < count; i++)
        {
            MyMusicTreeModelNode *child = m_children[i];
            delete child;
        }
    }

    bool IsContainer()                                        { return m_isContainer; }

    MyMusicTreeModelNode* GetParent()                         { return m_parent; }
    MyMusicTreeModelNodes &GetChildren()                      { return m_children; }
    MyMusicTreeModelNode* GetNthChild( unsigned int n )       { return m_children.Item( n ); }
    void Insert( MyMusicTreeModelNode* child, unsigned int n) { m_children.Insert( child, n); }
    void Append( MyMusicTreeModelNode* child )                { m_children.Add( child ); }
    unsigned int GetChildCount()                              { return m_children.GetCount(); }

public:
    wxString            m_title;
    wxString            m_artist;
    int                 m_year;
    wxString            m_quality;

private:
    MyMusicTreeModelNode   *m_parent;
    MyMusicTreeModelNodes   m_children;
    bool                    m_isContainer;
};

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
    int GetYear( const wxDataViewItem &item ) const;

    // helper methods to change the model

    void AddToClassical( const wxString &title, const wxString &artist, int year );
    void Delete( const wxDataViewItem &item );

    // override sorting to always sort branches ascendingly

    int Compare( const wxDataViewItem &item1, const wxDataViewItem &item2,
                 unsigned int column, bool ascending );

    // implementation of base class virtuals to define model

    virtual unsigned int GetColumnCount() const
    {
        return 6;
    }

    virtual wxString GetColumnType( unsigned int col ) const
    {
        if (col == 2)
            return wxT("long");

        return wxT("string");
    }

    virtual void GetValue( wxVariant &variant,
                           const wxDataViewItem &item, unsigned int col ) const;
    virtual bool SetValue( const wxVariant &variant,
                           const wxDataViewItem &item, unsigned int col );

    virtual wxDataViewItem GetParent( const wxDataViewItem &item ) const;
    virtual bool IsContainer( const wxDataViewItem &item ) const;
    virtual unsigned int GetChildren( const wxDataViewItem &parent, 
                                      wxDataViewItemArray &array ) const;

    wxDataViewItem GetNinthItem() const
    {
       return wxDataViewItem( m_ninth );
    }

private:
    MyMusicTreeModelNode*   m_root;
    MyMusicTreeModelNode*   m_pop;
    MyMusicTreeModelNode*   m_classical;
    MyMusicTreeModelNode*   m_ninth;
    bool                    m_classicalMusicIsKnownToControl;
};


// ----------------------------------------------------------------------------
// MyListModel
// ----------------------------------------------------------------------------

class MyListModel: public wxDataViewVirtualListModel
{
public:
    MyListModel();

    // helper methods to change the model

    void Prepend( const wxString &text );
    void DeleteItem( const wxDataViewItem &item );
    void DeleteItems( const wxDataViewItemArray &items );
    void AddMany();


    // implementation of base class virtuals to define model

    virtual unsigned int GetColumnCount() const
    {
        return 3;
    }

    virtual wxString GetColumnType( unsigned int col ) const
    {
        if (col == 1)
            return wxT("wxDataViewIconText");

        return wxT("string");
    }

    virtual unsigned int GetRowCount()
    {
        return m_array.GetCount();
    }

    virtual void GetValueByRow( wxVariant &variant,
                                unsigned int row, unsigned int col ) const;
    virtual bool GetAttrByRow( unsigned int row, unsigned int col, wxDataViewItemAttr &attr );
    virtual bool SetValueByRow( const wxVariant &variant,
                                unsigned int row, unsigned int col );

private:
    wxArrayString    m_array;
    wxIcon           m_icon;
    int              m_virtualItems;
};

