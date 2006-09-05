/////////////////////////////////////////////////////////////////////////////
// Name:        wx/motif/icon.h
// Purpose:     wxIcon class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ICON_H_
#define _WX_ICON_H_

#include "wx/bitmap.h"

// Icon
class WXDLLEXPORT wxIcon : public wxBitmap
{
public:
    wxIcon();

    // Initialize with XBM data
    wxIcon(const char bits[], int width, int height);

    // Initialize with XPM data
    wxIcon(const char **data);
    wxIcon(char **data);

    wxIcon(const wxString& name, wxBitmapType type = wxBITMAP_TYPE_XPM,
           int desiredWidth = -1, int desiredHeight = -1)
    {
        LoadFile(name, type, desiredWidth, desiredHeight);
    }

    wxIcon(const wxIconLocation& loc)
    {
        LoadFile(loc.GetFileName(), wxBITMAP_TYPE_ANY);
    }

    virtual ~wxIcon();

    bool LoadFile(const wxString& name, wxBitmapType type,
                  int desiredWidth, int desiredHeight = -1);

    // unhide base class LoadFile()
    virtual bool LoadFile(const wxString& name,
                          wxBitmapType type = wxBITMAP_TYPE_XPM)
    {
        return LoadFile(name, type, -1, -1);
    }

    // create from bitmap (which should have a mask unless it's monochrome):
    // there shouldn't be any implicit bitmap -> icon conversion (i.e. no
    // ctors, assignment operators...), but it's ok to have such function
    void CopyFromBitmap(const wxBitmap& bmp);

    bool operator == (const wxIcon& icon) const
        { return m_refData == icon.m_refData; }
    bool operator != (const wxIcon& icon) const
        { return !(*this == icon); }


    DECLARE_DYNAMIC_CLASS(wxIcon)
};

#endif // _WX_ICON_H_
