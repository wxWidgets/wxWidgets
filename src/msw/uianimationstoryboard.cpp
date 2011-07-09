///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/ui_animation_storyboard.h
// Purpose:     wxUIAnimationStoryboard implementation using Windows Animation
// Author:      Chiciu Bogdan Gabriel
// Created:     2011-06-18
// RCS-ID:      $Id$
// Copyright:   (c) 2011 wxWidgets Team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/control.h"
#endif

#include <UIAnimation.h>

#include "wx/uianimation.h"
#include "wx/uianimationstoryboard.h"

#include "wx/msw/uianimation.h"
#include "wx/msw/private/uianimationeventhandler.h"

DEFINE_EVENT_TYPE(wxEVT_STORYBOARD)

// Creates the IUIAnimationStoryboard and related MSW animation classes.
wxUIAnimationStoryboardMSW::wxUIAnimationStoryboardMSW()
{
    if(Initialize() == true)
    {
        HRESULT result;
        m_storyboard = NULL;
        result = m_animationManager->CreateStoryboard(&m_storyboard);
        if(!SUCCEEDED(result))
        {
            //wxLogWarning/Error
        }
    }
}

// This should release every COM object that we have a reference to (TODO/NOTE: some object are left out on purpose for now)
wxUIAnimationStoryboardMSW::~wxUIAnimationStoryboardMSW()
{
    m_animations.clear();

    // TODO: check the way these objects get released (mainly if there are no references to them elsewhere)
    
    m_animationTimer->SetTimerEventHandler(NULL);
    m_animationManager->SetManagerEventHandler(NULL);

    m_transitionLibrary->Release();
    m_storyboard->Release();
    m_animationManager->Release();

    // This will cause an access violation in UIAnimation.dll!726c24c6
    // IsEnabled returns OK even though the destructor gets called when animations finish.
    // And even if we disable the timer(successfully) the access violation will still happen.
    //m_animationTimer->Release();
}

// Schedules the storyboard to start playing right away.
void wxUIAnimationStoryboardMSW::Start()
{
    UI_ANIMATION_SECONDS now;
    m_animationTimer->GetTime(&now);
    HRESULT result;

    if(m_repeatCount > 0)
    {
        UI_ANIMATION_KEYFRAME endKeyframe;

        IUIAnimationTransition* finalTransition = m_animations.back()->GetTransitions().back();
        m_storyboard->AddKeyframeAfterTransition(finalTransition, &endKeyframe);
        
        m_storyboard->RepeatBetweenKeyframes(UI_ANIMATION_KEYFRAME_STORYBOARD_START, endKeyframe, m_repeatCount);
    }

    UI_ANIMATION_SCHEDULING_RESULT schedulingResult;
    result = m_storyboard->Schedule(now, &schedulingResult);


    if (SUCCEEDED(result))
    {
        if (schedulingResult == UI_ANIMATION_SCHEDULING_SUCCEEDED)
        {
            m_animationTimer->Enable();
        }
    }
}

// Abandons the IUIAnimationStoryboard which causes all animations to stop.
void wxUIAnimationStoryboardMSW::Stop()
{
    HRESULT result;
    result = m_storyboard->Abandon();
    if(!SUCCEEDED(result))
    {
        // wxLogWarning/Error
    }
}

bool wxUIAnimationStoryboardMSW::Initialize()
{
    HRESULT result;
    result = CoCreateInstance(CLSID_UIAnimationManager,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_IUIAnimationManager,
        reinterpret_cast<void**>(&m_animationManager));
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = CoCreateInstance(CLSID_UIAnimationTimer,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_IUIAnimationTimer,
        reinterpret_cast<void**>(&m_animationTimer));
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = CoCreateInstance(CLSID_UIAnimationTransitionLibrary,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_IUIAnimationTransitionLibrary,
        reinterpret_cast<void**>(&m_transitionLibrary));
    if(!SUCCEEDED(result))
    {
        return false;
    }
 
    UIAnimationManagerEventHandlerBase* animation_manager_handler = new UIAnimationManagerEventHandlerBase(this);
    result = m_animationManager->SetManagerEventHandler(animation_manager_handler);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = animation_manager_handler->Release();
    if(!SUCCEEDED(result))
    {
        return false;
    }

    IUIAnimationTimerUpdateHandler *timerUpdateHandler;
    result = m_animationManager->QueryInterface(IID_IUIAnimationTimerUpdateHandler, reinterpret_cast<void**>(&timerUpdateHandler));
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = m_animationTimer->SetTimerUpdateHandler(timerUpdateHandler, UI_ANIMATION_IDLE_BEHAVIOR_DISABLE);
    if(!SUCCEEDED(result))
    {
        return false;
    }
    result = timerUpdateHandler->Release();
    if(!SUCCEEDED(result))
    {
        return false;
    }
    UIAnimationTimerEventHandlerBase* timerEventHandler = new UIAnimationTimerEventHandlerBase(this);
    result = m_animationTimer->SetTimerEventHandler(timerEventHandler);
    timerEventHandler->Release();
    if(!SUCCEEDED(result))
    {
        return false;
    }
    return true;
}

// This method gets called with every animation tick. The visual update happens here.
void wxUIAnimationStoryboardMSW::Update()
{
    if(m_targetControl == NULL)// our target is gone, stop updating
    {
        m_animationTimer->Disable();
        return;
    }
    // Update the target property for our animations.
    wxVector<wxSharedPtr<wxUIAnimationMSW>>::iterator iter;
    for(iter = m_animations.begin(); iter != m_animations.end(); ++iter)
    {
        switch((*iter)->GetTargetProperty())
        {
        case wxANIMATION_TARGET_PROPERTY_POSITION:
            {
                m_targetControl->Move((*iter)->GetValue<wxPoint>());
                break;
            }
        case wxANIMATION_TARGET_PROPERTY_BACKGROUND_COLOR:
            {
                m_targetControl->SetBackgroundColour((*iter)->GetValue<wxColour>());
                break;
            }
        case wxANIMATION_TARGET_PROPERTY_OPACITY:
            {
                m_targetControl->SetTransparent((*iter)->GetValue<int>());
                break;
            }
        case wxANIMATION_TARGET_PROPERTY_ANCHOR_POINT:
        case wxANIMATION_TARGET_PROPERTY_BACKGROUND_FILTERS:
        case wxANIMATION_TARGET_PROPERTY_BORDER_COLOR:
        case wxANIMATION_TARGET_PROPERTY_BORDER_WIDTH:
        case wxANIMATION_TARGET_PROPERTY_BOUNDS:
        case wxANIMATION_TARGET_PROPERTY_COMPOSITING_FILTER:
        case wxANIMATION_TARGET_PROPERTY_CONTENTS:
        case wxANIMATION_TARGET_PROPERTY_CONTENTSRECT:
        case wxANIMATION_TARGET_PROPERTY_CORNER_RADIUS:
        case wxANIMATION_TARGET_PROPERTY_DOUBLESIDED:
        case wxANIMATION_TARGET_PROPERTY_FILTERS:
        case wxANIMATION_TARGET_PROPERTY_FRAME:
        case wxANIMATION_TARGET_PROPERTY_HIDDEN:
        case wxANIMATION_TARGET_PROPERTY_MASK:
        case wxANIMATION_TARGET_PROPERTY_MASKS_TO_BOUNDS:
        case wxANIMATION_TARGET_PROPERTY_SHADOW_COLOR:
            //etc
            {
                //Not suppoted under MSW?
            }
        }
    }
}

