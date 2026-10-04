///////////////////////////////////////////////////////////////////////////////
// Name:        ribbon/backstagecontrols.h
// Purpose:     interface of the wxBackstage page controls
// Author:      Blake Madden
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/**
    The layout of a wxBackstageButton.

    @since 3.3.4
*/
enum class wxBackstageButtonStyle
{
    /// A framed tile with the icon above its label (the default).
    wxBackstageButtonTile,

    /// The icon on the left, with a title and description to its right.
    wxBackstageButtonWide,

    /// A large framed thumbnail with a caption (and description) below it.
    wxBackstageButtonCard
};

/**
    How a wxBackstageHeading is displayed.

    @since 3.3.4
*/
enum class wxBackstageHeadingStyle
{
    /// A large page title (e.g., "Save As" (the default)).
    wxBackstageHeadingTitle,

    /// A smaller, bold section heading.
    wxBackstageHeadingSection
};

/**
    @class wxBackstageButton

    A large, icon-based button for a wxBackstagePage.

    @beginEventEmissionTable{wxCommandEvent}
    @event{EVT_BUTTON(id, func)}
        Triggered when the button is clicked or activated with the keyboard.
    @endEventTable

    @library{wxribbon}
    @category{ribbon}

    @since 3.3.4
*/
class wxBackstageButton : public wxControl
{
public:
    /**
        Constructor.

        @param parent
            The parent window (usually a wxBackstagePage).
        @param id
            The window ID.
        @param label
            The button's label (or title).
        @param icon
            The button's icon.
        @param style
            The layout of the button.
        @param description
            The text below or beside the title (for the wide and card styles).
    */
    wxBackstageButton(wxWindow* parent, wxWindowID id, const wxString& label,
                      const wxBitmapBundle& icon = wxBitmapBundle(),
                      wxBackstageButtonStyle style = wxBackstageButtonStyle::wxBackstageButtonTile,
                      wxString description = wxString());

    /**
        Sets the description.
    */
    void SetDescription(const wxString& description);

    /**
        Returns the description.
    */
    const wxString& GetDescription() const;

    /**
        Sets the icon.
    */
    void SetIcon(const wxBitmapBundle& icon);

    /**
        Sets the size to draw the icon at, in DIPs.

        The default is 32x32, or 180x110 for the card style.
    */
    void SetIconSize(const wxSize& size);

    /**
        Shows a drop-down arrow on the button (visual effect only).
    */
    void ShowDropDownArrow(bool show = true);

    /**
        Gives the button a highlighted "callout" background in this colour.
    */
    void SetCalloutColour(const wxColour& colour);
};

/**
    @class wxBackstageHeading

    A heading for a wxBackstagePage, either a page title or a section heading.

    @library{wxribbon}
    @category{ribbon}

    @since 3.3.4
*/
class wxBackstageHeading : public wxControl
{
public:
    /**
        Constructor.

        @param parent
            The parent window (usually a wxBackstagePage).
        @param id
            The window ID.
        @param label
            The heading's text.
        @param style
            Whether this is a page title or a section heading.
    */
    wxBackstageHeading(wxWindow* parent, wxWindowID id, const wxString& label,
                       wxBackstageHeadingStyle style = wxBackstageHeadingStyle::wxBackstageHeadingTitle);
};

/**
    @class wxBackstageCallout

    A highlighted banner for a wxBackstagePage (e.g., "Checked Out Document"),
    with an optional tile button on the left and an action button below the
    message.

    @library{wxribbon}
    @category{ribbon}

    @since 3.3.4
*/
class wxBackstageCallout : public wxPanel
{
public:
    /**
        Constructor.

        @param parent
            The parent window (usually a wxBackstagePage).
        @param id
            The window ID.
        @param title
            The banner's title.
        @param message
            The text below the title.
    */
    wxBackstageCallout(wxWindow* parent, wxWindowID id, const wxString& title,
                       const wxString& message = wxString());

    /**
        Sets the button shown on the left.

        The button should be a child of the callout.
    */
    void SetTile(wxBackstageButton* tile);

    /**
        Sets the button shown below the message.

        The button should be a child of the callout.
    */
    void SetAction(wxBackstageButton* action);
};

/**
    @class wxBackstageItemList

    A scrolling list of items, with optional group headers, for a
    wxBackstagePage.

    Each item has an icon, a title, a subtitle, and right-aligned text.

    @beginEventEmissionTable{wxCommandEvent}
    @event{EVT_BACKSTAGE_ITEM_CLICKED(id, func)}
        Triggered when an item is clicked or activated. wxCommandEvent::GetInt()
        returns the item's index (not counting headers) and
        wxCommandEvent::GetString() returns the item's user string (e.g., a
        file path).
    @endEventTable

    @library{wxribbon}
    @category{ribbon}

    @since 3.3.4
*/
class wxBackstageItemList : public wxScrolledCanvas
{
public:
    /**
        Constructor.
    */
    explicit wxBackstageItemList(wxWindow* parent, wxWindowID id = wxID_ANY);

    /**
        Adds a group header.
    */
    void AddHeader(const wxString& text);

    /**
        Adds an item.

        @param title
            The item's main text.
        @param subtitle
            The text below the title.
        @param rightText
            The right-aligned text (e.g., a date).
        @param icon
            The item's icon.
        @param userString
            A string returned with the item's click event.
        @return The item's index (not counting headers).
    */
    size_t AddItem(const wxString& title, const wxString& subtitle = wxString(),
                   const wxString& rightText = wxString(),
                   const wxBitmapBundle& icon = wxBitmapBundle(),
                   const wxString& userString = wxString());

    /**
        Removes all items and headers.
    */
    void Clear();

    /**
        Returns the number of items (not counting headers).
    */
    size_t GetItemCount() const;

    /**
        Returns the user string of an item.
    */
    wxString GetItemString(size_t index) const;

    /**
        Sets the text to show when the list is empty.
    */
    void SetEmptyText(const wxString& text);
};

/**
    @class wxBackstageMRUList

    A most-recently-used (MRU) file list for a wxBackstagePage.

    Provide file paths (or paths with modified dates) and it:
    - sorts them
    - shows human-readable modified dates next to them (e.g., "Just now", "12 minutes
      ago", "Yesterday at 4:15 PM", or "Tue at 1:07 PM")
    - shows a readable folder path beneath each file (e.g., "Documents » Invoices")
    - and divides the files into sections deduced from those dates ("Today", "Yesterday",
      "This Week", "Last Week", ...).

    It sends the same event as wxBackstageItemList, with the file's path as the
    event's string.

    @library{wxribbon}
    @category{ribbon}

    @since 3.3.4
*/
class wxBackstageMRUList : public wxBackstageItemList
{
public:
    /**
        Constructor.
    */
    explicit wxBackstageMRUList(wxWindow* parent, wxWindowID id = wxID_ANY);

    /**
        Sets the files to show, reading their modified dates from disk.

        Files that can't be found are not shown, though they remain in the
        application's own MRU list.
    */
    void SetFiles(const wxArrayString& paths);

    /**
        Adds a file with a modified date that you provide, instead of reading it
        from disk. The file doesn't need to exist.
    */
    void AddFile(const wxString& path, const wxDateTime& modified);

    /**
        Removes all files.
    */
    void ClearFiles();

    /**
        Returns the number of files.
    */
    size_t GetFileCount() const;

    /**
        Sets a function that returns the icon to show for a file.

        By default, a standard file icon is used for all files.
    */
    void SetIconProvider(std::function<wxBitmapBundle(const wxString&)> provider);

    /**
        Shows or hides the section headers ("Today", "Yesterday", ...).
    */
    void ShowSectionHeaders(bool show = true);

    /**
        Sets the maximum number of files shown.

        The most recently modified files are the ones kept. The default is 10.
        Pass @c UNLIMITED_MRU_LIST to show all files.
    */
    void SetMaxFiles(size_t maxFiles);

    /**
        Pass to SetMaxFiles() to show all files.
    */
    static constexpr size_t UNLIMITED_MRU_LIST = static_cast<size_t>(-1);

    /**
        Formats a modified date as text such as "Just now", "12 minutes ago",
        "1:07 PM", "Yesterday at 4:15 PM", "Tue at 1:07 PM", "Mar 3", or
        "3/3/2023".

        @param modified
            The date to format.
        @param now
            The current time (only useful for testing).
    */
    static wxString FormatModifiedDate(const wxDateTime& modified,
                                       const wxDateTime& now = wxDateTime::Now());
};

wxEventType wxEVT_BACKSTAGE_ITEM_CLICKED;
