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


WX_CHECK_BUILD_OPTIONS("wxAnimation")

// TODO [iteration]: remove duplicate/similar code

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

bool wxUIAnimationMSW::CreateAndQueueVariable(double value)
{
    IUIAnimationVariable* variable;
    HRESULT result;
    result = m_animationManager->CreateAnimationVariable(value, &variable);
    if(!SUCCEEDED(result))
    {
        wxLogLastError(wxT("CreateAnimationVariable"));
        return false;
    }
    else
    {
        m_variables.push_back(variable);
        return true;
    }
}

bool wxUIAnimationMSW::CreateAndQueueTransition(double value)
{
    return CreateAndQueueTransition(m_durationInSeconds, value);
}

bool wxUIAnimationMSW::CreateAndQueueInstantaneousTransition(double value)
{
    IUIAnimationTransition* transition;
    HRESULT result;

    result = m_transitionLibrary->CreateInstantaneousTransition(value, &transition);
    if(!SUCCEEDED(result))
    {
        wxLogLastError(wxT("CreateInstantaneousTransition"));
        return false;
    }
    m_transitions.push_back(transition);
    return true;
}

bool wxUIAnimationMSW::CreateAndQueueDiscreteTransition(double value)
{
    return CreateAndQueueDiscreteTransition(m_durationInSeconds, value);
}

bool wxUIAnimationMSW::CreateAndQueueDiscreteTransition(double durationInSeconds, double value)
{
    IUIAnimationTransition* transition;
    HRESULT result;

    // The hold duration of a discrete transition is not important however 'low' values (ie 0.05) will
    // not trigger any changes in the animation variable.
    result = m_transitionLibrary->CreateDiscreteTransition(durationInSeconds, value, 0.5, &transition);
    if(!SUCCEEDED(result))
    {
        wxLogLastError(wxT("CreateInstantaneousTransition"));
        return false;
    }
    m_transitions.push_back(transition);
    return true;
}

bool wxUIAnimationMSW::CreateAndQueueTransition(double durationInSeconds, double value)
{
    IUIAnimationTransition* transition;
    HRESULT result;

    // The hard coded values of 0.0, 0.25, 0.5 used in the switch bellow should make the MSW animation 
    // look like the OS X equivalents (that do not accept parameters to easing functions).
    // TODO: re-check the values
    switch(m_curve)
    {
    case wxANIMATION_CURVE_EASE_IN_OUT:
        {
            result = m_transitionLibrary->CreateAccelerateDecelerateTransition(durationInSeconds, 
                value,
                0.5,
                0.5,
                &transition);
            if(!SUCCEEDED(result))
            {
                wxLogLastError(wxT("CreateAccelerateDecelerateTransition"));
                return false;
            }
            break;
        }
    case wxANIMATION_CURVE_LINEAR:
        {
            result = m_transitionLibrary->CreateLinearTransition(durationInSeconds,
                value,
                &transition);
            if(!SUCCEEDED(result))
            {
                wxLogLastError(wxT("CreateLinearTransition"));
                return false;
            }
            break;
        }
    case wxANIMATION_CURVE_EASE_IN:
        {
            result = m_transitionLibrary->CreateAccelerateDecelerateTransition(durationInSeconds,
                value,
                0,
                0.25,
                &transition);
            if(!SUCCEEDED(result))
            {
                wxLogLastError(wxT("CreateAccelerateDecelerateTransition"));
                return false;
            }
            break;
        }
    case wxANIMATION_CURVE_EASE_OUT:
        {
            result = m_transitionLibrary->CreateAccelerateDecelerateTransition(durationInSeconds,
                value,
                0.25,
                0,
                &transition);
            if(!SUCCEEDED(result))
            {
                wxLogLastError(wxT("CreateAccelerateDecelerateTransition"));
                return false;
            }
            break;
        }
    }
    m_transitions.push_back(transition);
    return true;
}

bool wxUIAnimationMSW::AddTransition(double initialValue, double targetValue)
{
    if(CreateAndQueueVariable(initialValue))
    {
        if(CreateAndQueueTransition(targetValue))
        {
            if(AddLastCreatedTransitionToStoryboard())
            {
                return true;
            }
        }
    }
    return false;
}

bool wxUIAnimationMSW::AddTransitionToStoryboard(IUIAnimationVariable* variable,
        IUIAnimationTransition* transition,
        UI_ANIMATION_KEYFRAME keyFrame)
{
    HRESULT result;
    result = m_storyboard->AddTransitionAtKeyframe(variable, transition, keyFrame);
    if(!SUCCEEDED(result))
    {
        wxLogLastError(wxT("AddTransitionAtKeyframe"));
        return false;
    }
    return true;
}

bool wxUIAnimationMSW::AddLastCreatedTransitionToStoryboard()
{
    return AddLastCreatedTransitionToStoryboard(m_variables.back());
}

bool wxUIAnimationMSW::AddLastCreatedTransitionToStoryboard(IUIAnimationVariable* variable)
{
    HRESULT result;
    result = m_storyboard->AddTransition(variable, m_transitions.back());
    if(!SUCCEEDED(result))
    {
        wxLogLastError(wxT("AddTransition"));
        return false;
    }
    return true;
}

bool wxUIAnimationMSW::AddLastCreatedTransitionToStoryboard(IUIAnimationVariable* variable,
    UI_ANIMATION_KEYFRAME keyFrame)
{
    if(AddTransitionToStoryboard(variable, m_transitions.back(), keyFrame))
    {
        return true;
    }
    return false;
}

template<> int wxUIAnimationMSW::GetValue()
{
    int value;
    wxASSERT(m_variables.size() == 1);
    m_variables.at(0)->GetIntegerValue(&value);
    return value;
}

template<> double wxUIAnimationMSW::GetValue()
{
    double value;
    wxASSERT(m_variables.size() == 1);
    m_variables.at(0)->GetValue(&value);
    return value;
}

template<> wxPoint wxUIAnimationMSW::GetValue()
{
    double x;
    double y;
    wxASSERT(m_variables.size() == 2);
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
    wxASSERT(m_variables.size() == 4);
    m_variables.at(0)->GetValue(&alpha);
    m_variables.at(1)->GetValue(&red);
    m_variables.at(2)->GetValue(&blue);
    m_variables.at(3)->GetValue(&green);
    
    return wxColour(red,green,blue,alpha);
}

template<> wxSize wxUIAnimationMSW::GetValue()
{
    double width;
    double height;
    wxASSERT(m_variables.size() == 2);
    m_variables.at(0)->GetValue(&width);
    m_variables.at(1)->GetValue(&height);
    return wxSize(width, height);
}

template<> bool wxUIAnimationMSW::GetValue()
{
    double value;
    wxASSERT(m_variables.size() == 1);
    m_variables.at(0)->GetValue(&value);
    if(value == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

template<> bool wxUIAnimationMSW::Build(int initialValue,
    int targetValue)
{
    return AddTransition(initialValue, targetValue);
}

template<> bool wxUIAnimationMSW::Build(double initialValue,
    double targetValue)
{
    return AddTransition(initialValue, targetValue);
}

template<> bool wxUIAnimationMSW::Build(wxPoint initialValue,
    wxPoint targetValue)
{
    if(AddTransition(initialValue.x, targetValue.y))
    {
        if(AddTransition(initialValue.y, targetValue.y))
        {
            return true;
        }
    }
    return false;
}

template<> bool wxUIAnimationMSW::Build(wxColour initialValue,
    wxColour targetValue)
{
    if(AddTransition(initialValue.Alpha(), targetValue.Alpha()))
    {
        if(AddTransition(initialValue.Red(), targetValue.Red()))
        {
            if(AddTransition(initialValue.Blue(), targetValue.Blue()))
            {
                if(AddTransition(initialValue.Green(), targetValue.Green()))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

template<> bool wxUIAnimationMSW::Build(wxSize initialValue,
    wxSize targetValue)
{
    if(AddTransition(initialValue.GetWidth(), targetValue.GetWidth()))
    {
        if(AddTransition(initialValue.GetHeight(), targetValue.GetHeight()))
        {
            return true;
        }
    }
    return false;
}

template<> bool wxUIAnimationMSW::Build(bool initialValue,
    bool targetValue)
{
    if(initialValue == targetValue)
    {
        return false;
    }
    if(CreateAndQueueVariable(initialValue))
    {
        if(CreateAndQueueDiscreteTransition(targetValue))
        {
            if(AddLastCreatedTransitionToStoryboard())
            {
                return true;
            }
        }
    }
    return false;
}

template<> bool wxUIAnimationMSW::Build(int initialValue)
{
    if(CreateAndQueueVariable(initialValue))
    {
        return true;
    }
    return false;
}

template<> bool wxUIAnimationMSW::Build(double initialValue)
{
    if(CreateAndQueueVariable(initialValue))
    {
        return true;
    }
    return false;
}

template<> bool wxUIAnimationMSW::Build(wxPoint initialValue)
{
    if(CreateAndQueueVariable(initialValue.x))
    {
        if(CreateAndQueueVariable(initialValue.y))
        {
            return true;
        }
    }
    return false;
}

template<> bool wxUIAnimationMSW::Build(wxColour initialValue)
{
    if(CreateAndQueueVariable(initialValue.Alpha()))
    {
        if(CreateAndQueueVariable(initialValue.Red()))
        {
            if(CreateAndQueueVariable(initialValue.Blue()))
            {
                if(CreateAndQueueVariable(initialValue.Green()))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

template<> bool wxUIAnimationMSW::Build(wxSize initialValue)
{
    if(CreateAndQueueVariable(initialValue.GetWidth()))
    {
        if(CreateAndQueueVariable(initialValue.GetHeight()))
        {
            return true;
        }
    }
    return false;
}

template<> bool wxUIAnimationMSW::Build(bool initialValue)
{
    if(CreateAndQueueVariable(initialValue))
    {
        return true;
    }
    return false;
}

template<> bool wxUIAnimationMSW::AddTransitionForKeyframe(const wxUIAnimationKeyframe<int>* keyFrame,
    const wxUIAnimationKeyframe<int>* nextKeyFrame,
    double delay)
{
    UI_ANIMATION_KEYFRAME animationKeyframe;
    HRESULT result;
    result = m_storyboard->AddKeyframeAtOffset(UI_ANIMATION_KEYFRAME_STORYBOARD_START,delay,&animationKeyframe);
    if(!SUCCEEDED(result))
    {
        wxLogLastError(wxT("AddKeyframeAtOffset"));
        return false;
    }

    if(CreateAndQueueTransition(keyFrame->GetDuration(), nextKeyFrame->GetValue()))
    {
        if(AddLastCreatedTransitionToStoryboard(m_variables.front(), animationKeyframe))
        {
            return true;
        }
    }

    return false;
}

template<> bool wxUIAnimationMSW::AddTransitionForKeyframe(const wxUIAnimationKeyframe<double>* keyFrame,
    const wxUIAnimationKeyframe<double>* nextKeyFrame,
    double delay)
{
    UI_ANIMATION_KEYFRAME animationKeyframe;
    HRESULT result;
    result = m_storyboard->AddKeyframeAtOffset(UI_ANIMATION_KEYFRAME_STORYBOARD_START,delay,&animationKeyframe);
    if(!SUCCEEDED(result))
    {
        wxLogLastError(wxT("AddKeyframeAtOffset"));
        return false;
    }

    if(CreateAndQueueTransition(keyFrame->GetDuration(), nextKeyFrame->GetValue()))
    {
        if(AddLastCreatedTransitionToStoryboard(m_variables.front(), animationKeyframe))
        {
            return true;
        }
    }

    return false;
}

template<> bool wxUIAnimationMSW::AddTransitionForKeyframe(const wxUIAnimationKeyframe<wxPoint>* keyFrame,
    const wxUIAnimationKeyframe<wxPoint>* nextKeyFrame,
    double delay)
{
    UI_ANIMATION_KEYFRAME animationKeyframe;
    HRESULT result;
    result = m_storyboard->AddKeyframeAtOffset(UI_ANIMATION_KEYFRAME_STORYBOARD_START,delay,&animationKeyframe);
    if(!SUCCEEDED(result))
    {
        wxLogLastError(wxT("AddKeyframeAtOffset"));
        return false;
    }

    if(CreateAndQueueTransition(keyFrame->GetDuration(), nextKeyFrame->GetValue().x))
    {
        if(AddLastCreatedTransitionToStoryboard(m_variables.at(0), animationKeyframe))
        {
            if(CreateAndQueueTransition(keyFrame->GetDuration(), nextKeyFrame->GetValue().y))
            {
                if(AddLastCreatedTransitionToStoryboard(m_variables.at(1), animationKeyframe))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

template<> bool wxUIAnimationMSW::AddTransitionForKeyframe(const wxUIAnimationKeyframe<wxColour>* keyFrame,
    const wxUIAnimationKeyframe<wxColour>* nextKeyFrame,
    double delay)
{
    UI_ANIMATION_KEYFRAME animationKeyframe;
    HRESULT result;
    result = m_storyboard->AddKeyframeAtOffset(UI_ANIMATION_KEYFRAME_STORYBOARD_START,delay,&animationKeyframe);
    if(!SUCCEEDED(result))
    {
        wxLogLastError(wxT("AddKeyframeAtOffset"));
        return false;
    }

    if(CreateAndQueueTransition(keyFrame->GetDuration(), nextKeyFrame->GetValue().Alpha()))
    {
        if(AddLastCreatedTransitionToStoryboard(m_variables.at(0), animationKeyframe))
        {
            if(CreateAndQueueTransition(keyFrame->GetDuration(), nextKeyFrame->GetValue().Red()))
            {
                if(AddLastCreatedTransitionToStoryboard(m_variables.at(1), animationKeyframe))
                {
                    if(CreateAndQueueTransition(keyFrame->GetDuration(), nextKeyFrame->GetValue().Blue()))
                    {
                        if(AddLastCreatedTransitionToStoryboard(m_variables.at(2), animationKeyframe))
                        {
                            if(CreateAndQueueTransition(keyFrame->GetDuration(), nextKeyFrame->GetValue().Green()))
                            {
                                if(AddLastCreatedTransitionToStoryboard(m_variables.at(3), animationKeyframe))
                                {
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}

template<> bool wxUIAnimationMSW::AddTransitionForKeyframe(const wxUIAnimationKeyframe<wxSize>* keyFrame,
    const wxUIAnimationKeyframe<wxSize>* nextKeyFrame,
    double delay)
{
    UI_ANIMATION_KEYFRAME animationKeyframe;
    HRESULT result;
    result = m_storyboard->AddKeyframeAtOffset(UI_ANIMATION_KEYFRAME_STORYBOARD_START,delay,&animationKeyframe);
    if(!SUCCEEDED(result))
    {
        wxLogLastError(wxT("AddKeyframeAtOffset"));
        return false;
    }

    if(CreateAndQueueTransition(keyFrame->GetDuration(), nextKeyFrame->GetValue().GetWidth()))
    {
        if(AddLastCreatedTransitionToStoryboard(m_variables.at(0), animationKeyframe))
        {
            if(CreateAndQueueTransition(keyFrame->GetDuration(), nextKeyFrame->GetValue().GetHeight()))
            {
                if(AddLastCreatedTransitionToStoryboard(m_variables.at(1), animationKeyframe))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

template<> bool wxUIAnimationMSW::AddTransitionForKeyframe(const wxUIAnimationKeyframe<bool>* keyFrame,
    const wxUIAnimationKeyframe<bool>* nextKeyFrame,
    double delay)
{
    UI_ANIMATION_KEYFRAME animationKeyframe;
    HRESULT result;
    result = m_storyboard->AddKeyframeAtOffset(UI_ANIMATION_KEYFRAME_STORYBOARD_START,delay,&animationKeyframe);
    if(!SUCCEEDED(result))
    {
        wxLogLastError(wxT("AddKeyframeAtOffset"));
        return false;
    }

    if(CreateAndQueueDiscreteTransition(keyFrame->GetDuration(), nextKeyFrame->GetValue()))
    {
        if(AddLastCreatedTransitionToStoryboard(m_variables.at(0), animationKeyframe))
        {
            return true;
        }
    }
    return false;
}


template <> bool wxUIAnimationMSW::QueueInstantaneousTransitionAtEnd(int value)
{
    if(CreateAndQueueInstantaneousTransition(value))
    {
        if(AddLastCreatedTransitionToStoryboard(m_variables.at(0)))
        {
            return true;
        }
    }
    return false;
}

template <> bool wxUIAnimationMSW::QueueInstantaneousTransitionAtEnd(double value)
{
    if(CreateAndQueueInstantaneousTransition(value))
    {
        if(AddLastCreatedTransitionToStoryboard(m_variables.at(0)))
        {
            return true;
        }
    }
    return false;
}

template <> bool wxUIAnimationMSW::QueueInstantaneousTransitionAtEnd(wxPoint value)
{
    if(CreateAndQueueInstantaneousTransition(value.x))
    {
        if(AddLastCreatedTransitionToStoryboard(m_variables.at(0)))
        {
            if(CreateAndQueueInstantaneousTransition(value.y))
            {
                if(AddLastCreatedTransitionToStoryboard(m_variables.at(1)))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

template <> bool wxUIAnimationMSW::QueueInstantaneousTransitionAtEnd(wxColour value)
{
    if(CreateAndQueueInstantaneousTransition(value.Alpha()))
    {
        if(AddLastCreatedTransitionToStoryboard(m_variables.at(0)))
        {
            if(CreateAndQueueInstantaneousTransition(value.Red()))
            {
                if(AddLastCreatedTransitionToStoryboard(m_variables.at(1)))
                {
                    if(CreateAndQueueInstantaneousTransition(value.Blue()))
                    {
                        if(AddLastCreatedTransitionToStoryboard(m_variables.at(2)))
                        {
                            if(CreateAndQueueInstantaneousTransition(value.Green()))
                            {
                                if(AddLastCreatedTransitionToStoryboard(m_variables.at(3)))
                                {
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

template <> bool wxUIAnimationMSW::QueueInstantaneousTransitionAtEnd(wxSize value)
{
    if(CreateAndQueueInstantaneousTransition(value.GetWidth()))
    {
        if(AddLastCreatedTransitionToStoryboard(m_variables.at(0)))
        {
            if(CreateAndQueueInstantaneousTransition(value.GetHeight()))
            {
                if(AddLastCreatedTransitionToStoryboard(m_variables.at(1)))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

template <> bool wxUIAnimationMSW::QueueInstantaneousTransitionAtEnd(bool value)
{
    if(CreateAndQueueInstantaneousTransition(value))
    {
        if(AddLastCreatedTransitionToStoryboard(m_variables.at(0)))
        {
            return true;
        }
    }
    return false;
}