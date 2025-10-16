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
    done in the constructor or Create() method.

    This class doesn't generate any events and, more generally, the only thing
    the application needs to do is to create it and position it properly. While
    mini map window may be positioned arbitrarily, it is customarily placed on
    the right side of the editor, see @ref page_samples_stc for an example of
    doing this.

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
            The associated main document control which must be valid.

        @return @true on success, @false in case of catastrophic failure.
    */
    bool Create(wxWindow* parent, wxStyledTextCtrl* edit);
};
