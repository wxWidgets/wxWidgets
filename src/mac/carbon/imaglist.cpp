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

#if wxUSE_IMAGLIST

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/defs.h"

#include "wx/imaglist.h"

#include "wx/icon.h"
#include "wx/image.h"
#include "wx/dc.h"

//-----------------------------------------------------------------------------
//  wxImageList
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxImageList, wxObject)

wxImageList::wxImageList( int width, int height, bool mask, int initialCount )
{
    (void)Create(width, height, mask, initialCount);
}

wxImageList::~wxImageList()
{
    (void)RemoveAll();
}

int wxImageList::GetImageCount() const
{
    return m_images.GetCount();
}

bool wxImageList::Create( int width, int height, bool WXUNUSED(mask), int WXUNUSED(initialCount) )
{
    m_width = width;
    m_height = height;

    return Create();
}

bool wxImageList::Create()
{
    return true;
}

int wxImageList::Add( const wxIcon &bitmap )
{
    wxASSERT_MSG( (bitmap.GetWidth() == m_width && bitmap.GetHeight() == m_height)
                  || (m_width == 0 && m_height == 0),
                  _T("invalid bitmap size in wxImageList: this might work ")
                  _T("on this platform but definitely won't under Windows.") );

    m_images.Append( new wxIcon( bitmap ) );

    if (m_width == 0 && m_height == 0)
    {
        m_width = bitmap.GetWidth();
        m_height = bitmap.GetHeight();
    }
    
    return m_images.GetCount()-1;
}

int wxImageList::Add( const wxBitmap &bitmap )
{
    wxASSERT_MSG( (bitmap.GetWidth() == m_width && bitmap.GetHeight() == m_height)
                  || (m_width == 0 && m_height == 0),
                  _T("invalid bitmap size in wxImageList: this might work ")
                  _T("on this platform but definitely won't under Windows.") );

    m_images.Append( new wxBitmap(bitmap) );

    if (m_width == 0 && m_height == 0)
    {
        m_width = bitmap.GetWidth();
        m_height = bitmap.GetHeight();
    }
    
    return m_images.GetCount()-1;
}

int wxImageList::Add( const wxBitmap& bitmap, const wxBitmap& mask )
{
    wxBitmap bmp(bitmap);
    if (mask.Ok())
        bmp.SetMask(new wxMask(mask));
    return Add(bmp);
}

int wxImageList::Add( const wxBitmap& bitmap, const wxColour& maskColour )
{
    wxImage img = bitmap.ConvertToImage();
    img.SetMaskColour(maskColour.Red(), maskColour.Green(), maskColour.Blue());
    return Add(wxBitmap(img));
}

// Get the bitmap
wxBitmap wxImageList::GetBitmap(int index) const
{
    wxList::compatibility_iterator node = m_images.Item( index );

    wxCHECK_MSG( node, wxNullBitmap , wxT("wrong index in image list") );

    wxObject* obj = (wxObject*) node->GetData();
    if ( obj == NULL )
        return wxNullBitmap ;
    else if ( obj->IsKindOf(CLASSINFO(wxIcon)) )
        return wxBitmap( *(wx_static_cast(wxIcon*,obj)) ) ;
    else 
        return *(wx_static_cast(wxBitmap*,obj)) ;
}

// Get the icon
wxIcon wxImageList::GetIcon(int index) const
{
    wxList::compatibility_iterator node = m_images.Item( index );

    wxCHECK_MSG( node, wxNullIcon , wxT("wrong index in image list") );

    wxObject* obj = (wxObject*) node->GetData();
    if ( obj == NULL )
        return wxNullIcon ;
    else if( obj->IsKindOf(CLASSINFO(wxBitmap)) )
    {
        wxFAIL_MSG( wxT("cannot convert from bitmap to icon") ) ;
        return wxNullIcon ;
    }
    else 
        return *(wx_static_cast(wxIcon*,obj)) ;
}

bool wxImageList::Replace( int index, const wxBitmap &bitmap )
{
    wxList::compatibility_iterator node = m_images.Item( index );

    wxCHECK_MSG( node, false, wxT("wrong index in image list") );

    wxBitmap* newBitmap = new wxBitmap(bitmap) ;

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

bool wxImageList::Replace( int index, const wxIcon &bitmap )
{
    wxList::compatibility_iterator node = m_images.Item( index );

    wxCHECK_MSG( node, false, wxT("wrong index in image list") );

    wxIcon* newBitmap = new wxIcon(bitmap) ;

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

bool wxImageList::Remove( int index )
{
    wxList::compatibility_iterator node = m_images.Item( index );

    wxCHECK_MSG( node, false, wxT("wrong index in image list") );

    delete node->GetData();
    m_images.Erase( node );

    return true;
}

bool wxImageList::RemoveAll()
{
    WX_CLEAR_LIST(wxList, m_images);
    m_images.Clear();

    return true;
}

bool wxImageList::GetSize( int index, int &width, int &height ) const
{
    width = 0;
    height = 0;

    wxList::compatibility_iterator node = m_images.Item( index );

    wxCHECK_MSG( node, false, wxT("wrong index in image list") );

    wxObject *obj = (wxObject*)node->GetData();
    if( obj->IsKindOf(CLASSINFO(wxIcon)))
    {
        wxIcon *bm = wx_static_cast( wxIcon* , obj ) ;
        width = bm->GetWidth();
        height = bm->GetHeight();
    }
    else
    {
        wxBitmap *bm = wx_static_cast( wxBitmap* , obj ) ;
        width = bm->GetWidth();
        height = bm->GetHeight();
    }
    return true;
}

bool wxImageList::Draw( int index, wxDC &dc, int x, int y,
                        int flags, bool WXUNUSED(solidBackground) )
{
    wxList::compatibility_iterator node = m_images.Item( index );

    wxCHECK_MSG( node, false, wxT("wrong index in image list") );

    wxObject *obj = (wxObject*)node->GetData();
    if( obj->IsKindOf(CLASSINFO(wxIcon)))
    {
        wxIcon *bm = wx_static_cast( wxIcon* , obj ) ;
        dc.DrawIcon( *bm , x, y);
    }
    else
    {
        wxBitmap *bm = wx_static_cast( wxBitmap* , obj ) ;
        dc.DrawBitmap( *bm, x, y, (flags & wxIMAGELIST_DRAW_TRANSPARENT) > 0 );
    }

    return true;
}

#endif // wxUSE_IMAGLIST

