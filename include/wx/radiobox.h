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
    // change/query the individual radio button state
    virtual bool Enable(int n, bool enable = true) = 0;
    virtual bool Show(int n, bool show = true) = 0;

    // NB: these functions are stubbed here for now but should become pure
    //     virtual once all ports implement them
    virtual bool IsItemEnabled(int WXUNUSED(n)) const { return true; }
    virtual bool IsItemShown(int WXUNUSED(n)) const { return true; }

    // return number of columns/rows in this radiobox
    int GetColumnCount() const { return m_numCols; }
    int GetRowCount() const { return m_numRows; }

    // return the item above/below/to the left/right of the given one
    int GetNextItem(int item, wxDirection dir, long style) const;


    // deprecated functions
    // --------------------

#if WXWIN_COMPATIBILITY_2_4
    wxDEPRECATED( int GetNumberOfRowsOrCols() const );
    wxDEPRECATED( void SetNumberOfRowsOrCols(int n) );
#endif // WXWIN_COMPATIBILITY_2_4

protected:
    wxRadioBoxBase()
    {
        m_majorDim = 0;
    }

    // return the number of items in major direction (which depends on whether
    // we have wxRA_SPECIFY_COLS or wxRA_SPECIFY_ROWS style)
    int GetMajorDim() const { return m_majorDim; }

    // sets m_majorDim and also updates m_numCols/Rows
    //
    // the style parameter should be the style of the radiobox itself
    void SetMajorDim(int majorDim, long style);


private:
    // the number of elements in major dimension (i.e. number of columns if
    // wxRA_SPECIFY_COLS or the number of rows if wxRA_SPECIFY_ROWS) and also
    // the number of rows/columns calculated from it
    int m_majorDim,
        m_numCols,
        m_numRows;
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
