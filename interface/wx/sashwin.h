/////////////////////////////////////////////////////////////////////////////
// Name:        sashwin.h
// Purpose:     interface of wxSashWindow
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxSashWindow

    wxSashWindow allows any of its edges to have a sash which can be dragged
    to resize the window. The actual content window will be created by the
    application
    as a child of wxSashWindow. The window (or an ancestor) will be notified of a
    drag
    via a wxSashEvent notification.

    @beginStyleTable
    @style{wxSW_3D}
           Draws a 3D effect sash and border.
    @style{wxSW_3DSASH}
           Draws a 3D effect sash.
    @style{wxSW_3DBORDER}
           Draws a 3D effect border.
    @style{wxSW_BORDER}
           Draws a thin black border.
    @endStyleTable

    @beginEventTable{wxSashEvent}
    @event{EVT_SASH_DRAGGED(id, func)}
           Process a wxEVT_SASH_DRAGGED event, when the user has finished
           dragging a sash.
    @event{EVT_SASH_DRAGGED_RANGE(id1, id2, func)}
           Process a wxEVT_SASH_DRAGGED_RANGE event, when the user has
           finished dragging a sash. The event handler is called when windows
           with ids in the given range have their sashes dragged.
    @endEventTable

    @library{wxadv}
    @category{miscwnd}

    @see wxSashEvent, wxSashLayoutWindow, @ref overview_eventhandling
*/
class wxSashWindow : public wxWindow
{
public:
    //@{
    /**
        Constructs a sash window, which can be a child of a frame, dialog or any other
        non-control window.

        @param parent
            Pointer to a parent window.
        @param id
            Window identifier. If -1, will automatically create an identifier.
        @param pos
            Window position. wxDefaultPosition is (-1, -1) which indicates that
        wxSashWindows
            should generate a default position for the window. If using the
        wxSashWindow class directly, supply
            an actual position.
        @param size
            Window size. wxDefaultSize is (-1, -1) which indicates that wxSashWindows
            should generate a default size for the window.
        @param style
            Window style. For window styles, please see wxSashWindow.
        @param name
            Window name.
    */
    wxSashWindow();
    wxSashWindow(wxWindow* parent, wxWindowID id,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxCLIP_CHILDREN | wxSW_3D,
                 const wxString& name = "sashWindow");
    //@}

    /**
        Destructor.
    */
    virtual ~wxSashWindow();

    /**
        Gets the maximum window size in the x direction.
    */
    virtual int GetMaximumSizeX() const;

    /**
        Gets the maximum window size in the y direction.
    */
    virtual int GetMaximumSizeY() const;

    /**
        Gets the minimum window size in the x direction.
    */
    virtual int GetMinimumSizeX() const;

    /**
        Gets the minimum window size in the y direction.
    */
    virtual int GetMinimumSizeY() const;

    /**
        Returns @true if a sash is visible on the given edge, @false otherwise.

        @param edge
            Edge. One of wxSASH_TOP, wxSASH_RIGHT, wxSASH_BOTTOM, wxSASH_LEFT.

        @see SetSashVisible()
    */
    bool GetSashVisible(wxSashEdgePosition edge) const;

    /**
        Returns @true if the sash has a border, @false otherwise.
        This function is obsolete since the sash border property is unused.

        @param edge
            Edge. One of wxSASH_TOP, wxSASH_RIGHT, wxSASH_BOTTOM, wxSASH_LEFT.

        @see SetSashBorder()
    */
    bool HasBorder(wxSashEdgePosition edge) const;

    /**
        Sets the maximum window size in the x direction.
    */
    virtual void SetMaximumSizeX(int min);

    /**
        Sets the maximum window size in the y direction.
    */
    virtual void SetMaximumSizeY(int min);

    /**
        Sets the minimum window size in the x direction.
    */
    virtual void SetMinimumSizeX(int min);

    /**
        Sets the minimum window size in the y direction.
    */
    virtual void SetMinimumSizeY(int min);

    /**
        Call this function to give the sash a border, or remove the border.
        This function is obsolete since the sash border property is unused.

        @param edge
            Edge to change. One of wxSASH_TOP, wxSASH_RIGHT, wxSASH_BOTTOM, wxSASH_LEFT.
        @param hasBorder
            @true to give the sash a border visible, @false to remove it.
    */
    void SetSashBorder(wxSashEdgePosition edge, bool hasBorder);

    /**
        Call this function to make a sash visible or invisible on a particular edge.

        @param edge
            Edge to change. One of wxSASH_TOP, wxSASH_RIGHT, wxSASH_BOTTOM, wxSASH_LEFT.
        @param visible
            @true to make the sash visible, @false to make it invisible.

        @see GetSashVisible()
    */
    void SetSashVisible(wxSashEdgePosition edge, bool visible);
};



/**
    @class wxSashEvent

    A sash event is sent when the sash of a wxSashWindow has been
    dragged by the user.

    @library{wxadv}
    @category{FIXME}

    @see wxSashWindow, @ref overview_eventhandlingoverview
*/
class wxSashEvent : public wxCommandEvent
{
public:
    /**
        Constructor.
    */
    wxSashEvent(int id = 0, wxSashEdgePosition edge = wxSASH_NONE);

    /**
        Returns the rectangle representing the new size the window would be if the
        resize was applied. It is
        up to the application to set the window size if required.
    */
    wxRect GetDragRect() const;

    /**
        Returns the status of the sash: one of wxSASH_STATUS_OK,
        wxSASH_STATUS_OUT_OF_RANGE.
        If the drag caused the notional bounding box of the window to flip over, for
        example, the drag will be out of rage.
    */
    wxSashDragStatus GetDragStatus() const;

    /**
        Returns the dragged edge. The return value is one of wxSASH_TOP, wxSASH_RIGHT,
        wxSASH_BOTTOM, wxSASH_LEFT.
    */
    wxSashEdgePosition GetEdge() const;
};

