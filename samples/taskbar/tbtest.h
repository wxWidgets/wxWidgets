/////////////////////////////////////////////////////////////////////////////
// Name:        tbtest.h
// Purpose:     wxTaskBarIcon sample
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

class MyTaskBarIcon: public wxTaskBarIcon
{
public:
	MyTaskBarIcon() {};

    virtual void OnMouseMove(void);
    virtual void OnLButtonDown(void);
    virtual void OnLButtonUp(void);
    virtual void OnRButtonDown(void);
    virtual void OnRButtonUp(void);
    virtual void OnLButtonDClick(void);
    virtual void OnRButtonDClick(void);
};


// Define a new application
class MyApp: public wxApp
{
public:
    bool OnInit(void);
protected:
    MyTaskBarIcon   m_taskBarIcon;
};

class MyDialog: public wxDialog
{
public:
    MyDialog(wxWindow* parent, const wxWindowID id, const wxString& title,
        const wxPoint& pos, const wxSize& size, const long windowStyle = wxDEFAULT_DIALOG_STYLE);

    void OnOK(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnCloseWindow(wxCloseEvent& event);
    void Init(void);

DECLARE_EVENT_TABLE()
};


