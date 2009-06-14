/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/bmpbuttn.h
// Purpose:     wxBitmapButton class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BMPBUTTN_H_
#define _WX_BMPBUTTN_H_

#include "wx/button.h"
#include "wx/bitmap.h"
#include "wx/brush.h"

class WXDLLIMPEXP_CORE wxBitmapButton : public wxBitmapButtonBase
{
public:
    wxBitmapButton() { Init(); }

    wxBitmapButton(wxWindow *parent,
                   wxWindowID id,
                   const wxBitmap& bitmap,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxBU_AUTODRAW,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxButtonNameStr)
    {
        Init();

        Create(parent, id, bitmap, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxBitmap& bitmap,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxBU_AUTODRAW,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxButtonNameStr);

    // Implementation
    virtual bool SetBackgroundColour(const wxColour& colour);
    virtual bool MSWOnDraw(WXDRAWITEMSTRUCT *item);
    virtual void DrawFace( WXHDC dc, int left, int top, int right, int bottom, bool sel );
    virtual void DrawButtonFocus( WXHDC dc, int left, int top, int right, int bottom, bool sel );
    virtual void DrawButtonDisable( WXHDC dc, int left, int top, int right, int bottom, bool with_marg );

protected:
    // common part of all ctors
    void Init()
    {
        m_disabledSetByUser =
        m_hoverSetByUser = false;
    }

    // reimplement some base class virtuals
    virtual wxSize DoGetBestSize() const;
    virtual WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const;

    virtual void DoSetBitmap(const wxBitmap& bitmap, State which);


    // invalidate m_brushDisabled when system colours change
    void OnSysColourChanged(wxSysColourChangedEvent& event);

    // change the currently bitmap if we have a hover one
    void OnMouseEnterOrLeave(wxMouseEvent& event);


    // the brush we use to draw disabled buttons
    wxBrush m_brushDisabled;

    // true if disabled bitmap was set by user, false if we created it
    // ourselves from the normal one
    bool m_disabledSetByUser;

    // true if hover bitmap was set by user, false if it was set from focused
    // one
    bool m_hoverSetByUser;


    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxBitmapButton)
};

#endif // _WX_BMPBUTTN_H_
