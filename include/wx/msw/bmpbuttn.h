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

#include "wx/button.h"
#include "wx/bitmap.h"

#define wxDEFAULT_BUTTON_MARGIN 4

class WXDLLEXPORT wxBitmapButton: public wxBitmapButtonBase
{
public:
    wxBitmapButton()
        { m_marginX = m_marginY = wxDEFAULT_BUTTON_MARGIN; }
    wxBitmapButton(wxWindow *parent,
                   wxWindowID id,
                   const wxBitmap& bitmap,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxBU_AUTODRAW,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxButtonNameStr)
    {
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

#if WXWIN_COMPATIBILITY
    wxBitmap *GetBitmap() const { return (wxBitmap *) & m_bmp; }
#endif

    // Implementation
    virtual void SetDefault();
    virtual bool MSWOnDraw(WXDRAWITEMSTRUCT *item);
    virtual void DrawFace( WXHDC dc, int left, int top, int right, int bottom, bool sel );
    virtual void DrawButtonFocus( WXHDC dc, int left, int top, int right, int bottom, bool sel );
    virtual void DrawButtonDisable( WXHDC dc, int left, int top, int right, int bottom, bool with_marg );

private:
    DECLARE_DYNAMIC_CLASS(wxBitmapButton)
};

#endif
    // _WX_BMPBUTTN_H_
