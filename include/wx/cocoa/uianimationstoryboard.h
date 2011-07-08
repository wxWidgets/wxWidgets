///////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/uianimationstoryboard.h
// Purpose:     Platform specific storyboard declaration
// Author:      Chiciu Bogdan Gabriel
// Created:     2011-07-06
// RCS-ID:      $Id$
// Copyright:   (c) 2011 wxWidgets Team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UI_ANIMATION_STORYBOARD_COCOA_H_
#define _WX_UI_ANIMATION_STORYBOARD_COCOA_H_

//includes are needed for AddAnimation
#include "wx/uianimation.h"
#include <CAAnimation.h>

#include <wx/sharedptr.h>

// ----------------------------------------------------------------------------
// Forward declarations.
// ----------------------------------------------------------------------------
class WXDLLIMPEXP_FWD_CORE wxControl;

// Cocoa storyboard implementation.
class WXDLLIMPEXP_ANIMATION wxUIAnimationStoryboardCocoa : public wxUIAnimationStoryboardBase
{
public:
    wxUIAnimationStoryboardCocoa();

    virtual ~wxUIAnimationStoryboardCocoa();

    // Adds a new animation to the storyboard.
    // NOTE: We do not take ownership of the object. (Will be replaced with const reference soon)
    template<wxAnimationTargetProperty Property> bool AddAnimation(wxUIAnimation<Property>* animation)
    {
        wxCHECK(animation, false);
        wxCHECK_MSG(animation->GetDuration() >= 0, false, "Animation duration can't be negative.");

        CABasicAnimation* cocoaAnimation = [CABasicAnimation animationWithKeyPath:wxPropertyType<Property>::OsXKeyPath];
        
        cocoaAnimation.fromValue = wxPropertyType<Property>::GetOsXValue(animation->GetInitialValue());
        cocoaAnimation.toValue = wxPropertyType<Property>::GetOsXValue(animation->GetTargetValue());
        
        cocoaAnimation.timingFunction = GetCAMediaTimingFunctionFromAnimationCurve(animation->GetAnimationCurve());
        cocoaAnimation.duration = animation->GetDuration();
        cocoaAnimation.delegate = this/*self?*/;

        // At this point we just need to store the CABasicAnimation object.
        m_animations.push_back(wxSharedPtr(cocoaAnimation));
        return true;
    }

    // Adds a new key frame animation to the storyboard.
    // NOTE: We do not take ownership of the object. (Will be replaced with const reference soon)
    template<wxAnimationTargetProperty Property> bool AddAnimation(wxUIKeyframeAnimation<Property>* animation)
    {
        wxCHECK(animation, false);
        wxCHECK_MSG(animation->GetDuration() >= 0, false, "Animation duration can't be negative.");

        const wxVector<const wxUIAnimationKeyframe<wxUIAnimation<Property>::Type>> keyframes = animation->GetKeyframes();
        wxVector<const wxUIAnimationKeyframe<wxUIAnimation<Property>::Type>>::const_iterator keyframeIter;

        wxCHECK_MSG(keyframes.size() >= 1, false, "Key frame animations must have more than one key frames added to them.");

        CAKeyframeAnimation* cocoaAnimation; 
        cocoaAnimation = [CAKeyframeAnimation animationWithKeyPath:wxPropertyType<Property>::OsXKeyPath]; 
        cocoaAnimation.duration = animation->GetDuration();// TODO: we might have to substract the final key frame duration 

        NSMutableArray* values = [NSMutableArray array];
        NSMutableArray* timings = [NSMutableArray array];
        NSMutableArray* keytimes = [NSMutableArray array];

        for(keyframeIter = keyframes.begin(); keyframeIter != keyframes.end(); ++keyframeIter)
        {
            wxCHECK_MSG(currentKeyframe.GetDuration() >= 0, false, "Keyframe duration can't be negative.");

            [values addObject:wxPropertyType<Property>::GetOSXValue((*keyframeIter).GetValue())];
            [timings addObject:GetCAMediaTimingFunctionFromAnimationCurve(animation->GetAnimationCurve())];
            [keytimes addObject:[NSNumber numberWithFloat:(*keyframeeIter).GetDuration()]];
        }
        
        animation.values = values;
        animation.timingFunctions = timings;
        animation.keyTimes = keytimes;

        m_animations.push_back(animation_data);
        return true;
    }

    // Begins playing the animations stored by the storyboard.
    void Start();

    // Stops all animations that the storyboard currently owns.
    void Stop();
private:
    static CAMediaTimingFunction GetCAMediaTimingFunctionFromAnimationCurve(wxAnimationCurve curve);

    //Stores CABasicAnimation/Keyframe animation objects that take part in the actual animation.
    wxVector<wxSharedPtr<CAAnimation>> m_animations;

    wxDECLARE_NO_COPY_CLASS(wxUIAnimationStoryboardCocoa);
};

#endif// _WX_UI_ANIMATION_STORYBOARD_COCOA_H_



