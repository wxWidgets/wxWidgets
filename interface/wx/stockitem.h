/////////////////////////////////////////////////////////////////////////////
// Name:        stockitem.h
// Purpose:     interface of global functions
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
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
       
        E.g. "&amp;Print...".
     */
    wxSTOCK_WITH_MNEMONIC = 1,

    /**
        Return the label with accelerator following it after TAB.

        E.g. "Print...\tCtrl-P". This can be combined with
        wxSTOCK_WITH_MNEMONIC to get "&Print...\tCtrl-P".
     */
    wxSTOCK_WITH_ACCELERATOR = 2
};

/** @addtogroup group_funcmacro_misc */
//@{

/**
    Returns label that should be used for given @a id element.

    @param id
        Given id of the wxMenuItem, wxButton, wxToolBar tool, etc.
    @param flags
        Combination of the elements of wxStockLabelQueryFlag.

    @header{wx/stockitem.h}
*/
wxString wxGetStockLabel(wxWindowID id, long flags = wxSTOCK_WITH_MNEMONIC);

//@}

