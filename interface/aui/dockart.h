/////////////////////////////////////////////////////////////////////////////
// Name:        aui/dockart.h
// Purpose:     interface of wxAuiDockArt
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxAuiDockArt
    @headerfile dockart.h wx/aui/dockart.h

    wxAuiDockArt is part of the wxAUI class framework.
    See also @ref overview_wxauioverview.

    Dock art provider code - a dock provider provides all drawing
    functionality to the wxAui dock manager. This allows the dock
    manager to have a plugable look-and-feel.

    By default, a wxAuiManager uses an
    instance of this class called @b wxAuiDefaultDockArt which
    provides bitmap art and a colour scheme that is adapted to
    the major platforms' look. You can either derive from that
    class to alter its behaviour or write a completely new dock
    art class. Call wxAuiManager::SetArtProvider
    to make use this new dock art.

    @library{wxaui}
    @category{aui}

    @see wxAuiManager, wxAuiPaneInfo
*/
class wxAuiDockArt
{
public:
    /**
        Constructor.
    */
    wxAuiDockArt();

    /**
        Destructor.
    */
    ~wxAuiDockArt();

    /**
        Draws a background.
    */
    virtual void DrawBackground(wxDC& dc, wxWindow* window,
                                int orientation,
                                const wxRect& rect);

    /**
        Draws a border.
    */
    virtual void DrawBorder(wxDC& dc, wxWindow* window,
                            const wxRect& rect,
                            wxAuiPaneInfo& pane);

    /**
        Draws a caption.
    */
    virtual void DrawCaption(wxDC& dc, wxWindow* window,
                             const wxString& text,
                             const wxRect& rect,
                             wxAuiPaneInfo& pane);

    /**
        Draws a gripper.
    */
    virtual void DrawGripper(wxDC& dc, wxWindow* window,
                             const wxRect& rect,
                             wxAuiPaneInfo& pane);

    /**
        Draws a button in the pane's title bar.
        @a button can be one of the values of @b wxAuiButtonId.
        @a button_state can be one of the values of @b wxAuiPaneButtonState.
    */
    virtual void DrawPaneButton(wxDC& dc, wxWindow* window,
                                int button,
                                int button_state,
                                const wxRect& rect,
                                wxAuiPaneInfo& pane);

    /**
        Draws a sash between two windows.
    */
    virtual void DrawSash(wxDC& dc, wxWindow* window,
                          int orientation,
                          const wxRect& rect);

    /**
        The same as GetColour().
    */
    virtual wxColour GetColor(int id);

    /**
        Get the colour of a certain setting.
        @a id can be one of the colour values of @b wxAuiPaneDockArtSetting.
    */
    virtual wxColour GetColour(int id);

    /**
        Get a font setting.
    */
    virtual wxFont GetFont(int id);

    /**
        Get the value of a certain setting.
        @a id can be one of the size values of @b wxAuiPaneDockArtSetting.
    */
    virtual int GetMetric(int id);

    /**
        The same as SetColour().
    */
    virtual void SetColor(int id, const wxColour& color);

    /**
        Set a certain setting with the value @e colour.
        @a id can be one of the colour values of @b wxAuiPaneDockArtSetting.
    */
    virtual void SetColour(int id, const wxColor& colour);

    /**
        Set a font setting.
    */
    virtual void SetFont(int id, const wxFont& font);

    /**
        Set a certain setting with the value @e new_val.
        @a id can be one of the size values of @b wxAuiPaneDockArtSetting.
    */
    virtual void SetMetric(int id, int new_val);
};

