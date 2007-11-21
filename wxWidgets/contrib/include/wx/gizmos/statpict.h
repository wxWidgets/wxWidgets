/////////////////////////////////////////////////////////////////////////////
// Name:        statpict.h
// Purpose:     wxStaticPicture class
// Author:      Wade Brainerd
// Modified by:
// Created:     2003-05-01
// RCS-ID:
// Copyright:   (c) Wade Brainerd
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STATPICT_H_
#define _WX_STATPICT_H_

#include "wx/control.h"

#include "wx/icon.h"
#include "wx/bitmap.h"
#include "wx/image.h"

#include "wx/gizmos/gizmos.h"

enum
{
    wxSCALE_HORIZONTAL = 0x1,
    wxSCALE_VERTICAL   = 0x2,
    wxSCALE_UNIFORM    = 0x4,
    wxSCALE_CUSTOM     = 0x8
};

WXDLLIMPEXP_GIZMOS extern const wxChar* wxStaticPictureNameStr;

class WXDLLIMPEXP_GIZMOS wxStaticPicture : public wxControl
{
    DECLARE_DYNAMIC_CLASS(wxStaticPicture)

public:
    wxStaticPicture() {}

    wxStaticPicture( wxWindow* parent, wxWindowID id,
        const wxBitmap& label,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxStaticPictureNameStr )
    {
        Create( parent, id, label, pos, size, style, name );
    }

    bool Create( wxWindow* parent, wxWindowID id,
        const wxBitmap& label,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxStaticPictureNameStr );

    virtual void Command(wxCommandEvent& WXUNUSED(event)) {}
    virtual bool ProcessCommand(wxCommandEvent& WXUNUSED(event)) {return true;}
    void OnPaint(wxPaintEvent& event);

    void SetBitmap( const wxBitmap& bmp );

    wxBitmap GetBitmap() const
    {
        return Bitmap;
    }

    // Icon interface for compatibility with wxStaticBitmap.
    void SetIcon( const wxIcon& icon )
    {
        wxBitmap bmp;
        bmp.CopyFromIcon( icon );
        SetBitmap( bmp );
    }

    wxIcon GetIcon() const
    {
        wxIcon icon;
        icon.CopyFromBitmap( Bitmap );
        return icon;
    }

    void SetAlignment( int align )
    {
        Align = align;
    }

    int GetAlignment() const
    {
        return Align;
    }

    void SetScale( int scale )
    {
        Scale = scale;
    }

    int GetScale() const
    {
        return Scale;
    }

    void SetCustomScale( float sx, float sy )
    {
        ScaleX = sx;
        ScaleY = sy;
    }

    void GetCustomScale( float* sx, float* sy ) const
    {
        *sx = ScaleX;
        *sy = ScaleY;
    }

protected:
    wxBitmap Bitmap;

    int Align;

    int Scale;
    float ScaleX;
    float ScaleY;

#ifndef __WXMSW__
    // When scaling is enabled, measures are taken to improve performance on non-Windows platforms.
    // - The original bitmap is stored as a wxImage, because conversion from wxBitmap to wxImage is slow.
    // - The latest scaled bitmap is cached, this improves performance when the control is repainted
    //   but the size hasn't changed (overlapping windows, movement, etc).
    wxImage OriginalImage;
    float LastScaleX;
    float LastScaleY;
    wxBitmap ScaledBitmap;
#endif

    DECLARE_EVENT_TABLE()
};

#endif // #ifndef _WX_STATPICT_H_
