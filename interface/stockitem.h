/////////////////////////////////////////////////////////////////////////////
// Name:        stockitem.h
// Purpose:     documentation for global functions
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
Returns label that should be used for given @a id element.

@param id
    given id of the wxMenuItem, wxButton, wxToolBar tool, etc.
@param withCodes
    if @false then strip accelerator code from the label;
    useful for getting labels without accelerator char code like for toolbar
tooltip or
    on platforms without traditional keyboard like smartphones
@param accelerator
    optional accelerator string automatically added to label; useful
    for building labels for wxMenuItem
*/
wxString wxGetStockLabel(wxWindowID id, bool withCodes = true,
                         const wxString& accelerator = wxEmptyString);


