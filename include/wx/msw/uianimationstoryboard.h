///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/uianimationstoryboard.h
// Purpose:     Platform specific storyboard declaration
// Author:      Chiciu Bogdan Gabriel
// Created:     2011-06-18
// RCS-ID:      $Id$
// Copyright:   (c) 2011 wxWidgets Team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UI_ANIMATION_STORYBOARD_MSW_H_
#define _WX_UI_ANIMATION_STORYBOARD_MSW_H_

//includes are needed for AddAnimation
#include "wx/uianimation.h"
#include "wx/msw/uianimation.h"

#include <wx/sharedptr.h>

// ----------------------------------------------------------------------------
// Forward declarations.
// ----------------------------------------------------------------------------
class WXDLLIMPEXP_FWD_CORE wxControl;
struct IUIAnimationManager;
struct IUIAnimationTimer;
struct IUIAnimationTransitionLibrary;
struct IUIAnimationStoryboard;

// MSW storyboard implementation.
class WXDLLIMPEXP_ANIMATION wxUIAnimationStoryboardMSW : public wxUIAnimationStoryboardBase
{
public:
    friend class UIAnimationTimerEventHandlerBase;// Allows the call to Update

    wxUIAnimationStoryboardMSW();

    virtual ~wxUIAnimationStoryboardMSW();

    // Adds animations to the storyboard -- also performs initialization of animation related Windows Animation objects
    // such as IUIAnimationTransition and IUIAnimationVariable.
    // NOTE: We do not take ownership of the object. (Will be replaced with const reference soon)
    template<wxAnimationTargetProperty Property> bool AddAnimation(wxUIAnimation<Property>* animation)
    {
        wxCHECK(animation, false);
        wxCHECK_MSG(animation->GetDuration() >= 0, false, "Animation duration can't be negative.");
        
        // The properties from the animation are given over to the MSW Animation. Should be replaced with a copy constructor.
        wxSharedPtr<wxUIAnimationMSW> animationData(new wxUIAnimationMSW(animation->GetDuration(), animation->GetTargetProperty(), animation->GetAnimationCurve()));

        if(!animationData->Build<wxUIAnimation<Property>::Type>(m_animationManager,
            m_transitionLibrary,
            animation->GetInitialValue(),
            animation->GetTargetValue()))
        {
            return false;
        }
        if(!animationData->AddTransitionsToStoryboard(m_storyboard))
        {
            return false;
        }
        m_animations.push_back(animationData);
        return true;
    }

    // NOTE: We do not take ownership of the object. (Will be replaced with const reference soon)
    template<wxAnimationTargetProperty Property> bool AddAnimation(wxUIKeyframeAnimation<Property>* animation)
    {
        // Keyframe animation works by having multiple transitions for the same variable
        // Build will take care of building the variable and AddTransitionForKeyframe
        // will create the transition.
        wxCHECK(animation, false);
        wxCHECK_MSG(animation->GetDuration() >= 0, false, "Animation duration can't be negative.");

        const wxVector<const wxUIAnimationKeyframe<wxUIAnimation<Property>::Type>> keyframes = animation->GetKeyframes();
        
        wxCHECK_MSG(keyframes.size() >= 1, false, "Key frame animations must have more than one key frames added to them.");
        wxVector<const wxUIAnimationKeyframe<wxUIAnimation<Property>::Type>>::const_iterator keyframe_iter = keyframes.begin();

        wxSharedPtr<wxUIAnimationMSW> animationData(new wxUIAnimationMSW(animation->GetDuration(), animation->GetTargetProperty(), animation->GetAnimationCurve()));

        if(!animationData->Build(m_animationManager, keyframes.at(0).GetValue()))
        {
            return false;
        }
        double delay = 0;

        // This iteration will add the current/next key frame transitions(the logic behind it is that we need
        // the values from one key frame to the next) ignoring the final keyframe as he was already paried with
        // the key frame before him.
        for(;;)
        {
            const wxUIAnimationKeyframe<wxUIAnimation<Property>::Type> currentKeyframe = (*keyframe_iter);
            wxCHECK_MSG(currentKeyframe.GetDuration() >= 0, false, "Keyframe duration can't be negative.");

            ++keyframe_iter;// Go to the next key frame.
            if( keyframe_iter == keyframes.end())// The last keyframe was already handled in the previous iteration.
            {
                break;
            }
            const wxUIAnimationKeyframe<wxUIAnimation<Property>::Type> nextKeyframe = (*keyframe_iter);
            if(!animationData->AddTransitionForKeyframe(m_storyboard,
                m_transitionLibrary,
                &currentKeyframe,
                &nextKeyframe,
                delay))
            {
                return false;
            }
            m_animations.push_back(animationData);
            delay += currentKeyframe.GetDuration();
        }
        return true;
    }

    // Begins playing the animations stored by the storyboard.
    void Start();

    // Stops all animations that the storyboard currently owns.
    void Stop();
private:
    // This function updates the target control's property to a new value. It is called on each animation tick.
    void Update();

    // Performs basic initialization of the Windows Animation classes.
    bool Initialize();
    // ----------------------------------------------------
    // UIAnimation objects.
    // ----------------------------------------------------
    IUIAnimationManager *m_animationManager;
    IUIAnimationTimer *m_animationTimer;
    IUIAnimationTransitionLibrary *m_transitionLibrary;
    IUIAnimationStoryboard *m_storyboard;

    //Stores wxUIAnimation objects that take part in the actual animation.
    wxVector<wxSharedPtr<wxUIAnimationMSW>> m_animations;

    wxDECLARE_NO_COPY_CLASS(wxUIAnimationStoryboardMSW);
};

#endif// _WX_UI_ANIMATION_STORYBOARD_MSW_H_