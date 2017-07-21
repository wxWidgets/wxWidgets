#include "gestures.h"
#include "wx/dcgraph.h"

MyGestureFrame::MyGestureFrame()
    : wxFrame(NULL, wxID_ANY, wxT("Multi-touch Gestures"), wxDefaultPosition, wxSize(800, 600))
{
    // Create controls
    MyGesturePanel *myPanel = new MyGesturePanel(this);
    m_logText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                    wxSize(wxDefaultCoord, 100), wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH);

    // Add controls to sizer
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(myPanel, wxSizerFlags(1).Expand());
    sizer->Add(m_logText, wxSizerFlags().Expand());
    SetSizer(sizer);

    // Log to the text control
    delete wxLog::SetActiveTarget(new wxLogTextCtrl(m_logText));

    // Bind all gestures to the same event handler, which must run before
    // the other handlers, to clear the log window
    myPanel->Bind(wxEVT_GESTURE_PAN, &MyGestureFrame::OnGesture, this);
    myPanel->Bind(wxEVT_GESTURE_ZOOM, &MyGestureFrame::OnGesture, this);
    myPanel->Bind(wxEVT_GESTURE_ROTATE, &MyGestureFrame::OnGesture, this);

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

void MyGestureFrame::OnGesture(wxGestureEvent& event)
{
    if ( event.IsGestureStart() )
        m_logText->Clear();

    event.Skip();
}

void MyGesturePanel::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC paintDC(this);

    wxGCDC dc(paintDC);

    dc.Clear();

    dc.SetTransformMatrix(m_affineMatrix);
    dc.DrawBitmap(m_bitmap, wxRound(m_translateDistance.m_x), wxRound(m_translateDistance.m_y));
}

void MyGesturePanel::OnPan(wxPanGestureEvent& event)
{
    if ( event.IsGestureStart() )
    {
        wxLogMessage("Pan gesture started\n");
    }

    wxLogMessage("Pan gesture performed with deltaX = %d and deltaY = %d, with current position (%d,%d)\n",
        event.GetDeltaX(), event.GetDeltaY(), event.GetPosition().x, event.GetPosition().y);

    // Transform the distance using the tranpose of the matrix,
    // in order to translate the image to match the screen coordinates
    wxMatrix2D m;
    m_affineMatrix.Get(&m, NULL);

    wxPoint2DDouble delta(m.m_11 * event.GetDeltaX() + m.m_12 * event.GetDeltaY(),
        m.m_21 * event.GetDeltaX() + m.m_22 * event.GetDeltaY());

    // Add it to the total translation
    m_translateDistance += delta;

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
        wxLogMessage("Zoom gesture started\n");
    }

    wxLogMessage("Zoom gesture performed with zoom center at (%d, %d) and zoom Delta = %f\n",
        event.GetPosition().x, event.GetPosition().y, event.GetZoomDelta());

    const wxPoint& zoomCenter = event.GetPosition();

    // Translate to zoom center
    m_affineMatrix.Translate(zoomCenter.x, zoomCenter.y);
    // Scale
    m_affineMatrix.Scale(event.GetZoomDelta(), event.GetZoomDelta());
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
        wxLogMessage("Rotate gesture started\n");
    }

    wxLogMessage("Rotate gesture performed with rotation center at (%d, %d) and rotation angle = %f\n",
        event.GetPosition().x, event.GetPosition().y, event.GetAngleDelta());

    const wxPoint& rotationCenter = event.GetPosition();

    // Translate to rotation center
    m_affineMatrix.Translate(rotationCenter.x, rotationCenter.y);
    // Rotate
    m_affineMatrix.Rotate(event.GetAngleDelta());
    // Translate back
    m_affineMatrix.Translate(-rotationCenter.x, -rotationCenter.y);

    if ( event.IsGestureEnd() )
    {
        wxLogMessage("Rotate gesture Ended\n");
    }

    Refresh();
}
