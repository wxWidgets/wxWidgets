/////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/icon.h
// Purpose:     wxIcon class
// Author:      David Elliott
// Modified by:
// Created:     2003/08/11
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COCOA_ICON_H__
#define _WX_COCOA_ICON_H__

#include "wx/gdicmn.h"
#include "wx/gdiobj.h"

// ========================================================================
// wxIcon
// ========================================================================
class WXDLLEXPORT wxIcon: public wxGDIObject
{
    DECLARE_DYNAMIC_CLASS(wxIcon)
public:
    wxIcon();

    wxIcon(const char **data) { CreateFromXpm(data); }
    wxIcon(char **data) { CreateFromXpm((const char**)data); }
    wxIcon(const char bits[], int width , int height );
    wxIcon(const wxString& name, int flags = wxBITMAP_TYPE_ICON_RESOURCE,
        int desiredWidth = -1, int desiredHeight = -1);
    wxIcon(const wxIconLocation& loc)
    {
        LoadFile(loc.GetFileName(), wxBITMAP_TYPE_ICON);
    }
    virtual ~wxIcon();

    bool LoadFile(const wxString& name, wxBitmapType flags /* = wxBITMAP_TYPE_ICON_RESOURCE */ ,
        int desiredWidth /* = -1 */ , int desiredHeight = -1);
    bool LoadFile(const wxString& name, wxBitmapType flags = wxBITMAP_TYPE_ICON_RESOURCE )
    {   return LoadFile( name , flags , -1 , -1 ) ; }

    bool operator==(const wxIcon& icon) const
    {   return m_refData == icon.m_refData; }
    bool operator!=(const wxIcon& icon) const { return !(*this == icon); }

    // create from bitmap (which should have a mask unless it's monochrome):
    // there shouldn't be any implicit bitmap -> icon conversion (i.e. no
    // ctors, assignment operators...), but it's ok to have such function
    void CopyFromBitmap(const wxBitmap& bmp);

    bool Ok() const;
    int GetWidth() const;
    int GetHeight() const;

    WX_NSImage GetNSImage() const;
    bool CreateFromXpm(const char **bits);
};

#endif // _WX_COCOA_ICON_H__
