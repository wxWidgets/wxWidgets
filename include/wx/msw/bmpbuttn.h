/////////////////////////////////////////////////////////////////////////////
// Name:        bmpbuttn.h
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

#ifdef __GNUG__
#pragma interface "bmpbuttn.h"
#endif

#if wxUSE_BMPBUTTON

#include "wx/button.h"
#include "wx/bitmap.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxButtonNameStr;

#define wxDEFAULT_BUTTON_MARGIN 4

class WXDLLEXPORT wxBitmapButton: public wxButton
{
    DECLARE_DYNAMIC_CLASS(wxBitmapButton)

public:
    wxBitmapButton() { m_marginX = wxDEFAULT_BUTTON_MARGIN; m_marginY = wxDEFAULT_BUTTON_MARGIN; }
    wxBitmapButton(wxWindow *parent, wxWindowID id, const wxBitmap& bitmap,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxBU_AUTODRAW,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxButtonNameStr)
    {
        Create(parent, id, bitmap, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent, wxWindowID id, const wxBitmap& bitmap,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxBU_AUTODRAW,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxButtonNameStr);

    virtual void SetLabel(const wxBitmap& bitmap)
    {
        SetBitmapLabel(bitmap);
    }

    virtual void SetBitmapLabel(const wxBitmap& bitmap);

#if WXWIN_COMPATIBILITY
    wxBitmap *GetBitmap() const { return (wxBitmap *) & m_buttonBitmap; }
#endif

    wxBitmap& GetBitmapLabel() const { return (wxBitmap&) m_buttonBitmap; }
    wxBitmap& GetBitmapSelected() const { return (wxBitmap&) m_buttonBitmapSelected; }
    wxBitmap& GetBitmapFocus() const { return (wxBitmap&) m_buttonBitmapFocus; }
    wxBitmap& GetBitmapDisabled() const { return (wxBitmap&) m_buttonBitmapDisabled; }

    void SetBitmapSelected(const wxBitmap& sel) { m_buttonBitmapSelected = sel; };
    void SetBitmapFocus(const wxBitmap& focus) { m_buttonBitmapFocus = focus; };
    void SetBitmapDisabled(const wxBitmap& disabled) { m_buttonBitmapDisabled = disabled; };

    void SetMargins(int x, int y) { m_marginX = x; m_marginY = y; }
    int GetMarginX() const { return m_marginX; }
    int GetMarginY() const { return m_marginY; }

    // Implementation
    virtual void SetDefault();
    virtual bool MSWOnDraw(WXDRAWITEMSTRUCT *item);
    virtual void DrawFace( WXHDC dc, int left, int top, int right, int bottom, bool sel );
    virtual void DrawButtonFocus( WXHDC dc, int left, int top, int right, int bottom, bool sel );
    virtual void DrawButtonDisable( WXHDC dc, int left, int top, int right, int bottom, bool with_marg );

protected:
    wxBitmap m_buttonBitmap;
    wxBitmap m_buttonBitmapSelected;
    wxBitmap m_buttonBitmapFocus;
    wxBitmap m_buttonBitmapDisabled;
    int      m_marginX;
    int      m_marginY;
};

#endif // wxUSE_BMPBUTTON

#endif
    // _WX_BMPBUTTN_H_
