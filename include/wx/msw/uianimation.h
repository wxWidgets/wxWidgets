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
#include <UIAnimation.h>

// MSW-only animation class.
class wxUIAnimationMSW : public wxUIAnimationBase
{
public:
    wxUIAnimationMSW(IUIAnimationManager* manager,
        IUIAnimationTransitionLibrary* library,
        IUIAnimationStoryboard* storyboard,
        double durationInSeconds,
        wxAnimationTargetProperty targetProperty,
        wxAnimationCurve curve)
        : wxUIAnimationBase(durationInSeconds, targetProperty, curve)
    {
        m_animationManager = manager;
        m_transitionLibrary = library;
        m_storyboard = storyboard;
    }

    ~wxUIAnimationMSW();

    // Builds the pair of varibles and transitions for a basic animation.
    template <class T> bool Build(T initialValue,
        T targetValue);
    
    // Builds the MSW animation variables for a key frame animation.
    template <class T> bool Build(T value);

    // Creates and adds transitions for current animation step of a key frame animation.
    template <class T> bool AddTransitionForKeyframe(const wxUIAnimationKeyframe<T>* keyframe,
        const wxUIAnimationKeyframe<T>* nextKeyframe,
        double delay);
    
    // Returns the value of the animation variables as the specified type.
    template <class T> T GetValue();

    const wxVector<IUIAnimationTransition*>& GetTransitions()
    {
        return m_transitions;
    }

    const wxVector<IUIAnimationVariable*>& GetVariables()
    {
        return m_variables;
    }

    // Queues an instantaneous transition in addition to the existing transitions.
    // This will allow cause the animation to snap back to the specified value.
    template <class T> bool QueueInstantaneousTransitionAtEnd(T value);

private:
    bool AddTransition(double initialValue, double targetValue);

    bool CreateAndQueueVariable(double value);

    bool CreateAndQueueTransition(double value);
    bool CreateAndQueueTransition(double durationInSeconds, double value);
    bool CreateAndQueueInstantaneousTransition(double value);
    bool CreateAndQueueDiscreteTransition(double value);
    bool CreateAndQueueDiscreteTransition(double durationInSeconds, double value);

    bool AddLastCreatedTransitionToStoryboard();
    bool AddLastCreatedTransitionToStoryboard(IUIAnimationVariable* variable);
    bool AddLastCreatedTransitionToStoryboard(IUIAnimationVariable* variable,
        UI_ANIMATION_KEYFRAME keyFrame);
    bool AddTransitionToStoryboard(IUIAnimationVariable* variable,
        IUIAnimationTransition* transition,
        UI_ANIMATION_KEYFRAME keyFrame);

    IUIAnimationManager *m_animationManager;
    IUIAnimationTransitionLibrary *m_transitionLibrary;
    IUIAnimationStoryboard *m_storyboard;

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

template<> wxSize wxUIAnimationMSW::GetValue();

template<> bool wxUIAnimationMSW::GetValue();

template<class T> inline T wxUIAnimationMSW::GetValue()
{
};

// Basic animation creation for different types. Since MSW variables are of type double different
// implementation is needed for different types.
template<> bool wxUIAnimationMSW::Build(int initialValue,
    int targetValue);

template<> bool wxUIAnimationMSW::Build(double initialValue,
    double targetValue);

template<> bool wxUIAnimationMSW::Build(wxPoint initialValue,
    wxPoint targetValue);

template<> bool wxUIAnimationMSW::Build(wxColour initialValue,
    wxColour targetValue);

template<> bool wxUIAnimationMSW::Build(wxSize initialValue,
    wxSize targetValue);

template<> bool wxUIAnimationMSW::Build(bool initialValue,
    bool targetValue);

// Keyframe animation creation for different types.
template<> bool wxUIAnimationMSW::Build(int initialValue);

template<> bool wxUIAnimationMSW::Build(double initialValue);

template<> bool wxUIAnimationMSW::Build(wxPoint initialValue);

template<> bool wxUIAnimationMSW::Build(wxColour initialValue);

template<> bool wxUIAnimationMSW::Build(wxColour initialValue);

template<> bool wxUIAnimationMSW::Build(wxSize initialValue);

template<> bool wxUIAnimationMSW::Build(bool initialValue);

template <> bool wxUIAnimationMSW::QueueInstantaneousTransitionAtEnd(bool value);

template <> bool wxUIAnimationMSW::QueueInstantaneousTransitionAtEnd(double value);

template <> bool wxUIAnimationMSW::QueueInstantaneousTransitionAtEnd(wxPoint value);

template <> bool wxUIAnimationMSW::QueueInstantaneousTransitionAtEnd(wxColour value);

template <> bool wxUIAnimationMSW::QueueInstantaneousTransitionAtEnd(wxSize value);

template <> bool wxUIAnimationMSW::QueueInstantaneousTransitionAtEnd(bool value);

// Keyframe transition creation
template<> bool wxUIAnimationMSW::AddTransitionForKeyframe(const wxUIAnimationKeyframe<int>* keyframe,
    const wxUIAnimationKeyframe<int>* nextKeyframe,
    double delay);

template<> bool wxUIAnimationMSW::AddTransitionForKeyframe(const wxUIAnimationKeyframe<double>* keyframe,
    const wxUIAnimationKeyframe<double>* nextKeyframe,
    double delay);

template<> bool wxUIAnimationMSW::AddTransitionForKeyframe(const wxUIAnimationKeyframe<wxPoint>* keyframe,
    const wxUIAnimationKeyframe<wxPoint>* nextKeyframe,
    double delay);

template<> bool wxUIAnimationMSW::AddTransitionForKeyframe(const wxUIAnimationKeyframe<wxColour>* keyframe,
    const wxUIAnimationKeyframe<wxColour>* nextKeyframe,
    double delay);

template<> bool wxUIAnimationMSW::AddTransitionForKeyframe(const wxUIAnimationKeyframe<wxSize>* keyframe,
    const wxUIAnimationKeyframe<wxSize>* nextKeyframe,
    double delay);

template<> bool wxUIAnimationMSW::AddTransitionForKeyframe(const wxUIAnimationKeyframe<bool>* keyframe,
    const wxUIAnimationKeyframe<bool>* nextKeyframe,
    double delay);

// The methods bellow return false to indicate that the type used is not supported.
template<class T> inline bool wxUIAnimationMSW::AddTransitionForKeyframe(const wxUIAnimationKeyframe<T>* keyframe,
    const wxUIAnimationKeyframe<T>* nextKeyframe,
    double delay)
{
    return false;
}

template<class T> inline bool wxUIAnimationMSW::Build(T value)
{
    return false;
}

template<class T> inline bool wxUIAnimationMSW::Build(T initialValue,
    T targetValue)
{
    return false;
};
#endif//_WX_UI_ANIMATION_MSW_H_