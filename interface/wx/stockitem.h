/////////////////////////////////////////////////////////////////////////////
// Name:        stockitem.h
// Purpose:     interface of global functions
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/** @addtogroup group_funcmacro_misc */
//@{

/**
    Returns label that should be used for given @a id element.

    @param id
        Given id of the wxMenuItem, wxButton, wxToolBar tool, etc.
    @param withCodes
        If @false then strip accelerator code from the label; useful for
        getting labels without accelerator char code like for toolbar tooltip
        or on platforms without traditional keyboard like smartphones.
    @param accelerator
        Optional accelerator string automatically added to label; useful for
        building labels for wxMenuItem.

    @header{wx/stockitem.h}
*/
wxString wxGetStockLabel(wxWindowID id, bool withCodes = true,
                         const wxString& accelerator = wxEmptyString);

//@}

