/////////////////////////////////////////////////////////////////////////////
// Name:        aui/serializer.h
// Purpose:     Documentation of wxAuiSerializer and wxAuiDeserializer.
// Author:      Vadim Zeitlin
// Created:     2024-02-20
// Copyright:   (c) 2024 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

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
    virtual void SavePane(const wxAuiPaneInfo& pane) = 0;

    /**
        Called after the last call to SavePane().

        Does nothing by default.
     */
    virtual void AfterSavePanes();

    /**
        Called before starting to save information about the docks.

        Does nothing by default.
     */
    virtual void BeforeSaveDocks();

    /**
        Save information about the given dock.

        This function will be called for all docks and must be implemented to
        save their data in a format from which it can be restored later using a
        matching wxAuiDeserializer implementation.

        As with SavePane(), the coordinates in @a dock are always in DIPs and
        this function does _not_ need to perform any scaling itself.
     */
    virtual void SaveDock(const wxAuiDockInfo& dock) = 0;

    /**
        Called after the last call to SaveDock().

        Does nothing by default.
     */
    virtual void AfterSaveDocks();

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
    virtual std::vector<wxAuiPaneInfo> LoadPanes() = 0;

    /**
        Create the window to be managed by the given pane if necessary.

        This function is called if any of the panes returned by LoadPanes()
        doesn't exist in the existing layout and allows to create windows on
        the fly.

        If this function returns @NULL, as it does by default, the pane is not
        added to the manager.
     */
    virtual wxWindow* CreatePaneWindow(const wxAuiPaneInfo& pane);

    /**
        Load information about all the docks previously saved with SaveDock().

        As with LoadPanes(), this function doesn't need to perform any scaling
        itself.
     */
    virtual std::vector<wxAuiDockInfo> LoadDocks() = 0;

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
