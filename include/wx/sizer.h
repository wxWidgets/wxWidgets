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
class wxBorderNewSizer;
class wxBoxNewSizer;

//---------------------------------------------------------------------------
// wxNewSizerItem
//---------------------------------------------------------------------------

class WXDLLEXPORT wxNewSizerItem: public wxObject
{
public:
  // spacer
  wxNewSizerItem( int width, int height, int option, int flag, int border );

  // window
  wxNewSizerItem( wxWindow *window, int option, int flag, int border );

  // subsizer
  wxNewSizerItem( wxNewSizer *sizer, int option, int flag, int border );

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
  int GetFlag() const
    { return m_flag; }
  int GetBorder() const
    { return m_border; }
  
protected:
  wxWindow    *m_window;
  wxNewSizer     *m_sizer;
  wxSize       m_size;
  wxSize       m_minSize;
  int          m_option;
  int          m_border;
  int          m_flag;
};

//---------------------------------------------------------------------------
// wxNewSizer
//---------------------------------------------------------------------------

class WXDLLEXPORT wxNewSizer: public wxObject
{
public:
   wxNewSizer();
   ~wxNewSizer();
   
   virtual void Add( wxWindow *window, int option = 0, int flag = 0, int border = 0 );
   virtual void Add( wxNewSizer *sizer, int option = 0, int flag = 0, int border = 0 );
   virtual void Add( int width, int height, int option = 0, int flag = 0, int border = 0  );
  
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
// wxBox
//---------------------------------------------------------------------------

class WXDLLEXPORT wxBox: public wxNewSizer
{
public:
   wxBox( int orient );
   
   void RecalcSizes();
   wxSize CalcMin();
   
   int GetOrientation()
     { return m_orient; }
   
protected:
    int m_orient;
    int m_stretchable;
    int m_minWidth;
    int m_minHeight;
    int m_fixedWidth;
    int m_fixedHeight;
};
  
#endif
  // __WXSIZER_H__
