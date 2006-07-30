/*
 * Program: wxTest
 * 
 * Author: Robert Roebling
 *
 * Copyright: (C) 1997, GNU (Robert Roebling)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __TESTH__
#define __TESTH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/wx.h"
#include "wx/dcscreen.h"
#include "wx/splitter.h"
#include "wx/toolbar.h"
#include "wx/fontdlg.h"

//-----------------------------------------------------------------------------
// derived classes
//-----------------------------------------------------------------------------

class MyDialog;
class MyFrame;
class MyApp;

//-----------------------------------------------------------------------------
// MyTimer
//-----------------------------------------------------------------------------

class MyTimer: public wxTimer
{
  public:
  
    MyTimer(void) {};
    void Notify(void)
    {
      printf( "OnTimer.\n" );
    };
};

//-----------------------------------------------------------------------------
// MyDialog
//-----------------------------------------------------------------------------

class MyDialog: public wxDialog
{
  DECLARE_DYNAMIC_CLASS(MyDialog)
  
  public:
  
    MyDialog(void) {};
    MyDialog( wxWindow *parent );
    
    void OnReturnButton( wxCommandEvent &event );
    void OnHelloButton( wxCommandEvent &event );
    
    void OnCheckBox( wxCommandEvent &event );
    void OnCheckBoxButtons( wxCommandEvent &event );
    
    void OnTextCtrl( wxCommandEvent &event );
    void OnTextCtrlButtons( wxCommandEvent &event );
    
    void OnChoice( wxCommandEvent &event );
    void OnChoiceButtons( wxCommandEvent &event );
    
    void OnListBox( wxCommandEvent &event );
    void OnListBoxButtons( wxCommandEvent &event );
    
    void OnRadioBox( wxCommandEvent &event );
    void OnRadioBoxButtons( wxCommandEvent &event );
    
    wxCheckBox    *m_checkbox;
    wxTextCtrl    *m_textctrl;
    wxChoice      *m_choice;
    wxListBox     *m_listbox;
    wxRadioBox    *m_radiobox;
    wxStaticText  *m_text1;
    wxStaticText  *m_text2;

  DECLARE_EVENT_TABLE()
};

//-----------------------------------------------------------------------------
// MyCanvas
//-----------------------------------------------------------------------------

class MyCanvas: public wxScrolledWindow
{
  DECLARE_DYNAMIC_CLASS(MyCanvas)
  
  public:
  
    MyCanvas(void) {};
    MyCanvas( wxWindow *parent, wxWindowID, const wxPoint &pos, const wxSize &size );
    ~MyCanvas(void);
    void OnPaint( wxPaintEvent &event );
    
    wxBitmap  *my_bitmap;
    wxBitmap  *my_horse;
    wxBitmap  *my_backstore;
    wxFont    *my_font;
    bool       m_isCreated;
    
  DECLARE_EVENT_TABLE()
};

//-----------------------------------------------------------------------------
// MyFrame
//-----------------------------------------------------------------------------

class MyFrame: public wxFrame
{
  DECLARE_DYNAMIC_CLASS(MyFrame)

  public:
  
    MyFrame(void);
    void OnSize( wxSizeEvent &event );
    void OnOpenDialog( wxCommandEvent &event );
    void OnFontDialog( wxCommandEvent &event );
    void OnMsg( wxCommandEvent &event );
    void OnDialog( wxCommandEvent &event );
    void OnAbout( wxCommandEvent &event );
    void OnQuit( wxCommandEvent &event );
    
    wxToolBar    *m_tb;
    MyCanvas         *m_canvas;
    
    MyTimer m_timer;
    
  DECLARE_EVENT_TABLE()
};

//-----------------------------------------------------------------------------
// MyApp
//-----------------------------------------------------------------------------

class MyApp: public wxApp
{
  public:
  
    MyApp(void);
    virtual bool OnInit(void);
};

#endif // wxGTK_h
