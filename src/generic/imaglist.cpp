/////////////////////////////////////////////////////////////////////////////
// Name:        generic/imaglist.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
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
#include "wx/image.h"

//-----------------------------------------------------------------------------
//  wxImageList
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGenericImageList, wxObject)

#if !defined(__WXMSW__) || defined(__WIN16__) || defined(__WXUNIVERSAL__)
/*
 * wxImageList has to be a real class or we have problems with
 * the run-time information.
 */

IMPLEMENT_DYNAMIC_CLASS(wxImageList, wxGenericImageList)
#endif

wxGenericImageList::wxGenericImageList( int width, int height, bool mask, int initialCount )
{
    (void)Create(width, height, mask, initialCount);
}

wxGenericImageList::~wxGenericImageList()
{
}

int wxGenericImageList::GetImageCount() const
{
    return m_images.Number();
}

bool wxGenericImageList::Create( int width, int height, bool WXUNUSED(mask), int WXUNUSED(initialCount) )
{
    m_width = width;
    m_height = height;

    return Create();
}

bool wxGenericImageList::Create()
{
    m_images.DeleteContents( TRUE );
    return TRUE;
}

int wxGenericImageList::Add( const wxBitmap &bitmap )
{
    if (bitmap.IsKindOf(CLASSINFO(wxIcon)))
        m_images.Append( new wxIcon( (const wxIcon&) bitmap ) );
    else
        m_images.Append( new wxBitmap(bitmap) );
    return m_images.Number()-1;
}

int wxGenericImageList::Add( const wxBitmap& bitmap, const wxBitmap& mask )
{
    wxBitmap bmp(bitmap);
    if (mask.Ok())
        bmp.SetMask(new wxMask(mask));
    return Add(bmp);
}

int wxGenericImageList::Add( const wxBitmap& bitmap, const wxColour& maskColour )
{
    wxImage img = bitmap.ConvertToImage();
    img.SetMaskColour(maskColour.Red(), maskColour.Green(), maskColour.Blue());
    return Add(wxBitmap(img));
}

const wxBitmap *wxGenericImageList::GetBitmap( int index ) const
{
    wxNode *node = m_images.Nth( index );

    wxCHECK_MSG( node, (wxBitmap *) NULL, wxT("wrong index in image list") );

    return (wxBitmap*)node->Data();
}

bool wxGenericImageList::Replace( int index, const wxBitmap &bitmap )
{
    wxNode *node = m_images.Nth( index );

    wxCHECK_MSG( node, FALSE, wxT("wrong index in image list") );

    wxBitmap* newBitmap = NULL;
    if (bitmap.IsKindOf(CLASSINFO(wxIcon)))
#if defined(__VISAGECPP__)
//just can't do this in VisualAge now, with all this new Bitmap-Icon stuff
//so construct it from a bitmap object until I can figure this nonsense out. (DW)
        newBitmap = new wxBitmap(bitmap) ;
#else
        newBitmap = new wxBitmap( (const wxIcon&) bitmap );
#endif
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

bool wxGenericImageList::Remove( int index )
{
    wxNode *node = m_images.Nth( index );

    wxCHECK_MSG( node, FALSE, wxT("wrong index in image list") );

    m_images.DeleteNode( node );

    return TRUE;
}

bool wxGenericImageList::RemoveAll()
{
    m_images.Clear();

    return TRUE;
}

bool wxGenericImageList::GetSize( int index, int &width, int &height ) const
{
    width = 0;
    height = 0;

    wxNode *node = m_images.Nth( index );

    wxCHECK_MSG( node, FALSE, wxT("wrong index in image list") );

    wxBitmap *bm = (wxBitmap*)node->Data();
    width = bm->GetWidth();
    height = bm->GetHeight();

    return TRUE;
}

bool wxGenericImageList::Draw( int index, wxDC &dc, int x, int y,
                        int flags, bool WXUNUSED(solidBackground) )
{
    wxNode *node = m_images.Nth( index );

    wxCHECK_MSG( node, FALSE, wxT("wrong index in image list") );

    wxBitmap *bm = (wxBitmap*)node->Data();

    if (bm->IsKindOf(CLASSINFO(wxIcon)))
        dc.DrawIcon( * ((wxIcon*) bm), x, y);
    else
        dc.DrawBitmap( *bm, x, y, (flags & wxIMAGELIST_DRAW_TRANSPARENT) > 0 );

    return TRUE;
}


