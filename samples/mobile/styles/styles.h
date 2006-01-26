/////////////////////////////////////////////////////////////////////////////
// Name:        styles.h
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Created:     04/07/02
/////////////////////////////////////////////////////////////////////////////

#ifndef __styles_H__
#define __styles_H__

// Include wxWidgets' headers

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//----------------------------------------------------------------------------
//   constants
//----------------------------------------------------------------------------

#define ID_ABOUT    100
#define ID_QUIT     204

//----------------------------------------------------------------------------
// MyFrame
//----------------------------------------------------------------------------

class MyFrame: public wxFrame
{
public:
    // constructors and destructors
    MyFrame( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE );

private:
    void CreateMyMenuBar();

private:
    void OnAbout( wxCommandEvent &event );
    void OnQuit( wxCommandEvent &event );

    void OnCloseWindow( wxCloseEvent &event );

private:
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// MyApp
//----------------------------------------------------------------------------

class MyApp: public wxApp
{
public:
    MyApp(){};

    virtual bool OnInit();
    virtual int OnExit();
};

#endif
