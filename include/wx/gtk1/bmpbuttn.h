/////////////////////////////////////////////////////////////////////////////
// Name:        bmpbutton.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __BMPBUTTONH__
#define __BMPBUTTONH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"

#if wxUSE_BMPBUTTON

#include "wx/object.h"
#include "wx/list.h"
#include "wx/control.h"
#include "wx/bitmap.h"

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
  wxBitmapButton();
  inline wxBitmapButton( wxWindow *parent, wxWindowID id, const wxBitmap& bitmap,
                         const wxPoint& pos = wxDefaultPosition,
                         const wxSize& size = wxDefaultSize, long style = wxBU_AUTODRAW,
                         const wxValidator& validator = wxDefaultValidator,
                         const wxString& name = wxButtonNameStr )
  {
      Create(parent, id, bitmap, pos, size, style, validator, name);
  }
  bool Create( wxWindow *parent, wxWindowID id, const wxBitmap& bitmap,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize, long style = wxBU_AUTODRAW,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxButtonNameStr);
  void SetDefault();
    
  void SetLabel( const wxString &label );
  wxString GetLabel() const;
  virtual void SetLabel( const wxBitmap& bitmap ) { SetBitmapLabel(bitmap); }
    
  wxBitmap& GetBitmapDisabled() const { return (wxBitmap&) m_disabled; }
  wxBitmap& GetBitmapFocus() const { return (wxBitmap&) m_focus; }
  wxBitmap& GetBitmapLabel() const { return (wxBitmap&) m_bitmap; }
  wxBitmap& GetBitmapSelected() const { return (wxBitmap&) m_selected; }
    
  void SetBitmapDisabled( const wxBitmap& bitmap );
  void SetBitmapFocus( const wxBitmap& bitmap );
  void SetBitmapLabel( const wxBitmap& bitmap );
  void SetBitmapSelected( const wxBitmap& bitmap );
    
  virtual bool Enable(const bool);
  
// implementation

  void HasFocus();
  void NotFocus();
  void StartSelect();
  void EndSelect();
  void SetBitmap();
  void ApplyWidgetStyle();    
  
  bool         m_hasFocus;
  bool         m_isSelected;
  wxBitmap     m_bitmap; 
  wxBitmap     m_disabled;
  wxBitmap     m_focus;
  wxBitmap     m_selected;   
};

#endif

#endif // __BMPBUTTONH__
