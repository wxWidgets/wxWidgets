/////////////////////////////////////////////////////////////////////////////
// Name:        sizer.h
// Purpose:     provide wxSizer class for layouting
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
    DECLARE_CLASS(wxSizerItem);
public:
  // spacer
  wxSizerItem( int width, int height, int option, int flag, int border, wxObject* userData);

  // window
  wxSizerItem( wxWindow *window, int option, int flag, int border, wxObject* userData );

  // subsizer
  wxSizerItem( wxSizer *sizer, int option, int flag, int border, wxObject* userData );

  ~wxSizerItem();

  virtual wxSize GetSize();
  virtual wxSize CalcMin();
  virtual void SetDimension( wxPoint pos, wxSize size );

  void SetRatio( int width, int height )
    // if either of dimensions is zero, ratio is assumed to be 1
    // to avoid "divide by zero" errors
    { m_ratio = (width && height) ? ((float) width / (float) height) : 1; }
  void SetRatio( wxSize size )
    { m_ratio = (size.x && size.y) ? ((float) size.x / (float) size.y) : 1; }
  void SetRatio( float ratio ) { m_ratio = ratio; }
  float GetRatio() const { return m_ratio; }

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
  wxObject* GetUserData()
    { return m_userData; }

protected:
  wxWindow    *m_window;
  wxSizer     *m_sizer;
  wxSize       m_size;
  wxSize       m_minSize;
  int          m_option;
  int          m_border;
  int          m_flag;
  // als: aspect ratio can always be calculated from m_size,
  //      but this would cause precision loss when the window
  //      is shrinked.  it is safer to preserve initial value.
  float        m_ratio;
  wxObject    *m_userData;
};

//---------------------------------------------------------------------------
// wxSizer
//---------------------------------------------------------------------------

class WXDLLEXPORT wxSizer: public wxObject
{
    DECLARE_CLASS(wxSizer);
public:
   wxSizer();
   ~wxSizer();

   virtual void Add( wxWindow *window, int option = 0, int flag = 0, int border = 0, wxObject* userData = NULL );
   virtual void Add( wxSizer *sizer, int option = 0, int flag = 0, int border = 0, wxObject* userData = NULL );
   virtual void Add( int width, int height, int option = 0, int flag = 0, int border = 0, wxObject* userData = NULL );

   virtual void Prepend( wxWindow *window, int option = 0, int flag = 0, int border = 0, wxObject* userData = NULL );
   virtual void Prepend( wxSizer *sizer, int option = 0, int flag = 0, int border = 0, wxObject* userData = NULL );
   virtual void Prepend( int width, int height, int option = 0, int flag = 0, int border = 0, wxObject* userData = NULL );

   virtual bool Remove( wxWindow *window );
   virtual bool Remove( wxSizer *sizer );
   virtual bool Remove( int pos );

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

  wxList& GetChildren()
    { return m_children; }

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
    DECLARE_CLASS(wxBoxSizer);
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
    DECLARE_CLASS(wxStaticBoxSizer);
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
