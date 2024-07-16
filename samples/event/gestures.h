#ifndef _WX_GESTURES_H_
#define _WX_GESTURES_H_

#include "wx/wx.h"

class MyGestureFrame : public wxFrame
{
public:
    MyGestureFrame();

    void OnGesture(wxGestureEvent& event);
    void OnQuit(wxCloseEvent& event);

private:
    wxLog *m_logOld;
    wxTextCtrl *m_logText;
};

class MyGesturePanel : public wxPanel
{
public:
    MyGesturePanel(MyGestureFrame* parent);

    void OnPaint(wxPaintEvent& event);
    void OnPan(wxPanGestureEvent& event);
    void OnZoom(wxZoomGestureEvent& event);
    void OnRotate(wxRotateGestureEvent& event);
    void OnTwoFingerTap(wxTwoFingerTapEvent& event);
    void OnLongPress(wxLongPressEvent& event);
    void OnPressAndTap(wxPressAndTapEvent& event);

private:
    wxBitmap m_bitmap;
    wxPoint2DDouble m_translateDistance;
    wxAffineMatrix2D m_affineMatrix;
    double m_lastZoomFactor;
    double m_lastRotationAngle;
    wxPoint m_lastGesturePos;
};

#endif // _WX_GESTURES_H_
