/*
 * Program: wxFile
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

#ifndef __WXFILEH__
#define __WXFILEH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/wx.h"
#include "wx/dcscreen.h"
#include "wx/splitter.h"
#include "wx/toolbar.h"
#include "filectrl.h"
#include "dirctrl.h"

//-----------------------------------------------------------------------------
// derived classes
//-----------------------------------------------------------------------------

class MyFrame;
class MyApp;

//-----------------------------------------------------------------------------
// MyFrame
//-----------------------------------------------------------------------------

class MyFrame: public wxFrame
{
  DECLARE_DYNAMIC_CLASS(MyFrame)

  public:
  
    MyFrame(void);
    void OnSize( wxSizeEvent &event );
    void OnCommand( wxCommandEvent &event );
    void OnAbout( wxCommandEvent &event );
    void OnView( wxCommandEvent &event );
    void OnListKeyDown( wxListEvent &event );
    void OnListDeleteItem( wxListEvent &event );
    void OnListEndLabelEdit( wxListEvent &event );
    void OnListDrag( wxListEvent &event );
    void OnTreeSelected( wxTreeEvent &event );
    void OnTreeKeyDown( wxTreeEvent &event );
    
    wxToolBarGTK       *m_tb;
    wxSplitterWindow   *m_splitter;
    wxFileCtrl         *m_rightFile;
    wxFileCtrl         *m_leftFile;
    wxDirCtrl          *m_dir;
    
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

#endif // __WXFILEH__
