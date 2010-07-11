/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/cursor.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/cursor.h"
#include "wx/qt/utils.h"

void wxSetCursor(const wxCursor& cursor)
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );
}

void wxBeginBusyCursor(const wxCursor *cursor)
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );
}

bool wxIsBusy()
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return false;
}

void wxEndBusyCursor()
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );
}


IMPLEMENT_DYNAMIC_CLASS( wxCursor, wxGDIObject )

wxCursor::wxCursor()
{
}

wxCursor::wxCursor( const wxCursor & )
{
}


void wxCursor::InitFromStock( wxStockCursor )
{
}


wxGDIRefData *wxCursor::CreateGDIRefData() const
{
    return NULL;
}

wxGDIRefData *wxCursor::CloneGDIRefData(const wxGDIRefData *data) const
{
    return NULL;
}


