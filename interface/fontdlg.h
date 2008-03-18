/////////////////////////////////////////////////////////////////////////////
// Name:        fontdlg.h
// Purpose:     interface of wxFontDialog
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxFontDialog
    @wxheader{fontdlg.h}

    This class represents the font chooser dialog.

    @library{wxcore}
    @category{cmndlg}

    @see Overview(), wxFontData, wxGetFontFromUser()
*/
class wxFontDialog : public wxDialog
{
public:
    //@{
    /**
        Constructor. Pass a parent window, and optionally the
        @ref overview_wxfontdata "font data" object to be used to initialize the dialog
        controls. If the default constructor is used,
        Create() must be called before the dialog can be
        shown.
    */
    wxFontDialog();
    wxFontDialog(wxWindow* parent);
    wxFontDialog(wxWindow* parent, const wxFontData& data);
    //@}

    //@{
    /**
        Creates the dialog if it the wxFontDialog object had been initialized using the
        default constructor. Returns @true on success and @false if an error
        occurred.
    */
    bool Create(wxWindow* parent);
    bool Create(wxWindow* parent, const wxFontData& data);
    //@}

    //@{
    /**
        Returns the @ref overview_wxfontdata "font data" associated with the font
        dialog.
    */
    const wxFontData GetFontData();
    const wxFontData&  GetFontData();
    //@}

    /**
        Shows the dialog, returning @c wxID_OK if the user pressed Ok, and
        @c wxID_CANCEL otherwise.
        If the user cancels the dialog (ShowModal returns @c wxID_CANCEL), no font
        will be created. If the user presses OK, a new wxFont will be created and
        stored in the font dialog's wxFontData structure.
    */
    int ShowModal();
};



// ============================================================================
// Global functions/macros
// ============================================================================

/** @ingroup group_funcmacro_dialog */
//@{

/**
    Shows the font selection dialog and returns the font selected by user or
    invalid font (use wxFont::IsOk() to test whether a font is valid) if the
    dialog was cancelled.

    @param parent
        The parent window for the font selection dialog.
    @param fontInit
        If given, this will be the font initially selected in the dialog.
    @param caption
        If given, this will be used for the dialog caption.

    @header{wx/fontdlg.h}
*/
wxFont wxGetFontFromUser(wxWindow* parent,
                         const wxFont& fontInit,
                         const wxString& caption = wxEmptyString);

//@}

