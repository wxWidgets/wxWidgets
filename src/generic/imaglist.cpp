/////////////////////////////////////////////////////////////////////////////
// Name:        imaglist.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "imaglist.h"
#endif
 
#include "wx/imaglist.h"

//-----------------------------------------------------------------------------
//  wxImageList
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxImageList, wxObject)

wxImageList::wxImageList(int width, int height, bool mask, int initialCount)
{
  Create();
};

wxImageList::~wxImageList()
{
};

int wxImageList::GetImageCount() const
{
  return m_images.Number();
};

bool wxImageList::Create()
{
  m_images.DeleteContents( TRUE );
  return TRUE;
};

int wxImageList::Add( const wxBitmap &bitmap )
{
  m_images.Append( new wxBitmap(bitmap) );
  return m_images.Number();
};

bool wxImageList::Replace( int index, const wxBitmap &bitmap )
{
  wxNode *node = m_images.Nth( index );
  if (!node) return FALSE;
  
  if (index == m_images.Number()-1)
  {
    m_images.DeleteNode( node );
    m_images.Append( new wxBitmap(bitmap) );
  }
  else
  {
    wxNode *next = node->Next();
    m_images.DeleteNode( node );
    m_images.Insert( next, new wxBitmap(bitmap) );
  };
  
  return TRUE;
};

bool wxImageList::Remove( int index )
{
  wxNode *node = m_images.Nth( index );
  if (node) m_images.DeleteNode( node );
  return (node != NULL);
};

bool wxImageList::RemoveAll()
{
  m_images.Clear();
  return TRUE;
};

bool wxImageList::GetSize( int index, int &width, int &height ) const
{
  wxNode *node = m_images.Nth( index );
  if (node)
  {
    wxBitmap *bm = (wxBitmap*)node->Data();
    width = bm->GetWidth();
    height = bm->GetHeight();
    return TRUE;
  }
  else
  {
    width = 0;
    height = 0;
    return FALSE;
  };
};

bool wxImageList::Draw( int index, wxDC &dc, 
                        int x, int y,
                        int WXUNUSED(flags), bool WXUNUSED(solidBackground) )
{
  wxNode *node = m_images.Nth( index );
  if (!node) return FALSE;
  wxBitmap *bm = (wxBitmap*)node->Data();
  wxIcon *icon = (wxIcon*)bm;
  dc.DrawIcon( *icon, x, y );
  return TRUE;
};


