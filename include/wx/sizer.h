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

class wxStaticBox;

class wxSizerItem;
class wxSizer;
class wxBoxSizer;
class wxStaticBoxSizer;

//---------------------------------------------------------------------------
// wxSizerItem
//---------------------------------------------------------------------------

class WXDLLEXPORT wxSizerItem: public wxObject
{
public:
  // spacer
  wxSizerItem( int width, int height, int option, int flag, int border );

  // window
  wxSizerItem( wxWindow *window, int option, int flag, int border );

  // subsizer
  wxSizerItem( wxSizer *sizer, int option, int flag, int border );

  virtual wxSize GetSize();
  virtual wxSize CalcMin();
  virtual void SetDimension( wxPoint pos, wxSize size );
  
  bool IsWindow();
  bool IsSizer();
  bool IsSpacer();
  
  wxWindow *GetWindow() const  
    { return m_window; }
  wxSizer *GetSizer() const    
    { return m_sizer; }
  int GetOption() const
    { return m_option; }
  int GetFlag() const
    { return m_flag; }
  int GetBorder() const
    { return m_border; }
  
protected:
  wxWindow    *m_window;
  wxSizer     *m_sizer;
  wxSize       m_size;
  wxSize       m_minSize;
  int          m_option;
  int          m_border;
  int          m_flag;
};

//---------------------------------------------------------------------------
// wxSizer
//---------------------------------------------------------------------------

class WXDLLEXPORT wxSizer: public wxObject
{
public:
   wxSizer();
   ~wxSizer();
   
   virtual void Add( wxWindow *window, int option = 0, int flag = 0, int border = 0 );
   virtual void Add( wxSizer *sizer, int option = 0, int flag = 0, int border = 0 );
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
// wxBoxSizer
//---------------------------------------------------------------------------

class WXDLLEXPORT wxBoxSizer: public wxSizer
{
public:
   wxBoxSizer( int orient );
   
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
  
//---------------------------------------------------------------------------
// wxStaticBoxSizer
//---------------------------------------------------------------------------

class WXDLLEXPORT wxStaticBoxSizer: public wxBoxSizer
{
public:
   wxStaticBoxSizer( wxStaticBox *box, int orient );
   
   void RecalcSizes();
   wxSize CalcMin();
   
   wxStaticBox *GetStaticBox()
     { return m_staticBox; }
   
protected:
   wxStaticBox   *m_staticBox;
};

#endif
  // __WXSIZER_H__
