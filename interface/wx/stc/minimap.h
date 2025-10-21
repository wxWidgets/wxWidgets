///////////////////////////////////////////////////////////////////////////////
// Name:        wx/stc/minimap.h
// Purpose:     Documentation of wxStyledTextCtrlMiniMap class.
// Author:      Vadim Zeitlin
// Created:     2025-10-17
// Copyright:   (c) 2025 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/**
    @class wxStyledTextCtrlMiniMap

    A mini map control to be used together with wxStyledTextCtrl.

    A mini map is a narrow window placed along a side of a text control and
    showing a small version of the text in the editor, giving a quick overview
    of the document, showing the current position in it and allowing to quickly
    navigate to another part of it.

    Objects of this class must be associated with a wxStyledTextCtrl instance
    (the "editor") which they will show a mini map for. This association is
    done in the constructor or Create() method and may be modified later using
    SetEdit(). If the map doesn't have any associated editor, it doesn't show
    anything and is effectively useless.

    This class doesn't generate any events and, more generally, the only thing
    the application needs to do is to create it and position it properly. While
    mini map window may be positioned arbitrarily, it is customarily placed on
    the right side of the editor, see @ref page_samples_stc for an example of
    doing this.

    Note that you may want to disable the scrollbar of the editor when using a
    mini map with it, as the mini map itself provides a scrollbar-like
    functionality. This can be done simply by calling
    `SetUseVerticalScrollBar(false)` on the editor. If you don't do this, you
    may also want to enable the maps own scrollbar, which is not shown by
    default to avoid cluttering the display with multiple scrollbars, by
    calling `SetUseVerticalScrollBar(true)` on the mini map itself.

    @library{wxstc}
    @category{stc}

    @see wxStyledTextCtrl

    @since 3.3.2
*/
class wxStyledTextCtrlMiniMap : public wxStyledTextCtrl
{
public:
    /**
        Default constructor, use Create() later.
    */
    wxStyledTextCtrlMiniMap();

    /**
        Constructor really creating the minimap window.

        @param parent
            The parent window.
        @param edit
            The associated main document control which must be valid.
     */
    wxStyledTextCtrlMiniMap(wxWindow* parent, wxStyledTextCtrl* edit);

    /**
        Create the minimap with the given parent window and associated with the
        given editor.

        This function should be called only if the object was created using the
        default constructor.

        @param parent
            The parent window.
        @param edit
            The associated main document control which should typically be
            valid but may be @NULL if the map shouldn't show anything
            initially.

        @return @true on success, @false in case of catastrophic failure.
    */
    bool Create(wxWindow* parent, wxStyledTextCtrl* edit);

    /**
        Set the associated main document control.

        @param edit
            The associated main document control which may be @NULL to reset
            the editor.
     */
    void SetEdit(wxStyledTextCtrl* edit);

    /**
        Set the colours used to draw the visible zone indicator.

        Note that alpha components of the colours are also used here and should
        typically be non-zero to make the thumb semi-transparent.

        If any of the parameters is an invalid colour, the corresponding colour
        is left unchanged.

        @param colNormal
            Colour used to draw the thumb in normal state.
        @param colDragging
            Colour used to draw the thumb while it is being dragged.
     */
    void SetThumbColours(const wxColour& colNormal, const wxColour& colDragging);
};
