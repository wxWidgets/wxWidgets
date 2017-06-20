#include "gestures.h"

MyGestureFrame::MyGestureFrame() : wxFrame(NULL, wxID_ANY, wxT("Multi-touch Gestures"))
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    MyGesturePanel *myPanel = new MyGesturePanel(this);
    sizer->Add(myPanel, wxSizerFlags(1).Expand());
 
    wxTextCtrl* logText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                                         wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY);
    wxLog::SetActiveTarget(new wxLogTextCtrl(logText));
 
    sizer->Add(logText, wxSizerFlags().Expand());  
    SetSizer(sizer);
}
 
MyGesturePanel::MyGesturePanel(MyGestureFrame *parent) : wxPanel(parent, wxID_ANY)
{
    wxInitAllImageHandlers();
    if(!m_bitmap.LoadFile("../image/horse.jpg", wxBITMAP_TYPE_ANY))
    {
        wxLogError("Can't load the image");
    }
     
    Bind(wxEVT_PAINT, &MyGesturePanel::OnPaint, this);
    Bind(wxEVT_GESTURE_PAN, &MyGesturePanel::OnPan, this);
    Bind(wxEVT_GESTURE_ZOOM, &MyGesturePanel::OnZoom, this);
    Bind(wxEVT_GESTURE_ROTATE, &MyGesturePanel::OnRotate, this);
}
 
void MyGesturePanel::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);
    dc.DrawBitmap(m_bitmap, 0, 0);
}
 
void MyGesturePanel::OnPan(wxPanGestureEvent& event)
{
    static int previousLocationX, previousLocationY;
    if(event.IsGestureStart())
    {
        previousLocationX = event.GetPosition().x;
        previousLocationY = event.GetPosition().y;
        wxLogMessage("Pan gesture started\n");
        return ;
    }
 
    int panDeltaY = abs(event.GetPosition().y - previousLocationY);
    int panDeltaX = abs(event.GetPosition().x - previousLocationX);
    
    wxDirection direction = event.GetPanDirection();

    if(direction == wxUP)
    {
        wxLogMessage("Pan gesture performed in Up direction");
    }
    else if(direction == wxDOWN)
    {
        wxLogMessage("Pan gesture performed in Down direction");
    }
    else if(direction == wxRIGHT)
    {
        wxLogMessage("Pan gesture performed in Right direction");   
    }
    else
    {
        wxLogMessage("Pan gesture performed in Left direction");
    }

    wxLogMessage(wxT("with deltaX = %d and deltaY = %d\n"),panDeltaX, panDeltaY);
 
    previousLocationX = event.GetPosition().x;
    previousLocationY = event.GetPosition().y;
 
}
void MyGesturePanel::OnZoom(wxZoomGestureEvent& event)
{
    if(event.IsGestureStart())
    {
        wxLogMessage("Zoom gesture started\n");
        return ;
    }

    wxLogMessage(wxT("Zoom gesture performed with zoom center at (%d, %d) and zoom factor = %f\n"),
        event.GetPosition().x, event.GetPosition().y, event.GetZoomFactor());
}
 
void MyGesturePanel::OnRotate(wxRotateGestureEvent& event)
{
    if(event.IsGestureStart())
    {
        wxLogMessage("Rotate gesture started\n");
        return ;
    }

    wxLogMessage(wxT("Rotate gesture performed with rotation center at (%d, %d) and rotation angle = %f\n"),
        event.GetPosition().x, event.GetPosition().y, event.GetAngle());
}