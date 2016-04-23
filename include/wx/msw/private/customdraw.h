///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/customdraw.h
// Purpose:     Helper for implementing custom drawing support in wxMSW
// Author:      Vadim Zeitlin
// Created:     2016-04-16
// Copyright:   (c) 2016 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_CUSTOMDRAW_H_
#define _WX_MSW_CUSTOMDRAW_H_

#include "wx/itemattr.h"

#include "wx/msw/wrapcctl.h"

namespace wxMSWImpl
{

// ----------------------------------------------------------------------------
// CustomDraw: inherit from this class and forward NM_CUSTOMDRAW to it
// ----------------------------------------------------------------------------

class CustomDraw
{
public:
    // Trivial default ctor needed for non-copyable class.
    CustomDraw()
    {
    }

    // Virtual dtor for the base class.
    virtual ~CustomDraw()
    {
    }

    // Implementation of NM_CUSTOMDRAW handler, returns one of CDRF_XXX
    // constants, possibly CDRF_DODEFAULT if custom drawing is not necessary.
    LPARAM HandleCustomDraw(LPARAM lParam);

private:
    // Return true if we need custom drawing at all.
    virtual bool HasCustomDrawnItems() const = 0;

    // Return the attribute to use for the given item, can return NULL if this
    // item doesn't need to be custom-drawn.
    virtual const wxItemAttr* GetItemAttr(DWORD_PTR dwItemSpec) const = 0;


    // Set the colours and font for the specified HDC, return CDRF_NEWFONT if
    // the font was changed.
    LPARAM HandleItemPrepaint(const wxItemAttr& attr, HDC hdc);

    wxDECLARE_NO_COPY_CLASS(CustomDraw);
};

} // namespace wxMSWImpl

#endif // _WX_MSW_CUSTOMDRAW_H_
