/* -*- c++ -*- */

#ifndef __WXLAYOUTH__
#define __WXLAYOUTH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/wx.h"

#include "wxllist.h"
#include "wxlwindow.h"
#include "wxlparser.h"

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
   void Edit(void);
   void AddSampleText(wxLayoutList &llist);
   void Clear(void);
   void OnCommand( wxCommandEvent &event );
   DECLARE_EVENT_TABLE()

private:
   wxLayoutWindow  *m_lwin;
   
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

#endif // __WXCONVERTH__
