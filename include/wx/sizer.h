/////////////////////////////////////////////////////////////////////////////
// Name:        sizer.h
// Purpose:     provide wxNewSizer class for layounting
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

class wxNewSizerItem;
class wxNewSizer;
class wxBoxNewSizer;

//---------------------------------------------------------------------------
// wxNewSizerItem
//---------------------------------------------------------------------------

class WXDLLEXPORT wxNewSizerItem: public wxObject
{
public:
  // spacer
  wxNewSizerItem( int width, int height, int option );

  // window
  wxNewSizerItem( wxWindow *window, int option );

  // subsizer
  wxNewSizerItem( wxNewSizer *sizer, int option );

  virtual wxSize GetSize();
  virtual wxSize CalcMin();
  virtual void SetDimension( wxPoint pos, wxSize size );
  
  bool IsWindow();
  bool IsNewSizer();
  bool IsSpacer();
  
  wxWindow *GetWindow() const  
    { return m_window; }
  wxNewSizer *GetNewSizer() const    
    { return m_sizer; }
  int GetOption() const
    { return m_option; }
  
protected:
  wxWindow    *m_window;
  wxNewSizer     *m_sizer;
  wxSize       m_minSize;
  int          m_option;
};

//---------------------------------------------------------------------------
// wxNewSizer
//---------------------------------------------------------------------------

class WXDLLEXPORT wxNewSizer: public wxObject
{
public:
   wxNewSizer();
   ~wxNewSizer();
   
   virtual void Add( wxWindow *window, int option = 0 );
   virtual void Add( wxNewSizer *sizer, int option = 0  );
   virtual void Add( int width, int height, int option = 0  );
  
   void SetDimension( int x, int y, int width, int height );
  
   wxSize GetSize()
     { return m_size; }
   wxPoint GetPosition()
     { return m_position; }
   wxSize GetMinSize()
     { return CalcMin(); }

   virtual void RecalcSizes() = 0;
   virtual wxSize CalcMin() = 0;
   
   virtual void Layout();

   void Fit( wxWindow *window );
   void SetSizeHints( wxWindow *window );
   
protected:
   wxSize  m_size;
   wxPoint m_position;
   wxList  m_children;
   
   wxSize GetMinWindowSize( wxWindow *window );
};

//---------------------------------------------------------------------------
// wxBorderNewSizer
//---------------------------------------------------------------------------

#define wxWEST  wxLEFT
#define wxEAST  wxRIGHT
#define wxNORTH wxUP
#define wxSOUTH wxDOWN
#define wxALL_DIRECTIONS (wxNORTH | wxSOUTH | wxEAST | wxWEST)

class WXDLLEXPORT wxBorderNewSizer: public wxNewSizer
{
public:
   wxBorderNewSizer( int sides = wxALL_DIRECTIONS );
   
   virtual void Add( wxWindow *window, int option = 10 );
   virtual void Add( wxNewSizer *sizer, int option = 10 );
   virtual void Add( int width, int height, int option = 10 );
   
   void RecalcSizes();
   wxSize CalcMin();
   
   int GetSides()
     { return m_sides; }
   
protected:
   int    m_sides;
};
  

#endif
  // __WXSIZER_H__
