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
 
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/generic/imaglist.h"
#include "wx/icon.h"

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
    if (bitmap.IsKindOf(CLASSINFO(wxIcon)))
        m_images.Append( new wxIcon( (const wxIcon&) bitmap ) );
    else
        m_images.Append( new wxBitmap(bitmap) );
    return m_images.Number();
}

const wxBitmap *wxImageList::GetBitmap( int index ) const 
{
    wxNode *node = m_images.Nth( index );
    
    wxCHECK_MSG( node, (wxBitmap *) NULL, _T("wrong index in image list") );
    
    return (wxBitmap*)node->Data();
}
    
bool wxImageList::Replace( int index, const wxBitmap &bitmap )
{
    wxNode *node = m_images.Nth( index );
    
    wxCHECK_MSG( node, FALSE, _T("wrong index in image list") );

    wxBitmap* newBitmap = NULL;
    if (bitmap.IsKindOf(CLASSINFO(wxIcon)))
        newBitmap = new wxIcon( (const wxIcon&) bitmap );
    else
        newBitmap = new wxBitmap(bitmap) ;

    if (index == m_images.Number()-1)
    {
        m_images.DeleteNode( node );
        m_images.Append( newBitmap );
    }
    else
    {
        wxNode *next = node->Next();
        m_images.DeleteNode( node );
        m_images.Insert( next, newBitmap );
    }
  
    return TRUE;
}

bool wxImageList::Remove( int index )
{
    wxNode *node = m_images.Nth( index );
    
    wxCHECK_MSG( node, FALSE, _T("wrong index in image list") );
    
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
    
    wxCHECK_MSG( node, FALSE, _T("wrong index in image list") );
    
    wxBitmap *bm = (wxBitmap*)node->Data();
    width = bm->GetWidth();
    height = bm->GetHeight();
    
    return TRUE;
}

bool wxImageList::Draw( int index, wxDC &dc, int x, int y,
                        int flags, bool WXUNUSED(solidBackground) )
{
    wxNode *node = m_images.Nth( index );
    
    wxCHECK_MSG( node, FALSE, _T("wrong index in image list") );
    
    wxBitmap *bm = (wxBitmap*)node->Data();

    if (bm->IsKindOf(CLASSINFO(wxIcon)))
        dc.DrawIcon( * ((wxIcon*) bm), x, y);
    else
        dc.DrawBitmap( *bm, x, y, (flags & wxIMAGELIST_DRAW_TRANSPARENT) > 0 );

    return TRUE;
}


