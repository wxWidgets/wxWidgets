///////////////////////////////////////////////////////////////////////////////
// Name:        ribbon/tooltip.h
// Purpose:     interface of wxRibbonToolTipInfo
// Author:      Blake Madden
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/**
    @class wxRibbonToolTipInfo

    Contents of the rich tooltip of a wxRibbonButtonBar button,
    wxRibbonToolBar tool, wxRibbonGallery item, or wxRibbonPanel extension
    button.

    The tooltip is shown below the ribbon and stays open while the mouse is
    over the item or the tooltip. It shows a bold title, a description,
    an optional image to the left of the description and an optional help link.
    Clicking the link sends the control's help event (e.g.,
    @c wxEVT_RIBBONBUTTONBAR_HELP_CLICKED).

    The setters return a reference to this object, so they can be chained:
    @code
    bar->SetButtonToolTip(ID_TABLE,
        wxRibbonToolTipInfo("Table")
            .Description("Insert a table into the document.")
            .Image(tableBundle)
            .HelpLink());
    @endcode

    @library{wxribbon}
    @category{ribbon}

    @see wxRibbonButtonBar::SetButtonToolTip(), wxRibbonToolBar::SetToolToolTip(),
        wxRibbonGallery::SetItemToolTip(), wxRibbonPanel::SetExtButtonToolTip()

    @since 3.3.5
*/
class wxRibbonToolTipInfo
{
public:
    /// Creates the tooltip contents with the given title.
    explicit wxRibbonToolTipInfo(const wxString& title = wxString());

    /// Sets the bold title.
    wxRibbonToolTipInfo& Title(const wxString& title);

    /**
        Sets the description, which may contain several lines.

        If empty, the item's help string is used instead.
    */
    wxRibbonToolTipInfo& Description(const wxString& description);

    /**
        Sets the description shown instead when the item is disabled
        (e.g., to explain why).
    */
    wxRibbonToolTipInfo& DisabledDescription(const wxString& description);

    /// Sets the image shown to the left of the description.
    wxRibbonToolTipInfo& Image(const wxBitmapBundle& image);

    /**
        Shows, or hides, the help link below a divider.

        The link is not shown if @c wxUSE_HYPERLINKCTRL is 0.
    */
    wxRibbonToolTipInfo& HelpLink(bool show = true);

    /// Sets the text of the help link, "Click here for more help" by default.
    wxRibbonToolTipInfo& HelpLinkText(const wxString& text);

    const wxString& GetTitle() const;
    const wxString& GetDescription() const;
    const wxString& GetDisabledDescription() const;
    const wxBitmapBundle& GetImage() const;
    bool HasHelpLink() const;

    /// Returns the help link text, or the default text if none was set.
    wxString GetHelpLinkText() const;

    /// Returns true if nothing was set.
    bool IsEmpty() const;
};
