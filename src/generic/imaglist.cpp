/////////////////////////////////////////////////////////////////////////////
// Name:        generic/imaglist.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "imaglist.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/defs.h"

#ifndef __WXPALMOS__

#include "wx/generic/imaglist.h"

#include "wx/icon.h"
#include "wx/image.h"
#include "wx/dc.h"

//-----------------------------------------------------------------------------
//  wxImageList
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGenericImageList, wxObject)

#if !defined(__WXMSW__) || defined(__WXUNIVERSAL__)
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
    (void)RemoveAll();
}

int wxGenericImageList::GetImageCount() const
{
    return m_images.GetCount();
}

bool wxGenericImageList::Create( int width, int height, bool WXUNUSED(mask), int WXUNUSED(initialCount) )
{
    m_width = width;
    m_height = height;

    return Create();
}

bool wxGenericImageList::Create()
{
    return true;
}

int wxGenericImageList::Add( const wxBitmap &bitmap )
{
    wxASSERT_MSG( (bitmap.GetWidth() == m_width && bitmap.GetHeight() == m_height)
                  || (m_width == 0 && m_height == 0),
                  _T("invalid bitmap size in wxImageList: this might work ")
                  _T("on this platform but definitely won't under Windows.") );

    if (bitmap.IsKindOf(CLASSINFO(wxIcon)))
        m_images.Append( new wxIcon( (const wxIcon&) bitmap ) );
    else
        m_images.Append( new wxBitmap(bitmap) );

    if (m_width == 0 && m_height == 0)
    {
        m_width = bitmap.GetWidth();
        m_height = bitmap.GetHeight();
    }
    
    return m_images.GetCount()-1;
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
    wxList::compatibility_iterator node = m_images.Item( index );

    wxCHECK_MSG( node, (wxBitmap *) NULL, wxT("wrong index in image list") );

    return (wxBitmap*)node->GetData();
}

bool wxGenericImageList::Replace( int index, const wxBitmap &bitmap )
{
    wxList::compatibility_iterator node = m_images.Item( index );

    wxCHECK_MSG( node, false, wxT("wrong index in image list") );

    wxBitmap* newBitmap = (bitmap.IsKindOf(CLASSINFO(wxIcon))) ?
                             #if defined(__VISAGECPP__)
                               //just can't do this in VisualAge now, with all this new Bitmap-Icon stuff
                               //so construct it from a bitmap object until I can figure this nonsense out. (DW)
                               new wxBitmap(bitmap)
                             #else
                               new wxBitmap( (const wxIcon&) bitmap )
                             #endif
                               : new wxBitmap(bitmap) ;

    if (index == (int) m_images.GetCount() - 1)
    {
        delete node->GetData();
        m_images.Erase( node );
        m_images.Append( newBitmap );
    }
    else
    {
        wxList::compatibility_iterator next = node->GetNext();
        delete node->GetData();
        m_images.Erase( node );
        m_images.Insert( next, newBitmap );
    }

    return true;
}

bool wxGenericImageList::Remove( int index )
{
    wxList::compatibility_iterator node = m_images.Item( index );

    wxCHECK_MSG( node, false, wxT("wrong index in image list") );

    delete node->GetData();
    m_images.Erase( node );

    return true;
}

bool wxGenericImageList::RemoveAll()
{
    WX_CLEAR_LIST(wxList, m_images);
    m_images.Clear();

    return true;
}

bool wxGenericImageList::GetSize( int index, int &width, int &height ) const
{
    width = 0;
    height = 0;

    wxList::compatibility_iterator node = m_images.Item( index );

    wxCHECK_MSG( node, false, wxT("wrong index in image list") );

    wxBitmap *bm = (wxBitmap*)node->GetData();
    width = bm->GetWidth();
    height = bm->GetHeight();

    return true;
}

bool wxGenericImageList::Draw( int index, wxDC &dc, int x, int y,
                        int flags, bool WXUNUSED(solidBackground) )
{
    wxList::compatibility_iterator node = m_images.Item( index );

    wxCHECK_MSG( node, false, wxT("wrong index in image list") );

    wxBitmap *bm = (wxBitmap*)node->GetData();

    if (bm->IsKindOf(CLASSINFO(wxIcon)))
        dc.DrawIcon( * ((wxIcon*) bm), x, y);
    else
        dc.DrawBitmap( *bm, x, y, (flags & wxIMAGELIST_DRAW_TRANSPARENT) > 0 );

    return true;
}

#endif // __WXPALMOS__
