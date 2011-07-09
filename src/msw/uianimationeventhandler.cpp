///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/uianimationeventhandler.h
// Purpose:     Implementation of the Timer/Manager event handler classes
// Author:      Chiciu Bogdan Gabriel
// Created:     2011-06-16
// RCS-ID:      $Id$
// Copyright:   (c) 2011 wxWidgets Team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
//we don't really need anything
#endif

#include "wx/uianimationstoryboard.h"
#include "wx/msw/private/uianimationeventhandler.h"

#if 0
BEGIN_IID_TABLE(UIAnimationManagerEventHandlerBase)
    ADD_IID(Unknown)
    ADD_IID(UIAnimationManagerEventHandler)
END_IID_TABLE;

IMPLEMENT_IUNKNOWN_METHODS(UIAnimationManagerEventHandlerBase)

BEGIN_IID_TABLE(UIAnimationTimerEventHandlerBase)
    ADD_IID(Unknown)
    ADD_IID(UIAnimationTimerEventHandler)
END_IID_TABLE;

IMPLEMENT_IUNKNOWN_METHODS(UIAnimationTimerEventHandlerBase)
#endif

UIAnimationTimerEventHandlerBase::UIAnimationTimerEventHandlerBase(wxUIAnimationStoryboardMSW* storyboard)
{
    m_storyboard = storyboard;
};

STDMETHODIMP UIAnimationTimerEventHandlerBase::OnPreUpdate()
{
    //PreUpdate seems to offer smoother animation than PostUpdate
    //TODO: document better
    m_storyboard->Update();
    return S_OK;
};

STDMETHODIMP UIAnimationTimerEventHandlerBase::OnPostUpdate()
{
    return S_OK;
};

STDMETHODIMP UIAnimationTimerEventHandlerBase::OnRenderingTooSlow(UINT32 framesPerSecond)
{
    return S_OK;
};

UIAnimationManagerEventHandlerBase::UIAnimationManagerEventHandlerBase(wxUIAnimationStoryboardMSW* storyboard)
{
    m_storyboard = storyboard;
};

STDMETHODIMP UIAnimationManagerEventHandlerBase::OnManagerStatusChanged(UI_ANIMATION_MANAGER_STATUS newStatus,
    UI_ANIMATION_MANAGER_STATUS previousStatus)
{
    if((newStatus == UI_ANIMATION_MANAGER_IDLE) && 
        (previousStatus == UI_ANIMATION_MANAGER_BUSY))
    {
        m_storyboard->SetStoryboardStatus(wxSTORYBOARD_STATUS_FINISHED);
        m_storyboard->ProcessEvent(wxUIAnimationStoryboardEvent(m_storyboard));
    }
    if((newStatus == UI_ANIMATION_MANAGER_BUSY) && 
        (previousStatus == UI_ANIMATION_MANAGER_IDLE))
    {
        m_storyboard->SetStoryboardStatus(wxSTORYBOARD_STATUS_STARTED);
        m_storyboard->ProcessEvent(wxUIAnimationStoryboardEvent(m_storyboard));
    }
    return S_OK;
}
