/////////////////////////////////////////////////////////////////////////////
// Name:        joytest.cpp
// Purpose:     Joystick sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// Define a new application
class MyApp: public wxApp
{
public:
    bool OnInit() override;

    // Joystick max values
    int     m_minX;
    int     m_minY;
    int     m_maxX;
    int     m_maxY;

#if wxUSE_SOUND
    wxSound  m_fire;
#endif // wxUSE_SOUND
};

wxDECLARE_APP(MyApp);

class MyCanvas
    : public wxPanel
{
private:
    wxPoint m_pos;
    wxPoint m_point;
    bool m_validPoint;

    wxJoystick* m_stick;
    int m_nButtons;

public:
    MyCanvas(wxWindow* parent);
    ~MyCanvas();
    void OnJoystickEvent(wxJoystickEvent& event);
    void OnPaint(wxPaintEvent& evt);
    void OnSize(wxSizeEvent& evt);

    wxDECLARE_EVENT_TABLE();
};

class MyFrame: public wxFrame
{
    MyCanvas* canvas;

public:
    MyFrame(wxFrame* parent, const wxString& title);

    void OnActivate(wxActivateEvent& event);
    void OnQuit(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();
};
