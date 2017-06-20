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
    if(!m_bitmap.LoadFile("image.jpg", wxBITMAP_TYPE_ANY))
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
 
}
void MyGesturePanel::OnZoom(wxZoomGestureEvent& event)
{

}
 
void MyGesturePanel::OnRotate(wxRotateGestureEvent& event)
{
 
}