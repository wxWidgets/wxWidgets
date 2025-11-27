/*
 * trackpadstate.h
 * Implemented for BricsCAD only
 */

#ifndef _WX_TRACKPADSTATE_H_
#define _WX_TRACKPADSTATE_H_

#include "wx/gdicmn.h"      // for wxRealPoint
#include "wx/kbdstate.h"

// the symbolic names for realtime statuses
enum wxState
{
    wxNoState          = -1,
    //panning
    wxPanningStartMove = 0,
    wxPanningMove      = 1,
    wxPanningEnd       = 2,
    //rotation
    wxRotationStart    = 3,
    wxRotationMove     = 4,
    wxRotationEnd      = 5,
};

class WXDLLIMPEXP_CORE wxTrackPadState : public wxKeyboardState
{
public:
    wxTrackPadState() : m_state(wxNoState),
                        m_ip(),
                        m_touchesSpace(0),
                        m_angle(0)
    { }

    //Setting and getting points should be done different if there are more than 2 touches!
    //Maybe using a map, or array of touches!?
    void SetAngle(double newAngle)               { m_angle = newAngle; }
    void SetPosition(wxRealPoint ip)             { m_ip = ip; }
    void SetTouchesDist(double new_dist)         { m_touchesSpace = new_dist; }
    void SetState(wxState newState)              { m_state = newState; }

    wxRealPoint GetPosition() const { return m_ip; }
    wxState GetState()        const { return m_state; }
    double GetTouchesDist()   const { return m_touchesSpace; }
    double GetAngle()         const { return m_angle; }

private:
    wxState     m_state;
    wxRealPoint m_ip;    //initial point
    double      m_touchesSpace; //distance between 2 touches( fingers )
    double      m_angle;        //rotation angle, usualy is used when sending a wxEVT_ROTATE event
};


#endif /* _WX_TRACKPADSTATE_H_ */
