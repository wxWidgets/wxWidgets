/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/icon.h
// Purpose:     wxIcon class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ICON_H_
#define _WX_ICON_H_

#ifdef __GNUG__
    #pragma interface "icon.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// compatible (even if incorrect) behaviour by default: derive wxIcon from
// wxBitmap
#ifndef wxICON_IS_BITMAP
    #define wxICON_IS_BITMAP 1
#endif

#if wxICON_IS_BITMAP
    #include "wx/bitmap.h"

    #define wxIconRefDataBase   wxBitmapRefData
    #define wxIconBase          wxBitmap
#else
    #include "wx/msw/gdiimage.h"

    #define wxIconRefDataBase   wxGDIImageRefData
    #define wxIconBase          wxGDIImage
#endif

// ---------------------------------------------------------------------------
// icon data
// ---------------------------------------------------------------------------

// notice that although wxIconRefData inherits from wxBitmapRefData, it is not
// a valid wxBitmapRefData
class WXDLLEXPORT wxIconRefData : public wxIconRefDataBase
{
public:
    wxIconRefData() { }
    virtual ~wxIconRefData() { Free(); }

    virtual void Free();
};

// ---------------------------------------------------------------------------
// Icon
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxIcon : public wxIconBase
{
public:
    wxIcon();

    // Copy constructors
    wxIcon(const wxIcon& icon) { Ref(icon); }

    wxIcon(const char bits[], int width, int height);
    wxIcon(const wxString& name,
           long type = wxBITMAP_TYPE_ICO_RESOURCE,
           int desiredWidth = -1, int desiredHeight = -1);
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

    wxIconRefData *GetIconData() const { return (wxIconRefData *)m_refData; }

    void SetHICON(WXHICON icon) { SetHandle((WXHANDLE)icon); }
    WXHICON GetHICON() const { return (WXHICON)GetHandle(); }

protected:
    virtual wxGDIImageRefData *CreateData() const
    {
        return new wxIconRefData;
    }

private:
    DECLARE_DYNAMIC_CLASS(wxIcon)
};

#endif
    // _WX_ICON_H_
