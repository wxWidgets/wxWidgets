/////////////////////////////////////////////////////////////////////////////
// Name:        bmpbuttn.h
// Purpose:     wxBitmapButton class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BMPBUTTN_H_
#define _WX_BMPBUTTN_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "bmpbuttn.h"
#endif

#include "wx/button.h"

WXDLLEXPORT_DATA(extern const char*) wxButtonNameStr;

#define wxDEFAULT_BUTTON_MARGIN 4

class WXDLLEXPORT wxBitmapButton: public wxBitmapButtonBase
{
    DECLARE_DYNAMIC_CLASS(wxBitmapButton)

public:
    wxBitmapButton()
        {
            SetMargins(wxDEFAULT_BUTTON_MARGIN, wxDEFAULT_BUTTON_MARGIN);
        }
    
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

    virtual void SetBitmapLabel(const wxBitmap& bitmap);

/*
  // TODO: Implementation
  virtual bool MSWOnDraw(WXDRAWITEMSTRUCT *item);
  virtual void DrawFace( WXHDC dc, int left, int top, int right, int bottom, bool sel );
  virtual void DrawButtonFocus( WXHDC dc, int left, int top, int right, int bottom, bool sel );
  virtual void DrawButtonDisable( WXHDC dc, int left, int top, int right, int bottom, bool with_marg );
*/
};

#endif
    // _WX_BMPBUTTN_H_
