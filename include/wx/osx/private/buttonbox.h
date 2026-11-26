/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/private/buttonbox.h
// Purpose:     Interface of the native peer of wxButtonBox in wxOSX
// Author:      Wlodzimierz ABX Skiba, Assisted-by: Claude Sonnet 5.5
// Created:     22.11.26
// Copyright:   (c) 2026 Wlodzimierz ABX Skiba
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OSX_PRIVATE_BUTTONBOX_H_
#define _WX_OSX_PRIVATE_BUTTONBOX_H_

#include "wx/buttonbox.h"

// Mixin interface implemented by the native peers of wxButtonBox: currently
// only the Cocoa one, see src/osx/cocoa/buttonbox.mm. It is used by the common
// wxOSX code in src/osx/buttonbox_osx.cpp.
class wxButtonBoxImpl
{
public:
    virtual ~wxButtonBoxImpl() { }

    // Note that the item functions have names different from the ones of
    // wxWidgetImpl, which wxButtonBoxCocoaImpl also derives from, in order not
    // to hide its virtual functions.

    virtual void SetMode(long style) = 0;
    virtual size_t GetCount() const = 0;
    virtual void Insert(size_t pos, const wxString& label,
                        const wxBitmapBundle& bitmap) = 0;
    virtual void Delete(size_t pos) = 0;
    virtual int  GetSelection() const = 0;          // -1 if none
    virtual bool IsSelected(size_t n) const = 0;
    virtual void SetSelected(size_t n, bool selected) = 0;
    virtual void ClearSelection() = 0;
    virtual void SetItemLabel(size_t n, const wxString& label) = 0;
    virtual wxString GetItemLabel(size_t n) const = 0;
    virtual void SetItemBitmap(size_t n, const wxBitmapBundle& bitmap) = 0;
    virtual void SetItemWidth(size_t n, int width) = 0;
    virtual int  GetItemWidth(size_t n) const = 0;
    virtual void EnableItem(size_t n, bool enable) = 0;
    virtual bool IsItemEnabled(size_t n) const = 0;
    virtual void SetItemToolTip(size_t n, const wxString& tip) = 0;
    virtual wxString GetItemToolTip(size_t n) const = 0;
};

#endif // _WX_OSX_PRIVATE_BUTTONBOX_H_
