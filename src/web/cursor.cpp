/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/cursor.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id: cursor.cpp 45848 2007-05-05 19:49:02Z PC $
// Copyright:   (c) 1998 Robert Roebling, John Wilmes
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/cursor.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/image.h"
    #include "wx/colour.h"
#endif // WX_PRECOMP

//-----------------------------------------------------------------------------
// wxCursor
//-----------------------------------------------------------------------------

class wxCursorRefData: public wxObjectRefData
{
  public:

    wxCursorRefData();
    virtual ~wxCursorRefData();

    wxString m_name;
};

wxCursorRefData::wxCursorRefData()
{
    m_name = "default";
}

wxCursorRefData::~wxCursorRefData()
{
}

//-----------------------------------------------------------------------------

#define M_CURSORDATA ((wxCursorRefData *)m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxCursor, wxObject)

wxCursor::wxCursor()
{
}

wxCursor::wxCursor(int cursorId)
{
    m_refData = new wxCursorRefData();

    wxString name;
    switch (cursorId)
    {
        case wxCURSOR_HAND:             name = "pointer"; break;
        case wxCURSOR_CROSS:            name = "crosshair"; break;
        case wxCURSOR_SIZEWE:           name = "W-resize"; break;
        case wxCURSOR_SIZENS:           name = "N-resize"; break;
        case wxCURSOR_ARROWWAIT:        //fall through
        case wxCURSOR_WAIT:             //fall through
        case wxCURSOR_WATCH:            name = "wait"; break;
        case wxCURSOR_IBEAM:            name = "text"; break;
        case wxCURSOR_NO_ENTRY:         name = "not-allowed"; break; //Microsoft extension
        case wxCURSOR_QUESTION_ARROW:   name = "help"; break;
        case wxCURSOR_CHAR:             name = "text"; break;

        default:                        name = "default"; break;
    }

    M_CURSORDATA->m_name = name;
}

wxCursor::wxCursor(const char bits[], int width, int  height,
                   int hotSpotX, int hotSpotY,
                   const char maskBits[], const wxColour *fg, const wxColour *bg)
{
    //TODO actually implement this
    m_refData = new wxCursorRefData;
    M_CURSORDATA->m_name = "default";
}

wxCursor::~wxCursor()
{
}

bool wxCursor::Ok() const
{
    return (m_refData != NULL);
}

//-----------------------------------------------------------------------------
// busy cursor routines
//-----------------------------------------------------------------------------

const wxCursor wxBusyCursor::GetBusyCursor()
{
    return wxCursor(wxCURSOR_WATCH);
}

void wxEndBusyCursor()
{
}

void wxBeginBusyCursor( const wxCursor *WXUNUSED(cursor) )
{
}
