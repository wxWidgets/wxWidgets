#include "gestures.h"
#include "wx/dcgraph.h"
#include "wx/dcbuffer.h"

#include "../image/horse.xpm"

MyGestureFrame::MyGestureFrame()
    : wxFrame(NULL, wxID_ANY, "Multi-touch Gestures", wxDefaultPosition, wxSize(800, 600))
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

    // Set a minimum size for the frame
    wxSize dsplySz = wxGetDisplaySize();
    SetSizeHints(wxMin(800,dsplySz.GetWidth()), wxMin(600,dsplySz.GetHeight()));

    // Log to the text control
    delete wxLog::SetActiveTarget(new wxLogTextCtrl(m_logText));

    // Bind all gestures to the same event handler, which must run before
    // the other handlers, to clear the log window
    myPanel->Bind(wxEVT_GESTURE_PAN, &MyGestureFrame::OnGesture, this);
    myPanel->Bind(wxEVT_GESTURE_ZOOM, &MyGestureFrame::OnGesture, this);
    myPanel->Bind(wxEVT_GESTURE_ROTATE, &MyGestureFrame::OnGesture, this);
    myPanel->Bind(wxEVT_TWO_FINGER_TAP, &MyGestureFrame::OnGesture, this);
    myPanel->Bind(wxEVT_LONG_PRESS, &MyGestureFrame::OnGesture, this);
    myPanel->Bind(wxEVT_PRESS_AND_TAP, &MyGestureFrame::OnGesture, this);

    Bind(wxEVT_CLOSE_WINDOW, &MyGestureFrame::OnQuit, this);
}

MyGesturePanel::MyGesturePanel(MyGestureFrame *parent)
    : wxPanel(parent, wxID_ANY),
      m_bitmap(horse_xpm)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &MyGesturePanel::OnPaint, this);

    if ( !EnableTouchEvents(wxTOUCH_ALL_GESTURES) )
    {
        wxLogError("Failed to enable touch events");

        // Still bind event handlers just in case they still work?
    }

    // Event handlers
    Bind(wxEVT_GESTURE_PAN, &MyGesturePanel::OnPan, this);
    Bind(wxEVT_GESTURE_ZOOM, &MyGesturePanel::OnZoom, this);
    Bind(wxEVT_GESTURE_ROTATE, &MyGesturePanel::OnRotate, this);
    Bind(wxEVT_TWO_FINGER_TAP, &MyGesturePanel::OnTwoFingerTap, this);
    Bind(wxEVT_LONG_PRESS, &MyGesturePanel::OnLongPress, this);
    Bind(wxEVT_PRESS_AND_TAP, &MyGesturePanel::OnPressAndTap, this);

    m_lastZoomFactor = 1.0;
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
    wxAutoBufferedPaintDC paintDC(this);
    paintDC.Clear();

    wxGCDC dc(paintDC);
    dc.SetTransformMatrix(m_affineMatrix);
    dc.DrawBitmap(m_bitmap, wxRound(m_translateDistance.m_x), wxRound(m_translateDistance.m_y));
}

void MyGesturePanel::OnPan(wxPanGestureEvent& event)
{
    if ( event.IsGestureStart() )
    {
        wxLogMessage("Pan gesture started");
    }

    const wxPoint delta = event.GetDelta();
    wxLogMessage("Pan gesture performed with delta = (%d,%d), "
                 "with current position (%d,%d)",
                 delta.x, delta.y,
                 event.GetPosition().x, event.GetPosition().y);

    // Transform the distance using the transpose of the matrix,
    // in order to translate the image to match the screen coordinates
    wxMatrix2D m;
    m_affineMatrix.Get(&m, NULL);

    wxPoint2DDouble deltaD(m.m_11 * delta.x + m.m_12 * delta.y,
                           m.m_21 * delta.x + m.m_22 * delta.y);

    // Add it to the total translation
    m_translateDistance += deltaD;

    if ( event.IsGestureEnd() )
    {
        wxLogMessage("Pan gesture ended");
    }

    Refresh();
}

void MyGesturePanel::OnZoom(wxZoomGestureEvent& event)
{
    if ( event.IsGestureStart() )
    {
        wxLogMessage("Zoom gesture started");

        m_lastZoomFactor = 1.0;
    }

    wxLogMessage("Zoom gesture performed with zoom center at (%d, %d) and zoom Factor = %f",
        event.GetPosition().x, event.GetPosition().y, event.GetZoomFactor());

    const wxPoint& zoomCenter = event.GetPosition();

    // Translate to zoom center
    m_affineMatrix.Translate(zoomCenter.x, zoomCenter.y);
    // Scale
    m_affineMatrix.Scale(event.GetZoomFactor() / m_lastZoomFactor, event.GetZoomFactor() / m_lastZoomFactor);
    // Translate back
    m_affineMatrix.Translate(-zoomCenter.x, -zoomCenter.y);

    if ( event.IsGestureEnd() )
    {
        wxLogMessage("Zoom gesture ended");
    }

    m_lastZoomFactor = event.GetZoomFactor();

    Refresh();
}

void MyGesturePanel::OnRotate(wxRotateGestureEvent& event)
{
    if ( event.IsGestureStart() )
    {
        wxLogMessage("Rotate gesture started");

        m_lastRotationAngle = 0.0;
    }

    wxLogMessage("Rotate gesture performed with rotation center at (%d, %d) and cumulative rotation angle = %f",
        event.GetPosition().x, event.GetPosition().y, event.GetRotationAngle());

    const wxPoint& rotationCenter = event.GetPosition();

    // Translate to rotation center
    m_affineMatrix.Translate(rotationCenter.x, rotationCenter.y);
    // Rotate
    m_affineMatrix.Rotate(event.GetRotationAngle() - m_lastRotationAngle);
    // Translate back
    m_affineMatrix.Translate(-rotationCenter.x, -rotationCenter.y);

    if ( event.IsGestureEnd() )
    {
        wxLogMessage("Rotate gesture ended");
    }

    m_lastRotationAngle = event.GetRotationAngle();

    Refresh();
}

void MyGesturePanel::OnTwoFingerTap(wxTwoFingerTapEvent& event)
{
    if ( event.IsGestureStart() )
    {
        wxLogMessage("Two Finger Tap gesture gesture started");
    }

    wxLogMessage("Two Finger Tap gesture performed at (%d, %d)", event.GetPosition().x, event.GetPosition().y);

    if ( event.IsGestureEnd() )
    {
        wxLogMessage("Two Finger Tap gesture ended");
    }
}

void MyGesturePanel::OnLongPress(wxLongPressEvent& event)
{
    if ( event.IsGestureStart() )
    {
        wxLogMessage("Long Press gesture started");
    }

    wxLogMessage("Long Press gesture performed at (%d,%d)", event.GetPosition().x, event.GetPosition().y);

    if ( event.IsGestureEnd() )
    {
        wxLogMessage("Long Press gesture ended");
    }
}

void MyGesturePanel::OnPressAndTap(wxPressAndTapEvent& event)
{
    if ( event.IsGestureStart() )
    {
        wxLogMessage("Press and Tap gesture started");
    }

    wxLogMessage("Press and Tap gesture performed at (%d,%d)", event.GetPosition().x, event.GetPosition().y);

    if ( event.IsGestureEnd() )
    {
        wxLogMessage("Press and Tap gesture ended");
    }
}
