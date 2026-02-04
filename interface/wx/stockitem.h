/////////////////////////////////////////////////////////////////////////////
// Name:        stockitem.h
// Purpose:     interface of global functions
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    Possible values for flags parameter of wxGetStockLabel().

    The elements of this enum are bit masks and may be combined with each other
    (except when specified otherwise).
 */
enum wxStockLabelQueryFlag
{
    /**
        Indicates absence of wxSTOCK_WITH_MNEMONIC and wxSTOCK_WITH_ACCELERATOR.

        Requests just the label (e.g. "Print...").
     */
    wxSTOCK_NOFLAGS = 0,

    /**
        Request the label with mnemonics character.

        E.g. "&Print...".
     */
    wxSTOCK_WITH_MNEMONIC = 1,

    /**
        Return the label with accelerator following it after TAB.

        E.g. "Print...\tCtrl-P". This can be combined with
        wxSTOCK_WITH_MNEMONIC to get "&Print...\tCtrl-P".
     */
    wxSTOCK_WITH_ACCELERATOR = 2,

    /**
        Return the label without any ellipsis at the end.

        By default, stock items text is returned with ellipsis, if appropriate,
        this flag allows one to avoid having it. So using the same example as
        above, the returned string would be "Print" or "&Print" if
        wxSTOCK_WITH_MNEMONIC were also used.

        This flag can't be combined with wxSTOCK_WITH_ACCELERATOR.

        @since 2.9.1
     */
    wxSTOCK_WITHOUT_ELLIPSIS = 4,

    /**
        Return the label appropriate for a button and not a menu item.

        Currently the main difference is that the trailing ellipsis used in
        some stock labels is never included in the returned label. Also, the
        mnemonics is included if this flag is used. So the returned value for
        wxID_PRINT when this flag is used is "&Print".

        This flag can't be combined with wxSTOCK_WITH_ACCELERATOR.

        @since 2.9.1
     */
    wxSTOCK_FOR_BUTTON = wxSTOCK_WITHOUT_ELLIPSIS | wxSTOCK_WITH_MNEMONIC
};

/** @addtogroup group_funcmacro_misc */
///@{

/**
    Returns label that should be used for given @a id element.

    @param id
        Given id of the wxMenuItem, wxButton, wxToolBar tool, etc.
    @param flags
        Combination of the elements of wxStockLabelQueryFlag.

    @header{wx/stockitem.h}
*/
wxString wxGetStockLabel(wxWindowID id, long flags = wxSTOCK_WITH_MNEMONIC);

///@}

/** @addtogroup group_funcmacro_misc */
///@{

/**
    Return help string associated with given stock @a id and @a client.

    This function uses the default help string shown in the status bar when the
    item with the given @a id is selected in a menu, e.g. for ::wxID_EXIT the
    returned string is "Quit this program".

    @param id
        The stock item identifier to get the help string for.
    @param client
        The client context for which the help string is requested, currently
        can only be `wxSTOCK_MENU`, which is the default value for this
        parameter.
    @return
        The translated help string for the given stock item identifier and
        client context, or an empty string if no default help string is
        available for this @a id.

    @header{wx/stockitem.h}
 */
wxString wxGetStockHelpString(wxWindowID id,
                              wxStockHelpStringClient client = wxSTOCK_MENU);

///@}
