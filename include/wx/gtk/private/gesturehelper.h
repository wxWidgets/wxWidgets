#ifndef _GTK_PRIVATE_GESTUREHELPER_H_
#define _GTK_PRIVATE_GESTUREHELPER_H_

class WXDLLIMPEXP_CORE GestureHelper
{
public:
    GestureHelper()
    {
        m_gestureStart = false;
        m_lastOffset = 0;
        m_lastScale = 1.0;
        m_lastAngle = 0.0;
        m_horizontalPanActive = false;
        m_verticalPanActive = false;
        m_lastGesturePoint.x = 0;
        m_lastGesturePoint.y = 0;
        m_isTwoFingerTapPossible = false;
        m_lastTime = 0;
        m_fingerCount = 0;
        m_lastTouchPoint.x = 0;
        m_lastTouchPoint.y = 0;
    }

    // This is true when the gesture has just started (currently used for pan gesture only)
    bool m_gestureStart;

    // Last offset for the pan gesture, this is used to calculate deltas for pan gesture event
    double m_lastOffset;

    // Last scale provided by GTK
    double m_lastScale;

    // This is used to calculate angle delta.
    double m_lastAngle;

    // True if "pan" signal was emitted for horizontal pan gesture
    bool m_horizontalPanActive;

    // True if "pan" signal was emitted for vertical pan gesture
    bool m_verticalPanActive;

    // Last Zoom/Rotate gesture point
    wxPoint m_lastGesturePoint;

    // True if "Two Finger Tap" is possible
    bool m_isTwoFingerTapPossible;

    // Used for "Two Finger Tap" Gesture
    int m_fingerCount;

    // Used for "Two Finger Tap" Gesture
    int m_lastTime;

    // Used for "Two Finger Tap" Gesture
    wxPoint m_lastTouchPoint;
};
#endif // _GTK_PRIVATE_GESTUREHELPER_H_
