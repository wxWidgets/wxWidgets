///////////////////////////////////////////////////////////////////////////////
// Name:        dnd.cpp
// Purpose:     wxDropTarget class
// Author:      Robert Roebling
// Copyright:   Robert Roebling
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dnd.h"
#endif

#include "wx/dnd.h"
#include "wx/window.h"
#include "wx/app.h"
#include "wx/gdicmn.h"

// ----------------------------------------------------------------------------
// global
// ----------------------------------------------------------------------------

extern bool g_blockEventsOnDrag;

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

bool wxTextDropTarget::OnDrop( long x, long y, const void *pData )
{
  OnDropText( x, y, (const char*)pData );
  return TRUE;
};

bool wxTextDropTarget::OnDropText( long x, long y, const char *psz )
{
  printf( "Got dropped text: %s.\n", psz );
  printf( "At x: %d, y: %d.\n", (int)x, (int)y );
  return TRUE;
};

size_t wxTextDropTarget::GetFormatCount() const
{
  return 1;
}

wxDataFormat wxTextDropTarget::GetFormat(size_t WXUNUSED(n)) const
{
  return wxDF_TEXT;
}

// ----------------------------------------------------------------------------
// wxFileDropTarget
// ----------------------------------------------------------------------------

bool wxFileDropTarget::OnDropFiles( long x, long y, size_t nFiles, const char * const WXUNUSED(aszFiles)[] )
{
  printf( "Got %d dropped files.\n", (int)nFiles );
  printf( "At x: %d, y: %d.\n", (int)x, (int)y );
  return TRUE;
}

bool wxFileDropTarget::OnDrop(long x, long y, const void *WXUNUSED(pData) )
{
  char *str = "/this/is/a/path.txt";

  return OnDropFiles(x, y, 1, &str ); 
}

size_t wxFileDropTarget::GetFormatCount() const
{
  return 1;
}

wxDataFormat wxFileDropTarget::GetFormat(size_t WXUNUSED(n)) const
{
  return wxDF_FILENAME;
}

//-------------------------------------------------------------------------
// wxDropSource
//-------------------------------------------------------------------------

wxDropSource::wxDropSource( wxWindow *WXUNUSED(win) )
{
  g_blockEventsOnDrag = TRUE;
};

wxDropSource::wxDropSource( wxDataObject &data, wxWindow *WXUNUSED(win) )
{
  g_blockEventsOnDrag = TRUE;
  
  m_data = &data;  
};

void wxDropSource::SetData( wxDataObject &data )
{
  m_data = &data;  
};

wxDropSource::~wxDropSource(void)
{
//  if (m_data) delete m_data;

  g_blockEventsOnDrag = FALSE;
};
   
wxDropSource::DragResult wxDropSource::DoDragDrop( bool WXUNUSED(bAllowMove) )
{
  return Copy;
};

