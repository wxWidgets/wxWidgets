/////////////////////////////////////////////////////////////////////////////
// Name:        cursor.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "cursor.h"
#endif

#include "wx/cursor.h"

//-----------------------------------------------------------------------------
// wxCursor
//-----------------------------------------------------------------------------

class wxCursorRefData: public wxObjectRefData
{
  public:
  
    wxCursorRefData(void);
    ~wxCursorRefData(void);
  
};

wxCursorRefData::wxCursorRefData(void)
{
};

wxCursorRefData::~wxCursorRefData(void)
{
};

//-----------------------------------------------------------------------------

#define M_CURSORDATA ((wxCursorRefData *)m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxCursor,wxObject)

wxCursor::wxCursor(void)
{
};

wxCursor::wxCursor( int WXUNUSED(cursorId) )
{
  m_refData = new wxCursorRefData();
};

wxCursor::wxCursor( const wxCursor &cursor )
{
  Ref( cursor );
};

wxCursor::wxCursor( const wxCursor *cursor )
{
  UnRef();
  if (cursor) Ref( *cursor );
};

wxCursor::~wxCursor(void)
{
};

wxCursor& wxCursor::operator = ( const wxCursor& cursor )
{
  if (*this == cursor) return (*this); 
  Ref( cursor ); 
  return *this; 
};

bool wxCursor::operator == ( const wxCursor& cursor )
{
  return m_refData == cursor.m_refData; 
};

bool wxCursor::operator != ( const wxCursor& cursor )
{
  return m_refData != cursor.m_refData; 
};

bool wxCursor::Ok(void) const
{
  return TRUE;
};

//-----------------------------------------------------------------------------
// busy cursor routines
//-----------------------------------------------------------------------------

bool g_isBusy = FALSE;

void wxEndBusyCursor(void)
{
  g_isBusy = FALSE;
};

void wxBeginBusyCursor( wxCursor *WXUNUSED(cursor) )
{
  g_isBusy = TRUE;
};

bool wxIsBusy(void)
{
  return g_isBusy;
};

void wxSetCursor( const wxCursor& cursor )
{
  extern wxCursor *g_globalCursor;
  if (g_globalCursor) (*g_globalCursor) = cursor;

  if (cursor.Ok()) {};
};


