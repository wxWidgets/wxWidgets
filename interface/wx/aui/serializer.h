/////////////////////////////////////////////////////////////////////////////
// Name:        aui/serializer.h
// Purpose:     Documentation of wxAuiSerializer and wxAuiDeserializer.
// Author:      Vadim Zeitlin
// Created:     2024-02-20
// Copyright:   (c) 2024 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    Description of a docked element layout.

    The fields in this struct are shared by wxAuiPaneLayoutInfo and
    wxAuiTabLayoutInfo and contain information about the layout of a docked
    pane or tab layout.

    Note that when saving the objects of this type, fields that have the value
    of 0 can be omitted, as this is their default value in any case (except for
    `dock_direction` which is never 0), to make the serialized representation
    more compact.

    @since 3.3.0
*/
struct wxAuiDockLayoutInfo
{
    /// Direction of the dock containing the pane.
    int dock_direction   = wxAUI_DOCK_LEFT;

    /// Layer of the dock containing the pane.
    int dock_layer       = 0;

    /// Row of the dock containing the pane.
    int dock_row         = 0;

    /// Position of the pane in the dock containing it.
    int dock_pos         = 0;

    /// Relative proportion of the dock allocated to this pane.
    int dock_proportion  = 0;

    /// Size of the containing dock.
    int dock_size        = 0;
};

/**
    Contains information about the layout of a tab control in a wxAuiNotebook.

    This includes where it is docked, via the fields inherited from
    wxAuiDockLayoutInfo, and, optionally, the order of pages in it if it was
    changed as well as pinned pages indices, if any.

    @since 3.3.0
*/
struct wxAuiTabLayoutInfo : wxAuiDockLayoutInfo
{
    /**
        Indices of the pages in this tab control in their order on screen.

        If this vector is empty, it means that the tab control contains all
        notebook pages in natural order.
    */
    std::vector<int> pages;

    /**
        Indices of the pinned pages in this tab control.

        This vector can be empty if there are no pinned pages in this tab
        control. Otherwise it should be a subset of the `pages` vector if it is
        not empty.
     */
    std::vector<int> pinned;

    /**
        Index of the currently selected page in this tab control.

        Note that the value of this field is a page index in the notebook, not
        the position of the active tab in this tab control, and should be one
        of the elements of the `pages` vector if it is not empty.
     */
    int active = 0;
};

/**
    Description of user-modifiable pane layout information.

    This struct is used with wxAuiSerializer and wxAuiDeserializer to store the
    pane layout. Its fields, including the inherited ones from
    wxAuiDockLayoutInfo, have the same meaning as the corresponding fields in
    wxAuiPaneInfo (with the exception of `is_maximized` and `is_hidden`, which
    rather correspond to the individual bits of its state field), but it
    doesn't contain the fields that it wouldn't make sense to serialize.

    @since 3.3.0
 */
struct wxAuiPaneLayoutInfo : wxAuiDockLayoutInfo
{
    /**
        Ctor sets the name, which is always required.
     */
    explicit wxAuiPaneLayoutInfo(wxString name);

    /// Unique name of the pane.
    wxString name;

    /// Position of the pane when floating, may be invalid.
    wxPoint floating_pos = wxDefaultPosition;

    /// Size of the pane when floating, may be invalid.
    wxSize floating_size = wxDefaultSize;


    /// True if the pane is currently maximized.
    bool is_maximized   = false;

    /// True if the pane is currently hidden.
    bool is_hidden      = false;
};

/**
    @class wxAuiBookSerializer

    wxAuiBookSerializer is used for serializing wxAuiNotebook layout.

    This includes the tab controls layout and the order of pages in them.

    It can be used standalone with wxAuiNotebook::SaveLayout() or as base class
    of wxAuiSerializer for saving and restoring the entire layout.

    @library{wxaui}
    @category{aui}

    @since 3.3.0
 */
class wxAuiBookSerializer
{
public:
    /// Trivial default ctor.
    wxAuiBookSerializer() = default;

    /// Trivial but virtual destructor.
    virtual ~wxAuiBookSerializer() = default;

    /**
        Called before starting to save information about the tabs in the
        notebook in the AUI pane with the given name.

        This function needs to be overridden to keep record of the notebook for
        which SaveNotebookTabControl() will be called next.

        If this class is used as a base class of wxAuiSerializer, saving
        notebook layout may be unnecessary, e.g. because the program doesn't
        use wxAuiNotebook at all, and the implementation can be trivial and
        just do nothing because it is not going to be called at all if there
        are no notebooks in the full layout.

        When using wxAuiNotebook::SaveLayout() directly, this function is
        always called and is the first function of this class to be called.
     */
    virtual void BeforeSaveNotebook(const wxString& name) = 0;

    /**
        Called to save information about a single tab control in the given
        notebook.

        This function will be called for all tab controls in the notebook after
        BeforeSaveNotebook().

        As with that function, it has to be implemented, but can simply do
        nothing if saving notebook layout is not necessary.
     */
    virtual void SaveNotebookTabControl(const wxAuiTabLayoutInfo& tab) = 0;

    /**
        Called after saving information about all the pages of the notebook in
        the AUI pane with the given name.

        Does nothing by default.
     */
    virtual void AfterSaveNotebook();

};

/**
    @class wxAuiSerializer

    wxAuiSerializer is used by wxAuiManager::SaveLayout() to store layout
    information.

    This is an abstract base class, you need to inherit from it and override its
    pure virtual functions, including those inherited from its base
    wxAuiBookSerializer class, in your derived class.

    In particular, SavePane() must be overridden and will be called by
    wxAuiManager for each pane and dock present in the layout. Most of the
    other functions don't need to be overridden, but it is often convenient to
    perform some actions before or after starting to save the objects of the
    given type or at the beginning or end of the whole saving process, so this
    class provides hooks for doing it.

    If any of the functions of the derived class throw an exception, it is
    propagated out of wxAuiManager::SaveLayout() and it's callers responsibility
    to handle it.

    @library{wxaui}
    @category{aui}

    @since 3.3.0
 */
class wxAuiSerializer : public wxAuiBookSerializer
{
public:
    /// Trivial default ctor.
    wxAuiSerializer() = default;

    /**
        Called before doing anything else.

        Does nothing by default.
     */
    virtual void BeforeSave();

    /**
        Called before starting to save information about the panes.

        Does nothing by default.
     */
    virtual void BeforeSavePanes();

    /**
        Save information about the given pane.

        This function will be called for all panes and must be implemented to
        save their data in a format from which it can be restored later using a
        matching wxAuiDeserializer implementation.

        Note that all sizes and positions in @a pane are using DIPs, i.e.
        resolution-independent pixels, when it is passed to this function, so
        it does _not_ need to perform any scaling itself to ensure that the
        stored values are restored correctly if the resolution changes.
     */
    virtual void SavePane(const wxAuiPaneLayoutInfo& pane) = 0;

    /**
        Called after the last call to SavePane().

        Does nothing by default.
     */
    virtual void AfterSavePanes();

    /**
        Called before starting to save information about the notebooks.

        Does nothing by default.

        Note that this function is called after AfterSavePanes() but may not be
        called at all if there are no panes containing wxAuiNotebook.

        wxAuiBookSerializer member functions will be called after this function
        if it is called at all.
     */
    virtual void BeforeSaveNotebooks();

    /**
        Called after the last call to SaveNotebook().

        Does nothing by default.

        This function is called after all wxAuiBookSerializer member functions
     */
    virtual void AfterSaveNotebooks();

    /**
        Called after saving everything.

        Does nothing by default.
     */
    virtual void AfterSave();
};

/**
    wxAuiBookDeserializer is used for deserializing wxAuiNotebook layout.

    Similarly to wxAuiBookSerializer, it can be used standalone with
    wxAuiNotebook::LoadLayout() or as base class of wxAuiDeserializer.

    @library{wxaui}
    @category{aui}

    @since 3.3.0
 */
class wxAuiBookDeserializer
{
public:
    /// Trivial default ctor.
    wxAuiBookDeserializer() = default;

    /// Trivial but virtual destructor.
    virtual ~wxAuiBookDeserializer() = default;

    /**
        Load information about the notebook tabs previously saved by
        wxAuiBookSerializer::SaveNotebookTabControl().

        When using this class as a base class of wxAuiDeserializer, this
        function is called by wxAuiManager::LoadLayout() after loading the pane
        with the name @a name if it is a wxAuiNotebook. Otherwise, i.e. when
        using wxAuiNotebook::LoadLayout() directly, this function is called
        with the same @a name as was passed to that function.

        If restoring the notebook layout is not necessary, this function can
        just return an empty vector which is interpreted as meaning that the
        default notebook layout should be used.
     */
    virtual std::vector<wxAuiTabLayoutInfo>
    LoadNotebookTabs(const wxString& name) = 0;

    /**
        Determine what should be done with the pages not attached to any tab
        control after restoring the pages order.

        It is possible that the data returned by LoadNotebookTabs() doesn't
        contain the layout information for all the currently existing pages,
        e.g. because data saved by an earlier program version is being loaded
        into a newer version in which new pages were added. In this case, this
        function is called for each @a page that wasn't assigned to any tab
        after restoring the pages order and can be overridden to determine what
        should be done with it.

        The default implementation of this function just returns @true without
        modifying the output arguments, which results in the page being
        appended to the main tab control. The overridden version may return
        @true but modify @a tabCtrl and @a tabIndex arguments to change where
        the page should be inserted, e.g. by setting @a tabIndex to 0 to insert
        the new pages at the beginning instead of appending them.

        Finally, the overridden function may return @false to indicate that the
        page should be removed from the notebook.

        @note The @a book parameter can be used to retrieve the total number of
            pages or to call functions such as wxAuiNotebook::GetMainTabCtrl()
            or wxAuiNotebook::GetAllTabCtrls() on it, but this function must
            not attempt to modify it by adding or removing pages to/from it.
     */
    virtual bool
    HandleOrphanedPage(wxAuiNotebook& book,
                       int page,
                       wxAuiTabCtrl** tabCtrl,
                       int* tabIndex);
};

/**
    @class wxAuiDeserializer

    wxAuiDeserializer is used by wxAuiManager::LoadLayout() to restore layout
    information saved by wxAuiManager::SaveLayout().

    As wxAuiSerializer, this is an abstract base class, you need to inherit from
    it and override its pure virtual functions in your derived class.

    Derived class function also may throw and, if any of them other than
    AfterLoad() does, the existing layout is not changed, i.e.
    wxAuiManager::LoadLayout() is exception-safe.

    @library{wxaui}
    @category{aui}

    @since 3.3.0
 */
class wxAuiDeserializer : public wxAuiBookDeserializer
{
public:
    /**
        Constructor taking the manager for which we're restoring the layout.

        The manager remains valid for the lifetime of this object.
     */
    explicit wxAuiDeserializer(wxAuiManager& manager);

    /// Trivial but virtual destructor.
    virtual ~wxAuiDeserializer() = default;


    /**
        Called before doing anything else.

        Does nothing by default.
     */
    virtual void BeforeLoad();

    /**
        Load information about all the panes previously saved by
        wxAuiSerializer::SavePane().

        Unlike the serializer function, this one is called only once and should
        return all the panes in the layout.

        Just as the serializer function, this one doesn't need to perform any
        scaling itself as this will be done, if necessary, by wxAuiManager
        itself.

        If some pane in the returned vector doesn't already exist, i.e. there
        is no pane with the matching name, CreatePaneWindow() is called to
        allow creating it on the fly.
     */
    virtual std::vector<wxAuiPaneLayoutInfo> LoadPanes() = 0;

    /**
        Create the window to be managed by the given pane if necessary.

        This function is called if any of the panes returned by LoadPanes()
        doesn't exist in the existing layout and allows to create windows on
        the fly.

        If this function returns @NULL, as it does by default, the pane is not
        added to the manager.

        If the function does create a new window, it should typically modify @a
        pane parameter to fill in the fields such as `caption` or `icon` that
        wouldn't normally be serialized and so wouldn't be restored by
        LoadPanes().
     */
    virtual wxWindow* CreatePaneWindow(wxAuiPaneInfo& pane);

    /**
        Called after restoring everything.

        Default implementation calls wxAuiManager::Update(). Override this
        function and do _not_ call the base class version if you want to
        prevent this from happening, e.g. if you need to make further changes
        to the restored layout before updating it.
     */
    virtual void AfterLoad();

protected:
    /// The manager for which we're restoring the layout.
    wxAuiManager& m_manager;
};
