/////////////////////////////////////////////////////////////////////////////
// Name:        toolbar.h
// Purpose:     documentation for wxToolBar class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxToolBar
    @wxheader{toolbar.h}

    The name wxToolBar is defined to be a synonym for one of the following classes:

     @b wxToolBar95 The native Windows 95 toolbar. Used on Windows 95, NT 4 and
    above.
     @b wxToolBarMSW A Windows implementation. Used on 16-bit Windows.
     @b wxToolBarGTK The GTK toolbar.


    @beginStyleTable
    @style{wxTB_FLAT}:
           Gives the toolbar a flat look (Windows and GTK only).
    @style{wxTB_DOCKABLE}:
           Makes the toolbar floatable and dockable (GTK only).
    @style{wxTB_HORIZONTAL}:
           Specifies horizontal layout (default).
    @style{wxTB_VERTICAL}:
           Specifies vertical layout.
    @style{wxTB_TEXT}:
           Shows the text in the toolbar buttons; by default only icons are
           shown.
    @style{wxTB_NOICONS}:
           Specifies no icons in the toolbar buttons; by default they are
           shown.
    @style{wxTB_NODIVIDER}:
           Specifies no divider (border) above the toolbar (Windows only).
    @style{wxTB_NOALIGN}:
           Specifies no alignment with the parent window (Windows only, not
           very useful).
    @style{wxTB_HORZ_LAYOUT}:
           Shows the text and the icons alongside, not vertically stacked
           (Windows and GTK 2 only). This style must be used with wxTB_TEXT.
    @style{wxTB_HORZ_TEXT}:
           Combination of wxTB_HORZ_LAYOUT and wxTB_TEXT.
    @style{wxTB_NO_TOOLTIPS}:
           Don't show the short help tooltips for the tools when the mouse
           hovers over them.
    @style{wxTB_BOTTOM}:
           Align the toolbar at the bottom of parent window.
    @style{wxTB_RIGHT}:
           Align the toolbar at the right side of parent window.
    @endStyleTable

    @library{wxbase}
    @category{miscwnd}

    @seealso
    @ref overview_wxtoolbaroverview "Toolbar overview", wxScrolledWindow
*/
class wxToolBar : public wxControl
{
public:
    //@{
    /**
        Constructs a toolbar.
        
        @param parent
            Pointer to a parent window.
        @param id
            Window identifier. If -1, will automatically create an identifier.
        @param pos
            Window position. wxDefaultPosition is (-1, -1) which indicates that
        wxWidgets
            should generate a default position for the window. If using the wxWindow
        class directly, supply
            an actual position.
        @param size
            Window size. wxDefaultSize is (-1, -1) which indicates that wxWidgets
            should generate a default size for the window.
        @param style
            Window style. See wxToolBar for details.
        @param name
            Window name.
        
        @remarks After a toolbar is created, you use AddTool() and
                 perhaps AddSeparator(), and then you must call
                 Realize() to construct and display the toolbar
                 tools.
    */
    wxToolBar();
    wxToolBar(wxWindow* parent, wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxTB_HORIZONTAL | wxBORDER_NONE,
              const wxString& name = wxPanelNameStr);
    //@}

    /**
        Toolbar destructor.
    */
    ~wxToolBar();

    /**
        Adds a new check (or toggle) tool to the toolbar. The parameters are the same
        as in AddTool().
        
        @see AddTool()
    */
    wxToolBarToolBase* AddCheckTool(int toolId,
                                    const wxString& label,
                                    const wxBitmap& bitmap1,
                                    const wxBitmap& bitmap2,
                                    const wxString& shortHelpString = "",
                                    const wxString& longHelpString = "",
                                    wxObject* clientData = NULL);

    /**
        Adds any control to the toolbar, typically e.g. a combobox.
        
        @param control
            The control to be added.
        @param label
            Text to be displayed near the control.
        
        @remarks wxMSW: the label is only displayed if there is enough space
                 available below the embedded control.
    */
    bool AddControl(wxControl* control, const wxString label = "");

    /**
        Adds a new radio tool to the toolbar. Consecutive radio tools form a radio
        group such that exactly one button in the group is pressed at any moment, in
        other words whenever a button in the group is pressed the previously pressed
        button is automatically released. You should avoid having the radio groups of
        only one element as it would be impossible for the user to use such button.
        By default, the first button in the radio group is initially pressed, the
        others are not.
        
        @see AddTool()
    */
    wxToolBarToolBase* AddRadioTool(int toolId,
                                    const wxString& label,
                                    const wxBitmap& bitmap1,
                                    const wxBitmap& bitmap2,
                                    const wxString& shortHelpString = "",
                                    const wxString& longHelpString = "",
                                    wxObject* clientData = NULL);

    /**
        Adds a separator for spacing groups of tools.
        
        @see AddTool(), SetToolSeparation()
    */
    void AddSeparator();

    //@{
    /**
        Adds a tool to the toolbar. The first (short and most commonly used) version
        has fewer parameters than the full version at the price of not being able to
        specify some of the more rarely used button features. The last version allows
        you to add an existing tool.
        
        @param toolId
            An integer by which
            the tool may be identified in subsequent operations.
        @param kind
            May be wxITEM_NORMAL for a normal button (default),
            wxITEM_CHECK for a checkable tool (such tool stays pressed after it had been
            toggled) or wxITEM_RADIO for a checkable tool which makes part of a radio
            group of tools each of which is automatically unchecked whenever another
        button
            in the group is checked
        @param bitmap1
            The primary tool bitmap.
        @param bitmap2
            The bitmap used when the tool is disabled. If it is equal to
            wxNullBitmap, the disabled bitmap is automatically generated by greing the
            normal one.
        @param shortHelpString
            This string is used for the tools tooltip
        @param longHelpString
            This string is shown in the statusbar (if any) of the
            parent frame when the mouse pointer is inside the tool
        @param clientData
            An optional pointer to client data which can be
            retrieved later using GetToolClientData().
        @param tool
            The tool to be added.
        
        @remarks After you have added tools to a toolbar, you must call
                 Realize() in order to have the tools appear.
        
        @see AddSeparator(), AddCheckTool(), AddRadioTool(),
             InsertTool(), DeleteTool(), Realize()
    */
    wxToolBarToolBase* AddTool(int toolId, const wxString& label,
                               const wxBitmap& bitmap1,
                               const wxString& shortHelpString = "",
                               wxItemKind kind = wxITEM_NORMAL);
    wxToolBarToolBase* AddTool(int toolId, const wxString& label,
                               const wxBitmap& bitmap1,
                               const wxBitmap& bitmap2 = wxNullBitmap,
                               wxItemKind kind = wxITEM_NORMAL,
                               const wxString& shortHelpString = "",
                               const wxString& longHelpString = "",
                               wxObject* clientData = NULL);
    wxToolBarToolBase* AddTool(wxToolBarToolBase* tool);
    //@}

    /**
        Deletes all the tools in the toolbar.
    */
    void ClearTools();

    /**
        Removes the specified tool from the toolbar and deletes it. If you don't want
        to delete the tool, but just to remove it from the toolbar (to possibly add it
        back later), you may use RemoveTool() instead.
        Note that it is unnecessary to call Realize() for the
        change to take place, it will happen immediately.
        Returns @true if the tool was deleted, @false otherwise.
        
        @see DeleteToolByPos()
    */
    bool DeleteTool(int toolId);

    /**
        This function behaves like DeleteTool() but it
        deletes the tool at the specified position and not the one with the given id.
    */
    bool DeleteToolByPos(size_t pos);

    /**
        Enables or disables the tool.
        
        @param toolId
            Tool to enable or disable.
        @param enable
            If @true, enables the tool, otherwise disables it.
        
        @remarks Some implementations will change the visible state of the tool
                 to indicate that it is disabled.
        
        @see GetToolEnabled(), ToggleTool()
    */
    void EnableTool(int toolId, bool enable);

    /**
        Returns a pointer to the tool identified by @a id or
        @NULL if no corresponding tool is found.
    */
    wxToolBarToolBase* FindById(int id);

    /**
        Returns a pointer to the control identified by @a id or
        @NULL if no corresponding control is found.
    */
    wxControl* FindControl(int id);

    /**
        Finds a tool for the given mouse position.
        
        @param x
            X position.
        @param y
            Y position.
        
        @returns A pointer to a tool if a tool is found, or @NULL otherwise.
        
        @remarks Currently not implemented in wxGTK (always returns @NULL there).
    */
    wxToolBarToolBase* FindToolForPosition(wxCoord x, wxCoord y) const;

    /**
        Returns the left/right and top/bottom margins, which are also used for
        inter-toolspacing.
        
        @see SetMargins()
    */
    wxSize GetMargins() const;

    /**
        Returns the size of bitmap that the toolbar expects to have. The default bitmap
        size is 16 by 15 pixels.
        
        @remarks Note that this is the size of the bitmap you pass to
                 AddTool(), and not the eventual size of the
                 tool button.
        
        @see SetToolBitmapSize(), GetToolSize()
    */
    wxSize GetToolBitmapSize();

    /**
        Get any client data associated with the tool.
        
        @param toolId
            Id of the tool, as passed to AddTool().
        
        @returns Client data, or @NULL if there is none.
    */
    wxObject* GetToolClientData(int toolId) const;

    /**
        Called to determine whether a tool is enabled (responds to user input).
        
        @param toolId
            Id of the tool in question.
        
        @returns @true if the tool is enabled, @false otherwise.
        
        @see EnableTool()
    */
    bool GetToolEnabled(int toolId) const;

    /**
        Returns the long help for the given tool.
        
        @param toolId
            The tool in question.
        
        @see SetToolLongHelp(), SetToolShortHelp()
    */
    wxString GetToolLongHelp(int toolId) const;

    /**
        Returns the value used for packing tools.
        
        @see SetToolPacking()
    */
    int GetToolPacking() const;

    /**
        Returns the tool position in the toolbar, or @c wxNOT_FOUND if the tool is not
        found.
    */
    int GetToolPos(int toolId) const;

    /**
        Returns the default separator size.
        
        @see SetToolSeparation()
    */
    int GetToolSeparation() const;

    /**
        Returns the short help for the given tool.
        
        @param toolId
            The tool in question.
        
        @see GetToolLongHelp(), SetToolShortHelp()
    */
    wxString GetToolShortHelp(int toolId) const;

    /**
        Returns the size of a whole button, which is usually larger than a tool bitmap
        because
        of added 3D effects.
        
        @see SetToolBitmapSize(), GetToolBitmapSize()
    */
    wxSize GetToolSize();

    /**
        Gets the on/off state of a toggle tool.
        
        @param toolId
            The tool in question.
        
        @returns @true if the tool is toggled on, @false otherwise.
        
        @see ToggleTool()
    */
    bool GetToolState(int toolId) const;

    /**
        Returns the number of tools in the toolbar.
    */
    int GetToolsCount() const;

    /**
        Inserts the control into the toolbar at the given position.
        You must call Realize() for the change to take place.
        
        @see AddControl(), InsertTool()
    */
    wxToolBarToolBase* InsertControl(size_t pos, wxControl* control);

    /**
        Inserts the separator into the toolbar at the given position.
        You must call Realize() for the change to take place.
        
        @see AddSeparator(), InsertTool()
    */
    wxToolBarToolBase* InsertSeparator(size_t pos);

    //@{
    /**
        Inserts the tool with the specified attributes into the toolbar at the given
        position.
        You must call Realize() for the change to take place.
        
        @see AddTool(), InsertControl(), InsertSeparator()
    */
    wxToolBarToolBase* InsertTool(size_t pos, int toolId,
                                  const wxBitmap& bitmap1,
                                  const wxBitmap& bitmap2 = wxNullBitmap,
                                  bool isToggle = false,
                                  wxObject* clientData = NULL,
                                  const wxString& shortHelpString = "",
                                  const wxString& longHelpString = "");
    wxToolBarToolBase* InsertTool(size_t pos,
                                  wxToolBarToolBase* tool);
    //@}

    /**
        Called when the user clicks on a tool with the left mouse button.
        This is the old way of detecting tool clicks; although it will still work,
        you should use the EVT_MENU or EVT_TOOL macro instead.
        
        @param toolId
            The identifier passed to AddTool().
        @param toggleDown
            @true if the tool is a toggle and the toggle is down, otherwise is @false.
        
        @returns If the tool is a toggle and this function returns @false, the
                 toggle toggle state (internal and visual) will not be
                 changed. This provides a way of specifying that toggle
                 operations are not permitted in some circumstances.
        
        @see OnMouseEnter(), OnRightClick()
    */
    bool OnLeftClick(int toolId, bool toggleDown);

    /**
        This is called when the mouse cursor moves into a tool or out of
        the toolbar.
        This is the old way of detecting mouse enter events; although it will still
        work,
        you should use the EVT_TOOL_ENTER macro instead.
        
        @param toolId
            Greater than -1 if the mouse cursor has moved into the tool,
            or -1 if the mouse cursor has moved. The
            programmer can override this to provide extra information about the tool,
            such as a short description on the status line.
        
        @remarks With some derived toolbar classes, if the mouse moves quickly
                 out of the toolbar, wxWidgets may not be able to detect
                 it. Therefore this function may not always be called
                 when expected.
    */
    void OnMouseEnter(int toolId);

    /**
        Called when the user clicks on a tool with the right mouse button. The
        programmer should override this function to detect right tool clicks.
        This is the old way of detecting tool right clicks; although it will still work,
        you should use the EVT_TOOL_RCLICKED macro instead.
        
        @param toolId
            The identifier passed to AddTool().
        @param x
            The x position of the mouse cursor.
        @param y
            The y position of the mouse cursor.
        
        @remarks A typical use of this member might be to pop up a menu.
        
        @see OnMouseEnter(), OnLeftClick()
    */
    void OnRightClick(int toolId, float x, float y);

    /**
        This function should be called after you have added tools.
    */
    bool Realize();

    /**
        Removes the given tool from the toolbar but doesn't delete it. This allows to
        insert/add this tool back to this (or another) toolbar later.
        Note that it is unnecessary to call Realize() for the
        change to take place, it will happen immediately.
        
        @see DeleteTool()
    */
    wxToolBarToolBase* RemoveTool(int id);

    /**
        Sets the bitmap resource identifier for specifying tool bitmaps as indices
        into a custom bitmap. Windows CE only.
    */
    void SetBitmapResource(int resourceId);

    /**
        Sets the dropdown menu for the tool given by its @e id. The tool itself will
        delete the menu when it's no longer needed.
        If you define a EVT_TOOL_DROPDOWN handler in your program, you must call
        wxEvent::Skip from it or the menu won't be displayed.
    */
    bool SetDropdownMenu(int id, wxMenu* menu);

    //@{
    /**
        Set the values to be used as margins for the toolbar.
        
        @param size
            Margin size.
        @param x
            Left margin, right margin and inter-tool separation value.
        @param y
            Top margin, bottom margin and inter-tool separation value.
        
        @remarks This must be called before the tools are added if absolute
                 positioning is to be used, and the default (zero-size)
                 margins are to be overridden.
        
        @see GetMargins(), wxSize
    */
    void SetMargins(const wxSize& size);
    void SetMargins(int x, int y);
    //@}

    /**
        Sets the default size of each tool bitmap. The default bitmap size is 16 by 15
        pixels.
        
        @param size
            The size of the bitmaps in the toolbar.
        
        @remarks This should be called to tell the toolbar what the tool bitmap
                 size is. Call it before you add tools.
        
        @see GetToolBitmapSize(), GetToolSize()
    */
    void SetToolBitmapSize(const wxSize& size);

    /**
        Sets the client data associated with the tool.
    */
    void SetToolClientData(int id, wxObject* clientData);

    /**
        Sets the bitmap to be used by the tool with the given ID when the tool
        is in a disabled state.  This can only be used on Button tools, not
        controls.  NOTE: The native toolbar classes on the main platforms all
        synthesize the disabled bitmap from the normal bitmap, so this
        function will have no effect on those platforms.
    */
    void SetToolDisabledBitmap(int id, const wxBitmap& bitmap);

    /**
        Sets the long help for the given tool.
        
        @param toolId
            The tool in question.
        @param helpString
            A string for the long help.
        
        @remarks You might use the long help for displaying the tool purpose on
                 the status line.
        
        @see GetToolLongHelp(), SetToolShortHelp(),
    */
    void SetToolLongHelp(int toolId, const wxString& helpString);

    /**
        Sets the bitmap to be used by the tool with the given ID.  This can
        only be used on Button tools, not controls.
    */
    void SetToolNormalBitmap(int id, const wxBitmap& bitmap);

    /**
        Sets the value used for spacing tools. The default value is 1.
        
        @param packing
            The value for packing.
        
        @remarks The packing is used for spacing in the vertical direction if the
                 toolbar is horizontal, and for spacing in the
                 horizontal direction if the toolbar is vertical.
        
        @see GetToolPacking()
    */
    void SetToolPacking(int packing);

    /**
        Sets the default separator size. The default value is 5.
        
        @param separation
            The separator size.
        
        @see AddSeparator()
    */
    void SetToolSeparation(int separation);

    /**
        Sets the short help for the given tool.
        
        @param toolId
            The tool in question.
        @param helpString
            The string for the short help.
        
        @remarks An application might use short help for identifying the tool
                 purpose in a tooltip.
        
        @see GetToolShortHelp(), SetToolLongHelp()
    */
    void SetToolShortHelp(int toolId, const wxString& helpString);

    /**
        Toggles a tool on or off. This does not cause any event to get emitted.
        
        @param toolId
            Tool in question.
        @param toggle
            If @true, toggles the tool on, otherwise toggles it off.
        
        @remarks Only applies to a tool that has been specified as a toggle tool.
    */
    void ToggleTool(int toolId, bool toggle);
};
