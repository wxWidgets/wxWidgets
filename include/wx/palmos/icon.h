/////////////////////////////////////////////////////////////////////////////
// Name:        wx/palmos/icon.h
// Purpose:     wxIcon class
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ICON_H_
#define _WX_ICON_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "icon.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/palmos/gdiimage.h"

// ---------------------------------------------------------------------------
// icon data
// ---------------------------------------------------------------------------

// notice that although wxIconRefData inherits from wxBitmapRefData, it is not
// a valid wxBitmapRefData
class WXDLLEXPORT wxIconRefData : public wxGDIImageRefData
{
public:
    wxIconRefData() { }
    virtual ~wxIconRefData() { Free(); }

    virtual void Free();
};

// ---------------------------------------------------------------------------
// Icon
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxIcon : public wxGDIImage
{
public:
    // ctors
        // default
    wxIcon() { }

        // copy
    wxIcon(const wxIcon& icon) { Ref(icon); }

        // from raw data
    wxIcon(const char bits[], int width, int height);

        // from XPM data
    wxIcon(const char **data) { CreateIconFromXpm(data); }

    wxIcon(char **data) { CreateIconFromXpm((const char **)data); }

        // from resource/file
    wxIcon(const wxString& name,
           long type = wxBITMAP_TYPE_ICO_RESOURCE,
           int desiredWidth = -1, int desiredHeight = -1);

    wxIcon(const wxIconLocation& loc);

    virtual ~wxIcon();

    virtual bool LoadFile(const wxString& name,
                          long type = wxBITMAP_TYPE_ICO_RESOURCE,
                          int desiredWidth = -1, int desiredHeight = -1);

    wxIcon& operator = (const wxIcon& icon)
        { if ( *this != icon ) Ref(icon); return *this; }
    bool operator == (const wxIcon& icon) const
        { return m_refData == icon.m_refData; }
    bool operator != (const wxIcon& icon) const
        { return m_refData != icon.m_refData; }

    // implementation only from now on
    wxIconRefData *GetIconData() const { return (wxIconRefData *)m_refData; }

    void SetHICON(WXHICON icon) { SetHandle((WXHANDLE)icon); }
    WXHICON GetHICON() const { return (WXHICON)GetHandle(); }

    // create from bitmap (which should have a mask unless it's monochrome):
    // there shouldn't be any implicit bitmap -> icon conversion (i.e. no
    // ctors, assignment operators...), but it's ok to have such function
    void CopyFromBitmap(const wxBitmap& bmp);

protected:
    virtual wxGDIImageRefData *CreateData() const
    {
        return new wxIconRefData;
    }

    // create from XPM data
    void CreateIconFromXpm(const char **data);

private:
    DECLARE_DYNAMIC_CLASS(wxIcon)
};

#endif
    // _WX_ICON_H_
