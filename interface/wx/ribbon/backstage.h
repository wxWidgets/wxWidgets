///////////////////////////////////////////////////////////////////////////////
// Name:        ribbon/backstage.h
// Purpose:     interface of wxBackstage
// Author:      Blake Madden
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/**
    How the highlighted button in a wxBackstage is drawn.

    @since 3.3.4
*/
enum class wxBackstageHighlightStyle
{
    /// A solid fill (the default).
    wxBackstageHighlightFlat,

    /// A glossy fill.
    wxBackstageHighlightGlossy
};

/**
    @class wxBackstagePage

    A scrollable page on the right side of a wxBackstage.

    Pages are real windows, so lay out their controls with sizers.
    They only scroll vertically, so their content should wrap or shrink to fit
    horizontally. (Create them with wxBackstage::AddPage().)

    @library{wxribbon}
    @category{ribbon}

    @since 3.3.4
*/
class wxBackstagePage : public wxScrolledWindow
{
};

/**
    @class wxBackstage

    A backstage view, as shown by a ribbon's "File" tab.

    The control has two areas: buttons on the left (which are defined once) and
    pages on the right (which are connected to the buttons by ID). The buttons
    can be disabled, and there can be separators and a flexible space. For example,
    buttons added after it are anchored to the bottom, such as "Account" and
    "Options".

    Every button fires a @c wxEVT_BACKSTAGE_CLICKED event carrying the
    button's ID. If the button has a page, then the page is shown afterwards
    (unless the handler vetoes the event). Buttons without a page are simply
    actions (e.g., "Exit"). Buttons on the pages fire regular @c wxEVT_BUTTON
    events with their own IDs.

    The backstage lives in the frame's client area, in the same spot as the
    application's main content, and never closes itself. Use
    wxRibbonBar::SetBackstage() to connect it to a ribbon bar. The controls in
    wxBackstageButton, wxBackstageHeading, wxBackstageCallout,
    wxBackstageItemList and wxBackstageMRUList are meant for building its
    pages.

    @beginEventEmissionTable{wxNotifyEvent}
    @event{EVT_BACKSTAGE_CLICKED(id, func)}
        Triggered when a button on the left is clicked or activated.
        The event's ID is the button's ID. Call wxNotifyEvent::Veto() to keep the
        button's page from being shown.
    @endEventTable

    @library{wxribbon}
    @category{ribbon}

    @see wxRibbonBar::SetBackstage()

    @since 3.3.4
*/
class wxBackstage : public wxWindow
{
public:
    /**
        Constructor.

        @param parent
            The parent window, which should be the same as the ribbon bar's.
        @param id
            The window ID.
    */
    explicit wxBackstage(wxWindow* parent, wxWindowID id = wxID_ANY);

    /**
        @name Buttons
    */
    ///@{

    /**
        Adds a button to the left side.

        @param id
            The button's ID, which also connects it to a page. It must be
            unique and not @c wxID_ANY.
        @param label
            The button's label.
        @param icon
            An optional icon.
        @return @true if the button was added, @false if the ID was invalid
            or already in use.
    */
    bool AddButton(wxWindowID id, const wxString& label,
                   const wxBitmapBundle& icon = wxBitmapBundle());

    /**
        Adds a separator line to the left side.
    */
    void AddSeparator();

    /**
        Anchors the buttons added after this to the bottom of the left side.

        Only the first call has an effect.
    */
    void AddFlexibleSpace();

    /**
        Enables or disables a button.
    */
    void EnableButton(wxWindowID id, bool enable = true);

    /**
        Returns @true if the button is enabled.
    */
    bool IsButtonEnabled(wxWindowID id) const;

    /**
        Changes a button's label.
    */
    void SetButtonLabel(wxWindowID id, const wxString& label);

    ///@}

    /**
        @name Pages
    */
    ///@{

    /**
        Creates a page and connects it to a button.

        @param buttonId
            The ID of the button that shows the page.
        @return The new page, to which controls can be added.
    */
    wxBackstagePage* AddPage(wxWindowID buttonId);

    /**
        Connects a window that you created to a button.

        The window is moved into the backstage's page area, which owns it.

        @return @true if the page was set. This fails if the button wasn't
            found or already has a page.
    */
    bool SetPage(wxWindowID buttonId, wxWindow* page);

    /**
        Removes the page connected to a button.

        @return @true if the button had a page.
    */
    bool RemovePage(wxWindowID buttonId);

    /**
        Returns the page connected to a button, or @NULL if there is none.
    */
    wxWindow* GetPage(wxWindowID buttonId) const;

    /**
        Shows the page connected to a button, and highlights the button.

        @return @true if the page was shown.
    */
    bool ShowPage(wxWindowID buttonId);

    /**
        Returns the ID of the button whose page is shown, or @c wxNOT_FOUND if
        there is none.
    */
    wxWindowID GetCurrentPageId() const;

    ///@}

    /**
        @name Appearance
    */
    ///@{

    /**
        Returns the background colour of the left side.

        If none was set, a theme-aware default is returned.
    */
    wxColour GetNavBackgroundColour() const;

    /**
        Sets the background colour of the left side.

        An invalid colour returns to the default. When used by a ribbon bar,
        this is set from the ribbon bar's art provider.
    */
    void SetNavBackgroundColour(const wxColour& colour);

    /**
        Returns the colour of the highlighted button, which is invalid if it is
        derived from the left side's background colour.
    */
    const wxColour& GetHighlightColour() const;

    /**
        Sets the colour of the highlighted button.

        An invalid colour derives it from the left side's background colour.
    */
    void SetHighlightColour(const wxColour& colour);

    /**
        Returns how the highlighted button is drawn.
    */
    wxBackstageHighlightStyle GetHighlightStyle() const;

    /**
        Sets how the highlighted button is drawn.
    */
    void SetHighlightStyle(wxBackstageHighlightStyle style);

    /**
        Returns the background colour of the pages.

        If none was set, a theme-aware default is returned.
    */
    wxColour GetPageBackgroundColour() const;

    /**
        Sets the background colour of the pages.

        An invalid colour returns to the default. Controls on the pages that
        use the old colours are moved to the new colours.
    */
    void SetPageBackgroundColour(const wxColour& colour);

    /**
        Returns the colour to draw text with on the pages (black or white,
        depending on the page's background colour).
    */
    wxColour GetPageForegroundColour() const;

    /**
        Keeps a control on a page from following the page's colours, so
        that its own colours are left alone.
    */
    void KeepWindowColours(wxWindow* window);

    ///@}

    /**
        @name Helpers

        Static functions for controls that are drawn on a page.
    */
    ///@{

    /**
        Returns @true if the colour is dark.
    */
    static bool IsDark(const wxColour& colour);

    /**
        Returns a darker version of a light colour, or a lighter version of a
        dark one.

        @param colour
            The colour to shade or tint.
        @param shadeOrTintValue
            How much to change it, from 0 to 1.
    */
    static wxColour ShadeOrTint(const wxColour& colour, double shadeOrTintValue = 0.2);

    /**
        Returns black for a light colour or white for a dark one.
    */
    static wxColour BlackOrWhiteContrast(const wxColour& colour);

    /**
        Mixes two colours.

        @param from
            The colour to start from.
        @param to
            The colour to mix in.
        @param amount
            How much of @a to to use, from 0 to 1.
    */
    static wxColour Blend(const wxColour& from, const wxColour& to, double amount);

    /**
        Returns the backstage that a window is on, or @NULL if there is none.
    */
    static const wxBackstage* FindBackstage(const wxWindow* window);

    /**
        Gets the colours that a window on a page should be drawn with.

        If the window isn't on a backstage, then system colours are returned.
    */
    static void GetPageColours(const wxWindow* window, wxColour& background,
                              wxColour& foreground);

    /**
        Draws a bitmap, scaled to fit in a rectangle while keeping its aspect ratio.
    */
    static void DrawBitmapFit(wxDC& dc, const wxWindow* window,
                              const wxBitmapBundle& bundle, const wxRect& rect);

    /**
        Draws a rectangle with a glossy fill.
    */
    static void DrawGlossyRect(wxDC& dc, const wxRect& rect, const wxColour& colour);

    /**
        Draws a keyboard focus rectangle.
    */
    static void DrawFocusRect(wxDC& dc, const wxRect& rect, const wxColour& colour);

    /**
        Returns the size of text (which can be multi-line) in a font.
    */
    static wxSize MeasureText(const wxWindow* window, const wxString& text,
                              const wxFont& font);

    /**
        Skips a key event if it has modifiers (such as Alt+Space or Ctrl+Tab),
        leaving it to the system.

        @return @true if the event was skipped.
    */
    static bool SkipIfShortcutKey(wxKeyEvent& event);

    /**
        Returns @true if the key is Space, Enter, or numpad Enter.
    */
    static bool IsActivateKey(int keyCode);

    ///@}
};

wxEventType wxEVT_BACKSTAGE_CLICKED;
