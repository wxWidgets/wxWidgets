/////////////////////////////////////////////////////////////////////////////
// Name:        imaglist.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $id$
// Copyright:   (c) 1998 Robert Roebling
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

wxImageList::wxImageList( int width, int height, bool WXUNUSED(mask), int WXUNUSED(initialCount) )
{
    m_width = width;
    m_height = height;
    Create();
}

wxImageList::~wxImageList()
{
}

int wxImageList::GetImageCount() const
{
    return m_images.Number();
}

bool wxImageList::Create()
{
    m_images.DeleteContents( TRUE );
    return TRUE;
}

int wxImageList::Add( const wxBitmap &bitmap )
{
    m_images.Append( new wxBitmap(bitmap) );
    return m_images.Number();
}

const wxBitmap *wxImageList::GetBitmap( int index ) const 
{
    wxNode *node = m_images.Nth( index );
    
    wxCHECK_MSG( node, (wxBitmap *) NULL, "wrong index in image list" );
    
    return (wxBitmap*)node->Data();
}
    
bool wxImageList::Replace( int index, const wxBitmap &bitmap )
{
    wxNode *node = m_images.Nth( index );
    
    wxCHECK_MSG( node, FALSE, "wrong index in image list" );
  
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
    }
  
    return TRUE;
}

bool wxImageList::Remove( int index )
{
    wxNode *node = m_images.Nth( index );
    
    wxCHECK_MSG( node, FALSE, "wrong index in image list" );
    
    m_images.DeleteNode( node );
     
    return TRUE;
}

bool wxImageList::RemoveAll()
{
    m_images.Clear();
    
    return TRUE;
}

bool wxImageList::GetSize( int index, int &width, int &height ) const
{
    width = 0;
    height = 0;
    
    wxNode *node = m_images.Nth( index );
    
    wxCHECK_MSG( node, FALSE, "wrong index in image list" );
    
    wxBitmap *bm = (wxBitmap*)node->Data();
    width = bm->GetWidth();
    height = bm->GetHeight();
    
    return TRUE;
}

bool wxImageList::Draw( int index, wxDC &dc, int x, int y,
                        int flags, bool WXUNUSED(solidBackground) )
{
    wxNode *node = m_images.Nth( index );
    
    wxCHECK_MSG( node, FALSE, "wrong index in image list" );
    
    wxBitmap *bm = (wxBitmap*)node->Data();
  
    dc.DrawBitmap( *bm, x, y, (flags & wxIMAGELIST_DRAW_TRANSPARENT) > 0 );

    return TRUE;
}


