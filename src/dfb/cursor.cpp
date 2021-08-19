/////////////////////////////////////////////////////////////////////////////
// Name:        src/dfb/cursor.cpp
// Purpose:     wxCursor implementation
// Author:      Vaclav Slavik
// Created:     2006-08-08
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/cursor.h"
#include "wx/bitmap.h"

//-----------------------------------------------------------------------------
// wxCursorRefData
//-----------------------------------------------------------------------------

class wxCursorRefData : public wxGDIRefData
{
public:
    wxCursorRefData(const wxBitmap& bmp = wxNullBitmap, int id = -1)
        : m_id(id), m_bitmap(bmp) {}

    virtual bool IsOk() const { return m_bitmap.IsOk(); }

    // Create a deep copy of this object.
    wxCursorRefData *Clone() const
    {
        wxBitmap bitmapCopy(m_bitmap);
        bitmapCopy.UnShare();

        return new wxCursorRefData(bitmapCopy, m_id);
    }

    int      m_id;
    wxBitmap m_bitmap;
};

#define M_CURSOR_OF(c) ((wxCursorRefData*)((c).m_refData))
#define M_CURSOR       M_CURSOR_OF(*this)

//-----------------------------------------------------------------------------
// wxCursor
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxCursor, wxObject);

void wxCursor::InitFromStock(wxStockCursor cursorId)
{
#warning "FIXME -- implement the cursor as bitmaps (that's what DFB uses)"
}

#if wxUSE_IMAGE

wxCursor::wxCursor(const wxImage& image)
{
#warning "FIXME"
}

wxCursor::wxCursor(const char* const* xpmData)
{
#warning "FIXME"
}

#endif // wxUSE_IMAGE

wxCursor::wxCursor(const wxString& cursor_file,
                   wxBitmapType type,
                   int WXUNUSED(hotSpotX), int WXUNUSED(hotSpotY))
{
#warning "FIXME"
}

wxGDIRefData *wxCursor::CreateGDIRefData() const
{
    return new wxCursorRefData;
}

wxGDIRefData *wxCursor::CloneGDIRefData(const wxGDIRefData *data) const
{
    return static_cast<const wxCursorRefData *>(data)->Clone();
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
