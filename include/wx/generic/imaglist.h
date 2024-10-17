/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/imaglist.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Copyright:   (c) 1998 Robert Roebling and Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAGLISTG_H_
#define _WX_IMAGLISTG_H_

#include "wx/gdicmn.h"
#include "wx/vector.h"

class WXDLLIMPEXP_CORE wxGenericImageList : public wxImageListBase
{
public:
    wxGenericImageList();
    wxGenericImageList( int width, int height, bool mask = true, int initialCount = 1 );
    virtual ~wxGenericImageList();
    bool Create( int width, int height, bool mask = true, int initialCount = 1 );
    virtual void Destroy() override;

    virtual int GetImageCount() const override;
    virtual bool GetSize( int index, int &width, int &height ) const override;

    using wxImageListBase::GetSize;

    virtual int Add( const wxBitmap& bitmap ) override;
    virtual int Add( const wxBitmap& bitmap, const wxBitmap& mask ) override;
    virtual int Add( const wxBitmap& bitmap, const wxColour& maskColour ) override;

    virtual wxBitmap GetBitmap(int index) const override;
    virtual wxIcon GetIcon(int index) const override;
    virtual bool Replace( int index,
                  const wxBitmap& bitmap,
                  const wxBitmap& mask = wxNullBitmap ) override;
    virtual bool Remove( int index ) override;
    virtual bool RemoveAll() override;

    virtual bool Draw(int index, wxDC& dc, int x, int y,
              int flags = wxIMAGELIST_DRAW_NORMAL,
              bool solidBackground = false) override;

#if WXWIN_COMPATIBILITY_3_0
    wxDEPRECATED_MSG("Don't use this overload: it's not portable and does nothing")
    bool Create() { return true; }

    wxDEPRECATED_MSG("Use GetBitmap() instead")
    const wxBitmap *GetBitmapPtr(int index) const { return DoGetPtr(index); }
#endif // WXWIN_COMPATIBILITY_3_0

private:
    const wxBitmap *DoGetPtr(int index) const;

    wxBitmap GetImageListBitmap(const wxBitmap& bitmap) const;

    wxVector<wxBitmap> m_images;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxGenericImageList);
};

#ifndef wxHAS_NATIVE_IMAGELIST

/*
 * wxImageList has to be a real class or we have problems with
 * the run-time information.
 */

class WXDLLIMPEXP_CORE wxImageList: public wxGenericImageList
{
    wxDECLARE_DYNAMIC_CLASS(wxImageList);

public:
    using wxGenericImageList::wxGenericImageList;
};
#endif // !wxHAS_NATIVE_IMAGELIST

#endif  // _WX_IMAGLISTG_H_
