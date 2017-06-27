#include "gestures.h"

MyGestureFrame::MyGestureFrame()
    : wxFrame(NULL, wxID_ANY, wxT("Multi-touch Gestures"), wxDefaultPosition, wxSize(800, 600))
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    MyGesturePanel *myPanel = new MyGesturePanel(this);
    sizer->Add(myPanel, wxSizerFlags(1).Expand());
 
    m_logText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                                        wxSize(wxDefaultCoord, 100), wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH);
    wxLog::SetActiveTarget(new wxLogTextCtrl(m_logText));
 
    sizer->Add(m_logText, wxSizerFlags().Expand());
    SetSizer(sizer);

    Bind(wxEVT_CLOSE_WINDOW, &MyGestureFrame::OnQuit, this);
}
 
MyGesturePanel::MyGesturePanel(MyGestureFrame *parent) : wxPanel(parent, wxID_ANY)
{
    // Load an image
    if ( !m_bitmap.LoadFile("../image/horse.jpg", wxBITMAP_TYPE_JPEG) )
    {
        wxLogError("Can't load the image");
    }

    else
    {
        Bind(wxEVT_PAINT, &MyGesturePanel::OnPaint, this);
    }

    // Event handlers
    Bind(wxEVT_GESTURE_PAN, &MyGesturePanel::OnPan, this);
    Bind(wxEVT_GESTURE_ZOOM, &MyGesturePanel::OnZoom, this);
    Bind(wxEVT_GESTURE_ROTATE, &MyGesturePanel::OnRotate, this);
}

void MyGestureFrame::OnQuit(wxCloseEvent& WXUNUSED(event))
{
    Destroy();
}

void MyGesturePanel::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    dc.SetTransformMatrix(m_affineMatrix);
    dc.DrawBitmap(m_bitmap, wxRound(m_translateDistance.m_x), wxRound(m_translateDistance.m_y));
}

void MyGesturePanel::OnPan(wxPanGestureEvent& event)
{
    if ( event.IsGestureStart() )
    {
        ((MyGestureFrame*) GetParent())->m_logText->Clear();
        wxLogMessage("Pan gesture started\n");
    }

    wxLogMessage(wxT("Pan gesture performed with deltaX = %d and deltaY = %d, with current position (%d,%d)\n"),
        event.GetDeltaX(), event.GetDeltaY(), event.GetPosition().x, event.GetPosition().y);

    wxPoint dist(event.GetDeltaX(), event.GetDeltaY());
    m_translateDistance += m_affineMatrix.TransformDistance(dist);

    if ( event.IsGestureEnd() )
    {
        wxLogMessage("Pan gesture Ended\n");
    }

    Refresh();
}

void MyGesturePanel::OnZoom(wxZoomGestureEvent& event)
{
    if ( event.IsGestureStart() )
    {
        ((MyGestureFrame*) GetParent())->m_logText->Clear();
        wxLogMessage("Zoom gesture started\n");
    }

    wxLogMessage(wxT("Zoom gesture performed with zoom center at (%d, %d) and zoom factor = %f\n"),
        event.GetPosition().x, event.GetPosition().y, event.GetZoomFactor());

    const wxPoint& zoomCenter = event.GetPosition();

    // Translate to zoom center
    m_affineMatrix.Translate(zoomCenter.x, zoomCenter.y);
    // Scale
    m_affineMatrix.Scale(event.GetZoomFactor(), event.GetZoomFactor());
    // Translate back
    m_affineMatrix.Translate(-zoomCenter.x, -zoomCenter.y);

    if ( event.IsGestureEnd() )
    {
        wxLogMessage("Zoom gesture Ended\n");
    }

    Refresh();
}

void MyGesturePanel::OnRotate(wxRotateGestureEvent& event)
{
    if ( event.IsGestureStart() )
    {
        ((MyGestureFrame*) GetParent())->m_logText->Clear();
        wxLogMessage("Rotate gesture started\n");
    }

    wxLogMessage(wxT("Rotate gesture performed with rotation center at (%d, %d) and rotation angle = %f\n"),
        event.GetPosition().x, event.GetPosition().y, event.GetAngle());

    const wxPoint& rotationCenter = event.GetPosition();

    // Translate to rotation center
    m_affineMatrix.Translate(rotationCenter.x, rotationCenter.y);
    // Rotate
    m_affineMatrix.Rotate(event.GetAngle());
    // Translate back
    m_affineMatrix.Translate(-rotationCenter.x, -rotationCenter.y);

    if ( event.IsGestureEnd() )
    {
        wxLogMessage("Rotate gesture Ended\n");
    }

    Refresh();
}
