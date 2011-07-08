///////////////////////////////////////////////////////////////////////////////
// Name:        ui_animation.h
// Purpose:     Interface of wxUIAnimation
// Author:      Chiciu Bogdan Gabriel
// Created:     2011-06-30
// RCS-ID:      $Id
// Copyright:   (c) wxWidgets Team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/**
    Animation curves are predefined functions that define the pacing of the animation.

    @since 2.9.3
*/
enum wxAnimationCurve
{
    /// Ease-in pacing causes the animation to begin slowly, and then speed up as it progresses.
    wxANIMATION_CURVE_EASE_IN,

    /// An ease-out pacing causes the animation to begin quickly, and then slow as it completes.
    wxANIMATION_CURVE_EASE_OUT,

    /// An ease-in ease-out animation begins slowly, accelerates through the middle of its duration, and then slows again before completing.
    wxANIMATION_CURVE_EASE_IN_OUT,
    
    /// Linear pacing causes an animation to occur evenly over its duration.
    wxANIMATION_CURVE_LINEAR,
};

/**
    Represents properties that can be animated for a specified target control. 

    @since 2.9.3
*/
enum wxAnimationTargetProperty
{
    /// When set the animation will change the background color of the target.
    wxANIMATION_TARGET_PROPERTY_BACKGROUND_COLOR,

    /// When set the animation will change the position of the target.
    wxANIMATION_TARGET_PROPERTY_POSITION,

    /// When set the animation will change the opacity of the target control, if supported.
    wxANIMATION_TARGET_PROPERTY_OPACITY
}

/**
    @class wxUIAnimationBase

    wxUIAnimationBase provides basic, single-keyframe animation capabilities for a
    control property such as background, foreground or size.
    
    The target property is animated between the two target values using the specified
    animation curve for interpolation over the specified duration.

    @since 2.9.3

    @see @ref supported_animation_types, @ref animation_overview
*/
template <wxAnimationTargetProperty Property> class wxUIAnimation
{
public:
    /**
        Creates a new animation with the specified values and duration.

        The type will be inforced by the selected property type.

        If left un-specified it will use an ease in-out animation curve.
    */
    wxUIAnimation(Type initialValue,
        Type targetValue,
        double duration,
        wxAnimationCurve curve = wxANIMATION_CURVE_EASE_IN_OUT);

    /**
        Sets the length of time (in seconds) for which this animation plays.
    */
    void SetDuration(double animation_duration);

    /**
        Gets the length of time (in seconds) for which this animation plays.
    */
    double GetDuration() const;

    /**
        Sets the animation's starting value.
    */
    void SetInitialValue(T initial_value);

    /**
        Gets the animation's starting value.
    */
    T GetInitialValue() const;

    /**
        Sets the animation's ending value.
    */
    void SetTargetValue(T target_value);

    /**
        Gets the animation's ending value.
    */
    T GetTargetValue() const;

    /**
        Sets the animation curve that will be used for interpolation.
    */
    void SetAnimationCurve(wxAnimationCurve animation_curve);

    /**
        Gets the animation curve that will be used for interpolation.
    */
    wxAnimationCurve GetAnimationCurve();

    /**
        Gets the target property for this animation.
    */
    wxAnimationTargetProperty GetTargetProperty() const;
};

/**
    @class wxUIAnimationKeyframe

    The key frame will animate from the value of a previous key frame to its own value over the
    previous key frame duration using the animation curve of the parent animation.

    @since 2.9.3

    @see @ref supported_animation_types, @ref animation_overview
*/
template<class T> class wxUIAnimationKeyframe
{
public:
    /**
        Creates a new key frame with the specified value and duration.
    */
    wxUIAnimationKeyframe(T value, double durationInSeconds);

    /**
        Sets the length of time (in seconds) for which the next key frame plays.
    */
    void SetDuration(double keyframe_duration);

    /**
        Gets the length of time (in seconds) for which the next key frame plays.
    */
    double GetDuration() const;

    /**
        Sets the value that the previous key frame will animate to and that the next key frame
        will animate from.
    */
    void SetValue(T value);

    /**
        Gets the value that the previous key frame will animate to and that the next key frame
        will animate from.
    */
    T GetValue() const;
};

/**
    @class wxUIKeyframeAnimation

    wxUIAnimationBase provides basic, multi-keyframe animation capabilities for a control 
    property.

    The animation will start at the first key frame using its duration and advance to the value
    of the next key frame. As such the duration of the last key frame is always ignored.

    Please note that users should not use this class directly but instead used the supported
    predefined key frame animation types such as wxDoubleKeyframeAnimation or wxPointKeyframeAnimation.

    @since 2.9.3

    @see @ref supported_animation_types, @ref animation_overview
*/
template<class T> class wxUIKeyframeAnimation
{
public:
    /**
        Creates a new key frame animation with the specified values and duration.

        The type will be inforced by the selected property type.

        If left un-specified it will use an ease in-out animation curve.
    */
    wxUIAnimation(Type initialValue,
        Type targetValue,
        double duration,
        wxAnimationCurve curve = wxANIMATION_CURVE_EASE_IN_OUT);

    /**
        TOOD: decide what to do with this function
    */
    void SetDuration(double animation_duration);

    /**
        Returns the length of time (in seconds) for which this animation plays as the sum of its key 
        frames' duration.
    */
    double GetDuration() const;

    /**
        Sets the animation curve that will be used for interpolation.
    */
    void SetAnimationCurve(wxAnimationCurve animation_curve);

    /**
        Gets the animation curve that will be used for interpolation.
    */
    wxAnimationCurve GetAnimationCurve() const;

    /**
        Gets the target property for this animation.
    */
    wxAnimationTargetProperty GetTargetProperty() const;

    /**
        Adds a new key frame to the animation.
    */
    void AddKeyframe(wxUIAnimationKeyframe<T>* keyframe);
    
    /**
        Returns the key frames that define this animation.
    */
    const wxVector<const wxUIAnimationKeyframe<T>>& GetKeyframes();
};