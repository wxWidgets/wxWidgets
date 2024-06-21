#include "gestures.h"
#include "wx/dcgraph.h"
#include "wx/dcbuffer.h"

#include "../image/horse.xpm"

MyGestureFrame::MyGestureFrame()
    : wxFrame(nullptr, wxID_ANY, "Multi-touch Gestures", wxDefaultPosition, wxSize(800, 600))
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
    m_logOld = wxLog::SetActiveTarget(new wxLogTextCtrl(m_logText));

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
    delete wxLog::SetActiveTarget(m_logOld);
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
    dc.DrawBitmap(m_bitmap, 0, 0);
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
    wxPoint2DDouble tr;

    m_affineMatrix.Get(&m, &tr);
    tr += event.GetDelta();
    m_affineMatrix.Set(m, tr);

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
        m_lastGesturePos = event.GetPosition();
    }

    wxLogMessage("Zoom gesture performed with zoom center at (%d, %d) and zoom Factor = %f",
        event.GetPosition().x, event.GetPosition().y, event.GetZoomFactor());

    const wxPoint &evtPos = event.GetPosition();
    double factor = event.GetZoomFactor() / m_lastZoomFactor;

    wxMatrix2D m;
    wxPoint2DDouble tr;
    m_affineMatrix.Get(&m, &tr);

    tr -= m_lastGesturePos;

    wxAffineMatrix2D inv = m_affineMatrix;
    inv.Invert();
    tr = inv.TransformDistance(tr);
    m_affineMatrix.Scale(factor, factor);
    tr = m_affineMatrix.TransformDistance(tr);

    tr += evtPos;

    m_affineMatrix.Get(&m, nullptr);
    m_affineMatrix.Set(m, tr);

    if ( event.IsGestureEnd() )
    {
        wxLogMessage("Zoom gesture ended");
    }

    m_lastZoomFactor = event.GetZoomFactor();
    m_lastGesturePos = evtPos;

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

    const wxPoint& evtPos = event.GetPosition();

    wxMatrix2D m;
    wxPoint2DDouble tr;

    m_affineMatrix.Get(&m, &tr);

    tr -= evtPos;

    wxAffineMatrix2D inv = m_affineMatrix;
    inv.Invert();
    tr = inv.TransformDistance(tr);
    m_affineMatrix.Rotate(event.GetRotationAngle() - m_lastRotationAngle);
    tr = m_affineMatrix.TransformDistance(tr);

    tr += evtPos;

    m_affineMatrix.Get(&m, nullptr);
    m_affineMatrix.Set(m, tr);

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
