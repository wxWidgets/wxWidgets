///////////////////////////////////////////////////////////////////////////////
// Name:        dnd.cpp
// Purpose:     wxDropTarget, wxDropSource, wxDataObject implementation
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 AUTHOR
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dnd.h"
#endif

#include "wx/dnd.h"
#include "wx/window.h"
#include "wx/app.h"
#include "wx/gdicmn.h"

#if wxUSE_DRAG_AND_DROP

// ----------------------------------------------------------------------------
// global
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// wxDropTarget
// ----------------------------------------------------------------------------

wxDropTarget::wxDropTarget()
{
};

wxDropTarget::~wxDropTarget()
{
};

// ----------------------------------------------------------------------------
// wxTextDropTarget
// ----------------------------------------------------------------------------
#ifndef __WXMAC_X__
bool wxTextDropTarget::OnDrop( wxCoord x, wxCoord y, const void *pData )
{
  OnDropText( x, y, (const char*)pData );
  return TRUE;
};
#endif

bool wxTextDropTarget::OnDropText( wxCoord x, wxCoord y, const wxString &psz )
{
  printf( "Got dropped text: %s.\n", (char *)psz );
  printf( "At x: %d, y: %d.\n", (int)x, (int)y );
  return TRUE;
};

#ifndef __WXMAC_X__
size_t wxTextDropTarget::GetFormatCount() const
{
  return 1;
}

wxDataFormat wxTextDropTarget::GetFormat(size_t WXUNUSED(n)) const
{
  return wxDF_TEXT;
}
#endif

// ----------------------------------------------------------------------------
// wxFileDropTarget
// ----------------------------------------------------------------------------

#ifndef __WXMAC_X__
bool wxFileDropTarget::OnDropFiles( wxCoord x, wxCoord y, size_t nFiles, const char * const WXUNUSED(aszFiles)[] )
{
  printf( "Got %d dropped files.\n", (int)nFiles );
  printf( "At x: %d, y: %d.\n", (int)x, (int)y );
  return TRUE;
}
#endif

bool wxFileDropTarget::OnDrop(wxCoord x, wxCoord y, const wxArrayString& filenames)
{
  return OnDropFiles(x, y, 1, &filenames); 
}

#ifndef __WXMAC_X__
size_t wxFileDropTarget::GetFormatCount() const
{
  return 1;
}

wxDataFormat wxFileDropTarget::GetFormat(size_t WXUNUSED(n)) const
{
  return wxDF_FILENAME;
}
#endif

//-------------------------------------------------------------------------
// wxDropSource
//-------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// drag request

wxDropSource::wxDropSource( wxWindow *win )
{
    // TODO
  //    m_window = win;
    m_data = NULL;

    //    m_defaultCursor = wxCursor( wxCURSOR_NO_ENTRY );
    //    m_goaheadCursor = wxCursor( wxCURSOR_HAND );
};

wxDropSource::wxDropSource( wxDataObject &data, wxWindow *win )
{
    // TODO
  //    m_window = win;
    m_data = &data;

    //    m_defaultCursor = wxCursor( wxCURSOR_NO_ENTRY );
    //    m_goaheadCursor = wxCursor( wxCURSOR_HAND );
};

void wxDropSource::SetData( wxDataObject &data )
{
    m_data = &data;
};

wxDropSource::~wxDropSource(void)
{
};
   
wxDragResult wxDropSource::DoDragDrop( bool WXUNUSED(bAllowMove) )
{
    // TODO
    return wxDragError;
};

#endif
