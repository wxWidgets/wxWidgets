/////////////////////////////////////////////////////////////////////////////
// Name:        sizer.h
// Purpose:     provide wxSizer class for layounting
// Author:      Robert Roebling and Robin Dunn
// Modified by:
// Created:     
// RCS-ID:      $Id$
// Copyright:   (c) Robin Dunn, Dirk Holtwick and Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WXSIZER_H__
#define __WXSIZER_H__

#ifdef __GNUG__
#pragma interface "sizer.h"
#endif

#include "wx/defs.h"

#include "wx/window.h"
#include "wx/frame.h"
#include "wx/dialog.h"

//---------------------------------------------------------------------------
// classes
//---------------------------------------------------------------------------

class wxSizerItem;
class wxSizer;
class wxBoxSizer;

//---------------------------------------------------------------------------
// wxSizerItem
//---------------------------------------------------------------------------

class WXDLLEXPORT wxSizerItem: wxObject
{
public:
  // spacer
  wxSizerItem( int width, int height, int option )

  // window
  wxSizerItem( wxWindow *window, int option );

  // subsizer
  wxSizerItem( wxSizer *sizer, int option );

  virtual wxSize GetMinSize();
  
  bool IsWindow();
  bool IsSizer();
  bool IsSpacer();
  
  wxWindow *GetWindow() const  
    { return m_window; }
  wxSizer *GetSizer() const    
    { return m_sizer; }
  int GetOption() const
    { return m_option; }
  
private:
  wxWindow    *m_window;
  wxSizer     *m_sizer;
  wxSize       m_minSize;
  int          m_option;
}

//---------------------------------------------------------------------------
// wxSizer
//---------------------------------------------------------------------------

class WXDLLEXPORT wxSizer: wxObject
{
public:
   wxSizer()
   ~wxSizer()
   
   virtual void Add( wxWindow *window, int option = 0 );
   virtual void Add( wxSizer *sizer, int option = 0  );
   virtual void Add( int width, int height, int option = 0  );
  
   void SetDimension( int x, int y, int width, int height )
     { DoSetDimension( x, y, width, height ); }
  
   wxSize GetSize()
     { return m_size; }
   wxPoint GetPosition()
     { return m_position; }
   wxSize GetMinSize()
     { return CalcMin(); }
     
   virtual void RecalcSizes() = 0;
   virtual wxSize CalcMin() = 0;

   void Fit( wxWindow *window );
   void SetSizeHints( wxWindow *window );
   
private:
   wxSize  m_size;
   wxPoint m_position;
   wxList  m_children;
   
   wxSize GetMinWindowSize( wxWindow *window );
   virtual void DoSetDimension( int x, int y, int width, int height );
}

#endif
  // __WXSIZER_H__
