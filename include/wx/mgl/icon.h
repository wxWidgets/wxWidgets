/////////////////////////////////////////////////////////////////////////////
// Name:        icon.h
// Purpose:
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001-2002 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __WX_ICON_H__
#define __WX_ICON_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "icon.h"
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/bitmap.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxIcon;

//-----------------------------------------------------------------------------
// wxIcon
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxIcon: public wxBitmap
{
public:
    wxIcon() : wxBitmap() {}
    wxIcon(const wxIcon& icon);
    wxIcon(const char **bits, int width=-1, int height=-1);
    wxIcon(char **bits, int width=-1, int height=-1);

    // For compatibility with wxMSW where desired size is sometimes required to
    // distinguish between multiple icons in a resource.
    wxIcon(const wxString& filename, int type = wxBITMAP_TYPE_ICO_RESOURCE,
           int WXUNUSED(desiredWidth)=-1, int WXUNUSED(desiredHeight)=-1 ) :
           wxBitmap(filename, (wxBitmapType)type) {}

    wxIcon(const wxIconLocation& loc)
        : wxBitmap(loc.GetFileName(), wxBITMAP_TYPE_ANY)
    {
    }

    wxIcon& operator=(const wxIcon& icon);
    bool operator==(const wxIcon& icon) const { return m_refData == icon.m_refData; }
    bool operator!=(const wxIcon& icon) const { return !(*this == icon); }

    // create from bitmap (which should have a mask unless it's monochrome):
    // there shouldn't be any implicit bitmap -> icon conversion (i.e. no
    // ctors, assignment operators...), but it's ok to have such function
    void CopyFromBitmap(const wxBitmap& bmp);

private:
    DECLARE_DYNAMIC_CLASS(wxIcon)
};


#endif // __WX_ICON_H__
