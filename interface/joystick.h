/////////////////////////////////////////////////////////////////////////////
// Name:        joystick.h
// Purpose:     documentation for wxJoystick class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxJoystick
    @wxheader{joystick.h}
    
    wxJoystick allows an application to control one or more joysticks.
    
    @library{wxadv}
    @category{FIXME}
    
    @seealso
    wxJoystickEvent
*/
class wxJoystick : public wxObject
{
public:
    /**
        Constructor. @e joystick may be one of wxJOYSTICK1, wxJOYSTICK2, indicating the
        joystick
        controller of interest.
    */
    wxJoystick(int joystick = wxJOYSTICK1);

    /**
        Destroys the wxJoystick object.
    */
    ~wxJoystick();

    //@{
    /**
        Returns the state of the specified joystick button.
        
        @param id 
        The button id to report, from 0 to GetNumberButtons() - 1
    */
    int GetButtonState();
        bool GetButtonState(unsigned id);
    //@}

    /**
        Returns the manufacturer id.
    */
    int GetManufacturerId();

    /**
        Returns the movement threshold, the number of steps outside which the joystick
        is deemed to have
        moved.
    */
    int GetMovementThreshold();

    /**
        Returns the number of axes for this joystick.
    */
    int GetNumberAxes();

    /**
        Returns the number of buttons for this joystick.
    */
    int GetNumberButtons();

    /**
        Returns the number of joysticks currently attached to the computer.
    */
    static int GetNumberJoysticks();

    /**
        Returns the point-of-view position, expressed in continuous, one-hundredth of a
        degree units.
        Returns -1 on error.
    */
    int GetPOVCTSPosition();

    /**
        Returns the point-of-view position, expressed in continuous, one-hundredth of a
        degree units,
        but limited to return 0, 9000, 18000 or 27000.
        Returns -1 on error.
    */
    int GetPOVPosition();

    /**
        Returns the maximum polling frequency.
    */
    int GetPollingMax();

    /**
        Returns the minimum polling frequency.
    */
    int GetPollingMin();

    //@{
    /**
        Returns the position of the specified joystick axis.
        
        @param axis 
        The joystick axis to report, from 0 to GetNumberAxes() - 1.
    */
    wxPoint GetPosition();
        int GetPosition(unsigned axis);
    //@}

    /**
        Returns the product id for the joystick.
    */
    int GetProductId();

    /**
        Returns the product name for the joystick.
    */
    wxString GetProductName();

    /**
        Returns the maximum rudder position.
    */
    int GetRudderMax();

    /**
        Returns the minimum rudder position.
    */
    int GetRudderMin();

    /**
        Returns the rudder position.
    */
    int GetRudderPosition();

    /**
        Returns the maximum U position.
    */
    int GetUMax();

    /**
        Returns the minimum U position.
    */
    int GetUMin();

    /**
        Gets the position of the fifth axis of the joystick, if it exists.
    */
    int GetUPosition();

    /**
        Returns the maximum V position.
    */
    int GetVMax();

    /**
        Returns the minimum V position.
    */
    int GetVMin();

    /**
        Gets the position of the sixth axis of the joystick, if it exists.
    */
    int GetVPosition();

    /**
        Returns the maximum x position.
    */
    int GetXMax();

    /**
        Returns the minimum x position.
    */
    int GetXMin();

    /**
        Returns the maximum y position.
    */
    int GetYMax();

    /**
        Returns the minimum y position.
    */
    int GetYMin();

    /**
        Returns the maximum z position.
    */
    int GetZMax();

    /**
        Returns the minimum z position.
    */
    int GetZMin();

    /**
        Returns the z position of the joystick.
    */
    int GetZPosition();

    /**
        Returns @true if the joystick has a point of view control.
    */
#define bool HasPOV()     /* implementation is private */

    /**
        Returns @true if the joystick point-of-view supports discrete values (centered,
        forward, backward, left, and right).
    */
    bool HasPOV4Dir();

    /**
        Returns @true if the joystick point-of-view supports continuous degree bearings.
    */
#define bool HasPOVCTS()     /* implementation is private */

    /**
        Returns @true if there is a rudder attached to the computer.
    */
    bool HasRudder();

    /**
        Returns @true if the joystick has a U axis.
    */
#define bool HasU()     /* implementation is private */

    /**
        Returns @true if the joystick has a V axis.
    */
#define bool HasV()     /* implementation is private */

    /**
        Returns @true if the joystick has a Z axis.
    */
#define bool HasZ()     /* implementation is private */

    /**
        Returns @true if the joystick is functioning.
    */
#define bool IsOk()     /* implementation is private */

    /**
        Releases the capture set by @b SetCapture.
        
        @returns @true if the capture release succeeded.
        
        @sa SetCapture(), wxJoystickEvent
    */
    bool ReleaseCapture();

    /**
        Sets the capture to direct joystick events to @e win.
        
        @param win 
        The window that will receive joystick events.
        
        @param pollingFreq 
        If zero, movement events are sent when above the
        threshold. If greater than zero, events are received every pollingFreq
        milliseconds.
        
        @returns @true if the capture succeeded.
        
        @sa ReleaseCapture(), wxJoystickEvent
    */
    bool SetCapture(wxWindow* win, int pollingFreq = 0);

    /**
        Sets the movement threshold, the number of steps outside which the joystick is
        deemed to have
        moved.
    */
    void SetMovementThreshold(int threshold);
};
