/////////////////////////////////////////////////////////////////////////////
// Name:        propgridpagestate.h
// Purpose:     interface of wxPGProperty
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


/**
    @section propgrid_hittestresult wxPropertyGridHitTestResult

    A return value from wxPropertyGrid::HitTest(),
    contains all you need to know about an arbitrary location on the grid.
*/
class wxPropertyGridHitTestResult
{
    friend class wxPropertyGridPageState;
public:
    wxPropertyGridHitTestResult();

    ~wxPropertyGridHitTestResult();

    /**
        Returns column hit. -1 for margin.
    */
    int GetColumn() const;

    /**
        Returns property hit. @NULL if empty space below
        properties was hit instead.
    */
    wxPGProperty* GetProperty() const;

    /**
        Returns index of splitter hit, -1 for none.
    */
    int GetSplitter() const;

    /**
        If splitter hit, then this member function
        returns offset to the exact splitter position.
    */
    int GetSplitterHitOffset() const;
};

// -----------------------------------------------------------------------

#define wxPG_IT_CHILDREN(A)         ((A)<<16)

/** @section propgrid_iterator_flags wxPropertyGridIterator Flags
    @{

    NOTES: At lower 16-bits, there are flags to check if item will be included. At higher
      16-bits, there are same flags, but to instead check if children will be included.
*/

enum wxPG_ITERATOR_FLAGS
{
/**
    Iterate through 'normal' property items (does not include children of
    aggregate or hidden items by default).
    @hideinitializer
*/
wxPG_ITERATE_PROPERTIES = wxPG_PROP_PROPERTY |
                          wxPG_PROP_MISC_PARENT |
                          wxPG_PROP_AGGREGATE |
                          wxPG_PROP_COLLAPSED |
                          wxPG_IT_CHILDREN(wxPG_PROP_MISC_PARENT) |
                          wxPG_IT_CHILDREN(wxPG_PROP_CATEGORY),

/**
    Iterate children of collapsed parents, and individual items that are hidden.
    @hideinitializer
*/
wxPG_ITERATE_HIDDEN = wxPG_PROP_HIDDEN |
                      wxPG_IT_CHILDREN(wxPG_PROP_COLLAPSED),

/**
    Iterate children of parent that is an aggregate property (ie has fixed
    children).
    @hideinitializer
*/
wxPG_ITERATE_FIXED_CHILDREN = wxPG_IT_CHILDREN(wxPG_PROP_AGGREGATE) |
                              wxPG_ITERATE_PROPERTIES,

/** Iterate categories.
    Note that even without this flag, children of categories are still iterated
    through.
    @hideinitializer
*/
wxPG_ITERATE_CATEGORIES = wxPG_PROP_CATEGORY |
                          wxPG_IT_CHILDREN(wxPG_PROP_CATEGORY) |
                          wxPG_PROP_COLLAPSED,

/**
    @hideinitializer
*/
wxPG_ITERATE_ALL_PARENTS = wxPG_PROP_MISC_PARENT |
                           wxPG_PROP_AGGREGATE |
                           wxPG_PROP_CATEGORY,

/**
    @hideinitializer
*/
wxPG_ITERATE_ALL_PARENTS_RECURSIVELY = wxPG_ITERATE_ALL_PARENTS |
                                       wxPG_IT_CHILDREN(
                                                wxPG_ITERATE_ALL_PARENTS),

/**
    @hideinitializer
*/
wxPG_ITERATOR_FLAGS_ALL = wxPG_PROP_PROPERTY |
                          wxPG_PROP_MISC_PARENT |
                          wxPG_PROP_AGGREGATE |
                          wxPG_PROP_HIDDEN |
                          wxPG_PROP_CATEGORY |
                          wxPG_PROP_COLLAPSED,

/**
    @hideinitializer
*/
wxPG_ITERATOR_MASK_OP_ITEM = wxPG_ITERATOR_FLAGS_ALL,

// (wxPG_PROP_MISC_PARENT|wxPG_PROP_AGGREGATE|wxPG_PROP_CATEGORY)
/**
    @hideinitializer
*/
wxPG_ITERATOR_MASK_OP_PARENT = wxPG_ITERATOR_FLAGS_ALL,

/**
    Combines all flags needed to iterate through visible properties
    (i.e. hidden properties and children of collapsed parents are skipped).
    @hideinitializer
*/
wxPG_ITERATE_VISIBLE = wxPG_ITERATE_PROPERTIES |
                       wxPG_PROP_CATEGORY |
                       wxPG_IT_CHILDREN(wxPG_PROP_AGGREGATE),

/**
    Iterate all items.
    @hideinitializer
*/
wxPG_ITERATE_ALL = wxPG_ITERATE_VISIBLE |
                   wxPG_ITERATE_HIDDEN,

/**
    Iterate through individual properties (ie categories and children of
    aggregate properties are skipped).
    @hideinitializer
*/
wxPG_ITERATE_NORMAL = wxPG_ITERATE_PROPERTIES |
                      wxPG_ITERATE_HIDDEN,

/**
    Default iterator flags.
    @hideinitializer
*/
wxPG_ITERATE_DEFAULT = wxPG_ITERATE_NORMAL

};

/** @}
*/


/**
    @section propgrid_iterator_class wxPropertyGridIterator

    Preferable way to iterate through contents of wxPropertyGrid,
    wxPropertyGridManager, and wxPropertyGridPage.

    See wxPropertyGridInterface::GetIterator() for more information about usage.

    @library{wxpropgrid}
    @category{propgrid}

    @{
*/
/**
    Base for wxPropertyGridIterator classes.
*/
class wxPropertyGridIteratorBase
{
protected:
    wxPropertyGridIteratorBase();

public:
    void Assign( const wxPropertyGridIteratorBase& it );

    bool AtEnd() const;

    /**
        Get current property.
    */
    wxPGProperty* GetProperty() const;

    void Init( wxPropertyGridPageState* state,
               int flags,
               wxPGProperty* property,
               int dir = 1 );

    void Init( wxPropertyGridPageState* state,
               int flags,
               int startPos = wxTOP,
               int dir = 0 );

    /**
        Iterate to the next property.
    */
    void Next( bool iterateChildren = true );

    /**
        Iterate to the previous property.
    */
    void Prev();

    /**
        Set base parent, i.e. a property when, in which iteration returns,
        it ends.

        Default base parent is the root of the used wxPropertyGridPageState.
    */
    void SetBaseParent( wxPGProperty* baseParent );
};

class wxPropertyGridIterator : public wxPropertyGridIteratorBase
{
public:
    wxPropertyGridIterator();
    wxPropertyGridIterator( wxPropertyGridPageState* state,
                            int flags = wxPG_ITERATE_DEFAULT,
                            wxPGProperty* property = nullptr, int dir = 1 );
    wxPropertyGridIterator( wxPropertyGridPageState* state,
                            int flags, int startPos, int dir = 0 );
    wxPropertyGridIterator( const wxPropertyGridIterator& it );
    ~wxPropertyGridIterator();
};

/**
    Const version of wxPropertyGridIterator.
*/
class wxPropertyGridConstIterator : public wxPropertyGridIteratorBase
{
public:
    /**
        Additional copy constructor.
    */
    wxPropertyGridConstIterator( const wxPropertyGridIterator& other );

    /**
        Additional assignment operator.
    */
    const wxPropertyGridConstIterator& operator=( const wxPropertyGridIterator& it );

    wxPropertyGridConstIterator();
    wxPropertyGridConstIterator( const wxPropertyGridPageState* state,
                                 int flags = wxPG_ITERATE_DEFAULT,
                                 const wxPGProperty* property = nullptr, int dir = 1 );
    wxPropertyGridConstIterator( wxPropertyGridPageState* state,
                                 int flags, int startPos, int dir = 0 );
    wxPropertyGridConstIterator( const wxPropertyGridConstIterator& it );
    ~wxPropertyGridConstIterator();
};

/** @}
*/

// -----------------------------------------------------------------------

/**
    @section propgrid_viterator_class wxPGVIterator

    Abstract implementation of a simple iterator. Can only be used
    to iterate in forward order, and only through the entire container.
    Used to have functions dealing with all properties work with both
    wxPropertyGrid and wxPropertyGridManager.
*/
class wxPGVIterator
{
public:
    wxPGVIterator();
    wxPGVIterator( wxPGVIteratorBase* obj );
    ~wxPGVIterator();
    void UnRef();
    wxPGVIterator( const wxPGVIterator& it );
    const wxPGVIterator& operator=( const wxPGVIterator& it );
    void Next();
    bool AtEnd() const;
    wxPGProperty* GetProperty() const;
};

// -----------------------------------------------------------------------

/** @class wxPropertyGridPageState

    Contains low-level property page information (properties, column widths,
    etc.) of a single wxPropertyGrid or single wxPropertyGridPage. Generally you
    should not use this class directly, but instead member functions in
    wxPropertyGridInterface, wxPropertyGrid, wxPropertyGridPage, and
    wxPropertyGridManager.

    @remarks
    Currently this class is not implemented in wxPython.

    @library{wxpropgrid}
    @category{propgrid}
*/
class wxPropertyGridPageState
{
    friend class wxPropertyGrid;
    friend class wxPropertyGridInterface;
    friend class wxPropertyGridPage;
    friend class wxPropertyGridManager;
    friend class wxPGProperty;
    friend class wxFlagsProperty;
    friend class wxPropertyGridIteratorBase;
public:

    /**
        Default constructor.
    */
    wxPropertyGridPageState();

    /**
        Destructor.
    */
    virtual ~wxPropertyGridPageState();

    /**
        Makes sure all columns have minimum width.
    */
    void CheckColumnWidths( int widthChange = 0 );

    /**
        Override this member function to add custom behaviour on property
        deletion.
    */
    virtual void DoDelete( wxPGProperty* item, bool doDelete = true );

    /**
        Override this member function to add custom behaviour on property
        insertion.
    */
    virtual wxPGProperty* DoInsert( wxPGProperty* parent,
                                    int index,
                                    wxPGProperty* property );

    /**
        This needs to be overridden in grid used the manager so that splitter
        changes can be propagated to other pages.
    */
    virtual void DoSetSplitterPosition( int pos,
                                        int splitterColumn = 0,
                                        int flags = 0 );

    bool EnableCategories( bool enable );

    /**
        Make sure virtual height is up-to-date.
    */
    void EnsureVirtualHeight();

    /**
        Returns (precalculated) height of contained visible properties.
    */
    unsigned int GetVirtualHeight() const;

    /**
        Returns (precalculated) height of contained visible properties.
    */
    unsigned int GetVirtualHeight();

    /**
        Returns actual height of contained visible properties.
        @remarks
        Mostly used for internal diagnostic purposes.
    */
    inline unsigned int GetActualVirtualHeight() const;

    unsigned int GetColumnCount() const;

    int GetColumnMinWidth( int column ) const;

    int GetColumnWidth( unsigned int column ) const;

    wxPropertyGrid* GetGrid() const;

    /**
        Returns last item which could be iterated using given flags.
        @param flags
            @ref propgrid_iterator_flags
    */
    wxPGProperty* GetLastItem( int flags = wxPG_ITERATE_DEFAULT );

    const wxPGProperty* GetLastItem( int flags = wxPG_ITERATE_DEFAULT ) const;

    /**
        Returns currently selected property.
    */
    wxPGProperty* GetSelection() const;

    wxPropertyCategory* GetPropertyCategory( const wxPGProperty* p ) const;

    /**
        Returns combined width of margin and all the columns.
    */
    int GetVirtualWidth() const;

    int GetColumnFullWidth(wxPGProperty* p, unsigned int col) const;

    /**
        Returns information about arbitrary position in the grid.

        @param pt
            Logical coordinates in the virtual grid space. Use
            wxScrolled<T>::CalcUnscrolledPosition() if you need to
            translate a scrolled position into a logical one.
    */
    wxPropertyGridHitTestResult HitTest( const wxPoint& pt ) const;

    /**
        Returns true if page is visibly displayed.
    */
    inline bool IsDisplayed() const;

    bool IsInNonCatMode() const;

    /**
        Called after virtual height needs to be recalculated.
    */
    void VirtualHeightChanged();
};
