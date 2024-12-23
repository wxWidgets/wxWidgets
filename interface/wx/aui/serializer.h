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
    wxAuiDockLayoutInfo, and the order of pages in it.

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
};

/**
    Description of user-modifiable pane layout information.

    This struct is used with wxAuiSerializer and wxAuiDeserializer to store the
    pane layout. Its fields, including the inherited ones from
    wxAuiDockLayoutInfo, have the same meaning as the corresponding fields in
    wxAuiPaneInfo (with the exception of `is_maximized`), but it doesn't
    contain the fields that it wouldn't make sense to serialize.

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
};

/**
    @class wxAuiSerializer

    wxAuiSerializer is used by wxAuiManager::SaveLayout() to store layout
    information.

    This is an abstract base class, you need to inherit from it and override its
    pure virtual functions in your derived class.

    SavePane() and SaveDock() must be overridden and will be called by
    wxAuiManager for each pane and dock present in the layout. The other
    functions don't need to be overridden, but it is often convenient to
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
class wxAuiSerializer
{
public:
    /// Trivial default ctor.
    wxAuiSerializer() = default;

    /// Trivial but virtual destructor.
    virtual ~wxAuiSerializer() = default;

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
     */
    virtual void BeforeSaveNotebooks();

    /**
        Called before starting to save information about the tabs in the
        notebook in the AUI pane with the given name.

        This function needs to be overridden to keep record of the notebook for
        which SaveNotebookTabControl() will be called next. Of course, if
        saving notebook layout is unnecessary, e.g. because the program doesn't
        use wxAuiNotebook at all, the implementation can be trivial and just do
        nothing.

        This function is called one or more times after BeforeSaveNotebooks().
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

    /**
        Called after the last call to SaveNotebook().

        Does nothing by default.
     */
    virtual void AfterSaveNotebooks();

    /**
        Called after saving everything.

        Does nothing by default.
     */
    virtual void AfterSave();
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
class wxAuiDeserializer
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
        Load information about the notebook tabs previously saved by
        wxAuiSerializer::SaveNotebookTabControl().

        The pane with the name @a name is guaranteed to exist in the layout and
        have wxAuiNotebook as the associated window.

        If restoring the notebook layout is not necessary, this function can
        just return an empty vector.
     */
    virtual std::vector<wxAuiTabLayoutInfo>
    LoadNotebookTabs(const wxString& name) = 0;

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
