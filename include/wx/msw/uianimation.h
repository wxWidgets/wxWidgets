///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/uianimation.h
// Purpose:     Declares the internal MSW animation class used by the MSW storyboard
// Author:      Chiciu Bogdan Gabriel
// Created:     2011-06-26
// RCS-ID:      $Id$
// Copyright:   (c) 2011 wxWidgets Team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UI_ANIMATION_MSW_H_
#define _WX_UI_ANIMATION_MSW_H_

struct IUIAnimationTransitionLibrary;
struct IUIAnimationManager;
struct IUIAnimationVariable;
struct IUIAnimationTransition;
struct IUIAnimationStoryboard;

#include "wx/uianimation.h"

// MSW-only animation class.
class wxUIAnimationMSW : public wxUIAnimationBase
{
public:
    wxUIAnimationMSW(double durationInSeconds, wxAnimationTargetProperty targetProperty, wxAnimationCurve curve)
        : wxUIAnimationBase(durationInSeconds, targetProperty, curve)
    {
    }
    ~wxUIAnimationMSW();

    // Builds the pair of varibles and transitions for a basic animation.
    // TODO [iteration]: cleanup parameters
    template <class T> bool Build(IUIAnimationManager* manager, 
        IUIAnimationTransitionLibrary* library,
        T initialValue,
        T targetValue);
    
    // Builds the MSW animation variables for a key frame animation.
    template <class T> bool Build(IUIAnimationManager* manager,
        T initial_value);
    
    // Returns the value of the animation variables as the specified type.
    template <class T> T GetValue();
    
    // Adds the variable-transition pair to the MSW storyboard object. Called when a basic
    // animation is added.
    bool AddTransitionsToStoryboard(IUIAnimationStoryboard* storyboard);

    // Creates and adds transitions for current animation step of a keyframe animation.
    template <class T> bool AddTransitionForKeyframe(IUIAnimationStoryboard* storyboard,
        IUIAnimationTransitionLibrary* library,
        const wxUIAnimationKeyframe<T>* keyframe,
        const wxUIAnimationKeyframe<T>* nextKeyframe,
        double delay);
    
    const wxVector<IUIAnimationTransition*>& GetTransitions()
    {
        return m_transitions;
    }

    const wxVector<IUIAnimationVariable*>& GetVariables()
    {
        return m_variables;
    }

    //old: currently unused
    bool AddKeyframeTransitionsToStoryboard(IUIAnimationStoryboard* storyboard, double delay);

    // Queues an instantaneous transition in addition to the existing transitions.
    // This will allow cause the animation to snap back to the specified value.
    template <class T> bool QueueInstantaneousTransitionAtEnd(IUIAnimationStoryboard* storyboard,
        IUIAnimationTransitionLibrary* library,
        T value);

private:
    //TODO: should these pe wxSharedPtrs as well?
    wxVector<IUIAnimationTransition*> m_transitions;
    wxVector<IUIAnimationVariable*> m_variables;

    wxDECLARE_NO_COPY_CLASS(wxUIAnimationMSW);
};

//Each supported type is built/returned/handled differently.
template<> int wxUIAnimationMSW::GetValue();

template<> double wxUIAnimationMSW::GetValue();

template<> wxPoint wxUIAnimationMSW::GetValue();

template<> wxColour wxUIAnimationMSW::GetValue();

template<class T> inline T wxUIAnimationMSW::GetValue()
{
};

// Basic animation creation for different types. Since MSW variables are of type double different
// implementation is needed for different types.
template<> bool wxUIAnimationMSW::Build(IUIAnimationManager* manager,
    IUIAnimationTransitionLibrary* library,
    int initialValue,
    int targetValue);

template<> bool wxUIAnimationMSW::Build(IUIAnimationManager* manager,
    IUIAnimationTransitionLibrary* library,
    double initialValue,
    double targetValue);

template<> bool wxUIAnimationMSW::Build(IUIAnimationManager* manager,
    IUIAnimationTransitionLibrary* library,
    wxPoint initialValue,
    wxPoint targetValue);

template<> bool wxUIAnimationMSW::Build(IUIAnimationManager* manager,
    IUIAnimationTransitionLibrary* library,
    wxColour initialValue,
    wxColour targetValue);

// Keyframe animation creation for different types.
template<> bool wxUIAnimationMSW::Build(IUIAnimationManager* manager,
    int initial_value);

template<> bool wxUIAnimationMSW::Build(IUIAnimationManager* manager,
    double initial_value);

template<> bool wxUIAnimationMSW::Build(IUIAnimationManager* manager,
    wxPoint initial_value);

template<> bool wxUIAnimationMSW::Build(IUIAnimationManager* manager,
    wxColour initial_value);

template <> bool wxUIAnimationMSW::QueueInstantaneousTransitionAtEnd(IUIAnimationStoryboard* storyboard,
    IUIAnimationTransitionLibrary* library,
    int value);

template <> bool wxUIAnimationMSW::QueueInstantaneousTransitionAtEnd(IUIAnimationStoryboard* storyboard,
    IUIAnimationTransitionLibrary* library,
    double value);

template <> bool wxUIAnimationMSW::QueueInstantaneousTransitionAtEnd(IUIAnimationStoryboard* storyboard,
    IUIAnimationTransitionLibrary* library,
    wxPoint value);

template <> bool wxUIAnimationMSW::QueueInstantaneousTransitionAtEnd(IUIAnimationStoryboard* storyboard,
    IUIAnimationTransitionLibrary* library,
    wxColour value);

// Keyframe transition creation
template<> bool wxUIAnimationMSW::AddTransitionForKeyframe(IUIAnimationStoryboard* storyboard,
    IUIAnimationTransitionLibrary* library,
    const wxUIAnimationKeyframe<int>* keyframe,
    const wxUIAnimationKeyframe<int>* nextKeyframe,
    double delay);

template<> bool wxUIAnimationMSW::AddTransitionForKeyframe(IUIAnimationStoryboard* storyboard,
    IUIAnimationTransitionLibrary* library,
    const wxUIAnimationKeyframe<double>* keyframe,
    const wxUIAnimationKeyframe<double>* nextKeyframe,
    double delay);

template<> bool wxUIAnimationMSW::AddTransitionForKeyframe(IUIAnimationStoryboard* storyboard,
    IUIAnimationTransitionLibrary* library,
    const wxUIAnimationKeyframe<wxPoint>* keyframe,
    const wxUIAnimationKeyframe<wxPoint>* nextKeyframe,
    double delay);

template<> bool wxUIAnimationMSW::AddTransitionForKeyframe(IUIAnimationStoryboard* storyboard,
    IUIAnimationTransitionLibrary* library,
    const wxUIAnimationKeyframe<wxColour>* keyframe,
    const wxUIAnimationKeyframe<wxColour>* nextKeyframe,
    double delay);

// The methods bellow return false to indicate that the type used is not supported.
template<class T> inline bool wxUIAnimationMSW::AddTransitionForKeyframe(IUIAnimationStoryboard* storyboard,
    IUIAnimationTransitionLibrary* library,
    const wxUIAnimationKeyframe<T>* keyframe,
    const wxUIAnimationKeyframe<T>* nextKeyframe,
    double delay)
{
    return false;
}

template<class T> inline bool wxUIAnimationMSW::Build(IUIAnimationManager* manager,
    T initial_value)
{
    return false;
}

template<class T> inline bool wxUIAnimationMSW::Build(IUIAnimationManager* manager,
    IUIAnimationTransitionLibrary* library,
    T initialValue,
    T targetValue)
{
    return false;
};
#endif//_WX_UI_ANIMATION_MSW_H_