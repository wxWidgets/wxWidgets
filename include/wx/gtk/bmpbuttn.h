/////////////////////////////////////////////////////////////////////////////
// Name:        bmpbutton.h
// Purpose:
// Author:      Robert Roebling
// Id:          $id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __BMPBUTTONH__
#define __BMPBUTTONH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/list.h"
#include "wx/control.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxBitmapButton;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const char *wxButtonNameStr;

//-----------------------------------------------------------------------------
// wxBitmapButton
//-----------------------------------------------------------------------------

class wxBitmapButton: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxBitmapButton)

  public:

    wxBitmapButton(void);
    inline wxBitmapButton(wxWindow *parent, wxWindowID id, const wxBitmap& bitmap,
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
    void SetDefault(void);
    void SetLabel( const wxString &label );
    wxString GetLabel(void) const;
    virtual void SetLabel(const wxBitmap& bitmap) { SetBitmapLabel(bitmap); }
    virtual void SetBitmapLabel( const wxBitmap& bitmap );
    wxBitmap& GetBitmapLabel(void) const { return (wxBitmap&) m_bitmap; }
    
  public:
  
    wxBitmap   m_bitmap;    
    
};
#endif // __BMPBUTTONH__
