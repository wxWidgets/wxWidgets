/////////////////////////////////////////////////////////////////////////////
// Name:        cursor.cpp
// Purpose:
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "cursor.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/cursor.h"
#include "wx/utils.h"
#include "wx/log.h"
#include "wx/intl.h"

#include <mgraph.hpp>


//-----------------------------------------------------------------------------
// wxCursor
//-----------------------------------------------------------------------------

class wxCursorRefData: public wxObjectRefData
{
  public:

    wxCursorRefData();
    ~wxCursorRefData();

    MGLCursor *m_cursor;
};

wxCursorRefData::wxCursorRefData()
{
    m_cursor = (MGLCursor*) NULL;
}

wxCursorRefData::~wxCursorRefData()
{
    delete m_cursor;
}

//-----------------------------------------------------------------------------

#define M_CURSORDATA ((wxCursorRefData *)m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxCursor,wxObject)

wxCursor::wxCursor()
{
}

wxCursor::wxCursor(int cursorId)
{
    const char *cursorname = NULL;
    m_refData = new wxCursorRefData();

    switch (cursorId)
    {
        case wxCURSOR_ARROW:           cursorname = "arrow.cur"; break;
        case wxCURSOR_BULLSEYE:        cursorname = "bullseye.cur"; break;
        case wxCURSOR_CHAR:            cursorname = "char.cur"; break;
        case wxCURSOR_CROSS:           cursorname = "cross.cur"; break;
        case wxCURSOR_HAND:            cursorname = "hand.cur"; break;
        case wxCURSOR_IBEAM:           cursorname = "ibeam.cur"; break;
        case wxCURSOR_LEFT_BUTTON:     cursorname = "leftbtn.cur"; break;
        case wxCURSOR_MAGNIFIER:       cursorname = "magnif1.cur"; break;
        case wxCURSOR_MIDDLE_BUTTON:   cursorname = "midbtn.cur"; break;
        case wxCURSOR_NO_ENTRY:        cursorname = "noentry.cur"; break;
        case wxCURSOR_PAINT_BRUSH:     cursorname = "pbrush.cur"; break;
        case wxCURSOR_PENCIL:          cursorname = "pencil.cur"; break;
        case wxCURSOR_POINT_LEFT:      cursorname = "pntleft.cur"; break;
        case wxCURSOR_POINT_RIGHT:     cursorname = "pntright.cur"; break;
        case wxCURSOR_QUESTION_ARROW:  cursorname = "query.cur"; break;
        case wxCURSOR_RIGHT_BUTTON:    cursorname = "rightbtn.cur"; break;
        case wxCURSOR_SIZENESW:        cursorname = "sizenesw.cur"; break;
        case wxCURSOR_SIZENS:          cursorname = "sizens.cur"; break;
        case wxCURSOR_SIZENWSE:        cursorname = "sizenwse.cur"; break;
        case wxCURSOR_SIZEWE:          cursorname = "sizewe.cur"; break;
        case wxCURSOR_SIZING:          cursorname = "size.cur"; break;
        case wxCURSOR_SPRAYCAN:        cursorname = "spraycan.cur"; break;
        case wxCURSOR_WAIT:            cursorname = "wait.cur"; break;
        case wxCURSOR_WATCH:           cursorname = "clock.cur"; break;
        case wxCURSOR_BLANK:           cursorname = "blank.cur"; break;

        case wxCURSOR_NONE:
            // FIXME_MGL - make sure wxWindow uses cursor with
            //    GetMGLCursor() == NULL correctly, i.e. calls MS_hide()
            *this = wxNullCursor;
            return;
            break;

        default:
            wxFAIL_MSG(wxT("unsupported cursor type"));
            break;
    }
    
    M_CURSORDATA->m_cursor = new MGLCursor(cursorname);

    // if we cannot load arrow cursor, use MGL's default arrow cursor:
    if ( !M_CURSORDATA->m_cursor->valid() && cursorId == wxCURSOR_ARROW )
    {
        delete M_CURSORDATA->m_cursor;
        M_CURSORDATA->m_cursor = new MGLCursor(MGL_DEF_CURSOR);
    }
    
    if ( !M_CURSORDATA->m_cursor->valid() )
    {
        wxLogError(_("Couldn't create cursor."));
        UnRef();
    }
}

wxCursor::wxCursor(const char WXUNUSED(bits)[],
                   int WXUNUSED(width),
                   int WXUNUSED(height),
                   int WXUNUSED(hotSpotX), int WXUNUSED(hotSpotY),
                   const char WXUNUSED(maskBits)[],
                   wxColour * WXUNUSED(fg), wxColour * WXUNUSED(bg) )
{
    //FIXME_MGL
}

wxCursor::wxCursor(const wxString& cursor_file,
                   long flags,
                   int hotSpotX, int hotSpotY)
{
    if ( flags == wxBITMAP_TYPE_CUR || flags == wxBITMAP_TYPE_CUR_RESOURCE )
    {
        m_refData = new wxCursorRefData();
        M_CURSORDATA->m_cursor = new MGLCursor(cursor_file.mb_str());
        if ( !M_CURSORDATA->m_cursor->valid() )
        {
            wxLogError(_("Couldn't create cursor."));
            UnRef();
        }
    }
    else
    {
        wxLogError(wxT("Cannot load cursor resource of this type."));
    }
}

wxCursor::wxCursor(const wxCursor &cursor)
{
    Ref(cursor);
}

wxCursor::~wxCursor()
{
    // wxObject unrefs data
}

wxCursor& wxCursor::operator = (const wxCursor& cursor)
{
    if ( *this == cursor )
        return (*this);
    Ref(cursor);
    return *this;
}

bool wxCursor::operator == (const wxCursor& cursor) const
{
    return (m_refData == cursor.m_refData);
}

bool wxCursor::operator != (const wxCursor& cursor) const
{
    return (m_refData != cursor.m_refData);
}

bool wxCursor::Ok() const
{
    return (m_refData != NULL);
}

MGLCursor *wxCursor::GetMGLCursor() const
{
    return M_CURSORDATA->m_cursor;
}



// ----------------------------------------------------------------------------
// Global cursor setting
// ----------------------------------------------------------------------------


void wxSetCursor(const wxCursor& cursor)
{
    if ( cursor.Ok() )
    {
        //MGL_setGlobalCursor(cursor.GetMGLCursor());
        // FIXME_MGL -- needs MGL WM first
    }
}



//-----------------------------------------------------------------------------
// busy cursor routines
//-----------------------------------------------------------------------------

// FIXME_MGL -- do we need this? It may be better to incorporate 
//              support for it into MGL (a stack of global cursors?)
static wxCursor  gs_savedCursor;
static wxCursor  g_globalCursor;
static int       gs_busyCount = 0;

const wxCursor &wxBusyCursor::GetStoredCursor()
{
    return gs_savedCursor;
}

const wxCursor wxBusyCursor::GetBusyCursor()
{
    return wxCursor(wxCURSOR_WAIT);
}

void wxEndBusyCursor()
{
    if ( --gs_busyCount > 0 ) return;

    wxSetCursor(gs_savedCursor);
    gs_savedCursor = wxNullCursor;
    //wxYield(); FIXME_MGL - needed?
}

void wxBeginBusyCursor(wxCursor *WXUNUSED(cursor))
{
    if ( gs_busyCount++ > 0 ) return;

    wxASSERT_MSG( !gs_savedCursor.Ok(),
                  wxT("forgot to call wxEndBusyCursor, will leak memory") );

    gs_savedCursor = g_globalCursor;
    wxSetCursor(wxCursor(wxCURSOR_WAIT));
    //wxYield(); FIXME_MGL - needed?
}

bool wxIsBusy()
{
    return (gs_busyCount > 0);
}

