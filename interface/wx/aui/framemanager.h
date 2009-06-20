/////////////////////////////////////////////////////////////////////////////
// Name:        aui/aui.h
// Purpose:     interface of wxAuiManager
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


/**
    @todo wxAuiPaneInfo dock direction types used with wxAuiManager.
*/
enum wxAuiManagerDock
{
    wxAUI_DOCK_NONE = 0,
    wxAUI_DOCK_TOP = 1,
    wxAUI_DOCK_RIGHT = 2,
    wxAUI_DOCK_BOTTOM = 3,
    wxAUI_DOCK_LEFT = 4,
    wxAUI_DOCK_CENTER = 5,
    wxAUI_DOCK_CENTRE = wxAUI_DOCK_CENTER
};


/**
    @todo wxAuiManager behavior style flags.
*/
enum wxAuiManagerOption
{
    wxAUI_MGR_ALLOW_FLOATING           = 1 << 0,
    wxAUI_MGR_ALLOW_ACTIVE_PANE        = 1 << 1,
    wxAUI_MGR_TRANSPARENT_DRAG         = 1 << 2,
    wxAUI_MGR_TRANSPARENT_HINT         = 1 << 3,
    wxAUI_MGR_VENETIAN_BLINDS_HINT     = 1 << 4,
    wxAUI_MGR_RECTANGLE_HINT           = 1 << 5,
    wxAUI_MGR_HINT_FADE                = 1 << 6,
    wxAUI_MGR_NO_VENETIAN_BLINDS_FADE  = 1 << 7,
    wxAUI_MGR_LIVE_RESIZE              = 1 << 8,

    wxAUI_MGR_DEFAULT = wxAUI_MGR_ALLOW_FLOATING |
                        wxAUI_MGR_TRANSPARENT_HINT |
                        wxAUI_MGR_HINT_FADE |
                        wxAUI_MGR_NO_VENETIAN_BLINDS_FADE
};

/**
    @class wxAuiManager

    wxAuiManager is the central class of the wxAUI class framework.

    wxAuiManager manages the panes associated with it for a particular wxFrame,
    using a pane's wxAuiPaneInfo information to determine each pane's docking
    and floating behavior.

    wxAuiManager uses wxWidgets' sizer mechanism to plan the layout of each
    frame. It uses a replaceable dock art class to do all drawing, so all
    drawing is localized in one area, and may be customized depending on an
    application's specific needs.

    wxAuiManager works as follows: the programmer adds panes to the class,
    or makes changes to existing pane properties (dock position, floating
    state, show state, etc.). To apply these changes, wxAuiManager's
    Update() function is called. This batch processing can be used to avoid
    flicker, by modifying more than one pane at a time, and then "committing"
    all of the changes at once by calling Update().

    Panes can be added quite easily:

    @code
    wxTextCtrl* text1 = new wxTextCtrl(this, -1);
    wxTextCtrl* text2 = new wxTextCtrl(this, -1);
    m_mgr.AddPane(text1, wxLEFT, "Pane Caption");
    m_mgr.AddPane(text2, wxBOTTOM, "Pane Caption");
    m_mgr.Update();
    @endcode

    Later on, the positions can be modified easily. The following will float
    an existing pane in a tool window:

    @code
    m_mgr.GetPane(text1).Float();
    @endcode


    @section auimanager_layers Layers, Rows and Directions, Positions

    Inside wxAUI, the docking layout is figured out by checking several pane
    parameters. Five of these are important for determining where a pane will
    end up:

    @li Direction: Each docked pane has a direction, Top, Bottom, Left, Right,
        or Center. This is fairly self-explanatory. The pane will be placed in
        the location specified by this variable.
    @li Position: More than one pane can be placed inside of a dock. Imagine
        two panes being docked on the left side of a window. One pane can be
        placed over another. In proportionally managed docks, the pane
        position indicates its sequential position, starting with zero. So, in
        our scenario with two panes docked on the left side, the top pane in
        the dock would have position 0, and the second one would occupy
        position 1.
    @li Row: A row can allow for two docks to be placed next to each other.
        One of the most common places for this to happen is in the toolbar.
        Multiple toolbar rows are allowed, the first row being row 0, and the
        second row 1. Rows can also be used on vertically docked panes.
    @li Layer: A layer is akin to an onion. Layer 0 is the very center of the
        managed pane. Thus, if a pane is in layer 0, it will be closest to the
        center window (also sometimes known as the "content window").
        Increasing layers "swallow up" all layers of a lower value. This can
        look very similar to multiple rows, but is different because all panes
        in a lower level yield to panes in higher levels. The best way to
        understand layers is by running the wxAUI sample.
    @li Page: More then one pane can be docked in the exact same Direction,
        Position, Row and Layer. When this happens a tabbed notebook will be formed.
        Tab position will be decided based on the Page values of the panes with
        those that have a lower value appearing on the left, with 0 being the 
        absolute left.


    @beginEventEmissionTable{wxAuiManagerEvent}
    @event{EVT_AUI_PANE_BUTTON(func)}
        Triggered when any button is pressed for any docked panes.
    @event{EVT_AUI_PANE_CLOSE(func)}
        Triggered when a docked or floating pane is closed.
    @event{EVT_AUI_PANE_MAXIMIZE(func)}
        Triggered when a pane is maximized.
    @event{EVT_AUI_PANE_RESTORE(func)}
        Triggered when a pane is restored.
    @event{EVT_AUI_RENDER(func)}
        This event can be caught to override the default renderer in order to
        custom draw your wxAuiManager window (not recommended).
    @endEventTable

    @library{wxbase}
    @category{aui}

    @see @ref overview_aui, wxAuiNotebook, wxAuiDockArt, wxAuiPaneInfo
*/
class wxAuiManager : public wxEvtHandler
{
public:
    /**
        Constructor. @a managed_wnd specifies the wxFrame which should be managed.
        @a flags  specifies options which allow the frame management behavior
        to be modified.
    */
    wxAuiManager(wxWindow* managed_wnd = NULL,
                 unsigned int flags = wxAUI_MGR_DEFAULT);

    /**
        Dtor.
    */
    virtual ~wxAuiManager();

    //@{
    /**
        AddPane() tells the frame manager to start managing a child window.
        There are several versions of this function. The first version allows
        the full spectrum of pane parameter possibilities. The second version is
        used for simpler user interfaces which do not require as much configuration.
        The last version allows a drop position to be specified, which will determine
        where the pane will be added.
    */
    bool AddPane(wxWindow* window, const wxAuiPaneInfo& pane_info);
    bool AddPane(wxWindow* window, int direction = wxLEFT,
                 const wxString& caption = wxEmptyString);
    bool AddPane(wxWindow* window,
                 const wxAuiPaneInfo& pane_info,
                 const wxPoint& drop_pos);
    //@}

    /**
        Tells the wxAuiManager to stop managing the pane specified by window.
        The window, if in a floated frame, is reparented to the frame managed
        by wxAuiManager.
    */
    bool DetachPane(wxWindow* window);

    /**
        Returns an array of all panes managed by the frame manager.
    */
    wxAuiPaneInfoArray& GetAllPanes();

    /**
        Returns the current art provider being used.
        @see wxAuiDockArt.
    */
    wxAuiDockArt* GetArtProvider() const;

    /**
        Returns the current dock constraint values.
        See SetDockSizeConstraint() for more information.
    */
    void GetDockSizeConstraint(double* widthpct, double* heightpct) const;

    /**
        Returns the current manager's flags.
    */
    unsigned int GetFlags() const;

    /**
        Returns the frame currently being managed by wxAuiManager.
    */
    wxWindow* GetManagedWindow() const;

    /**
        Calling this method will return the wxAuiManager for a given window.
        The @a window parameter should specify any child window or sub-child
        window of the frame or window managed by wxAuiManager.

        The @a window parameter need not be managed by the manager itself, nor does it
        even need to be a child or sub-child of a managed window. It must however
        be inside the window hierarchy underneath the managed window.
    */
    static wxAuiManager* GetManager(wxWindow* window);

    //@{
    /**
        GetPane() is used to lookup a wxAuiPaneInfo object either by window pointer
        or by pane name, which acts as a unique id for a window pane.

        The returned wxAuiPaneInfo object may then be modified to change a pane's
        look, state or position. After one or more modifications to wxAuiPaneInfo,
        wxAuiManager::Update() should be called to commit the changes to the user
        interface. If the lookup failed (meaning the pane could not be found in the
        manager), a call to the returned wxAuiPaneInfo's IsOk() method will return @false.
    */
    wxAuiPaneInfo& GetPane(wxWindow* window);
    wxAuiPaneInfo& GetPane(const wxString& name);
    //@}

    /**
        HideHint() hides any docking hint that may be visible.
    */
    virtual void HideHint();

    /**
        This method is used to insert either a previously unmanaged pane window
        into the frame manager, or to insert a currently managed pane somewhere
        else. InsertPane() will push all panes, rows, or docks aside and
        insert the window into the position specified by @a insert_location.

        Because @a insert_location can specify either a pane, dock row, or dock
        layer, the @a insert_level parameter is used to disambiguate this.
        The parameter @a insert_level can take a value of wxAUI_INSERT_PANE,
        wxAUI_INSERT_ROW or wxAUI_INSERT_DOCK.
    */
    bool InsertPane(wxWindow* window,
                    const wxAuiPaneInfo& insert_location,
                    int insert_level = wxAUI_INSERT_PANE);

    /**
        LoadPaneInfo() is similar to to LoadPerspective, with the exception that it
        only loads information about a single pane.  It is used in combination with
        SavePaneInfo().
    */
    void LoadPaneInfo(wxString pane_part, wxAuiPaneInfo& pane);

    /**
        Loads a saved perspective. If update is @true, wxAuiManager::Update()
        is automatically invoked, thus realizing the saved perspective on screen.
    */
    bool LoadPerspective(const wxString& perspective,
                         bool update = true);

    /**
        SavePaneInfo() is similar to SavePerspective, with the exception that it only
        saves information about a single pane.  It is used in combination with
        LoadPaneInfo().
    */
    wxString SavePaneInfo(wxAuiPaneInfo& pane);

    /**
        Saves the entire user interface layout into an encoded wxString, which
        can then be stored by the application (probably using wxConfig).

        When a perspective is restored using LoadPerspective(), the entire user
        interface will return to the state it was when the perspective was saved.
    */
    wxString SavePerspective();

    /**
        Instructs wxAuiManager to use art provider specified by parameter
        @a art_provider for all drawing calls.
        This allows plugable look-and-feel features. The previous art provider object,
        if any, will be deleted by wxAuiManager.

        @see wxAuiDockArt.
    */
    void SetArtProvider(wxAuiDockArt* art_provider);

    /**
        When a user creates a new dock by dragging a window into a docked position,
        often times the large size of the window will create a dock that is unwieldly
        large. wxAuiManager by default limits the size of any new dock to 1/3 of the
        window size.  For horizontal docks, this would be 1/3 of the window height.
        For vertical docks, 1/3 of the width.

        Calling this function will adjust this constraint value. The numbers must be
        between 0.0 and 1.0.  For instance, calling SetDockSizeContraint with
        0.5, 0.5 will cause new docks to be limited to half of the size of the
        entire managed window.
    */
    void SetDockSizeConstraint(double widthpct, double heightpct);

    /**
        This method is used to specify wxAuiManager's settings flags. @a flags
        specifies options which allow the frame management behavior to be modified.
    */
    void SetFlags(unsigned int flags);

    /**
        Called to specify the frame or window which is to be managed by wxAuiManager.
        Frame management is not restricted to just frames.  Child windows or custom
        controls are also allowed.
    */
    void SetManagedWindow(wxWindow* managed_wnd);

    /**
        This function is used by controls to explicitly show a hint window at the
        specified rectangle. It is rarely called, and is mostly used by controls
        implementing custom pane drag/drop behaviour.
        The specified rectangle should be in screen coordinates.
    */
    virtual void ShowHint(const wxRect& rect);

    /**
        Uninitializes the framework and should be called before a managed frame or
        window is destroyed. UnInit() is usually called in the managed wxFrame's
        destructor.  It is necessary to call this function before the managed frame
        or window is destroyed, otherwise the manager cannot remove its custom event
        handlers from a window.
    */
    void UnInit();

    /**
        This method is called after any number of changes are
        made to any of the managed panes. Update() must be invoked after
        AddPane() or InsertPane() are called in order to "realize" or "commit"
        the changes. In addition, any number of changes may be made to
        wxAuiPaneInfo structures (retrieved with wxAuiManager::GetPane), but to
        realize the changes, Update() must be called. This construction allows
        pane flicker to be avoided by updating the whole layout at one time.
    */
    void Update();

protected:

    /**
        ProcessDockResult() is a protected member of the wxAUI layout manager.
        It can be overridden by derived classes to provide custom docking calculations.
    */
    virtual bool ProcessDockResult(wxAuiPaneInfo& target,
                                   const wxAuiPaneInfo& new_pos);
};



/**
    @class wxAuiPaneInfo

    wxAuiPaneInfo is part of the wxAUI class framework.
    See also @ref overview_aui.

    wxAuiPaneInfo specifies all the parameters for a pane.
    These parameters specify where the pane is on the screen, whether it is docked
    or floating, or hidden.
    In addition, these parameters specify the pane's docked position, floating
    position, preferred size, minimum size, caption text among many other parameters.

    @library{wxbase}
    @category{aui}

    @see wxAuiManager, wxAuiDockArt
*/
class wxAuiPaneInfo
{
public:
    wxAuiPaneInfo();

    /**
        Copy constructor.
    */
    wxAuiPaneInfo(const wxAuiPaneInfo& c);

    /**
        Makes a copy of the wxAuiPaneInfo object.
    */
    wxAuiPaneInfo& operator=(const wxAuiPaneInfo& c);

    //@{
    /**
        GetInfo() serializes the layout information for this pane into a wxString.
        This panes state can then be restored by calling GetInfo() on the same string.
    */
    wxString GetInfo() const;
    /**
        LoadInfo() sets the layout information for this pane to the values stored in a serialized wxString.
        A serialized wxString containing this information can be generated by calling GetInfo().
    */
    void LoadInfo(wxString& info);
    //@}

    //@{
    /**
        IsToolbar() returns @true if the pane contains a toolbar.
    */
    bool IsToolbar() const;
    //@}

    //@{
    /**
        IsOk() returns @true if the wxAuiPaneInfo structure is valid. A pane structure
        is valid if it has an associated window.
    */
    bool IsOk() const;
    /**
        GetWindow() returns the window that is associated with this pane.
    */
    wxWindow* GetWindow() const;
    /**
        SetWindow() assigns the window pointer that the wxAuiPaneInfo should use.
        This normally does not need to be specified, as the window pointer is
        automatically assigned to the wxAuiPaneInfo structure as soon as it is added
        to the manager.
    */
    wxAuiPaneInfo& SetWindow(wxWindow* w);
    /**
    @deprecated Use SetWindow() instead.
    */
    wxAuiPaneInfo& Window(wxWindow* w);
    //@}

    //@{
    /**
        GetFrame() returns the floating frame window that holds the pane.
    */
    wxFrame* GetFrame() const;
    /**
        SetFrame() sets the floating frame window that holds the pane.
    */
    wxAuiPaneInfo& SetFrame(wxFrame* frame);
    //@}

    //@{
    /**
        IsResizable() returns @true if the pane can be  resized.
        This method is the opposite of IsFixed().
    */
    bool IsResizable() const;
    /**
        SetResizable() allows a pane to be resized if the parameter is @true, and forces it
        to be a fixed size if the parameter is @false.
        This method is the opposite of SetFixed().
    */
    wxAuiPaneInfo& SetResizable(bool resizable = true);
    /**
        IsFixed() returns @true if the pane is fixed (not allowed to be resized).
        This method is the opposite of IsResizable().
    */
    /**
        @deprecated Use SetResizable() instead.
    */
    wxAuiPaneInfo& Resizable(bool resizable = true);
    bool IsFixed() const;
    /**
        SetFixed() forces a pane to be fixed (not allowed to be resized). After calling Fixed(),
        IsFixed() will return @true.
        This method is the opposite of SetResizable().
    */
    wxAuiPaneInfo& SetFixed();
    /**
        @deprecated Use SetFixed() instead.
    */
    wxAuiPaneInfo& Fixed();
    //@}

    //@{
    /**
        IsShown() returns @true if the pane is currently shown.
    */
    bool IsShown() const;
    /**
        Hide() indicates that a pane should be hidden.
        This method is the opposite of Show().
    */
    wxAuiPaneInfo& Hide();
    /**
        Show() indicates that a pane should be shown.
        This method is the opposite of Hide().
    */
    wxAuiPaneInfo& Show(bool show = true);
    //@}

    //@{
    /**
        IsFloating() returns @true if the pane is floating.
        This method is the opposite of IsDocked().
    */
    bool IsFloating() const;
    /**
        Float() indicates that a pane should be floated.
        This method is the opposite of Dock().
    */
    wxAuiPaneInfo& Float();
    /**
        IsDocked() returns @true if the pane is docked.
        This method is the opposite of IsFloating().
    */
    bool IsDocked() const;
    /**
        Dock() indicates that a pane should be docked.
        This method is the opposite of Float().
    */
    wxAuiPaneInfo& Dock();
    //@}


    //@{
    /**
        IsTopDockable() returns @true if the pane can be docked at the top of the
        managed frame.
    */
    bool IsTopDockable() const;
    /**
        SetTopDockable() indicates whether a pane can be docked at the top of the frame.
    */
    wxAuiPaneInfo& SetTopDockable(bool b = true);
    /**
        @deprecated Use SetTopDockable() instead.
    */
    wxAuiPaneInfo& TopDockable(bool b = true);
    /**
        IsBottomDockable() returns @true if the pane can be docked at the bottom of the
        managed frame.
    */
    bool IsBottomDockable() const;
    /**
        SetBottomDockable() indicates whether a pane can be docked at the bottom of the
        frame.
    */
    wxAuiPaneInfo& SetBottomDockable(bool b = true);
    /**
        @deprecated Use SetBottomDockable() instead.
    */
    wxAuiPaneInfo& BottomDockable(bool b = true);
    /**
        IsLeftDockable() returns @true if the pane can be docked on the left of the
        managed frame.
    */
    bool IsLeftDockable() const;
    /**
        SetLeftDockable() indicates whether a pane can be docked on the left of the frame.
    */
    wxAuiPaneInfo& SetLeftDockable(bool b = true);
    /**
        @deprecated Use SetLeftDockable() instead.
    */
    wxAuiPaneInfo& LeftDockable(bool b = true);
    /**
        IsRightDockable() returns @true if the pane can be docked on the right of the
        managed frame.
    */
    bool IsRightDockable() const;
    /**
        SetRightDockable() indicates whether a pane can be docked on the right of the
        frame.
    */
    wxAuiPaneInfo& SetRightDockable(bool b = true);
    /**
        @deprecated Use SetRightDockable() instead.
    */
    wxAuiPaneInfo& RightDockable(bool b = true);
    /**
        SetDockable() specifies whether a frame can be docked or not. It is the same as
        specifying TopDockable(b).BottomDockable(b).LeftDockable(b).RightDockable(b).
    */
    wxAuiPaneInfo& SetDockable(bool b = true);
    /**
        @deprecated Use SetDockable() instead.
    */
    wxAuiPaneInfo& Dockable(bool b = true);
    //@}

    //@{
    /**
        IsFloatable() returns @true if the pane can be undocked and displayed as a
        floating window.
    */
    bool IsFloatable() const;
    /**
        SetFloatable() sets whether the user will be able to undock a pane and turn it
        into a floating window.
    */
    wxAuiPaneInfo& SetFloatable(bool b = true);
    /**
        @deprecated Use SetFloatable() instead.
    */
    wxAuiPaneInfo& Floatable(bool b = true);
    //@}

    //@{
    /**
        IsMovable() returns @true if the docked frame can be undocked or moved to
        another dock position.
    */
    bool IsMovable() const;
    /**
        SetMovable() indicates whether a frame can be moved.
    */
    wxAuiPaneInfo& SetMovable(bool b = true);
    /**
        @deprecated Use SetMovable() instead.
    */
    wxAuiPaneInfo& Movable(bool b = true);
    //@}

    //@{
    /**
        IsMaximized() returns @true if the pane is maximized.
    */
    bool IsMaximized() const;
    /**
        Maximize() sets a pane to display as maximized.
    */
    wxAuiPaneInfo& Maximize();
    /**
        Restore() restores a pane to its previous(non maximized) size.
    */
    wxAuiPaneInfo& Restore();
    //@}

    //@{
    /**
        IsDestroyOnClose() returns @true if a pane should be destroyed when it is closed.
        Normally a pane is simply hidden when the close button is clicked.
    */
    bool IsDestroyOnClose() const;
    /**
        SetDestroyOnClose() indicates whether a pane should be detroyed when it is closed.
        Normally a pane is simply hidden when the close button is clicked.
        Setting DestroyOnClose to @true will cause the window to be destroyed when
        the user clicks the pane's close button.
    */
    wxAuiPaneInfo& SetDestroyOnClose(bool b = true);
    /**
        @deprecated Use SetDestroyOnClose() instead.
    */
    wxAuiPaneInfo& DestroyOnClose(bool b = true);
    //@}

    //@{
    /**
        HasCaption() returns @true if the pane displays a caption.
    */
    bool HasCaption() const;
    /**
        SetCaptionVisible() indicates that a pane caption should be visible. If @false, no
        pane caption is drawn.
    */
    wxAuiPaneInfo& SetCaptionVisible(bool visible = true);
    /**
        @deprecated Use SetCaptionVisible() instead.
    */
    wxAuiPaneInfo& CaptionVisible(bool visible = true);
    //@}

    //@{
    /**
        HasGripper() returns @true if the pane displays a gripper.
    */
    bool HasGripper() const;
    /**
        SetGripper() indicates that a gripper should be drawn for the pane.
    */
    wxAuiPaneInfo& SetGripper(bool visible = true);
    /**
        @deprecated Use SetGripper() instead.
    */
    wxAuiPaneInfo& Gripper(bool visible = true);
    /**
        HasGripperTop() returns @true if the pane displays a gripper at the top.
    */
    bool HasGripperTop() const;
    /**
        SetGripperTop() indicates that a gripper should be drawn at the top of the pane.
    */
    wxAuiPaneInfo& SetGripperTop(bool attop = true);
    /**
        @deprecated Use SetGripper() instead.
    */
    wxAuiPaneInfo& GripperTop(bool attop = true);
    //@}

    //@{
    /**
        HasBorder() returns @true if the pane displays a border.
    */
    bool HasBorder() const;
    /**
        SetBorder indicates that a border should be drawn for the pane.
    */
    wxAuiPaneInfo& SetBorder(bool visible = true);
    /**
        @deprecated Use SetBorder() instead.
    */
    wxAuiPaneInfo& PaneBorder(bool visible = true);
    //@}

    //@{
    /**
        HasCloseButton() returns @true if the pane displays a button to close the pane.
    */
    bool HasCloseButton() const;
    /**
        SetCloseButton() indicates that a close button should be drawn for the pane.
    */
    wxAuiPaneInfo& SetCloseButton(bool visible = true);
    /**
        @deprecated Use SetCloseButton() instead.
    */
    wxAuiPaneInfo& CloseButton(bool visible = true);
    //@}

    //@{
    /**
        HasMaximizeButton() returns @true if the pane displays a button to maximize the
        pane.
    */
    bool HasMaximizeButton() const;
    /**
        SetMaximizeButton() indicates that a maximize button should be drawn for the pane.
    */
    wxAuiPaneInfo& SetMaximizeButton(bool visible = true);
    /**
        @deprecated Use SetMaximizeButton() instead.
    */
    wxAuiPaneInfo& MaximizeButton(bool visible = true);
    //@}

    //@{
    /**
        HasMinimizeButton() returns @true if the pane displays a button to minimize the
        pane.
    */
    bool HasMinimizeButton() const;
    /**
        SetMinimizeButton() indicates that a minimize button should be drawn for the pane.
    */
    wxAuiPaneInfo& SetMinimizeButton(bool visible = true);
    /**
        @deprecated Use SetMinimizeButton() instead.
    */
    wxAuiPaneInfo& MinimizeButton(bool visible = true);
    //@}

    //@{
    /**
        HasPinButton() returns @true if the pane displays a button to float the pane.
    */
    bool HasPinButton() const;
    /**
        SetPinButton() indicates that a pin button should be drawn for the pane.
    */
    wxAuiPaneInfo& SetPinButton(bool visible = true);
    /**
        @deprecated Use SetPinButton() instead.
    */
    wxAuiPaneInfo& PinButton(bool visible = true);
    //@}

    //@{
    /**
        GetName() gets the name that has been assigned to the pane.
    */
    wxString GetName() const;
    /**
        SetName() sets the name of the pane so it can be referenced in lookup functions.
        If a name is not specified by the user, a random name is assigned to the pane
        when it is added to the manager.
    */
    wxAuiPaneInfo& SetName(const wxString& n);
    /**
    @deprecated Use SetName() instead.
    */
    wxAuiPaneInfo& Name(const wxString& n);
    //@{
    /**
        GetCaption() gets the caption of the pane.
    */
    wxString GetCaption() const;
    /**
        SetCaption() sets the caption of the pane.
    */
    wxAuiPaneInfo& SetCaption(const wxString& c);
    /**
    @deprecated Use SetCaption() instead.
    */
    wxAuiPaneInfo& Caption(const wxString& c);
    //@}

    //@{
    /**
        GetDirection() gets the direction that has been set for the docked pane.
    */
    int GetDirection() const;
    /**
        SetDirection() determines the direction of the docked pane. It is functionally the
        same as calling SetDirectionLeft(), SetDirectionRight(), SetDirectionTop() or SetDirectionBottom(), 
        except that docking direction may be specified programmatically via the parameter.
    */
    wxAuiPaneInfo& SetDirection(int direction);
    /**
    @deprecated Use SetDirection() instead.
    */
    wxAuiPaneInfo& Direction(int direction);
    /**
        SetDirectionLeft() sets the pane dock position to the left side of the frame. This is the
        same thing as calling Direction(wxAUI_DOCK_LEFT).
    */
    wxAuiPaneInfo& SetDirectionLeft();
    /**
    @deprecated Use SetDirectionLeft() instead.
    */
    wxAuiPaneInfo& Left();
    /**
        SetDirectionRight() sets the pane dock position to the right side of the frame. This is the
        same thing as calling Direction(wxAUI_DOCK_RIGHT).
    */
    wxAuiPaneInfo& SetDirectionRight();
    /**
    @deprecated Use SetDirectionRight() instead.
    */
    wxAuiPaneInfo& Right();
    /**
        SetDirectionTop() sets the pane dock position to the top of the frame. This is the
        same thing as calling Direction(wxAUI_DOCK_TOP).
    */
    wxAuiPaneInfo& SetDirectionTop();
    /**
    @deprecated Use SetDirectionTop() instead.
    */
    wxAuiPaneInfo& Top();
    /**
        SetDirectionBottom() sets the pane dock position to the top of the frame. This is the
        same thing as calling Direction(wxAUI_DOCK_BOTTOM).
    */
    wxAuiPaneInfo& SetDirectionBottom();
    /**
    @deprecated Use SetDirectionBottom() instead.
    */
    wxAuiPaneInfo& Bottom();
    /**
        SetDirectionCenter() sets the pane dock position to the left side of the frame.
        The centre pane is the space in the middle after all border panes (left, top,
        right, bottom) are subtracted from the layout.
        This is the same thing as calling Direction(wxAUI_DOCK_CENTRE).
    */
    wxAuiPaneInfo& SetDirectionCenter();
    /**
        Same as SetDirectionCenter().
    */
    wxAuiPaneInfo& SetDirectionCentre();
    /**
    @deprecated Use SetDirectionCenter() instead.
    */
    wxAuiPaneInfo& Center();
    /**
    @deprecated Use SetDirectionCentre() instead.
    */
    wxAuiPaneInfo& Centre();
    //@}

    //@{
    /**
        GetLayer() gets the layer of the docked pane. The dock layer is similar to
        an onion, the inner-most layer being layer 0. Each shell moving in the outward
        direction has a higher layer number. This allows for more complex docking layout
        formation.
    */
    int GetLayer() const;
    /**
        SetLayer() sets the layer of the docked pane. The dock layer is similar to
        an onion, the inner-most layer being layer 0. Each shell moving in the outward
        direction has a higher layer number. This allows for more complex docking layout
        formation.
    */
    wxAuiPaneInfo& SetLayer(int layer);
    /**
    @deprecated Use SetLayer() instead.
    */
    wxAuiPaneInfo& Layer(int layer);
    //@}

    //@{
    /**
        GetRow() gets the row of the docked pane.
    */
    int GetRow() const;
    /**
        SetRow() sets the row of the docked pane.
    */
    wxAuiPaneInfo& SetRow(int row);
    /**
    @deprecated Use SetRow() instead.
    */
    wxAuiPaneInfo& Row(int row);
    //@}

    //@{
    /**
        GetPosition() gets the position of the docked pane.
    */
    int GetPosition() const;
    /**
        SetPosition() sets the position of the docked pane.
    */
    wxAuiPaneInfo& SetPosition(int pos);
    /**
    @deprecated Use SetPosition() instead.
    */
    wxAuiPaneInfo& Position(int pos);
    //@}

    //@{
    /**
        GetPage() gets the tab position of the docked pane.
    */
    wxAuiPaneInfo& GetPage(int page);
    /**
        SetPage() sets the tab position of the docked pane.
    */
    int SetPage() const;
    //@}

    //@{
    /**
        GetProportion() gets the proportion of the docked pane.
    */
    int GetProportion() const;
    /**
        SetProportion() sets the proportion of the docked pane.
    */
    wxAuiPaneInfo& SetProportion(int proportion);
    //@}

    //@{
    /**
        GetFloatingPosition() gets the position of the floating pane.
    */
    wxPoint GetFloatingPosition() const;
    /**
        SetFloatingPosition() sets the position of the floating pane.
    */
    wxAuiPaneInfo& SetFloatingPosition(int x, int y);
    /**
        SetFloatingPosition() sets the position of the floating pane.
    */
    wxAuiPaneInfo& SetFloatingPosition(const wxPoint& pos);
    /**
        @deprecated Use SetFloatingPosition() instead.
    */
    FloatingPosition(const wxPoint& pos);
    /**
        @deprecated Use SetFloatingPosition() instead.
    */
    FloatingPosition(int x, int y);
    //@}

    //@{
    /**
        GetFloatingSize() gets the size of the floating pane.
    */
    wxSize GetFloatingSize() const;
    /**
        SetFloatingSize() sets the size of the floating pane.
    */
    wxAuiPaneInfo& SetFloatingSize(int x, int y);
    /**
        SetFloatingSize() sets the size of the floating pane.
    */
    wxAuiPaneInfo& SetFloatingSize(const wxSize& size);
    /**
        @deprecated Use SetFloatingSize() instead.
    */
    FloatingSize(const wxSize& size);
    /**
        @deprecated Use SetFloatingSize() instead.
    */
    FloatingSize(int x, int y);
    //@}

    //@{
    /**
        GetBestSize() gets the ideal size for the pane. The docking manager will attempt
        to use this size as much as possible when docking or floating the pane.
    */
    wxSize GetBestSize() const;
    /**
        SetBestSize() sets the ideal size for the pane. The docking manager will attempt
        to use this size as much as possible when docking or floating the pane.
    */
    wxAuiPaneInfo& SetBestSize(int x, int y);
    wxAuiPaneInfo& SetBestSize(const wxSize& size);
    /**
        @deprecated Use SetBestSize() instead.
    */
    wxAuiPaneInfo& BestSize(const wxSize& size);
    /**
        @deprecated Use SetBestSize() instead.
    */
    wxAuiPaneInfo& BestSize(int x, int y);
    //@}

    //@{
    /**
        GetMinSize() gets the minimum size of the pane.
    */
    wxSize GetMinSize() const;
    /**
        SetMinSize() sets the minimum size of the pane. Please note that this is only
        partially supported as of this writing.
    */
    wxAuiPaneInfo& SetMinSize(int x, int y);
    /**
        SetMinSize() sets the minimum size of the pane. Please note that this is only
        partially supported as of this writing.
    */
    wxAuiPaneInfo& SetMinSize(const wxSize& size);
    /**
        @deprecated Use SetMinSize() instead.
    */
    wxAuiPaneInfo& MinSize(const wxSize& size);
    /**
        @deprecated Use SetMinSize() instead.
    */
    wxAuiPaneInfo& MinSize(int x, int y);
    //@}

    //@{
    /**
        GetMaxSize() gets the maximum size of the pane.
    */
    wxSize GetMaxSize() const;
    /**
        SetMaxSize() sets the maximum size of the pane.
    */
    wxAuiPaneInfo& SetMaxSize(int x, int y);
    /**
        SetMaxSize() sets the maximum size of the pane.
    */
    wxAuiPaneInfo& SetMaxSize(const wxSize& size);
    /**
        @deprecated Use SetMaxSize() instead.
    */
    wxAuiPaneInfo& MaxSize(const wxSize& size);
    /**
        @deprecated Use SetMaxSize() instead.
    */
    wxAuiPaneInfo& MaxSize(int x, int y);
    //@}

    //@{
    // get/set the current rectangle (populated by wxAUI).
    wxRect GetRect() const;
    wxAuiPaneInfo& SetRect(const wxRect& rect);
    //@}

    //@{
    // get the array of buttons that are present on the pane.
    wxAuiPaneButtonArray& GetButtons();
    //@}

    //@{
    /**
        IsDockFixed() returns @true if the containing dock has no resize sash.
    */
    bool IsDockFixed() const;
    /**
        SetDockFixed() causes the containing dock to have no resize sash if b is &true.
        This is useful for creating panes that span the entire width or height of a dock, but should
        not be resizable in the other direction.
    */
    wxAuiPaneInfo& SetDockFixed(bool b = true);
    /**
        @deprecated Use SetDockFixed() instead.
    */
    wxAuiPaneInfo& DockFixed(bool b = true);
    //@}

    //@{
    /**
        HasFlag() returns @true if the property specified by flag is active for
        the pane.
    */
    bool HasFlag(int flag) const;
    /**
        SetFlag() turns the property given by flag on or off with the option_state
        parameter.
    */
    wxAuiPaneInfo& SetFlag(int flag, bool option_state);
    //@}

    //@{
    /**
        SetCentrePane() specifies that the pane should adopt the default center pane
        settings. Centre panes usually do not have caption bars.
        This function provides an easy way of preparing a pane to be displayed in
        the center dock position.
    */
    wxAuiPaneInfo& SetCentrePane();
    /**
        Same as SetCentrePane().
    */
    wxAuiPaneInfo& SetCenterPane();
    /**
        @deprecated Use SetCentrePane() instead.
    */
    wxAuiPaneInfo& CentrePane();
    /**
        @deprecated Use SetCenterPane() instead.
    */
    wxAuiPaneInfo& CenterPane();
    //@}

    //@{
    /**
        SetDefaultPane() specifies that the pane should adopt the default pane settings.
    */
    wxAuiPaneInfo& SetDefaultPane();
    /**
        @deprecated Use SetDefaultPane() instead.
    */
    wxAuiPaneInfo& DefaultPane();
    //@}

    //@{
    /**
        SetToolbarPane() specifies that the pane should adopt the default toolbar pane
        settings.
    */
    wxAuiPaneInfo& SetToolbarPane();
    /**
        @deprecated Use SetToolbarPane() instead.
    */
    wxAuiPaneInfo& ToolbarPane();
    //@}

    //@{
    /**
        Write the safe parts of a newly loaded PaneInfo structure "source" into "this"
        used on loading perspectives etc.
    */
    void SafeSet(wxAuiPaneInfo source);
    //@}
};



/**
    @class wxAuiManagerEvent

    Event used to indicate various actions taken with wxAuiManager.

    See wxAuiManager for available event types.

    @library{wxaui}
    @category{events,aui}

    @see wxAuiManager, wxAuiPaneInfo
*/
class wxAuiManagerEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxAuiManagerEvent(wxEventType type = wxEVT_NULL);

    /**
        @return @true if this event can be vetoed.

        @see Veto()
    */
    bool CanVeto();

    /**
        @return The ID of the button that was clicked.
    */
    int GetButton();

    /**
        @todo What is this?
    */
    wxDC* GetDC();

    /**
        @return @true if this event was vetoed.

        @see Veto()
    */
    bool GetVeto();

    /**
        @return The wxAuiManager this event is associated with.
    */
    wxAuiManager* GetManager();

    /**
        @return The pane this event is associated with.
    */
    wxAuiPaneInfo* GetPane();

    /**
        Sets the ID of the button clicked that triggered this event.
    */
    void SetButton(int button);

    /**
        Sets whether or not this event can be vetoed.
    */
    void SetCanVeto(bool can_veto);

    /**
        @todo What is this?
    */
    void SetDC(wxDC* pdc);

    /**
        Sets the wxAuiManager this event is associated with.
    */
    void SetManager(wxAuiManager* manager);

    /**
        Sets the pane this event is associated with.
    */
    void SetPane(wxAuiPaneInfo* pane);

    /**
        Cancels the action indicated by this event if CanVeto() is @true.
    */
    void Veto(bool veto = true);
};

