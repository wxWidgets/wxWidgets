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

    Bind(wxEVT_CLOSE_WINDOW, &MyGestureFrame::OnQuit, this);
}
 
MyGesturePanel::MyGesturePanel(MyGestureFrame *parent) : wxPanel(parent, wxID_ANY)
{
    wxImage::AddHandler(new wxJPEGHandler);
    if(!m_bitmap.LoadFile("../image/horse.jpg", wxBITMAP_TYPE_ANY))
    {
        wxLogError("Can't load the image");
    }
    
    else
    {
        Bind(wxEVT_PAINT, &MyGesturePanel::OnPaint, this);
    }

    Bind(wxEVT_GESTURE_PAN, &MyGesturePanel::OnPan, this);
    Bind(wxEVT_GESTURE_ZOOM, &MyGesturePanel::OnZoom, this);
    Bind(wxEVT_GESTURE_ROTATE, &MyGesturePanel::OnRotate, this);
}

void MyGestureFrame::OnQuit(wxCloseEvent& event)
{
    Destroy();
}
 
void MyGesturePanel::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);
    dc.DrawBitmap(m_bitmap, 0, 0);
}
 
void MyGesturePanel::OnPan(wxPanGestureEvent& event)
{
    if(event.IsGestureStart())
    {
        wxLogMessage("Pan gesture started\n");
    }

    wxLogMessage(wxT("Pan gesture performed with deltaX = %d and deltaY = %d, with current position (%d,%d)\n"),
        event.GetPanDeltaX(), event.GetPanDeltaY(), event.GetPosition().x, event.GetPosition().y);

    if(event.IsGestureEnd())
    {
        wxLogMessage("Pan gesture Ended\n");
    }   
}

void MyGesturePanel::OnZoom(wxZoomGestureEvent& event)
{
    if(event.IsGestureStart())
    {
        wxLogMessage("Zoom gesture started\n");
    }

    wxLogMessage(wxT("Zoom gesture performed with zoom center at (%d, %d) and zoom factor = %f\n"),
        event.GetPosition().x, event.GetPosition().y, event.GetZoomFactor());

    if(event.IsGestureEnd())
    {
        wxLogMessage("Zoom gesture Ended\n");
    }
}
 
void MyGesturePanel::OnRotate(wxRotateGestureEvent& event)
{
    if(event.IsGestureStart())
    {
        wxLogMessage("Rotate gesture started\n");
    }

    wxLogMessage(wxT("Rotate gesture performed with rotation center at (%d, %d) and rotation angle = %f\n"),
        event.GetPosition().x, event.GetPosition().y, event.GetAngle());

    if(event.IsGestureEnd())
    {
        wxLogMessage("Rotate gesture Ended\n");
    }
}
