///////////////////////////////////////////////////////////////////////////////
// Name:        wx/radiobox.h
// Purpose:     wxRadioBox declaration
// Author:      Vadim Zeitlin
// Modified by:
// Created:     10.09.00
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RADIOBOX_H_BASE_
#define _WX_RADIOBOX_H_BASE_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "radioboxbase.h"
#endif

#if wxUSE_RADIOBOX

#include "wx/ctrlsub.h"

extern WXDLLEXPORT_DATA(const wxChar*) wxRadioBoxNameStr;

// ----------------------------------------------------------------------------
// wxRadioBoxBase is not a normal base class, but rather a mix-in because the
// real wxRadioBox derives from different classes on different platforms: for
// example, it is a wxStaticBox in wxUniv and wxMSW but not in other ports
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxRadioBoxBase : public wxItemContainerImmutable
{
public:
    // change the individual radio button state
    virtual bool Enable(int n, bool enable = true) = 0;
    virtual bool Show(int n, bool show = true) = 0;

    // layout parameters
    virtual int GetColumnCount() const = 0;
    virtual int GetRowCount() const = 0;

    // return the item above/below/to the left/right of the given one
    int GetNextItem(int item, wxDirection dir, long style) const;


    // implement some of wxItemContainerImmutable functions
    virtual int FindString(const wxString& s) const;

    // deprecated functions
    // --------------------

#if WXWIN_COMPATIBILITY_2_4
    wxDEPRECATED( int GetNumberOfRowsOrCols() const );
    wxDEPRECATED( void SetNumberOfRowsOrCols(int n) );
#endif // WXWIN_COMPATIBILITY_2_4

    // for compatibility only, don't use these methods in new code!
#if WXWIN_COMPATIBILITY_2_2
    wxDEPRECATED( int Number() const );
    wxDEPRECATED( wxString GetLabel(int n) const );
    wxDEPRECATED( void SetLabel(int n, const wxString& label) );
#endif // WXWIN_COMPATIBILITY_2_2
};

#if defined(__WXUNIVERSAL__)
    #include "wx/univ/radiobox.h"
#elif defined(__WXMSW__)
    #include "wx/msw/radiobox.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/radiobox.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/radiobox.h"
#elif defined(__WXMAC__)
    #include "wx/mac/radiobox.h"
#elif defined(__WXCOCOA__)
    #include "wx/cocoa/radiobox.h"
#elif defined(__WXPM__)
    #include "wx/os2/radiobox.h"
#elif defined(__WXPALMOS__)
    #include "wx/palmos/radiobox.h"
#endif

#endif // wxUSE_RADIOBOX

#endif
    // _WX_RADIOBOX_H_BASE_
