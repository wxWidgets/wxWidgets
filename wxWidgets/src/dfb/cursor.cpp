/////////////////////////////////////////////////////////////////////////////
// Name:        src/dfb/cursor.cpp
// Purpose:     wxCursor implementation
// Author:      Vaclav Slavik
// Created:     2006-08-08
// RCS-ID:      $Id$
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/cursor.h"
#include "wx/bitmap.h"

//-----------------------------------------------------------------------------
// wxCursorRefData
//-----------------------------------------------------------------------------

class wxCursorRefData: public wxObjectRefData
{
public:
    wxCursorRefData(const wxBitmap& bmp = wxNullBitmap, int id = -1)
        : m_id(id), m_bitmap(bmp) {}

    int      m_id;
    wxBitmap m_bitmap;
};

#define M_CURSOR_OF(c) ((wxCursorRefData*)((c).m_refData))
#define M_CURSOR       M_CURSOR_OF(*this)

//-----------------------------------------------------------------------------
// wxCursor
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxCursor, wxObject)

wxCursor::wxCursor(int cursorId)
{
#warning "FIXME -- implement the cursor as bitmaps (that's what DFB uses)"
}

wxCursor::wxCursor(const char WXUNUSED(bits)[],
                   int WXUNUSED(width),
                   int WXUNUSED(height),
                   int WXUNUSED(hotSpotX), int WXUNUSED(hotSpotY),
                   const char WXUNUSED(maskBits)[],
                   wxColour * WXUNUSED(fg), wxColour * WXUNUSED(bg) )
{
#warning "FIXME"
}

wxCursor::wxCursor(const wxString& cursor_file,
                   long flags,
                   int WXUNUSED(hotSpotX), int WXUNUSED(hotSpotY))
{
#warning "FIXME"
}

bool wxCursor::IsOk() const
{
    return m_refData && M_CURSOR->m_bitmap.Ok();
}

wxObjectRefData *wxCursor::CreateRefData() const
{
    return new wxCursorRefData;
}

wxObjectRefData *wxCursor::CloneRefData(const wxObjectRefData *data) const
{
    return new wxCursorRefData(*(wxCursorRefData *)data);
}


// ----------------------------------------------------------------------------
// Global cursor setting
// ----------------------------------------------------------------------------

void wxSetCursor(const wxCursor& cursor)
{
#warning "FIXME: implement"
}



//-----------------------------------------------------------------------------
// busy cursor routines
//-----------------------------------------------------------------------------

#warning "FIXME: this should be common code"
#if 0
static wxCursor  gs_savedCursor = wxNullCursor;
static int       gs_busyCount = 0;

const wxCursor &wxBusyCursor::GetStoredCursor()
{
    return gs_savedCursor;
}

const wxCursor wxBusyCursor::GetBusyCursor()
{
    return gs_globalCursor;
}
#endif

void wxEndBusyCursor()
{
    #warning "FIXME - do this logic in common code?"
}

void wxBeginBusyCursor(const wxCursor *cursor)
{
    #warning "FIXME"
}

bool wxIsBusy()
{
    #warning "FIXME"
    return false;
}
