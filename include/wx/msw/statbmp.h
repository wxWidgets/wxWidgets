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

#include "wx/control.h"
#include "wx/icon.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxStaticBitmapNameStr;

// a control showing an icon or a bitmap
class WXDLLEXPORT wxStaticBitmap : public wxControl
{
    DECLARE_DYNAMIC_CLASS(wxStaticBitmap)

public:
    wxStaticBitmap() { Init(); }

    wxStaticBitmap(wxWindow *parent,
                   wxWindowID id,
                   const wxBitmap& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxString& name = wxStaticBitmapNameStr)
    {
        Create(parent, id, label, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxBitmap& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxStaticBitmapNameStr);

    virtual ~wxStaticBitmap() { Free(); }

    virtual void SetIcon(const wxIcon& icon) { SetBitmap(icon); }
    virtual void SetBitmap(const wxBitmap& bitmap);

    // assert failure is provoked by an attempt to get an icon from bitmap or
    // vice versa
    const wxIcon& GetIcon() const
        { wxASSERT( m_isIcon ); return *m_image.icon; }
    const wxBitmap& GetBitmap() const
        { wxASSERT( !m_isIcon ); return *m_image.bitmap; }

    // overriden base class virtuals
    virtual bool AcceptsFocus() const { return FALSE; }

    // IMPLEMENTATION
#ifdef __WIN16__
    virtual bool MSWOnDraw(WXDRAWITEMSTRUCT *item);
    virtual long MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
#endif // __WIN16__

protected:
    void Init() { m_isIcon = TRUE; m_image.icon = NULL; }
    void Free();

    // TRUE if icon/bitmap is valid
    bool ImageIsOk() const;

    // we can have either an icon or a bitmap
    bool m_isIcon;
    union
    {
        wxIcon *icon;
        wxBitmap *bitmap;
    } m_image;

    virtual wxSize DoGetBestSize() const;
};

#endif
    // _WX_STATBMP_H_
