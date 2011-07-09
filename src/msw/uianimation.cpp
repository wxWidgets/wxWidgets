///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/uianimation.cpp
// Purpose:     Animation class implementation using Windows Animation
// Author:      Chiciu Bogdan Gabriel
// Created:     2011-06-26
// RCS-ID:      $Id$
// Copyright:   (c) 2011 wxWidgets Team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif
#include "wx/msw/uianimation.h"
#include "wx/uianimation.h"

#include <UIAnimation.h>

WX_CHECK_BUILD_OPTIONS("wxAnimation")

// TODO: not every call to Windows Animation is checked for errors.
// TODO: remove duplicate/similar code

namespace 
{

IUIAnimationTransition* CreateTransitionFromAnimationCurve(IUIAnimationTransitionLibrary* library, 
    wxAnimationCurve curve,
    UI_ANIMATION_SECONDS duration,
    double finalValue)
{
    IUIAnimationTransition* transition = NULL;
    HRESULT result;
    switch(curve)
    {
    case wxANIMATION_CURVE_EASE_IN_OUT:
        {
            //The hard coded values of 0.0, 0.25, 0.5 used should make the MSW animation look like the OS X equivalents.
            //TODO: re-check the values
            result = library->CreateAccelerateDecelerateTransition(duration, finalValue, 0.5, 0.5, &transition);
            if(!SUCCEEDED(result))
            {
                return NULL;
            }
            break;
        }
    case wxANIMATION_CURVE_LINEAR:
        {
            result = library->CreateLinearTransition(duration, finalValue, &transition);
            if(!SUCCEEDED(result))
            {
                return NULL;
            }
            break;
        }
    case wxANIMATION_CURVE_EASE_IN:
        {
            result = library->CreateAccelerateDecelerateTransition(duration, finalValue, 0, 0.25, &transition);
            if(!SUCCEEDED(result))
            {
                return NULL;
            }
            break;
        }
    case wxANIMATION_CURVE_EASE_OUT:
        {
            result = library->CreateAccelerateDecelerateTransition(duration, finalValue, 0.25, 0, &transition);
            if(!SUCCEEDED(result))
            {
                return NULL;
            }
            break;
        }
#if 0//MSW-only easing functions. Kept for reference
    case wxAnimationCurve::wxANIMATION_CURVE_CONSTANT:
        {
            //Pointless animation. The value remains the same.
            library->CreateConstantTransition(duration, &transition);
            break;
        }
    case wxAnimationCurve::wxANIMATION_CURVE_CUBIC:
        {
            library->CreateCubicTransition(duration,final_value,0,&transition);
            break;
        }
    case wxAnimationCurve::wxANIMATION_CURVE_DISCRETE:
        {
            library->CreateDiscreteTransition(0,final_value,0,&transition);
            break;
        }

    case wxAnimationCurve::wxANIMATION_CURVE_INSTANTANEOUS:
        {
            library->CreateInstantaneousTransition(final_value,&transition);
            break;
        }
    case wxAnimationCurve::wxANIMATION_CURVE_LINEAR_FROM_SPEED:
        {
            library->CreateLinearTransitionFromSpeed(0,final_value,&transition);
            break;
        }
    case wxAnimationCurve::wxANIMATION_CURVE_PARABOLIC_FROM_ACCELERATION:
        {
            library->CreateParabolicTransitionFromAcceleration(final_value, 0,0, &transition);
            break;
        }
    case wxAnimationCurve::wxANIMATION_CURVE_REVERSAL:
        {
            library->CreateReversalTransition(duration, &transition);
            break;
        }
    case wxAnimationCurve::wxANIMATION_CURVE_SINUSOIDAL_FROM_RANGE:
        {
            UI_ANIMATION_SLOPE slope = UI_ANIMATION_SLOPE::UI_ANIMATION_SLOPE_DECREASING;
            library->CreateSinusoidalTransitionFromRange(duration, 0,0,0,slope, &transition);
            break;
        }
    case wxAnimationCurve::wxANIMATION_CURVE_SINUSOIDAL_FROM_VELOCITY:
        {
            library->CreateSinusoidalTransitionFromVelocity(duration,0,&transition);
            break;
        }
    case wxAnimationCurve::wxANIMATION_CURVE_SMOOTH_STEP:
        {
            library->CreateSmoothStopTransition(duration,final_value, &transition);
            break;
        }
#endif
    }
    return transition;
}

IUIAnimationVariable* BuildAnimationVariable(IUIAnimationManager* manager, double initialValue)
{
    IUIAnimationVariable* variable;
    HRESULT result;
    result = manager->CreateAnimationVariable(initialValue, &variable);
    if(!SUCCEEDED(result))
    {
        return NULL;
    }
    return variable;
}

}
// This should release every COM object that we have a reference to.
wxUIAnimationMSW::~wxUIAnimationMSW()
{
    wxVector<IUIAnimationTransition*>::iterator transitionIter;
    wxVector<IUIAnimationVariable*>::iterator variableIter;

    for(transitionIter = m_transitions.begin(); transitionIter != m_transitions.end(); ++transitionIter)
    {
        (*transitionIter)->Release();
    }

    for(variableIter = m_variables.begin(); variableIter != m_variables.end(); ++variableIter)
    {
        (*variableIter)->Release();
    }

    m_transitions.clear();
    m_variables.clear();
}

bool wxUIAnimationMSW::AddTransitionsToStoryboard(IUIAnimationStoryboard* storyboard)
{
    wxVector<IUIAnimationTransition*>::iterator transitionIter;
    wxVector<IUIAnimationVariable*>::iterator variableIter;
    HRESULT result;
    wxCHECK(m_transitions.size() == m_variables.size(), false);
    for(transitionIter = m_transitions.begin(),variableIter = m_variables.begin(); 
        transitionIter != m_transitions.end();
        ++variableIter, ++transitionIter)
    {
        result = storyboard->AddTransition((*variableIter), (*transitionIter));
        if(!SUCCEEDED(result))
        {
            return false;
        }
    }
    return true;
}

bool wxUIAnimationMSW::AddKeyframeTransitionsToStoryboard(IUIAnimationStoryboard* storyboard, double delay)
{
    //Each variable has at least one transition and each variable has and equal number of transitions.
    //As a rule we will always have at least and equal number of transitions to the number of variables.

    // TODO: add checks
    wxVector<IUIAnimationTransition*>::iterator transition_iter;
    wxVector<IUIAnimationVariable*>::iterator variable_iter = m_variables.begin();
    HRESULT result;
    for(transition_iter = m_transitions.begin(); transition_iter != m_transitions.end(); transition_iter++)
    {
        UI_ANIMATION_KEYFRAME keyframe;
        result = storyboard->AddKeyframeAtOffset(UI_ANIMATION_KEYFRAME_STORYBOARD_START,delay,&keyframe);
        if(!SUCCEEDED(result))
        {
            return false;
        }
        result = storyboard->AddTransitionAtKeyframe((*variable_iter), (*transition_iter), keyframe);
        if(!SUCCEEDED(result))
        {
            return false;
        }
        
        ++variable_iter;
        if(variable_iter == m_variables.end())//we're at the end of the variable list so start over
        {
            variable_iter = m_variables.begin();
        }
    }
    return true;
}

template<> int wxUIAnimationMSW::GetValue()
{
    int value;
    m_variables.at(0)->GetIntegerValue(&value);
    return value;
}

template<> double wxUIAnimationMSW::GetValue()
{
    double value;
    m_variables.at(0)->GetValue(&value);
    return value;
}

template<> wxPoint wxUIAnimationMSW::GetValue()
{
    double x;
    double y;
    m_variables.at(0)->GetValue(&x);
    m_variables.at(1)->GetValue(&y);
    return wxPoint(x,y);
}

template<> wxColour wxUIAnimationMSW::GetValue()
{
    double alpha;
    double red;
    double green;
    double blue;
    
    m_variables.at(0)->GetValue(&alpha);
    m_variables.at(1)->GetValue(&red);
    m_variables.at(2)->GetValue(&blue);
    m_variables.at(3)->GetValue(&green);
    
    return wxColour(red,green,blue,alpha);
}

template<> bool wxUIAnimationMSW::Build(IUIAnimationManager* manager,
    IUIAnimationTransitionLibrary* library,
    int initialValue,
    int targetValue)
{
    IUIAnimationVariable* variable;
    IUIAnimationTransition* transition;
    // Create transition
    transition = CreateTransitionFromAnimationCurve(library, m_curve, m_durationInSeconds, targetValue);
    
    // Create variable
    HRESULT result;
    result = manager->CreateAnimationVariable(initialValue,&variable);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    m_variables.push_back(variable);
    m_transitions.push_back(transition);

    return true;
}
template<> bool wxUIAnimationMSW::Build(IUIAnimationManager* manager,
    IUIAnimationTransitionLibrary* library,
    double initialValue,
    double targetValue)
{
    IUIAnimationVariable* variable = BuildAnimationVariable(manager, initialValue);
    IUIAnimationTransition* transition = CreateTransitionFromAnimationCurve(library, 
        m_curve,
        m_durationInSeconds,
        targetValue);

    if(transition == NULL)
    {
        return false;
    }
    m_variables.push_back(variable);
    m_transitions.push_back(transition);

    return true;
}

template<> bool wxUIAnimationMSW::Build(IUIAnimationManager* manager,
    IUIAnimationTransitionLibrary* library,
    wxPoint initialValue,
    wxPoint targetValue)
{
    IUIAnimationVariable* variable_x;
    IUIAnimationVariable* variable_y;
    IUIAnimationTransition* transition_x;
    IUIAnimationTransition* transition_y;
    
    transition_x = CreateTransitionFromAnimationCurve(library, m_curve, m_durationInSeconds, targetValue.x);
    transition_y = CreateTransitionFromAnimationCurve(library, m_curve, m_durationInSeconds, targetValue.y);

    if((transition_x == NULL) || (transition_y == NULL))
    {
        return false;
    }
    HRESULT result;
    result = manager->CreateAnimationVariable(initialValue.x,&variable_x);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = manager->CreateAnimationVariable(initialValue.y,&variable_y);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    m_variables.push_back(variable_x);
    m_transitions.push_back(transition_x);
    m_variables.push_back(variable_y);
    m_transitions.push_back(transition_y);
    return true;
}

template<> bool wxUIAnimationMSW::Build(IUIAnimationManager* manager,
    IUIAnimationTransitionLibrary* library,
    wxColour initialValue,
    wxColour targetValue)
{
    IUIAnimationVariable* variable_alpha;
    IUIAnimationVariable* variable_red;
    IUIAnimationVariable* variable_blue;
    IUIAnimationVariable* variable_green;
    IUIAnimationTransition* transition_alpha = CreateTransitionFromAnimationCurve(library,
        m_curve,
        m_durationInSeconds, 
        targetValue.Alpha());

    IUIAnimationTransition* transition_red = CreateTransitionFromAnimationCurve(library,
        m_curve, 
        m_durationInSeconds, 
        targetValue.Red());

    IUIAnimationTransition* transition_blue = CreateTransitionFromAnimationCurve(library, 
        m_curve, 
        m_durationInSeconds, 
        targetValue.Blue());

    IUIAnimationTransition* transition_green = CreateTransitionFromAnimationCurve(library,
        m_curve,
        m_durationInSeconds,
        targetValue.Green());

    if((transition_alpha == NULL) || (transition_red == NULL) || 
        (transition_blue == NULL) || (transition_green == NULL))
    {
        return false;
    }
    HRESULT result;
    result = manager->CreateAnimationVariable(initialValue.Alpha(),&variable_alpha);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = manager->CreateAnimationVariable(initialValue.Red(),&variable_red);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = manager->CreateAnimationVariable(initialValue.Blue(),&variable_blue);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = manager->CreateAnimationVariable(initialValue.Green(),&variable_green);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    m_variables.push_back(variable_alpha);
    m_transitions.push_back(transition_alpha);
    m_variables.push_back(variable_red);
    m_transitions.push_back(transition_red);
    m_variables.push_back(variable_blue);
    m_transitions.push_back(transition_blue);
    m_variables.push_back(variable_green);
    m_transitions.push_back(transition_green);
    return true;
}

template<> bool wxUIAnimationMSW::Build(IUIAnimationManager* manager, int initialValue)
{
    IUIAnimationVariable* variable;
    HRESULT result;
    result = manager->CreateAnimationVariable(initialValue,&variable);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    m_variables.push_back(variable);
    return true;
}

template<> bool wxUIAnimationMSW::Build(IUIAnimationManager* manager, double initialValue)
{
    IUIAnimationVariable* variable;
    HRESULT result;
    result = manager->CreateAnimationVariable(initialValue,&variable);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    m_variables.push_back(variable);
    return true;
}

template<> bool wxUIAnimationMSW::Build(IUIAnimationManager* manager, wxPoint initialValue)
{
    IUIAnimationVariable* variable_x;
    IUIAnimationVariable* variable_y;
    HRESULT result;
    result = manager->CreateAnimationVariable(initialValue.x,&variable_x);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = manager->CreateAnimationVariable(initialValue.y,&variable_y);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    m_variables.push_back(variable_x);
    m_variables.push_back(variable_y);
    return true;
}

template<> bool wxUIAnimationMSW::Build(IUIAnimationManager* manager, wxColour initialValue)
{
    IUIAnimationVariable* variable_alpha;
    IUIAnimationVariable* variable_red;
    IUIAnimationVariable* variable_blue;
    IUIAnimationVariable* variable_green;
    HRESULT result;
    result = manager->CreateAnimationVariable(initialValue.Alpha(),&variable_alpha);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = manager->CreateAnimationVariable(initialValue.Red(),&variable_red);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = manager->CreateAnimationVariable(initialValue.Blue(),&variable_blue);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = manager->CreateAnimationVariable(initialValue.Green(),&variable_green);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    m_variables.push_back(variable_alpha);
    m_variables.push_back(variable_red);
    m_variables.push_back(variable_blue);
    m_variables.push_back(variable_green);
    return true;
}

template<> bool wxUIAnimationMSW::AddTransitionForKeyframe(IUIAnimationStoryboard* storyboard,
    IUIAnimationTransitionLibrary* library,
    const wxUIAnimationKeyframe<int>* keyFrame,
    const wxUIAnimationKeyframe<int>* nextKeyFrame,
    double delay)
{
    //TODO: undefault easing function (relates to TODO: cleanup parameters)
    IUIAnimationTransition* transition = CreateTransitionFromAnimationCurve(library,
        wxANIMATION_CURVE_EASE_IN_OUT,
        keyFrame->GetDuration(),
        nextKeyFrame->GetValue());
    if(transition == NULL)
    {
        return false;
    }
    m_transitions.push_back(transition);

    UI_ANIMATION_KEYFRAME animation_keyframe;
    HRESULT result;
    result = storyboard->AddKeyframeAtOffset(UI_ANIMATION_KEYFRAME_STORYBOARD_START,delay,&animation_keyframe);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    //TODO: check our out of range
    result = storyboard->AddTransitionAtKeyframe(m_variables.at(0), transition, animation_keyframe);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    return true;
}

template<> bool wxUIAnimationMSW::AddTransitionForKeyframe(IUIAnimationStoryboard* storyboard,
    IUIAnimationTransitionLibrary* library,
    const wxUIAnimationKeyframe<double>* keyframe,
    const wxUIAnimationKeyframe<double>* nextKeyframe,
    double delay)
{
    //TODO: undefault easing function (relates to TODO: cleanup parameters)
    IUIAnimationTransition* transition = CreateTransitionFromAnimationCurve(library,
        wxANIMATION_CURVE_EASE_IN_OUT,
        keyframe->GetDuration(), 
        nextKeyframe->GetValue());

    if(transition == NULL)
    {
        return false;
    }
    m_transitions.push_back(transition);

    UI_ANIMATION_KEYFRAME animation_keyframe;
    HRESULT result;
    result = storyboard->AddKeyframeAtOffset(UI_ANIMATION_KEYFRAME_STORYBOARD_START,delay,&animation_keyframe);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = storyboard->AddTransitionAtKeyframe(m_variables.at(0), transition, animation_keyframe);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    return true;
}

template<> bool wxUIAnimationMSW::AddTransitionForKeyframe(IUIAnimationStoryboard* storyboard, 
    IUIAnimationTransitionLibrary* library,
    const wxUIAnimationKeyframe<wxPoint>* keyframe,
    const wxUIAnimationKeyframe<wxPoint>* nextKeyframe,
    double delay)
{
    IUIAnimationTransition* transition_x = CreateTransitionFromAnimationCurve(library,
        wxANIMATION_CURVE_EASE_IN_OUT,
        keyframe->GetDuration(),
        nextKeyframe->GetValue().x);

    IUIAnimationTransition* transition_y = CreateTransitionFromAnimationCurve(library,
        wxANIMATION_CURVE_EASE_IN_OUT,
        keyframe->GetDuration(), 
        nextKeyframe->GetValue().y);

    if((transition_x == NULL) || (transition_y == NULL))
    {
        return false;
    }
    
    m_transitions.push_back(transition_x);
    m_transitions.push_back(transition_y);

    UI_ANIMATION_KEYFRAME animation_keyframe;
    HRESULT result;
    result = storyboard->AddKeyframeAtOffset(UI_ANIMATION_KEYFRAME_STORYBOARD_START,delay,&animation_keyframe);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = storyboard->AddTransitionAtKeyframe(m_variables.at(0), transition_x, animation_keyframe);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = storyboard->AddTransitionAtKeyframe(m_variables.at(1), transition_y, animation_keyframe);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    return true;
}

template<> bool wxUIAnimationMSW::AddTransitionForKeyframe(IUIAnimationStoryboard* storyboard,
    IUIAnimationTransitionLibrary* library,
    const wxUIAnimationKeyframe<wxColour>* keyframe,
    const wxUIAnimationKeyframe<wxColour>* nextKeyframe,
    double delay)
{
    IUIAnimationTransition* transitionAlpha = CreateTransitionFromAnimationCurve(library,
        wxANIMATION_CURVE_EASE_IN_OUT, 
        keyframe->GetDuration(),
        nextKeyframe->GetValue().Alpha());

    IUIAnimationTransition* transitionRed = CreateTransitionFromAnimationCurve(library,
        wxANIMATION_CURVE_EASE_IN_OUT,
        keyframe->GetDuration(), 
        nextKeyframe->GetValue().Red());

    IUIAnimationTransition* transitionBlue = CreateTransitionFromAnimationCurve(library, 
        wxANIMATION_CURVE_EASE_IN_OUT, 
        keyframe->GetDuration(), 
        nextKeyframe->GetValue().Blue());

    IUIAnimationTransition* transitionGreen = CreateTransitionFromAnimationCurve(library, 
        wxANIMATION_CURVE_EASE_IN_OUT, 
        keyframe->GetDuration(), 
        nextKeyframe->GetValue().Green());

    if((transitionAlpha == NULL) || (transitionRed == NULL) ||
        (transitionBlue == NULL) ||  (transitionGreen == NULL))
    {
        return false;
    }
    m_transitions.push_back(transitionAlpha);
    m_transitions.push_back(transitionRed);
    m_transitions.push_back(transitionBlue);
    m_transitions.push_back(transitionGreen);

    UI_ANIMATION_KEYFRAME animationKeyframe;
    HRESULT result;
    result = storyboard->AddKeyframeAtOffset(UI_ANIMATION_KEYFRAME_STORYBOARD_START,delay,&animationKeyframe);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = storyboard->AddTransitionAtKeyframe(m_variables.at(0), transitionAlpha, animationKeyframe);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = storyboard->AddTransitionAtKeyframe(m_variables.at(1), transitionRed, animationKeyframe);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = storyboard->AddTransitionAtKeyframe(m_variables.at(2), transitionBlue, animationKeyframe);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = storyboard->AddTransitionAtKeyframe(m_variables.at(3), transitionGreen, animationKeyframe);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    return true;
}

template <> bool wxUIAnimationMSW::QueueInstantaneousTransitionAtEnd(IUIAnimationStoryboard* storyboard,
    IUIAnimationTransitionLibrary* library,
    int value)
{
    IUIAnimationTransition* transition;
    HRESULT result;
    result = library->CreateInstantaneousTransition(value, &transition);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    m_transitions.push_back(transition);
    result = storyboard->AddTransition(m_variables.at(0), transition);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    return true;
}

template <> bool wxUIAnimationMSW::QueueInstantaneousTransitionAtEnd(IUIAnimationStoryboard* storyboard,
    IUIAnimationTransitionLibrary* library,
    double value)
{
    IUIAnimationTransition* transition;
    HRESULT result;
    result = library->CreateInstantaneousTransition(value, &transition);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    m_transitions.push_back(transition);
    result = storyboard->AddTransition(m_variables.at(0), transition);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    return true;
}

template <> bool wxUIAnimationMSW::QueueInstantaneousTransitionAtEnd(IUIAnimationStoryboard* storyboard,
    IUIAnimationTransitionLibrary* library,
    wxPoint value)
{
    IUIAnimationTransition* transitionX;
    IUIAnimationTransition* transitionY;
    HRESULT result;
    result = library->CreateInstantaneousTransition(value.x, &transitionX);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = library->CreateInstantaneousTransition(value.y, &transitionY);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    m_transitions.push_back(transitionX);
    m_transitions.push_back(transitionY);

    result = storyboard->AddTransition(m_variables.at(0), transitionX);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = storyboard->AddTransition(m_variables.at(1), transitionY);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    return true;
}

template <> bool wxUIAnimationMSW::QueueInstantaneousTransitionAtEnd(IUIAnimationStoryboard* storyboard,
    IUIAnimationTransitionLibrary* library,
    wxColour value)
{
    IUIAnimationTransition* transitionAlpha;
    IUIAnimationTransition* transitionRed;
    IUIAnimationTransition* transitionBlue;
    IUIAnimationTransition* transitionGreen;

    HRESULT result;
    result = library->CreateInstantaneousTransition(value.Alpha(), &transitionAlpha);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = library->CreateInstantaneousTransition(value.Red(), &transitionRed);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = library->CreateInstantaneousTransition(value.Blue(), &transitionBlue);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = library->CreateInstantaneousTransition(value.Green(), &transitionGreen);
    if(!SUCCEEDED(result))
    {
        return false;
    }

    m_transitions.push_back(transitionAlpha);
    m_transitions.push_back(transitionRed);
    m_transitions.push_back(transitionBlue);
    m_transitions.push_back(transitionGreen);

    result = storyboard->AddTransition(m_variables.at(0), transitionAlpha);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = storyboard->AddTransition(m_variables.at(1), transitionRed);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = storyboard->AddTransition(m_variables.at(2), transitionBlue);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = storyboard->AddTransition(m_variables.at(3), transitionGreen);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    return true;
}