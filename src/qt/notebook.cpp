/////////////////////////////////////////////////////////////////////////////
// Name:        notebook.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "notebook.h"
#endif

#include "wx/notebook.h"
#include "wx/panel.h"
#include "wx/utils.h"
#include "wx/imaglist.h"
#include "wx/log.h"

//-----------------------------------------------------------------------------
// wxNotebookPage
//-----------------------------------------------------------------------------

class wxNotebookPage: public wxObject
{
public:
  wxNotebookPage()
  {
    m_id = -1;
    m_text = "";
    m_image = -1;
    m_page = NULL;
    m_client = NULL;
    m_parent = NULL;
  };

//private:
  int                m_id;
  wxString           m_text;
  int                m_image;
  wxWindow          *m_client;
};

//-----------------------------------------------------------------------------
// wxNotebook
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxNotebook,wxControl)

void wxNotebook::Init()
{
  m_imageList = NULL;
  m_pages.DeleteContents( TRUE );
}

wxNotebook::wxNotebook()
{
  Init();
};

wxNotebook::wxNotebook( wxWindow *parent, wxWindowID id,
      const wxPoint& pos, const wxSize& size,
      long style, const wxString& name )
{
  Init();
  Create( parent, id, pos, size, style, name );
};

wxNotebook::~wxNotebook()
{
  DeleteAllPages();
};

bool wxNotebook::Create(wxWindow *parent, wxWindowID id,
      const wxPoint& pos, const wxSize& size,
      long style, const wxString& name )
{
  PreCreation( parent, id, pos, size, style, name );

  PostCreation();

  Show( TRUE );

  return TRUE;
};

int wxNotebook::GetSelection() const
{
};

int wxNotebook::GetPageCount() const
{
};

int wxNotebook::GetRowCount() const
{
};

wxString wxNotebook::GetPageText( int page ) const
{
};

int wxNotebook::GetPageImage( int page ) const
{
};

wxNotebookPage* wxNotebook::GetNotebookPage(int page) const
{
  return NULL;
};

int wxNotebook::SetSelection( int page )
{
};

void wxNotebook::AdvanceSelection(bool bForward)
{
}

void wxNotebook::SetImageList( wxImageList* imageList )
{
  m_imageList = imageList;
};

bool wxNotebook::SetPageText( int page, const wxString &text )
{
  return TRUE;
};

bool wxNotebook::SetPageImage( int page, int image )
{
  return TRUE;
};

void wxNotebook::SetPageSize( const wxSize &WXUNUSED(size) )
{
};

void wxNotebook::SetPadding( const wxSize &WXUNUSED(padding) )
{
};

bool wxNotebook::DeleteAllPages()
{
  return TRUE;
};

bool wxNotebook::DeletePage( int page )
{
  return TRUE;
};

bool wxNotebook::AddPage(wxWindow* win, const wxString& text,
                         bool bSelect, int imageId)
{
  return TRUE;
};

wxWindow *wxNotebook::GetPage( int page ) const
{
    return NULL;
};

void wxNotebook::AddChild( wxWindow *win )
{
};

// override these 2 functions to do nothing: everything is done in OnSize
void wxNotebook::SetConstraintSizes( bool WXUNUSED(recurse) )
{
  // don't set the sizes of the pages - their correct size is not yet known
  wxControl::SetConstraintSizes(FALSE);
}

bool wxNotebook::DoPhase( int WXUNUSED(nPhase) )
{
  return TRUE;
}

//-----------------------------------------------------------------------------
// wxNotebookEvent
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxNotebookEvent, wxCommandEvent)
