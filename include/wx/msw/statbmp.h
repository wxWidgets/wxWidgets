/////////////////////////////////////////////////////////////////////////////
// Name:        statbmp.h
// Purpose:     wxStaticBitmap class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STATBMP_H_
#define _WX_STATBMP_H_

#ifdef __GNUG__
    #pragma interface "statbmp.h"
#endif

#if wxUSE_STATBMP

#include "wx/control.h"
#include "wx/icon.h"
#include "wx/bitmap.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxStaticBitmapNameStr;

// a control showing an icon or a bitmap
class WXDLLEXPORT wxStaticBitmap : public wxControl
{
    DECLARE_DYNAMIC_CLASS(wxStaticBitmap)

public:
    wxStaticBitmap() { Init(); }

    wxStaticBitmap(wxWindow *parent,
                   wxWindowID id,
                   const wxGDIImage& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxString& name = wxStaticBitmapNameStr)
    {
        Create(parent, id, label, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxGDIImage& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxStaticBitmapNameStr);

    virtual ~wxStaticBitmap() { Free(); }

    void SetIcon(const wxIcon& icon) { SetImage(&icon); }
    void SetBitmap(const wxBitmap& bitmap) { SetImage(&bitmap); }

    // assert failure is provoked by an attempt to get an icon from bitmap or
    // vice versa
    const wxIcon& GetIcon() const
        { wxASSERT( m_isIcon ); return *(wxIcon *)m_image; }
    const wxBitmap& GetBitmap() const
        { wxASSERT( !m_isIcon ); return *(wxBitmap *)m_image; }

    // overriden base class virtuals
    virtual bool AcceptsFocus() const { return FALSE; }

    // IMPLEMENTATION
#ifdef __WIN16__
    virtual bool MSWOnDraw(WXDRAWITEMSTRUCT *item);
#endif // __WIN16__
    virtual long MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

protected:
    virtual wxSize DoGetBestSize() const;

    void Init() { m_isIcon = TRUE; m_image = NULL; }
    void Free();

    // TRUE if icon/bitmap is valid
    bool ImageIsOk() const;

    void SetImage(const wxGDIImage* image);

    // we can have either an icon or a bitmap
    bool m_isIcon;
    wxGDIImage *m_image;
};

#endif // wxUSE_STATBMP

#endif
    // _WX_STATBMP_H_
