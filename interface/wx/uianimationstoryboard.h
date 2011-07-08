///////////////////////////////////////////////////////////////////////////////
// Name:        uianimationstoryboard.h
// Purpose:     Interface of wxUIAnimationStoryboard
// Author:      Chiciu Bogdan Gabriel
// Created:     2011-06-30
// RCS-ID:      $Id
// Copyright:   (c) wxWidgets Team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/**
    Specifies how a storyboard target behaves when the storyboard animations
    finish playing.
*/
enum wxStoryboardFillBehavior
{
    /// When used the target control will remain at its end value when the animation finishes.
    wxSTORYBOARD_FILL_BEHAVIOR_HOLD_END,
    /// When used the target control will reset to its initial value when the animation finishes.
    wxSTORYBOARD_FILL_BEHAVIOR_STOP,
};

enum wxStoryboardRepeatBehavior
{
    /// Indicates that the storyboard should repeat forever.
    wxSTORYBOARD_REPEAT_BEHAVIOR_FOREVER = -1,
}

/**
    @class wxUIAnimationStoryboardBase

    wxUIAnimationStoryboardBase acts as a container for animations and is responsible for 
    controlling and providing them with a target object.

    @see @ref animation_overview
*/
class wxUIAnimationStoryboardBase
{
public:
    /**
        Sets the target control that the animations contained by the storyboard
        will animate.
    */
    void SetAnimationTarget(wxControl* target_control);
    /**
        Start playing the animations contained by the storyboard.
    */
    virtual void Begin();
    /**
        Stops playing the animations contained by the storyboard.
    */
    virtual void Stop();
    /**
        Adds a new basic animation to the storyboard.
        Returns false if the animation is invalid or the underlaying API fails.
    */
    template <class T> bool AddAnimation(wxUIAnimationBase<T>* animation);
    /**
        Adds a new key frame animation to the storyboard.
        Returns false if the animation is invalid or the underlaying API fails.
    */
    template <class T> bool AddAnimation(wxUIKeyframeAnimationBase<T>* animation);
    /**
        Sets a value that represents the behavior of the control after the animation
        finishes playing.
    */
    void SetFillBehavior(wxStoryboardFillBehavior);
    /**
        Gets a value that represents the behavior of the control after the animation
        finishes playing.
    */
    wxStoryboardFillBehavior GetFillBehavior();
    /**
        Sets the number of times this storyboard should repeat playing.
    */
    void SetRepeatCount(int count);
    /**
        Gets the number of times this storyboard should repeat playing.
        Use wxSTORYBOARD_REPEAT_BEHAVIOR_FOREVER to make the storyboard repeat forever.
    */
    int GetRepeatCount();
};